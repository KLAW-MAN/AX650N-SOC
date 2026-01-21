// SPDX-License-Identifier: GPL-2.0-only

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/console.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/nmi.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>
#include <linux/smp.h>
#include <linux/security.h>
#include <linux/memblock.h>
#include <linux/syscalls.h>
#include <linux/crash_core.h>
#include <linux/ratelimit.h>
#include <linux/kmsg_dump.h>
#include <linux/syslog.h>
#include <linux/cpu.h>
#include <linux/rculist.h>
#include <linux/poll.h>
#include <linux/irq_work.h>
#include <linux/ctype.h>
#include <linux/uio.h>
#include <linux/sched/clock.h>
#include <linux/sched/debug.h>
#include <linux/sched/task_stack.h>
#include <linux/uaccess.h>
#include <asm/sections.h>
#include <trace/events/initcall.h>
#include "../../fs/proc/axlogctl.h"
#include "ax_printk_ringbuffer.h"
#include "internal.h"

#define  AX_CONSOLE_LOGLEVEL_DEFAULTN 4
char ax_devkmsg_log_str[AX_DEVKMSG_STR_MAX_SIZE] = "ratelimit";

enum ax_devkmsg_log_bits {
	__DEVKMSG_LOG_BIT_ON = 0,
	__DEVKMSG_LOG_BIT_OFF,
	__DEVKMSG_LOG_BIT_LOCK,
};

enum ax_devkmsg_log_masks {
	DEVKMSG_LOG_MASK_ON = BIT(__DEVKMSG_LOG_BIT_ON),
	DEVKMSG_LOG_MASK_OFF = BIT(__DEVKMSG_LOG_BIT_OFF),
	DEVKMSG_LOG_MASK_LOCK = BIT(__DEVKMSG_LOG_BIT_LOCK),
};

/* Keep both the 'on' and 'off' bits clear, i.e. ratelimit by default: */
#define DEVKMSG_LOG_MASK_DEFAULT    0

static unsigned int __read_mostly ax_devkmsg_log = DEVKMSG_LOG_MASK_DEFAULT;
static axlogctl_mem_cfg_ptr s_axlogctl_cfg_ptr = NULL;

/* syslog_lock protects syslog_* variables and write access to clear_seq. */
static DEFINE_MUTEX(ax_syslog_lock);

DECLARE_WAIT_QUEUE_HEAD(ax_log_wait);
/* All 3 protected by @syslog_lock. */
/* the next printk record to read by syslog(READ) or /proc/kmsg */
static u64 ax_syslog_seq;
static size_t ax_syslog_partial;
static bool ax_syslog_time;

struct ax_latched_seq {
	seqcount_latch_t latch;
	u64 val[2];
};

static struct ax_latched_seq clear_seq = {
	.latch = SEQCNT_LATCH_ZERO(clear_seq.latch),
	.val[0] = 0,
	.val[1] = 0,
};

#define PREFIX_MAX      64

/* the maximum size of a formatted record (i.e. with prefix added per line) */
#define CONSOLE_LOG_MAX     1024

/* the maximum size allowed to be reserved for a record */
#define LOG_LINE_MAX        (CONSOLE_LOG_MAX - PREFIX_MAX)

#define LOG_LEVEL(v)        ((v) & 0x07)
#define LOG_FACILITY(v)     ((v) >> 3 & 0xff)

/* record buffer */
#define LOG_ALIGN __alignof__(unsigned long)
#define __LOG_BUF_LEN (1 << CONFIG_AXLOG_BUF_SHIFT)
#define LOG_BUF_LEN_MAX (u32)(1 << 31)
static char __ax_log_buf[__LOG_BUF_LEN] __aligned(LOG_ALIGN);
static char *ax_log_buf = __ax_log_buf;
static u32 ax_log_buf_len = __LOG_BUF_LEN;

#define PRB_AVGBITS 5		/* 32 character average length */

#if CONFIG_AXLOG_BUF_SHIFT <= PRB_AVGBITS
#error CONFIG_AXLOG_BUF_SHIFT value too small.
#endif
_AX_DEFINE_PRINTKRB(ax_printk_rb_static, CONFIG_AXLOG_BUF_SHIFT - PRB_AVGBITS,
		 PRB_AVGBITS, &__ax_log_buf[0]);

static struct ax_printk_ringbuffer ax_printk_rb_dynamic;

static struct ax_printk_ringbuffer *prb = &ax_printk_rb_static;

static bool __ax_printk_percpu_data_ready __read_mostly;

static const char axlog_level_flags[]="MACEWNID";

bool ax_printk_percpu_data_ready(void)
{
	return __ax_printk_percpu_data_ready;
}

/* Must be called under syslog_lock. */
static void ax_latched_seq_write(struct ax_latched_seq *ls, u64 val)
{
	raw_write_seqcount_latch(&ls->latch);
	ls->val[0] = val;
	raw_write_seqcount_latch(&ls->latch);
	ls->val[1] = val;
}

/* Can be called from any context. */
static u64 ax_latched_seq_read_nolock(struct ax_latched_seq *ls)
{
	unsigned int seq;
	unsigned int idx;
	u64 val;

	do {
		seq = raw_read_seqcount_latch(&ls->latch);
		idx = seq & 0x1;
		val = ls->val[idx];
	} while (read_seqcount_latch_retry(&ls->latch, seq));

	return val;
}

/* Return log buffer address */
char *ax_log_buf_addr_get(void)
{
	return ax_log_buf;
}

/* Return log buffer size */
u32 ax_log_buf_len_get(void)
{
	return ax_log_buf_len;
}

#define MAX_LOG_TAKE_PART 4
static const char trunc_msg[] = "<truncated>";

static void ax_truncate_msg(u16 * text_len, u16 * trunc_msg_len)
{
	/*
	 * The message should not take the whole buffer. Otherwise, it might
	 * get removed too soon.
	 */
	u32 max_text_len = ax_log_buf_len / MAX_LOG_TAKE_PART;

	if (*text_len > max_text_len)
		*text_len = max_text_len;

	/* enable the warning message (if there is room) */
	*trunc_msg_len = strlen(trunc_msg);
	if (*text_len >= *trunc_msg_len)
		*text_len -= *trunc_msg_len;
	else
		*trunc_msg_len = 0;
}

int ax_dmesg_restrict = IS_ENABLED(CONFIG_SECURITY_DMESG_RESTRICT);

