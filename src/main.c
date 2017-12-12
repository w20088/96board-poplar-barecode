#include <stdio.h>

extern void uart_init(void *ptr);
extern int uart_getc(void);
extern void uart_putc(int);
int main()
{
    int c;
    uart_init(NULL);
    printf("Hello World! %s\n", "The first printf");
    while(1)
    {
        // 从串口接收数据后，输出
        c = getc();
        if (c == '\r')
        {
            putc('\n');
        }
        putc(c);
    }

    return 0;
}

