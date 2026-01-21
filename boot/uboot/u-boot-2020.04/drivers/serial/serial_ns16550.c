// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2000
 * Rob Taylor, Flying Pig Systems. robt@flyingpig.com.
 */

#include <common.h>
#include <clock_legacy.h>
#include <ns16550.h>
#include <serial.h>
#include <linux/compiler.h>

#ifndef CONFIG_NS16550_MIN_FUNCTIONS

DECLARE_GLOBAL_DATA_PTR;

#if !defined(CONFIG_CONS_INDEX)
#elif (CONFIG_CONS_INDEX < 1) || (CONFIG_CONS_INDEX > 6)
#error	"Invalid console index value."
#endif

#if CONFIG_CONS_INDEX == 1 && !defined(CONFIG_SYS_NS16550_COM1)
#error	"Console port 1 defined but not configured."
#elif CONFIG_CONS_INDEX == 2 && !defined(CONFIG_SYS_NS16550_COM2)
#error	"Console port 2 defined but not configured."
#elif CONFIG_CONS_INDEX == 3 && !defined(CONFIG_SYS_NS16550_COM3)
#error	"Console port 3 defined but not configured."
#elif CONFIG_CONS_INDEX == 4 && !defined(CONFIG_SYS_NS16550_COM4)
#error	"Console port 4 defined but not configured."
#elif CONFIG_CONS_INDEX == 5 && !defined(CONFIG_SYS_NS16550_COM5)
#error	"Console port 5 defined but not configured."
#elif CONFIG_CONS_INDEX == 6 && !defined(CONFIG_SYS_NS16550_COM6)
#error	"Console port 6 defined but not configured."
#endif

/* Note: The port number specified in the functions is 1 based.
 *	 the array is 0 based.
 */
static NS16550_t serial_ports[6] = {
#ifdef CONFIG_SYS_NS16550_COM1
	(NS16550_t)CONFIG_SYS_NS16550_COM1,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM2
	(NS16550_t)CONFIG_SYS_NS16550_COM2,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM3
	(NS16550_t)CONFIG_SYS_NS16550_COM3,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM4
	(NS16550_t)CONFIG_SYS_NS16550_COM4,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM5
	(NS16550_t)CONFIG_SYS_NS16550_COM5,
#else
	NULL,
#endif
#ifdef CONFIG_SYS_NS16550_COM6
	(NS16550_t)CONFIG_SYS_NS16550_COM6
#else
	NULL
#endif
};

#define PORT	serial_ports[port-1]

/* Multi serial device functions */
#define DECLARE_ESERIAL_FUNCTIONS(port) \
	static int  eserial##port##_init(void) \
	{ \
		int clock_divisor; \
		clock_divisor = ns16550_calc_divisor(serial_ports[port-1], \
				CONFIG_SYS_NS16550_CLK, gd->baudrate); \
		NS16550_init(serial_ports[port-1], clock_divisor); \
		return 0 ; \
	} \
	static void eserial##port##_setbrg(void) \
	{ \
		serial_setbrg_dev(port); \
	} \
	static int  eserial##port##_getc(void) \
	{ \
		return serial_getc_dev(port); \
	} \
	static int  eserial##port##_tstc(void) \
	{ \
		return serial_tstc_dev(port); \
	} \
	static void eserial##port##_putc(const char c) \
	{ \
		serial_putc_dev(port, c); \
	} \
	static void eserial##port##_puts(const char *s) \
	{ \
		serial_puts_dev(port, s); \
	}

/* Serial device descriptor */
#define INIT_ESERIAL_STRUCTURE(port, __name) {	\
	.name	= __name,			\
	.start	= eserial##port##_init,		\
	.stop	= NULL,				\
	.setbrg	= eserial##port##_setbrg,	\
	.getc	= eserial##port##_getc,		\
	.tstc	= eserial##port##_tstc,		\
	.putc	= eserial##port##_putc,		\
	.puts	= eserial##port##_puts,		\
}