static int ax_syslog_action_restricted(int type)
{
	if (ax_dmesg_restrict)
		return 1;
	/*
	 * Unless restricted, we allow "read all" and "get buffer size"
	 * for everybody.
	 */
	return type != SYSLOG_ACTION_READ_ALL &&
	    type != SYSLOG_ACTION_SIZE_BUFFER;
}

static int ax_check_syslog_permissions(int type, int source)
{
	/*
	 * If this is from /proc/kmsg and we've already opened it, then we've
	 * already done the capabilities checks at open time.
	 */
	if (source == SYSLOG_FROM_PROC && type != SYSLOG_ACTION_OPEN)
		goto ok;

	if (ax_syslog_action_restricted(type)) {
		if (capable(CAP_SYSLOG))
			goto ok;
		/*
		 * For historical reasons, accept CAP_SYS_ADMIN too, with
		 * a warning.
		 */
		if (capable(CAP_SYS_ADMIN)) {
			pr_warn_once("%s (%d): Attempt to access syslog with "
				     "CAP_SYS_ADMIN but no CAP_SYSLOG "
				     "(deprecated).\n",
				     current->comm, task_pid_nr(current));
			goto ok;
		}
		return -EPERM;
	}
ok:
	return security_syslog(type);
}

static void ax_append_char(char **pp, char *e, char c)
{
	if (*pp < e)
		*(*pp)++ = c;
}

static ssize_t ax_info_print_ext_header(char *buf, size_t size,
				     struct ax_printk_info *info)
{
	u64 ts_usec = info->ts_nsec;
	char caller[20];

	caller[0] = '\0';

	do_div(ts_usec, 1000);

	return scnprintf(buf, size, "%u,%llu,%llu,%c%s;",
			 (info->mid << 3) | info->level, info->seq,
			 ts_usec, info->flags & LOG_CONT ? 'c' : '-', caller);
}

static ssize_t ax_msg_add_ext_text(char *buf, size_t size,
				const char *text, size_t text_len,
				unsigned char endc)
{
	char *p = buf, *e = buf + size;
	size_t i;

	/* escape non-printable characters */
	for (i = 0; i < text_len; i++) {
		unsigned char c = text[i];

		if (c < ' ' || c >= 127 || c == '\\')
			p += scnprintf(p, e - p, "\\x%02x", c);
		else
			ax_append_char(&p, e, c);
	}
	ax_append_char(&p, e, endc);

	return p - buf;
}

static ssize_t ax_msg_add_dict_text(char *buf, size_t size,
				 const char *key, const char *val)
{
	size_t val_len = strlen(val);
	ssize_t len;

	if (!val_len)
		return 0;

	len = ax_msg_add_ext_text(buf, size, "", 0, ' ');	/* dict prefix */
	len += ax_msg_add_ext_text(buf + len, size - len, key, strlen(key), '=');
	len += ax_msg_add_ext_text(buf + len, size - len, val, val_len, '\n');

	return len;
}

static ssize_t ax_msg_print_ext_body(char *buf, size_t size,
				  char *text, size_t text_len,
				  struct dev_printk_info *dev_info)
{
	ssize_t len;

	len = ax_msg_add_ext_text(buf, size, text, text_len, '\n');

	if (!dev_info)
		goto out;

	len += ax_msg_add_dict_text(buf + len, size - len, "SUBSYSTEM",
				 dev_info->subsystem);
	len += ax_msg_add_dict_text(buf + len, size - len, "DEVICE",
				 dev_info->device);
out:
	return len;
}

/* /dev/axkmsg - userspace message inject/listen interface */
struct ax_devkmsg_user {
	atomic64_t seq;
	struct ratelimit_state rs;
	struct mutex lock;
	char buf[CONSOLE_EXT_LOG_MAX];

	struct ax_printk_info info;
	char text_buf[CONSOLE_EXT_LOG_MAX];
	struct ax_printk_record record;
};

static __printf(4, 5) __cold
    int ax_devkmsg_emit(int id, const char* tag, int level, const char *fmt, ...)
{
	va_list args;
	int r;
	axlog_lvl_info_ptr loglvlptr = NULL;
	unsigned char max_id;
	unsigned char klog_state;

	if (s_axlogctl_cfg_ptr != NULL){
		max_id = s_axlogctl_cfg_ptr->max_id;
		if (id < 0 || id >= max_id) return 0;
		klog_state = s_axlogctl_cfg_ptr->klog_state;
		loglvlptr = (axlog_lvl_info_ptr)s_axlogctl_cfg_ptr->klvladdr;
		if(!klog_state || level > loglvlptr[id].klevel)
			return 0;
	}

	va_start(args, fmt);
	r = ax_vprintk_emit(id, tag, level, NULL, fmt, args);
	va_end(args);

	return r;
}

