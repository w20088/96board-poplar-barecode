#if 0
#define GPIO5_DIR      (*(volatile unsigned long *)0xF8004400)
#define GPIO6_DIR      (*(volatile unsigned long *)0xF8B26400) 

#define GPIO6_3_DATA      (*(volatile unsigned long *)(0xF8B26000 + (4L<<3))) //LED D17
#define GPIO5_1_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<1))) //LED D16
#endif

#if 1
#define GPIO5_DIR      (*(volatile unsigned long *)0xB8004400)
#define GPIO6_DIR      (*(volatile unsigned long *)0xB8B26400)

#define GPIO6_3_DATA      (*(volatile unsigned long *)(0xB8B26000 + (4L<<3))) //LED D17
#define GPIO5_1_DATA      (*(volatile unsigned long *)(0xB8004000 + (4L<<1))) //LED D16
#endif

extern void display_c_code_run(void);
extern void uart_early_put_hex(int hex);
extern char __page_table_start[];
extern char __page_table_end[];

static inline void wait(volatile unsigned long dly)
{
	for(; dly > 0; dly--);
}

/*
 * 设置页表
 */
void create_page_table(unsigned int pgt_offset)
{
#define PAGE_RW           (3 << 10)
#define PAGE_DOMAIN       (0 << 5)
#define PAGE_SECTION      (2)
#define PAGE_XN           (1 << 4)
#define SECTION_COUNT     4096 
#define SECTION_SIZE      (0x100000)

    int ix = 0;
    unsigned int addr = 0;
    unsigned int *ptable = (unsigned int *)pgt_offset;
    unsigned int  attrib = PAGE_RW | PAGE_DOMAIN | PAGE_SECTION | PAGE_XN;

/*初始化段TLB，使得虚拟地址和物理地址一一映射*/    

    while (ix++ < SECTION_COUNT) {
        *ptable++ = addr | attrib;
        addr += SECTION_SIZE;
    }

/*****************************************************************
00100000: 00000c12 00100c12 00200c12 00300c12    .......... ...0.
00100010: 00400c12 00500c12 00600c12 00700c12    ..@...P...`...p.
00100020: 00800c12 00900c12 00a00c12 00b00c12    ................
00100030: 00c00c12 00d00c12 00e00c12 00f00c12    ................
00100040: 01000c12 01100c12 01200c12 01300c12    .......... ...0.
00100050: 01400c12 01500c12 01600c12 01700c12    ..@...P...`...p.
00100060: 01800c12 01900c12 01a00c12 01b00c12    ................
00100070: 01c00c12 01d00c12 01e00c12 01f00c12    ................
......
00102e00: b8000c12 b8100c12 b8200c12 b8300c12    .......... ...0.
00102e10: b8400c12 b8500c12 b8600c12 b8700c12    ..@...P...`...p.
00102e20: b8800c12 b8900c12 b8a00c12 b8b00c12    ................
00102e30: b8c00c12 b8d00c12 b8e00c12 b8f00c12    ................
......
*****************************************************************/

/*
将0xB8000000开始的1M虚拟地址空间映射到物理地址0xF8000000开始的1M地址空间
将0xB8B00000开始的1M虚拟地址空间映射到物理地址0xF8B00000开始的1M地址空间 
*/
    ptable = (unsigned int *)(pgt_offset + (0xB8000000 >> 20)*4);
    addr = 0xF8000000;
    *ptable = addr | attrib;

    ptable = (unsigned int *)(pgt_offset + (0xB8B00000 >> 20)*4);
    addr = 0xF8B00000;
    *ptable = addr | attrib;

/*特殊映射后的TLB*/
/*****************************************************************
......
00102e00: f8000c12 b8100c12 b8200c12 b8300c12    .......... ...0.
00102e10: b8400c12 b8500c12 b8600c12 b8700c12    ..@...P...`...p.
00102e20: b8800c12 b8900c12 b8a00c12 f8b00c12    ................
00102e30: b8c00c12 b8d00c12 b8e00c12 b8f00c12    ................
......
*****************************************************************/
}


/*
 * 启动MMU
 */
void mmu_init(unsigned int pgt_offset)
{
    uart_early_put_hex(pgt_offset);
__asm__(

	/* set page table */
        "mov r0, %0\n"
	"mcr	p15, 0, r0, c2, c0, 0\n" /* r0 = 页表基址 */
	"mov	r0, #-1\n"
	"mcr	p15, 0, r0, c3, c0, 0\n" /* 域访问控制寄存器设为0xFFFFFFFF，
                                         * 不进行权限检查
                                         */
	/* enable mmu dcache rr */
	"mrc	p15, 0, r0, c1, c0\n" /* 读出控制寄存器的值 */

        /* 控制寄存器的低16位含义为：.RVI ..RS B... .CAM
         * R : 表示换出Cache中的条目时使用的算法，
         *     0 = Random replacement；1 = Round robin replacement
         * V : 表示异常向量表所在的位置，
         *     0 = Low addresses = 0x00000000；1 = High addresses = 0xFFFF0000
         * I : 0 = 关闭ICaches；1 = 开启ICaches
         * R、S : 用来与页表中的描述符一起确定内存的访问权限
         * B : 0 = CPU为小字节序；1 = CPU为大字节序
         * C : 0 = 关闭DCaches；1 = 开启DCaches
         * A : 0 = 数据访问时不进行地址对齐检查；1 = 数据访问时进行地址对齐检查
         * M : 0 = 关闭MMU；1 = 开启MMU
         */

        "orr    r0, r0, #0x0002\n"          /* .... .... .... ..1. 开启对齐检查 */
        "orr    r0, r0, #0x0004\n"          /* .... .... .... .1.. 开启DCaches */
        "orr    r0, r0, #0x1000\n"          /* ...1 .... .... .... 开启ICaches */
        "orr    r0, r0, #0x0001\n"          /* .... .... .... ...1 使能MMU */

	"mcr	p15, 0, r0, c1, c0\n"

	"mov	r1, #0\n"
	
	"mcr     p15, 0, r1, c8, c7, 0\n"   /* 使无效指令、数据TLB */
	"mcr     p15, 0, r1, c7, c5, 0\n"   /* invalidate ICache */
	"mcr     p15, 0, r1, c7, c5, 4\n"   /* ISB */
        : /* 无输出 */
        : "r" (pgt_offset) );
}

int main()
{
    unsigned int page_table_start = (unsigned int)__page_table_start;
    display_c_code_run();
    create_page_table(page_table_start);
    mmu_init(page_table_start);
    GPIO6_DIR = GPIO6_DIR | (1L<<3);    // 设置gpio6-3为输出口    
    GPIO5_DIR = GPIO5_DIR | (1L<<1);    // 设置gpio5-1为输出口
   
    while(1)
    {
        GPIO6_3_DATA = (GPIO6_3_DATA | (1L<<3));    //gpio6-3输出1，LED D17灭
        GPIO5_1_DATA = (GPIO5_1_DATA | (1L<<1));    // gpio5-1输出1，LED D16灭
        wait(100000000);
        GPIO6_3_DATA = GPIO6_3_DATA & (~(1L<<3));    //gpio6-3输出0，LED D17亮
        GPIO5_1_DATA = GPIO5_1_DATA & (~(1L<<1));    // gpio5-1输出0，LED D16点亮
        wait(100000000);
    }
    return 0;
}
