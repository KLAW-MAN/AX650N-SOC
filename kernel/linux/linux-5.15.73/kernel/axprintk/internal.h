/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * internal.h - axprintk internal definitions
 */
#include <linux/percpu.h>

#ifdef CONFIG_PRINTK

/* Flags for a single printk record. */
enum axprintk_info_flags {
	LOG_NEWLINE	= 2,	/* text ended with a newline */
	LOG_CONT	= 8,	/* text is a fragment of a continuation line */
};

__printf(5, 0)
int ax_vprintk_store(const int id, const char *tag, const int level,
		     const struct dev_printk_info *dev_info,
		     const char *fmt, va_list args);


int ax_vprintk_default(const int id, const char* tag, const int level, const char *fmt, va_list args);

__printf(1, 0) int ax_vprintk_deferred(const char *fmt, va_list args);

bool ax_printk_percpu_data_ready(void);

void ax_defer_console_output(void);

u16 ax_printk_parse_prefix(const char *text, int *level,
			   enum axprintk_info_flags *flags);

#endif /* CONFIG_PRINTK */
