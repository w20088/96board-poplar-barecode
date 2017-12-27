# 96board-poplar-bare-led

这是在96board poplar 单板上的裸机层序。
poplar单板是基于华为Hi2798CV200CPU设计的机顶盒开发主板，可以从下面的链接购买：
https://item.taobao.com/item.htm?spm=a1z10.1-c.w13619360-16022635001.8.77020cc8Q0MXAG&id=537581702687

led_on branch演示用汇编程序点亮D16灯

led_on_c branch演示用c程序点亮板上的6个LED灯，并使其闪烁

uart branch演示用c语言接收和发送串口输入和输出

printf branch 演示格式化打印输出

timer branch演示如何计时

irq branch演示如何设置中断向量以及实现了定时器中断和GPIO中断

emmc branch演示如何对EMMC进行读写操作，并将boot.img读入内存

miniboot 演示如何读取环境变量及引导andorid linux kernel

编译工具：
wget https://releases.linaro.org/components/toolchain/binaries/7.1-2017.08/arm-linux-gnueabi/gcc-linaro-7.1.1-2017.08-x86_64_arm-linux-gnueabi.tar.xz

编译方法：make
运行方法：将fastboot.bin拷贝到U盘根目录，按住USB_BOOT键后上电程序就运行了。


1.	启动模式

芯片上电后，就执行复位后，CPU跳转到CPU内部的BOOTROM去执行。BOOTROM检测到USB_BOOT引脚是低电平时就从USB口读取fastboot.bin文件来启动系统；否则，系统将读取eMMC uboot分区到DDR内存后启动系统。

BOOTROM映射的地址为：
0xFFFE_0000 0xFFFE_FFFF BOOTROM

2．启动代码组织结构：

It complies with platform's BootROM requirements to support booting the Poplar board. This is achieved via encoding values as well as binaries at pre-defined locations in the program file.
Requirements:
	BootROM shall receive the PLL/DDR/regulator settings from start-up code
	BootROM shall receive the size of the binary to boot as well as the expected RAM location to boot it from.

The basic platform initialization is encoded in a number of pre-compiled binaries included at certain locations in the start-up code

AUXCODE.img
BOOT_[0..2].reg
The location (i.e., offset relative to the base of the TEXT segment) of the AUXCODE is defined by the address stored at offset CONFIG_AUXCODE_AREA_POS.
The location of BOOT_[0..2].reg blocks are defined by the address stored at offset CONFIG_PARAM_START_ADDR_POS.

Once the BootROM has executed the AUXCODE initialization binary, it loads the rest of the file in DDR for execution.

The location in DDR where the binary shall be placed is written at CONFIG_BOOT_STORE_ADDR_POS and its value defined by CONFIG_BOOT_STORE_ADDR_VAL. （当使用Hitool下载fastboot.bin，fastboot.bin代码将被下载到CONFIG_BOOT_STRORE_ADDR_VAL地址，然后跳转到CONFIG_BOOT_STRORE_ADDR_VAL+0x6500处执行。）
bigfish/sdk/source/boot/fastboot/include/configs/hi3798cv2x.h:
#define  CONFIG_BOOT_STRORE_ADDR_VAL   0x01000000

当从USB或EMMC启动时，从.=CONFIG_CHECKED_AREA_START开始的代码将被拷贝到CONFIG_BOOT_ENTRY_POS定义的内存，然后开始执行。
.=CONFIG_BOOT_ENTRY_POS
-       .word _checked_area_start

_checked_area_start的值依赖于链接地址-Ttext 0x00c00000

bigfish/sdk/source/boot/fastboot/board/hi3798cv2x/config.mk
#image should be loaded at 0x01000000
-TEXT_BASE = 0x00c00000

_checked_area_start = 0x000C6500

The size of the binary that shall be copied to DDR needs to be placed at CONFIG_TOTAL_BOOT_AREA_LEN_POS.

ROM then jumps to the offset specified in CONFIG_BOOT_ENTRY_POS, which contains the instruction to branch to.


#define CONFIG_AUXAREA_ADR_POS			(0x214)
#define CONFIG_AUXAREA_LEN_POS			(0x218)
#define CONFIG_BOOT_ENTRY_POS			(0x21C)
#define CONFIG_SCS_HASHED_AREA_LEN_POS		(0x404)
#define CONFIG_TOTAL_BOOT_AREA_LEN_POS		(0x408)
#define CONFIG_DEFAULT_BOOT_REG_POS		(0x480)
#define CONFIG_PARAM_AREA_POS			(0x400)
#define CONFIG_SCS_HASHED_AREA_OFF_POS		(0x400)
#define CONFIG_PARAM_AREA_SIG_POS		(0x2EC0)
#define CONFIG_SUPPORT_MULTI_PARAM_POS		(0x2FE0)
#define CONFIG_AUXCODE_AREA_POS			(0x3000)
#define CONFIG_CHECKED_AREA_START		(0x6500)

#define CONFIG_UNCHK_SIZE			(0x100)
#define CONFIG_SCS_HASHED_AREA_OFF_VAL		(0x100)
#define CONFIG_AUXAREA_LEN			(0x3400)

#define CONFIG_BOOT_FLAG_POS			(0x2FC4)
#define CONFIG_BOOT_FLAG_VAL			(0x435A590D)

#define CONFIG_AUX_ENC_FLAG_POS			(0x2FC8)
#define CONFIG_AUX_ENC_FLAG_VAL			(0x2A13C812)

#define CONFIG_PARAM_START_ADDR_POS		(0x2FE4)
#define CONFIG_PARAM_ITEM_LEN_POS		(0x2FE8)

#define CONFIG_BOOT_STORE_ADDR_POS		(0x2FEC)
#define CONFIG_BOOT_STORE_ADDR_VAL		(LLOADER_TEXT_BASE)

#define CONFIG_REG_SIZE				(0x2000)

