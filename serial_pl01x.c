#include "serial_pl01x.h"

#define IO_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (val))
#define IO_READ(addr) (*(volatile unsigned int *)(addr))

/*
 * Integrator AP has two UARTs, we use the first one, at 38400-8-N-1
 * Integrator CP has two UARTs, use the first one, at 38400-8-N-1
 * Versatile PB has four UARTs.
 */

#define PL011_CLOCK            75000000
#define REG_BASE_UART0         0xF8B00000
#define baudRate               115200

static void pl01x_putc (char c);
static int pl01x_getc (void);
static int pl01x_tstc (void);

int serial_init (void)
{
	unsigned int temp;
	unsigned int divider;
	unsigned int remainder;
	unsigned int fraction;

	/*
	 ** First, disable everything.
	 */
	IO_WRITE (REG_BASE_UART0 + UART_PL011_CR, 0x0);

	/*
	 ** Set baud rate
	 **
	 ** IBRD = UART_CLK / (16 * BAUD_RATE)
	 ** FBRD = ROUND((64 * MOD(UART_CLK,(16 * BAUD_RATE))) / (16 * BAUD_RATE))
	 */
	temp = 16 * baudRate;
	divider = PL011_CLOCK / temp;
	remainder = PL011_CLOCK % temp;
	temp = (8 * remainder) / baudRate;
	fraction = (temp >> 1) + (temp & 1);

	IO_WRITE (REG_BASE_UART0 + UART_PL011_IBRD, divider);
	IO_WRITE (REG_BASE_UART0 + UART_PL011_FBRD, fraction);

	/*
	 ** Set the UART to be 8 bits, 1 stop bit, no parity, fifo enabled.
	 */
	IO_WRITE (REG_BASE_UART0 + UART_PL011_LCRH,
		  (UART_PL011_LCRH_WLEN_8 | UART_PL011_LCRH_FEN));

	/*
	 ** Finally, enable the UART
	 */
	IO_WRITE (REG_BASE_UART0 + UART_PL011_CR,
		  (UART_PL011_CR_UARTEN | UART_PL011_CR_TXE |
		   UART_PL011_CR_RXE));

	return 0;
}

void serial_putc (const char c)
{
	if (c == '\r')
		pl01x_putc ('\n');
	pl01x_putc (c);
}

void serial_putc_raw(const char c)
{
	pl01x_putc (c);
}

void serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

int serial_getc (void)
{
	return pl01x_getc ();
}

int serial_tstc (void)
{
	return pl01x_tstc ();
}

void serial_setbrg (void)
{
}


static void pl01x_putc (char c)
{
	/* Wait until there is space in the FIFO */
	while (IO_READ (REG_BASE_UART0 + UART_PL01x_FR) & UART_PL01x_FR_TXFF);

	/* Send the character */
	IO_WRITE (REG_BASE_UART0 + UART_PL01x_DR, c);
}

static int pl01x_getc (void)
{
	unsigned int data=0;

	/* Wait until there is data in the FIFO */
	while (IO_READ (REG_BASE_UART0 + UART_PL01x_FR) & UART_PL01x_FR_RXFE);

	data = IO_READ (REG_BASE_UART0 + UART_PL01x_DR);

	/* Check for an error flag */
	if (data & 0xFFFFFF00) {
		/* Clear the error */
		IO_WRITE (REG_BASE_UART0 + UART_PL01x_ECR, 0xFFFFFFFF);
		return -1;
	}

	return (int) data;
}

static int pl01x_tstc (void)
{
	return !(IO_READ (REG_BASE_UART0 + UART_PL01x_FR) &
		 UART_PL01x_FR_RXFE);
}
