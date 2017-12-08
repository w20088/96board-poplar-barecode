
#define GPIO4_DIR      (*(volatile unsigned long *)0xF8B24400)
#define GPIO5_DIR      (*(volatile unsigned long *)0xF8004400)
#define GPIO6_DIR      (*(volatile unsigned long *)0xF8B26400) 
#define GPIO10_DIR      (*(volatile unsigned long *)0xF8B2A400)

#define GPIO6_3_DATA      (*(volatile unsigned long *)(0xF8B26000 + (4L<<3))) //LED D17
#define GPIO5_1_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<1))) //LED D16
#define GPIO5_2_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<2))) //LED D18
#define GPIO10_6_DATA      (*(volatile unsigned long *)(0xF8B2A000 + (4L<<6))) //LED D19
#define GPIO5_3_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<3))) //LED D15 blue for bluetooth
#define GPIO4_6_DATA      (*(volatile unsigned long *)(0xF8B24000 + (4L<<6))) //LED D20 yellow for wifi

extern void display_c_code_run(void);

void  wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}


int main()
{
    display_c_code_run();
    GPIO6_DIR = GPIO6_DIR | (1L<<3);    // 设置gpio6-3为输出口    
    GPIO5_DIR = GPIO5_DIR | (1L<<1);    // 设置gpio5-1为输出口
    GPIO5_DIR = GPIO5_DIR | (1L<<2);    // 设置gpio5-2为输出口
    GPIO10_DIR = GPIO10_DIR | (1L<<6);  // 设置gpio10-6为输出口
    GPIO5_DIR = GPIO5_DIR | (1L<<3);    // 设置gpio5-3为输出口
    GPIO4_DIR = GPIO4_DIR | (1L<<6);    // 设置gpio4-6为输出口
    
    while(1)
    {
        GPIO6_3_DATA = (GPIO6_3_DATA | (1L<<3));    //gpio6-3输出1，LED D17灭
        GPIO5_1_DATA = (GPIO5_1_DATA | (1L<<1));    // gpio5-1输出1，LED D16灭
        GPIO5_2_DATA = (GPIO5_2_DATA | (1L<<2));    // gpio5-1输出1，LED D18灭
        GPIO10_6_DATA = (GPIO10_6_DATA | (1L<<6));    // gpio5-1输出1，LED D19灭
        GPIO5_3_DATA =  (GPIO5_3_DATA | (1L<<3));    // gpio5-1输出1，LED D15灭
        GPIO4_6_DATA =  (GPIO4_6_DATA | (1L<<6));    // gpio5-1输出1，LED D20灭
 
        wait(100000000);
 
        GPIO6_3_DATA = GPIO6_3_DATA & (~(1L<<3));    //gpio6-3输出0，LED D17亮
        GPIO5_1_DATA = GPIO5_1_DATA & (~(1L<<1));    // gpio5-1输出0，LED D16点亮
        GPIO5_2_DATA = GPIO5_2_DATA & (~(1L<<2));    // gpio5-1输出0，LED D18点亮
        GPIO10_6_DATA = GPIO10_6_DATA & (~(1L<<6));    // gpio5-1输出0，LED D19点亮
        GPIO5_3_DATA =  GPIO5_3_DATA & (~(1L<<3));    // gpio5-1输出0，LED D15点亮
        GPIO4_6_DATA =  GPIO4_6_DATA & (~(1L<<6));    // gpio5-1输出0，LED D20点亮
        wait(100000000);
    }
    return 0;
}