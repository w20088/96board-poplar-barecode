extern int serial_init (void);
extern void serial_puts (const char *s);
extern void serial_putc (const char c);
extern int serial_getc();

int main()
{
    int c;

    serial_init();
    serial_puts("This is printed by serial_puts:\r\n\0");

    while(1)
    {
        // 从串口接收数据后，输出
        c = serial_getc();
        serial_putc((char)c);
    }

    return 0;
}