static void _serial_putc(const char c, const int port)
{
	if (c == '\n')
		NS16550_putc(PORT, '\r');

	NS16550_putc(PORT, c);
}

static void _serial_puts(const char *s, const int port)
{
	while (*s) {
		_serial_putc(*s++, port);
	}
}

static int _serial_getc(const int port)
{
	return NS16550_getc(PORT);
}

static int _serial_tstc(const int port)
{
	return NS16550_tstc(PORT);
}

static void _serial_setbrg(const int port)
{
	int clock_divisor;

	clock_divisor = ns16550_calc_divisor(PORT, CONFIG_SYS_NS16550_CLK,
					     gd->baudrate);
	NS16550_reinit(PORT, clock_divisor);
}

static inline void
serial_putc_dev(unsigned int dev_index,const char c)
{
	_serial_putc(c,dev_index);
}

static inline void
serial_puts_dev(unsigned int dev_index,const char *s)
{
	_serial_puts(s,dev_index);
}

static inline int
serial_getc_dev(unsigned int dev_index)
{
	return _serial_getc(dev_index);
}

static inline int
serial_tstc_dev(unsigned int dev_index)
{
	return _serial_tstc(dev_index);
}

static inline void
serial_setbrg_dev(unsigned int dev_index)
{
	_serial_setbrg(dev_index);
}

#if defined(CONFIG_SYS_NS16550_COM1)
DECLARE_ESERIAL_FUNCTIONS(1);
struct serial_device eserial1_device =
	INIT_ESERIAL_STRUCTURE(1, "eserial0");
#endif
#if defined(CONFIG_SYS_NS16550_COM2)
DECLARE_ESERIAL_FUNCTIONS(2);
struct serial_device eserial2_device =
	INIT_ESERIAL_STRUCTURE(2, "eserial1");
#endif
#if defined(CONFIG_SYS_NS16550_COM3)
DECLARE_ESERIAL_FUNCTIONS(3);
struct serial_device eserial3_device =
	INIT_ESERIAL_STRUCTURE(3, "eserial2");
#endif
#if defined(CONFIG_SYS_NS16550_COM4)
DECLARE_ESERIAL_FUNCTIONS(4);
struct serial_device eserial4_device =
	INIT_ESERIAL_STRUCTURE(4, "eserial3");
#endif
#if defined(CONFIG_SYS_NS16550_COM5)
DECLARE_ESERIAL_FUNCTIONS(5);
struct serial_device eserial5_device =
	INIT_ESERIAL_STRUCTURE(5, "eserial4");
#endif
#if defined(CONFIG_SYS_NS16550_COM6)
DECLARE_ESERIAL_FUNCTIONS(6);
struct serial_device eserial6_device =
	INIT_ESERIAL_STRUCTURE(6, "eserial5");
#endif

__weak struct serial_device *default_serial_console(void)
{
#if CONFIG_CONS_INDEX == 1
	return &eserial1_device;
#elif CONFIG_CONS_INDEX == 2
	return &eserial2_device;
#elif CONFIG_CONS_INDEX == 3
	return &eserial3_device;
#elif CONFIG_CONS_INDEX == 4
	return &eserial4_device;
#elif CONFIG_CONS_INDEX == 5
	return &eserial5_device;
#elif CONFIG_CONS_INDEX == 6
	return &eserial6_device;
#else
#error "Bad CONFIG_CONS_INDEX."
#endif
}

void ns16550_serial_initialize(void)
{
#if defined(CONFIG_SYS_NS16550_COM1)
	serial_register(&eserial1_device);
#endif
#if defined(CONFIG_SYS_NS16550_COM2)
	serial_register(&eserial2_device);
#endif
#if defined(CONFIG_SYS_NS16550_COM3)
	serial_register(&eserial3_device);
#endif
#if defined(CONFIG_SYS_NS16550_COM4)
	serial_register(&eserial4_device);
#endif
#if defined(CONFIG_SYS_NS16550_COM5)
	serial_register(&eserial5_device);
#endif
#if defined(CONFIG_SYS_NS16550_COM6)
	serial_register(&eserial6_device);
#endif
}