static ssize_t ax_devkmsg_write(struct kiocb *iocb, struct iov_iter *from)
{
	char *buf, *line;
	int level = default_message_loglevel;
	int mid = 0;	/* LOG_USER */
	struct file *file = iocb->ki_filp;
	struct ax_devkmsg_user *user = file->private_data;
	size_t len = iov_iter_count(from);
	ssize_t ret = len;

	if (!user || len > LOG_LINE_MAX)
		return -EINVAL;

	/* Ignore when user logging is disabled. */
	if (ax_devkmsg_log & DEVKMSG_LOG_MASK_OFF)
		return len;

	/* Ratelimit when not explicitly enabled. */
	if (!(ax_devkmsg_log & DEVKMSG_LOG_MASK_ON)) {
		if (!___ratelimit(&user->rs, current->comm))
			return ret;
	}

	buf = kmalloc(len + 1, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	buf[len] = '\0';
	if (!copy_from_iter_full(buf, len, from)) {
		kfree(buf);
		return -EFAULT;
	}

	line = buf;
	ax_devkmsg_emit(mid, "user", level, "%s", line);
	kfree(buf);
	return ret;
}

static ssize_t ax_devkmsg_read(struct file *file, char __user * buf,
			    size_t count, loff_t * ppos)
{
	struct ax_devkmsg_user *user = file->private_data;
	struct ax_printk_record *r = &user->record;
	size_t len;
	ssize_t ret;

	if (!user)
		return -EBADF;

	ret = mutex_lock_interruptible(&user->lock);
	if (ret)
		return ret;

	if (!ax_prb_read_valid(prb, atomic64_read(&user->seq), r)) {
		if (file->f_flags & O_NONBLOCK) {
			ret = -EAGAIN;
			goto out;
		}

		/*
		 * Guarantee this task is visible on the waitqueue before
		 * checking the wake condition.
		 *
		 * The full memory barrier within set_current_state() of
		 * prepare_to_wait_event() pairs with the full memory barrier
		 * within wq_has_sleeper().
		 *
		 * This pairs with __wake_up_axklogd:A.
		 */
		ret = wait_event_interruptible(ax_log_wait, ax_prb_read_valid(prb, atomic64_read(&user->seq), r));	/* LMM(ax_devkmsg_read:A) */
		if (ret)
			goto out;
	}

	if (r->info->seq != atomic64_read(&user->seq)) {
		/* our last seen message is gone, return error and reset */
		atomic64_set(&user->seq, r->info->seq);
		ret = -EPIPE;
		goto out;
	}

	len = ax_info_print_ext_header(user->buf, sizeof(user->buf), r->info);
	len += ax_msg_print_ext_body(user->buf + len, sizeof(user->buf) - len,
				  &r->text_buf[0], r->info->text_len,
				  &r->info->dev_info);

	atomic64_set(&user->seq, r->info->seq + 1);

	if (len > count) {
		ret = -EINVAL;
		goto out;
	}

	if (copy_to_user(buf, user->buf, len)) {
		ret = -EFAULT;
		goto out;
	}
	ret = len;
out:
	mutex_unlock(&user->lock);
	return ret;
}

static loff_t ax_devkmsg_llseek(struct file *file, loff_t offset, int whence)
{
	struct ax_devkmsg_user *user = file->private_data;
	loff_t ret = 0;

	if (!user)
		return -EBADF;
	if (offset)
		return -ESPIPE;

	switch (whence) {
	case SEEK_SET:
		/* the first record */
		atomic64_set(&user->seq, ax_prb_first_valid_seq(prb));
		break;
	case SEEK_DATA:
		/*
		 * The first record after the last SYSLOG_ACTION_CLEAR,
		 * like issued by 'dmesg -c'. Reading /dev/kmsg itself
		 * changes no global state, and does not clear anything.
		 */
		atomic64_set(&user->seq, ax_latched_seq_read_nolock(&clear_seq));
		break;
	case SEEK_END:
		/* after the last record */
		atomic64_set(&user->seq, ax_prb_next_seq(prb));
		break;
	default:
		ret = -EINVAL;
	}
	return ret;
}

static __poll_t ax_devkmsg_poll(struct file *file, poll_table * wait)
{
	struct ax_devkmsg_user *user = file->private_data;
	struct ax_printk_info info;
	__poll_t ret = 0;

	if (!user)
		return EPOLLERR | EPOLLNVAL;

	poll_wait(file, &ax_log_wait, wait);

	if (ax_prb_read_valid_info(prb, atomic64_read(&user->seq), &info, NULL)) {
		/* return error when data has vanished underneath us */
		if (info.seq != atomic64_read(&user->seq))
			ret = EPOLLIN | EPOLLRDNORM | EPOLLERR | EPOLLPRI;
		else
			ret = EPOLLIN | EPOLLRDNORM;
	}

	return ret;
}

static int ax_devkmsg_open(struct inode *inode, struct file *file)
{
	struct ax_devkmsg_user *user;
	int err;

	if (ax_devkmsg_log & DEVKMSG_LOG_MASK_OFF)
		return -EPERM;

	/* write-only does not need any file context */
	if ((file->f_flags & O_ACCMODE) != O_WRONLY) {
		err = ax_check_syslog_permissions(SYSLOG_ACTION_READ_ALL,
					       SYSLOG_FROM_READER);
		if (err)
			return err;
	}

	user = kmalloc(sizeof(struct ax_devkmsg_user), GFP_KERNEL);
	if (!user)
		return -ENOMEM;

	ratelimit_default_init(&user->rs);
	ratelimit_set_flags(&user->rs, RATELIMIT_MSG_ON_RELEASE);

	mutex_init(&user->lock);

	ax_prb_rec_init_rd(&user->record, &user->info,
			&user->text_buf[0], sizeof(user->text_buf));

	atomic64_set(&user->seq, ax_prb_first_valid_seq(prb));

	file->private_data = user;
	return 0;
}

static int ax_devkmsg_release(struct inode *inode, struct file *file)
{
	struct ax_devkmsg_user *user = file->private_data;

	if (!user)
		return 0;

	ratelimit_state_exit(&user->rs);

	mutex_destroy(&user->lock);
	kfree(user);
	return 0;
}

const struct file_operations ax_kmsg_fops = {
	.open = ax_devkmsg_open,
	.read = ax_devkmsg_read,
	.write_iter = ax_devkmsg_write,
	.llseek = ax_devkmsg_llseek,
	.poll = ax_devkmsg_poll,
	.release = ax_devkmsg_release,
};


/* requested log_buf_len from kernel cmdline */
static unsigned long __initdata ax_new_log_buf_len;

/* we practice scaling the ring buffer by powers of 2 */
static void __init ax_log_buf_len_update(u64 size)
{
	if (size > (u64) LOG_BUF_LEN_MAX) {
		size = (u64) LOG_BUF_LEN_MAX;
		pr_err("log_buf over 2G is not supported.\n");
	}

	if (size)
		size = roundup_pow_of_two(size);
	if (size > ax_log_buf_len)
		ax_new_log_buf_len = (unsigned long)size;
}

#ifdef CONFIG_SMP
#define __LOG_CPU_MAX_BUF_LEN (1 << CONFIG_LOG_CPU_MAX_BUF_SHIFT)

static void __init ax_log_buf_add_cpu(void)
{
	unsigned int cpu_extra;

	/*
	 * archs should set up cpu_possible_bits properly with
	 * set_cpu_possible() after setup_arch() but just in
	 * case lets ensure this is valid.
	 */
	if (num_possible_cpus() == 1)
		return;

	cpu_extra = (num_possible_cpus() - 1) * __LOG_CPU_MAX_BUF_LEN;

	/* by default this will only continue through for large > 64 CPUs */
	if (cpu_extra <= __LOG_BUF_LEN / 2)
		return;

	pr_info("ax_log_buf_len individual max cpu contribution: %d bytes\n",
		__LOG_CPU_MAX_BUF_LEN);
	pr_info("ax_log_buf_len total cpu_extra contributions: %d bytes\n",
		cpu_extra);
	pr_info("ax_log_buf_len min size: %d bytes\n", __LOG_BUF_LEN);

	ax_log_buf_len_update(cpu_extra + __LOG_BUF_LEN);
}
#else /* !CONFIG_SMP */
static inline void ax_log_buf_add_cpu(void)
{
}
#endif /* CONFIG_SMP */

static void __init ax_set_percpu_data_ready(void)
{
	__ax_printk_percpu_data_ready = true;
}

static unsigned int __init ax_add_to_rb(struct ax_printk_ringbuffer *rb,
				     struct ax_printk_record *r)
{
	struct ax_prb_reserved_entry e;
	struct ax_printk_record dest_r;

	ax_prb_rec_init_wr(&dest_r, r->info->text_len);

	if (!ax_prb_reserve(&e, rb, &dest_r))
		return 0;

	memcpy(&dest_r.text_buf[0], &r->text_buf[0], r->info->text_len);
	dest_r.info->text_len = r->info->text_len;
	dest_r.info->mid = r->info->mid;
	dest_r.info->level = r->info->level;
	dest_r.info->flags = r->info->flags;
	dest_r.info->ts_nsec = r->info->ts_nsec;
	dest_r.info->caller_id = r->info->caller_id;
	memcpy(&dest_r.info->dev_info, &r->info->dev_info,
	       sizeof(dest_r.info->dev_info));

	ax_prb_final_commit(&e);

	return ax_prb_record_text_space(&e);
}

static char ax_setup_text_buf[LOG_LINE_MAX] __initdata;

void __init ax_setup_log_buf(int early)
{
	struct ax_printk_info *new_infos;
	unsigned int new_descs_count;
	struct ax_prb_desc *new_descs;
	struct ax_printk_info info;
	struct ax_printk_record r;
	unsigned int text_size;
	size_t new_descs_size;
	size_t new_infos_size;
	unsigned long flags;
	char *new_log_buf;
	unsigned int free;
	u64 seq;

	/*
	 * Some archs call setup_log_buf() multiple times - first is very
	 * early, e.g. from setup_arch(), and second - when percpu_areas
	 * are initialised.
	 */
	if (!early)
		ax_set_percpu_data_ready();

	if (ax_log_buf != __ax_log_buf)
		return;

	if (!early && !ax_new_log_buf_len)
		ax_log_buf_add_cpu();

	if (!ax_new_log_buf_len)
		return;

	new_descs_count = ax_new_log_buf_len >> PRB_AVGBITS;
	if (new_descs_count == 0) {
		pr_err("ax_new_log_buf_len: %lu too small\n", ax_new_log_buf_len);
		return;
	}

	new_log_buf = memblock_alloc(ax_new_log_buf_len, LOG_ALIGN);
	if (unlikely(!new_log_buf)) {
		pr_err("log_buf_len: %lu text bytes not available\n",
			  ax_new_log_buf_len);
		return;
	}

	new_descs_size = new_descs_count * sizeof(struct ax_prb_desc);
	new_descs = memblock_alloc(new_descs_size, LOG_ALIGN);
	if (unlikely(!new_descs)) {
		pr_err("ax_log_buf_len: %zu desc bytes not available\n",
			  new_descs_size);
		goto err_free_log_buf;
	}

	new_infos_size = new_descs_count * sizeof(struct ax_printk_info);
	new_infos = memblock_alloc(new_infos_size, LOG_ALIGN);
	if (unlikely(!new_infos)) {
		pr_err("ax_log_buf_len: %zu info bytes not available\n",
			  new_infos_size);
		goto err_free_descs;
	}

	ax_prb_rec_init_rd(&r, &info, &ax_setup_text_buf[0], sizeof(ax_setup_text_buf));

	ax_prb_init(&ax_printk_rb_dynamic,
		    new_log_buf, ilog2(ax_new_log_buf_len),
		    new_descs, ilog2(new_descs_count), new_infos);

	local_irq_save(flags);

	ax_log_buf_len = ax_new_log_buf_len;
	ax_log_buf = new_log_buf;
	ax_new_log_buf_len = 0;

	free = __LOG_BUF_LEN;
	ax_prb_for_each_record(0, &ax_printk_rb_static, seq, &r) {
		text_size = ax_add_to_rb(&ax_printk_rb_dynamic, &r);
		if (text_size > free)
			free = 0;
		else
			free -= text_size;
	}

	prb = &ax_printk_rb_dynamic;

	local_irq_restore(flags);

	/*
	 * Copy any remaining messages that might have appeared from
	 * NMI context after copying but before switching to the
	 * dynamic buffer.
	 */
	ax_prb_for_each_record(seq, &ax_printk_rb_static, seq, &r) {
		text_size = ax_add_to_rb(&ax_printk_rb_dynamic, &r);
		if (text_size > free)
			free = 0;
		else
			free -= text_size;
	}

	if (seq != ax_prb_next_seq(&ax_printk_rb_static)) {
		pr_err("dropped %llu messages\n",
			  ax_prb_next_seq(&ax_printk_rb_static) - seq);
	}

	pr_info("ax_log_buf_len: %u bytes\n", ax_log_buf_len);
	pr_info("early log buf free: %u(%u%%)\n",
		   free, (free * 100) / __LOG_BUF_LEN);
	return;

err_free_descs:
	memblock_free_ptr(new_descs, new_descs_size);
err_free_log_buf:
	memblock_free_ptr(new_log_buf, ax_new_log_buf_len);
}

static bool ax_printk_time = IS_ENABLED(CONFIG_PRINTK_TIME);
module_param_named(time, ax_printk_time, bool, S_IRUGO | S_IWUSR);

static size_t ax_print_syslog(unsigned int level, char *buf)
{
	return sprintf(buf, "<%u>", level);
}

static size_t ax_print_time(u64 ts, char *buf)
{
	unsigned long rem_nsec = do_div(ts, 1000000000);

	return sprintf(buf, "[%5lu.%06lu]", (unsigned long)ts, rem_nsec / 1000);
}

static size_t ax_info_print_prefix(const struct ax_printk_info *info, bool syslog,
				bool time, char *buf)
{
	size_t len = 0;
	char *pBuf = NULL;

	if (syslog)
		len = ax_print_syslog((info->mid << 3) | info->level, buf);

	if (time) {
		len += ax_print_time(info->ts_nsec, buf + len);
	}
	/*[cpu][tag][level][tid]*/
	pBuf = buf + len;
	len += sprintf(pBuf, " [C%u][%s][%c][%u] ", info->cpu_id, info->tag, axlog_level_flags[info->level&7], info->caller_id);
	buf[len] = '\0';
	return len;
}

static size_t ax_record_print_text(struct ax_printk_record *r, bool syslog,
				bool time)
{
	size_t text_len = r->info->text_len;
	size_t buf_size = r->text_buf_size;
	char *text = r->text_buf;
	char prefix[PREFIX_MAX];
	bool truncated = false;
	size_t prefix_len;
	size_t line_len;
	size_t len = 0;
	char *next;

	/*
	 * If the message was truncated because the buffer was not large
	 * enough, treat the available text as if it were the full text.
	 */
	if (text_len > buf_size)
		text_len = buf_size;

	prefix_len = ax_info_print_prefix(r->info, syslog, time, prefix);

	/*
	 * @text_len: bytes of unprocessed text
	 * @line_len: bytes of current line _without_ newline
	 * @text:     pointer to beginning of current line
	 * @len:      number of bytes prepared in r->text_buf
	 */
	for (;;) {
		next = memchr(text, '\n', text_len);
		if (next) {
			line_len = next - text;
		} else {
			/* Drop truncated line(s). */
			if (truncated)
				break;
			line_len = text_len;
		}

		/*
		 * Truncate the text if there is not enough space to add the
		 * prefix and a trailing newline and a terminator.
		 */
		if (len + prefix_len + text_len + 1 + 1 > buf_size) {
			/* Drop even the current line if no space. */
			if (len + prefix_len + line_len + 1 + 1 > buf_size)
				break;

			text_len = buf_size - len - prefix_len - 1 - 1;
			truncated = true;
		}

		memmove(text + prefix_len, text, text_len);
		memcpy(text, prefix, prefix_len);

		/*
		 * Increment the prepared length to include the text and
		 * prefix that were just moved+copied. Also increment for the
		 * newline at the end of this line. If this is the last line,
		 * there is no newline, but it will be added immediately below.
		 */
		len += prefix_len + line_len + 1;
		if (text_len == line_len) {
			/*
			 * This is the last line. Add the trailing newline
			 * removed in vprintk_store().
			 */
			text[prefix_len + line_len] = '\n';
			break;
		}

		/*
		 * Advance beyond the added prefix and the related line with
		 * its newline.
		 */
		text += prefix_len + line_len + 1;

		/*
		 * The remaining text has only decreased by the line with its
		 * newline.
		 *
		 * Note that @text_len can become zero. It happens when @text
		 * ended with a newline (either due to truncation or the
		 * original string ending with "\n\n"). The loop is correctly
		 * repeated and (if not truncated) an empty line with a prefix
		 * will be prepared.
		 */
		text_len -= line_len + 1;
	}

	/*
	 * If a buffer was provided, it will be terminated. Space for the
	 * string terminator is guaranteed to be available. The terminator is
	 * not counted in the return value.
	 */
	if (buf_size > 0)
		r->text_buf[len] = 0;

	return len;
}

static size_t ax_get_record_print_text_size(struct ax_printk_info *info,
					 unsigned int line_count,
					 bool syslog, bool time)
{
	char prefix[PREFIX_MAX];
	size_t prefix_len;

	prefix_len = ax_info_print_prefix(info, syslog, time, prefix);

	/*
	 * Each line will be preceded with a prefix. The intermediate
	 * newlines are already within the text, but a final trailing
	 * newline will be added.
	 */
	return ((prefix_len * line_count) + info->text_len + 1);
}

static u64 ax_find_first_fitting_seq(u64 start_seq, u64 max_seq, size_t size,
				  bool syslog, bool time)
{
	struct ax_printk_info info;
	unsigned int line_count;
	size_t len = 0;
	u64 seq;

	/* Determine the size of the records up to @max_seq. */
	ax_prb_for_each_info(start_seq, prb, seq, &info, &line_count) {
		if (info.seq >= max_seq)
			break;
		len +=
		    ax_get_record_print_text_size(&info, line_count, syslog, time);
	}

	/*
	 * Adjust the upper bound for the next loop to avoid subtracting
	 * lengths that were never added.
	 */
	if (seq < max_seq)
		max_seq = seq;

	/*
	 * Move first record forward until length fits into the buffer. Ignore
	 * newest messages that were not counted in the above cycle. Messages
	 * might appear and get lost in the meantime. This is a best effort
	 * that prevents an infinite loop that could occur with a retry.
	 */
	ax_prb_for_each_info(start_seq, prb, seq, &info, &line_count) {
		if (len <= size || info.seq >= max_seq)
			break;
		len -=
		    ax_get_record_print_text_size(&info, line_count, syslog, time);
	}

	return seq;
}

static int ax_syslog_print(char __user * buf, int size)
{
	struct ax_printk_info info;
	struct ax_printk_record r;
	char *text;
	int len = 0;
	u64 seq;

	text = kmalloc(CONSOLE_LOG_MAX, GFP_KERNEL);
	if (!text)
		return -ENOMEM;

	ax_prb_rec_init_rd(&r, &info, text, CONSOLE_LOG_MAX);

	mutex_lock(&ax_syslog_lock);

	/*
	 * Wait for the @ax_syslog_seq record to be available. @ax_syslog_seq may
	 * change while waiting.
	 */
	do {
		seq = ax_syslog_seq;

		mutex_unlock(&ax_syslog_lock);
		/*
		 * Guarantee this task is visible on the waitqueue before
		 * checking the wake condition.
		 *
		 * The full memory barrier within set_current_state() of
		 * prepare_to_wait_event() pairs with the full memory barrier
		 * within wq_has_sleeper().
		 *
		 * This pairs with __wake_up_axklogd:A.
		 */
		len = wait_event_interruptible(ax_log_wait, ax_prb_read_valid(prb, seq, NULL));	/* LMM(ax_syslog_print:A) */
		mutex_lock(&ax_syslog_lock);

		if (len)
			goto out;
	} while (ax_syslog_seq != seq);

	/*
	 * Copy records that fit into the buffer. The above cycle makes sure
	 * that the first record is always available.
	 */
	do {
		size_t n;
		size_t skip;
		int err;

		if (!ax_prb_read_valid(prb, ax_syslog_seq, &r))
			break;

		if (r.info->seq != ax_syslog_seq) {
			/* message is gone, move to next valid one */
			ax_syslog_seq = r.info->seq;
			ax_syslog_partial = 0;
		}

		/*
		 * To keep reading/counting partial line consistent,
		 * use ax_printk_time value as of the beginning of a line.
		 */
		if (!ax_syslog_partial)
			ax_syslog_time = ax_printk_time;

		skip = ax_syslog_partial;
		n = ax_record_print_text(&r, true, ax_syslog_time);
		if (n - ax_syslog_partial <= size) {
			/* message fits into buffer, move forward */
			ax_syslog_seq = r.info->seq + 1;
			n -= ax_syslog_partial;
			ax_syslog_partial = 0;
		} else if (!len) {
			/* partial read(), remember position */
			n = size;
			ax_syslog_partial += n;
		} else
			n = 0;

		if (!n)
			break;

		mutex_unlock(&ax_syslog_lock);
		err = copy_to_user(buf, text + skip, n);
		mutex_lock(&ax_syslog_lock);

		if (err) {
			if (!len)
				len = -EFAULT;
			break;
		}

		len += n;
		size -= n;
		buf += n;
	} while (size);
out:
	mutex_unlock(&ax_syslog_lock);
	kfree(text);
	return len;
}

static int ax_syslog_print_all(char __user * buf, int size, bool clear)
{
	struct ax_printk_info info;
	struct ax_printk_record r;
	char *text;
	int len = 0;
	u64 seq;
	bool time;

	text = kmalloc(CONSOLE_LOG_MAX, GFP_KERNEL);
	if (!text)
		return -ENOMEM;

	time = ax_printk_time;
	/*
	 * Find first record that fits, including all following records,
	 * into the user-provided buffer for this dump.
	 */
	seq = ax_find_first_fitting_seq(ax_latched_seq_read_nolock(&clear_seq), -1,
				     size, true, time);

	ax_prb_rec_init_rd(&r, &info, text, CONSOLE_LOG_MAX);

	len = 0;
	ax_prb_for_each_record(seq, prb, seq, &r) {
		int textlen;

		textlen = ax_record_print_text(&r, true, time);

		if (len + textlen > size) {
			seq--;
			break;
		}

		if (copy_to_user(buf + len, text, textlen))
			len = -EFAULT;
		else
			len += textlen;

		if (len < 0)
			break;
	}

	if (clear) {
		mutex_lock(&ax_syslog_lock);
		ax_latched_seq_write(&clear_seq, seq);
		mutex_unlock(&ax_syslog_lock);
	}

	kfree(text);
	return len;
}

static void ax_syslog_clear(void)
{
	mutex_lock(&ax_syslog_lock);
	ax_latched_seq_write(&clear_seq, ax_prb_next_seq(prb));
	mutex_unlock(&ax_syslog_lock);
}

int do_axsyslog(int type, char __user * buf, int len, int source)
{
	struct ax_printk_info info;
	bool clear = false;
	int error;

	error = ax_check_syslog_permissions(type, source);
	if (error)
		return error;

	switch (type) {
	case SYSLOG_ACTION_CLOSE:	/* Close log */
		break;
	case SYSLOG_ACTION_OPEN:	/* Open log */
		break;
	case SYSLOG_ACTION_READ:	/* Read from log */
		if (!buf || len < 0)
			return -EINVAL;
		if (!len)
			return 0;
		if (!access_ok(buf, len))
			return -EFAULT;
		error = ax_syslog_print(buf, len);
		break;
		/* Read/clear last kernel messages */
	case SYSLOG_ACTION_READ_CLEAR:
		clear = true;
		fallthrough;
		/* Read last kernel messages */
	case SYSLOG_ACTION_READ_ALL:
		if (!buf || len < 0)
			return -EINVAL;
		if (!len)
			return 0;
		if (!access_ok(buf, len))
			return -EFAULT;
		error = ax_syslog_print_all(buf, len, clear);
		break;
		/* Clear ring buffer */
	case SYSLOG_ACTION_CLEAR:
		ax_syslog_clear();
		break;
		/* Number of chars in the log buffer */
	case SYSLOG_ACTION_SIZE_UNREAD:
		mutex_lock(&ax_syslog_lock);
		if (!ax_prb_read_valid_info(prb, ax_syslog_seq, &info, NULL)) {
			/* No unread messages. */
			mutex_unlock(&ax_syslog_lock);
			return 0;
		}
		if (info.seq != ax_syslog_seq) {
			/* messages are gone, move to first one */
			ax_syslog_seq = info.seq;
			ax_syslog_partial = 0;
		}
		if (source == SYSLOG_FROM_PROC) {
			/*
			 * Short-cut for poll(/"proc/kmsg") which simply checks
			 * for pending data, not the size; return the count of
			 * records, not the length.
			 */
			error = ax_prb_next_seq(prb) - ax_syslog_seq;
		} else {
			bool time = ax_syslog_partial ? ax_syslog_time : ax_printk_time;
			unsigned int line_count;
			u64 seq;

			ax_prb_for_each_info(ax_syslog_seq, prb, seq, &info,
					     &line_count) {
				error +=
				    ax_get_record_print_text_size(&info,
							       line_count, true,
							       time);
				time = ax_printk_time;
			}
			error -= ax_syslog_partial;
		}
		mutex_unlock(&ax_syslog_lock);
		break;
		/* Size of the log buffer */
	case SYSLOG_ACTION_SIZE_BUFFER:

		error = ax_log_buf_len;
		break;
	default:
		error = -EINVAL;
		break;
	}
	return error;
}

SYSCALL_DEFINE3(axsyslog, int, type, char __user *, buf, int, len)
{
	return do_axsyslog(type, buf, len, SYSLOG_FROM_READER);
}

SYSCALL_DEFINE0(ax_local_clock)
{
	return local_clock();
}

static DEFINE_PER_CPU(u8, ax_printk_count);
static u8 ax_printk_count_early;
#ifdef CONFIG_HAVE_NMI
static DEFINE_PER_CPU(u8, ax_printk_count_nmi);
static u8 ax_printk_count_nmi_early;
#endif


#define PRINTK_MAX_RECURSION 3

static u8 *__ax_printk_recursion_counter(void)
{
#ifdef CONFIG_HAVE_NMI
	if (in_nmi()) {
		if (ax_printk_percpu_data_ready())
			return this_cpu_ptr(&ax_printk_count_nmi);
		return &ax_printk_count_nmi_early;
	}
#endif
	if (ax_printk_percpu_data_ready())
		return this_cpu_ptr(&ax_printk_count);
	return &ax_printk_count_early;
}

#define ax_printk_enter_irqsave(recursion_ptr, flags)  \
({                          \
    bool success = true;                \
                            \
    typecheck(u8 *, recursion_ptr);         \
    local_irq_save(flags);              \
    (recursion_ptr) = __ax_printk_recursion_counter(); \
    if (*(recursion_ptr) > PRINTK_MAX_RECURSION) {  \
        local_irq_restore(flags);       \
        success = false;            \
    } else {                    \
        (*(recursion_ptr))++;           \
    }                       \
    success;                    \
})

/* Exit recursion tracking, restoring interrupts. */
#define ax_printk_exit_irqrestore(recursion_ptr, flags)    \
    do {                        \
        typecheck(u8 *, recursion_ptr);     \
        (*(recursion_ptr))--;           \
        local_irq_restore(flags);       \
    } while (0)

int ax_printk_delay_msec __read_mostly;

static inline u32 ax_printk_caller_id(void)
{
	return in_task()? task_pid_nr(current) : raw_smp_processor_id();
}

static inline u8 ax_printk_cpu_id(void)
{
	return raw_smp_processor_id();
}

static u16 ax_printk_sprint(char *text, u16 size, int mid,
			 enum axprintk_info_flags *flags, const char *fmt,
			 va_list args)
{
	u16 text_len;

	text_len = vscnprintf(text, size, fmt, args);

	/* Mark and strip a trailing newline. */
	if (text_len && text[text_len - 1] == '\n') {
		text_len--;
		*flags |= LOG_NEWLINE;
	}

	return text_len;
}

__printf(5, 0)
int ax_vprintk_store(const int id, const char *tag, const int level,
		     const struct dev_printk_info *dev_info,
		     const char *fmt, va_list args)
{
	const u32 caller_id = ax_printk_caller_id();
	const u8  cpu_id = ax_printk_cpu_id();
	struct ax_prb_reserved_entry e;  //need change
	enum axprintk_info_flags flags = 0;
	struct ax_printk_record r;       //need change
	unsigned long irqflags;
	u16 trunc_msg_len = 0;
	char prefix_buf[8];
	u8 *recursion_ptr;
	u16 reserve_size;
	va_list args2;
	u16 text_len;
	int ret = 0;
	u64 ts_nsec;

	/*
	 * Since the duration of printk() can vary depending on the message
	 * and state of the ringbuffer, grab the timestamp now so that it is
	 * close to the call of printk(). This provides a more deterministic
	 * timestamp with respect to the caller.
	 */
	ts_nsec = local_clock();

	if (!ax_printk_enter_irqsave(recursion_ptr, irqflags))
		return 0;

	/*
	 * The sprintf needs to come first since the syslog prefix might be
	 * passed in as a parameter. An extra byte must be reserved so that
	 * later the vscnprintf() into the reserved buffer has room for the
	 * terminating '\0', which is not counted by vsnprintf().
	 */
	va_copy(args2, args);
	reserve_size =
	    vsnprintf(&prefix_buf[0], sizeof(prefix_buf), fmt, args2) + 1;
	va_end(args2);

	if (reserve_size > LOG_LINE_MAX)
		reserve_size = LOG_LINE_MAX;

	if (dev_info)
		flags |= LOG_NEWLINE;

	/*
	 * Explicitly initialize the record before every ax_prb_reserve() call.
	 * ax_prb_reserve_in_last() and ax_prb_reserve() purposely invalidate the
	 * structure when they fail.
	 */
	ax_prb_rec_init_wr(&r, reserve_size);
	if (!ax_prb_reserve(&e, prb, &r)) {
		/* truncate the message if it is too long for empty buffer */
		ax_truncate_msg(&reserve_size, &trunc_msg_len);

		ax_prb_rec_init_wr(&r, reserve_size + trunc_msg_len);
		if (!ax_prb_reserve(&e, prb, &r))
			goto out;
	}

	/* fill message */
	text_len =
	    ax_printk_sprint(&r.text_buf[0], reserve_size, level, &flags, fmt,
			  args);

	if (trunc_msg_len)
		memcpy(&r.text_buf[text_len], trunc_msg, trunc_msg_len);
	r.info->text_len = text_len + trunc_msg_len;
	r.info->mid = id & 0xff;
	r.info->level = level & 7;
	r.info->flags = flags & 0x1f;
	r.info->ts_nsec = ts_nsec;
	r.info->caller_id = caller_id;
	r.info->cpu_id = cpu_id;
	if (tag)
		snprintf(r.info->tag, sizeof(r.info->tag), tag);
	if (dev_info)
		memcpy(&r.info->dev_info, dev_info, sizeof(r.info->dev_info));

	/* A message without a trailing newline can be continued. */
	if (!(flags & LOG_NEWLINE))
		ax_prb_commit(&e);
	else
		ax_prb_final_commit(&e);

	ret = text_len + trunc_msg_len;
out:
	ax_printk_exit_irqrestore(recursion_ptr, irqflags);
	return ret;
}

asmlinkage int ax_vprintk_emit(const int id, const char* tag, const int level,
			       const struct dev_printk_info *dev_info,
			       const char *fmt, va_list args)
{
	int printed_len;
	printed_len = ax_vprintk_store(id, tag, level, dev_info, fmt, args);

	/* If called from the scheduler, we can not call up(). */
	ax_wake_up_klogd();
	return printed_len;
}

EXPORT_SYMBOL(ax_vprintk_emit);

int ax_vprintk_default(const int id, const char* tag, const int level, const char *fmt, va_list args)
{
	return ax_vprintk_emit(id, tag, level, NULL, fmt, args);
}

EXPORT_SYMBOL_GPL(ax_vprintk_default);

asmlinkage __visible int _ax_printk(const int id, const char* tag, const int level, const char *fmt, ...)
{
	va_list args;
	int r;
	axlog_lvl_info_ptr loglvlptr = NULL;
	unsigned char max_id;
	unsigned char klog_state;

	if (s_axlogctl_cfg_ptr != NULL){
		max_id = s_axlogctl_cfg_ptr->max_id;
		if (id < 0 || id >= max_id) return 0;
		klog_state = s_axlogctl_cfg_ptr->klog_state;
		loglvlptr = (axlog_lvl_info_ptr)s_axlogctl_cfg_ptr->klvladdr;
		if(!klog_state || level > loglvlptr[id].klevel)
			return 0;
	}

	va_start(args, fmt);
	r = ax_vprintk(id, tag, level, fmt, args);
	va_end(args);

	return r;
}

EXPORT_SYMBOL(_ax_printk);

#define PRINTK_PENDING_WAKEUP	0x01
#define PRINTK_PENDING_OUTPUT	0x02

static DEFINE_PER_CPU(int, ax_printk_pending);

static void ax_wake_up_klogd_work_func(struct irq_work *irq_work)
{
	int pending = this_cpu_xchg(ax_printk_pending, 0);

	if (pending & PRINTK_PENDING_WAKEUP)
		wake_up_interruptible(&ax_log_wait);
}

static DEFINE_PER_CPU(struct irq_work, wake_up_ax_klogd_work) =
	IRQ_WORK_INIT_LAZY(ax_wake_up_klogd_work_func);

static void __wake_up_axklogd(int val)
{
	if (!ax_printk_percpu_data_ready())
		return;
	preempt_disable();
	/*
	 * Guarantee any new records can be seen by tasks preparing to wait
	 * before this context checks if the wait queue is empty.
	 *
	 * The full memory barrier within wq_has_sleeper() pairs with the full
	 * memory barrier within set_current_state() of
	 * prepare_to_wait_event(), which is called after ___wait_event() adds
	 * the waiter but before it has checked the wait condition.
	 *
	 * This pairs with ax_devkmsg_read:A and ax_syslog_print:A.
	 */
	if (wq_has_sleeper(&ax_log_wait) || /* LMM(__wake_up_axklogd:A) */
	    (val & PRINTK_PENDING_OUTPUT)) {
		this_cpu_or(ax_printk_pending, val);
		irq_work_queue(this_cpu_ptr(&wake_up_ax_klogd_work));
	}
	preempt_enable();
}

void ax_wake_up_klogd(void)
{
	__wake_up_axklogd(PRINTK_PENDING_WAKEUP);
}


#ifdef CONFIG_SMP
static atomic_t ax_printk_cpulock_owner = ATOMIC_INIT(-1);
static atomic_t ax_printk_cpulock_nested = ATOMIC_INIT(0);

/**
 * __printk_wait_on_cpu_lock() - Busy wait until the printk cpu-reentrant
 *                               spinning lock is not owned by any CPU.
 *
 * Context: Any context.
 */
void __ax_printk_wait_on_cpu_lock(void)
{
	do {
		cpu_relax();
	} while (atomic_read(&ax_printk_cpulock_owner) != -1);
}

EXPORT_SYMBOL(__ax_printk_wait_on_cpu_lock);

int __ax_printk_cpu_trylock(void)
{
	int cpu;
	int old;

	cpu = smp_processor_id();

	/*
	 * Guarantee loads and stores from this CPU when it is the lock owner
	 * are _not_ visible to the previous lock owner. This pairs with
	 * __printk_cpu_unlock:B.
	 *
	 * Memory barrier involvement:
	 *
	 * If __printk_cpu_trylock:A reads from __printk_cpu_unlock:B, then
	 * __printk_cpu_unlock:A can never read from __printk_cpu_trylock:B.
	 *
	 * Relies on:
	 *
	 * RELEASE from __printk_cpu_unlock:A to __printk_cpu_unlock:B
	 * of the previous CPU
	 *    matching
	 * ACQUIRE from __printk_cpu_trylock:A to __printk_cpu_trylock:B
	 * of this CPU
	 */
	old = atomic_cmpxchg_acquire(&ax_printk_cpulock_owner, -1, cpu);	/* LMM(__printk_cpu_trylock:A) */
	if (old == -1) {
		/*
		 * This CPU is now the owner and begins loading/storing
		 * data: LMM(__printk_cpu_trylock:B)
		 */
		return 1;

	} else if (old == cpu) {
		/* This CPU is already the owner. */
		atomic_inc(&ax_printk_cpulock_nested);
		return 1;
	}

	return 0;
}

EXPORT_SYMBOL(__ax_printk_cpu_trylock);

void __ax_printk_cpu_unlock(void)
{
	if (atomic_read(&ax_printk_cpulock_nested)) {
		atomic_dec(&ax_printk_cpulock_nested);
		return;
	}

	/*
	 * This CPU is finished loading/storing data:
	 * LMM(__printk_cpu_unlock:A)
	 *//*
	 * Guarantee loads and stores from this CPU when it was the
	 * lock owner are visible to the next lock owner. This pairs
	 * with __printk_cpu_trylock:A.
	 *
	 * Memory barrier involvement:
	 *
	 * If __printk_cpu_trylock:A reads from __printk_cpu_unlock:B,
	 * then __printk_cpu_trylock:B reads from __printk_cpu_unlock:A.
	 *
	 * Relies on:
	 *
	 * RELEASE from __printk_cpu_unlock:A to __printk_cpu_unlock:B
	 * of this CPU
	 *    matching
	 * ACQUIRE from __printk_cpu_trylock:A to __printk_cpu_trylock:B
	 * of the next CPU
	 */
	atomic_set_release(&ax_printk_cpulock_owner, -1);
	/* LMM(__printk_cpu_unlock:B) */
}

EXPORT_SYMBOL(__ax_printk_cpu_unlock);

static DEFINE_PER_CPU(int, ax_printk_context);

/* Can be preempted by NMI. */
void __ax_printk_safe_enter(void)
{
	this_cpu_inc(ax_printk_context);
}

/* Can be preempted by NMI. */
void __ax_printk_safe_exit(void)
{
	this_cpu_dec(ax_printk_context);
}

asmlinkage int ax_vprintk(const int id, const char* tag, const int level, const char *fmt, va_list args)
{
	/*
	 * Use the main logbuf even in NMI. But avoid calling console
	 * drivers that might have their own locks.
	 */
	if (this_cpu_read(ax_printk_context) || in_nmi()) {
		int len;

		len = ax_vprintk_store(id, tag, level, NULL, fmt, args);
		//defer_console_output();
		return len;
	}

	/* No obstacles. */
	return ax_vprintk_default(id, tag, level, fmt, args);
}

EXPORT_SYMBOL(ax_vprintk);


void ax_set_kloglvl_info(unsigned long viraddr)
{
	s_axlogctl_cfg_ptr = (axlogctl_mem_cfg_ptr) viraddr;
}
EXPORT_SYMBOL(ax_set_kloglvl_info);
#endif /* CONFIG_SMP */