#endif /* !CONFIG_NS16550_MIN_FUNCTIONS */

struct serial_device* get_uart_device_by_num(int uart_num)
{
    struct serial_device *uart = NULL;

    switch (uart_num) {
        case 1:
			uart = &eserial1_device;
			break;
		case 2:
			uart = &eserial2_device;
			break;
		case 3:
			uart = &eserial3_device;
			break;
		default:
			printf("Invalid UART number: %d\n", uart_num);
			return NULL;
	}
	return uart;
}

static u8 uart_getchar(int uart_num)
{
	int ret;
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);

	if (uart_dev == NULL) {
		printf("Failed to get UART device for UART number: %d\n", uart_num);
		return -1; // or some other error code
	}
	while (!uart_dev->tstc());

	return uart_dev->getc();
}

static int uart_open(int uart_num)
{
	int ret;
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);

	if (uart_dev == NULL) {
		printf("Failed to get UART device for UART number: %d\n", uart_num);
		return -1; // or some other error code
	}

	ret = uart_dev->start();
	return ret;
}

static int uart_read(int uart_num, u8 *buf, u32 len)
{
	u32 recv_count = 0;
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);
#if UART_DEBUG
	u32 debug_counter = 0;
#endif

	if (uart_dev == NULL) {
		printf("Failed to get UART device for UART number: %d\n", uart_num);
		return -1;
    }

	while (recv_count < len) {
		if (uart_dev->tstc()) {
			buf[recv_count++] = uart_dev->getc();
		}

#if UART_DEBUG
		// Debugging information
		udelay(1000); // Delay for 1ms
		debug_counter++;
		if (debug_counter >= 1000) { // Every 1 second
			printf("Debug: recv_count=%d,len=%d, so far...\n", recv_count, len);
			debug_counter = 0;
		}
#endif
	}

	return recv_count;
}

static u32 uart_write(int uart_num, const u8 *buf, u32 len)
{
	int ret;
	u8 ch;
	u32 wr_count = 0;
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);

	if (uart_dev == NULL) {
		printf("Failed to get UART device for UART number: %d\n", uart_num);
		return -1;
	}

	while (wr_count < len) {
		ch = *buf++;
		uart_dev->putc(ch);
		wr_count++;
	}

	return wr_count;
}

static int uart_putchar(int uart_num, const u8 ch)
{
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);
	uart_dev->putc(ch);

	return 1;
}

static int uart_setbaudrate(int uart_num, u32 baudrate)
{
	struct serial_device *uart_dev = get_uart_device_by_num(uart_num);
	u32 baudrate_temp = gd->baudrate;
	gd->baudrate = baudrate;
	printf("switch baud rate to  %d\n",baudrate);
	uart_dev->setbrg();
	gd->baudrate = baudrate_temp;
	return 0;
}

int do_uart_test(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int uart_num;
	u32 bytes_written;
	const char *msg = "Hello, U-Boot!\n";

	if (argc != 2){
		printf("Usage: %s <uart_num>\n", argv[0]);
		return CMD_RET_USAGE;
	}
	uart_num = simple_strtoul(argv[1], NULL, 10);
	// Open UART
	if (uart_open(uart_num) != 0) {
		printf("Failed to open UART %d\n", uart_num);
		return CMD_RET_FAILURE;
	}

	// Send "hello-world"
	bytes_written = uart_write(uart_num, (const u8 *)msg, strlen(msg));
	if (bytes_written != strlen(msg)) {
		printf("Failed to send complete message\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}
U_BOOT_CMD(
	uart_test,    // command name
	3,            // maxargs - maximum number of arguments
	1,            // repeatable - if the command can be repeated
	do_uart_test, // command function
	"Test UART communication", // description
	"<uart_num> - Send and receive data via UART\n" // usage - shows how to use the command
);
