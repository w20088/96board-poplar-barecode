
#define STACK_TOP               0xFFFF0C00
#define CONFIG_BOOTHEAD_GAP     0x1000
#define CONFIG_GPIO_LIGHT_SET   0x29 @@@gpio5-1 5*8+1=0x29
#define REG_BASE_SCTL           0xF8000000
#define REG_SC_GEN1             0x0084
#define REG_SC_GEN14            0x00B8
#define REG_BASE_GPIO5          0xF8004000


#define CONFIG_AUXAREA_ADR_POS			0x214
#define CONFIG_AUXAREA_LEN_POS			0x218
#define CONFIG_BOOT_ENTRY_POS			0x21C
#define CONFIG_PARAM_AREA_POS			0x400
#define CONFIG_SCS_HASHED_AREA_OFF_POS		0x400
#define CONFIG_SCS_HASHED_AREA_LEN_POS		0x404
#define CONFIG_TOTAL_BOOT_AREA_LEN_POS		0x408
#define CONFIG_DEFAULT_BOOT_REG_POS		0x480
#define CONFIG_PARAM_AREA_SIG_POS		0x2EC0
#define CONFIG_SUPPORT_MULTI_PARAM_POS		0x2FE0
#define CONFIG_UNCHK_AREA_HISI_POS	0x2FC0
#define CONFIG_SCS_SIM_FLAG_POS		0x2FC0
#define CONFIG_BOOT_FLAG_POS		0x2FC4
#define CONFIG_AUX_ENC_FLAG_POS		0x2FC8
#define CONFIG_SUPPORT_MULTI_PARAM_POS	0x2FE0
#define CONFIG_PARAM_START_ADDR_POS	0x2FE4
#define CONFIG_PARAM_ITEM_LEN_POS	0x2FE8
#define CONFIG_BOOT_STORE_ADDR_POS	0x2FEC
#define CONFIG_AUXCODE_AREA_POS		0x3000

#define  CONFIG_SCS_SIM_FLAG_VAL	0x69875AB4
#define  CONFIG_BOOT_FLAG_VAL		0x435A590D
#define  CONFIG_AUX_ENC_FLAG_VAL	0x2A13C812
#define CONFIG_SCS_HASHED_AREA_OFF_VAL		0x100
#define  CONFIG_BOOT_STRORE_ADDR_VAL	0x01000000
#define CONFIG_AUXAREA_LEN	 0x3400
#define CONFIG_REG_SIZE		0x2000
#define CONFIG_UNCHK_SIZE	0x100

#define CONFIG_CHECKED_AREA_START		(CONFIG_AUXCODE_AREA_POS + CONFIG_AUXAREA_LEN + CONFIG_UNCHK_SIZE) @6500

.globl _start
_start:

/*
 * +-----------------------------------------+
 * | 1. Head Area (Key and Param)            |
 * +-----------------------------------------+
 */
	b	reset

/* 1.1 auxarea address */
.=CONFIG_AUXAREA_ADR_POS
	.word _auxcode_start

/* 1.2 auxarea length */
.=CONFIG_AUXAREA_LEN_POS
	.word CONFIG_AUXAREA_LEN

/* 1.3 boot entry point */
.=CONFIG_BOOT_ENTRY_POS
	.word _checked_area_start

/*
 * +-----------------------------------------+
 * | 2. Param Area                           |
 * +-----------------------------------------+
 */
.=CONFIG_PARAM_AREA_POS

/* 2.1 SCS Hashed area offset */
.=CONFIG_SCS_HASHED_AREA_OFF_POS
	.word CONFIG_SCS_HASHED_AREA_OFF_VAL

/* 2.2 SCS Hashed area length */
.=CONFIG_SCS_HASHED_AREA_LEN_POS
	.word _checked_area_end - _checked_area_start

/* 2.3 Total boot area length */
.=CONFIG_TOTAL_BOOT_AREA_LEN_POS
	.word _total_boot_area_end - _total_boot_area_start

/* 2.4 Default boot reg */
.=CONFIG_DEFAULT_BOOT_REG_POS
	.incbin	"bin/BOOT_0.reg"

/* 2.5 Param signature */
.=CONFIG_PARAM_AREA_SIG_POS
	.fill 0x100,1,0

/*
 * +-----------------------------------------+
 * | 3. Unchecked area for hisi              |
 * +-----------------------------------------+
 */
.=CONFIG_UNCHK_AREA_HISI_POS
/* 3.1 SCS simulation flag */
.=CONFIG_SCS_SIM_FLAG_POS
	.word CONFIG_SCS_SIM_FLAG_VAL

/* 3.2 Boot flag */
.=CONFIG_BOOT_FLAG_POS
	.word CONFIG_BOOT_FLAG_VAL

/* 3.3 auxcode encrypt flag */
.=CONFIG_AUX_ENC_FLAG_POS
	.word CONFIG_AUX_ENC_FLAG_VAL

/* 3.4 Support multi param */
.=CONFIG_SUPPORT_MULTI_PARAM_POS

/* 3.5 param start address */
.=CONFIG_PARAM_START_ADDR_POS
	.word reg_list_start

/* 3.6 param item length */
.=CONFIG_PARAM_ITEM_LEN_POS
	.word CONFIG_REG_SIZE

/* 3.7 Boot store address */
.=CONFIG_BOOT_STORE_ADDR_POS
	.word CONFIG_BOOT_STRORE_ADDR_VAL

/*
 * +-----------------------------------------+
 * | 4. Auxiliary Code Area                  |
 * +-----------------------------------------+
 */
 .=CONFIG_AUXCODE_AREA_POS
_auxcode_start:
	.incbin   "bin/AUXCODE.img"
.align	8 /* align to 0x100 */
_auxcode_end:
 .=CONFIG_AUXCODE_AREA_POS + CONFIG_AUXAREA_LEN

/*
 * +-----------------------------------------+
 * | 5. Unchecked Area                       |
 * +-----------------------------------------+
 */
 _total_boot_area_start:
_unchecked_area:
	.fill CONFIG_UNCHK_SIZE,1,0

/*
 * +-----------------------------------------+
 * | 6. Checked Area (fastboot)              |
 * +-----------------------------------------+
 */

.=CONFIG_CHECKED_AREA_START
_checked_area_start:
	b	reset

.=_checked_area_start+0x40

.balignl 16,0xdeadbeef
__start:
	.word _start

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word __bss_end

.globl __checked_area_start
__checked_area_start:
	.word _checked_area_start

/*
 * the actual reset code
 */
reset:
	isb

	/*
	 *  read and write system register REG_SC_GEN1
	 *  judge BUS mux whether or not OK
	 */
	ldr	r0, =REG_BASE_SCTL
	mov	r1, #0
wait_mux_finish:
	add	r1, r1, #1
	str	r1, [r0, #REG_SC_GEN1]
	ldr	r2, [r0, #REG_SC_GEN1]
	cmp	r1, r2
	bne	wait_mux_finish


normal_start_flow:
	/* set the cpu to SVC32 mode */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f  /* r0 = ((~0x1F) & r0) */
	orr	r0, r0, #0xd3  /* r0 = (0xd3 | r0)    */
	msr	cpsr, r0

	mrc     p15, 0, r1, c1, c0, 1
	orr	r1, r1, #0x3
	mcr	p15, 0, r1, c1, c0, 1

	cps	#0x16

	mrc     p15, 0, r1, c1, c1, 0
	orr	r1, r1, #0x1
	mcr     p15, 0, r1, c1, c1, 0

	mrc     p15, 4, r1, c1, c0, 1
	orr	r1, r1, #0x3
	mcr     p15, 4, r1, c1, c0, 1

	mrc     p15, 0, r1, c1, c1, 0
	bic	r1, r1, #0x1
	mcr     p15, 0, r1, c1, c1, 0

	cps	#0x13

	/* Enable SMP mode for CPU0, by setting bit 6 of CPU Extend Ctl reg */
	mrrc	p15,1,r0,r1,c15
	orr	r0, r0, #(1 << 6)
	mcrr	p15,1,r0,r1,c15

	bl	flash_cache_all

	ldr	sp, =STACK_TOP

	/* enable I-Cache now */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #0x00001000     /* set bit 12 (I) I-Cache */
	mcr	p15, 0, r0, c1, c0, 0
	isb	@ add ISB or I-cache enabled is  not efficient right now


	/* Clear BSS (if any). Is below tx (watch load addr - need space) */
clear_bss:
	ldr	r0, _bss_start      @ find start of bss segment
	ldr	r1, _bss_end        @ stop here
	mov	r2, #0x00000000     @ clear value


clear_bss_loop:
	str	r2, [r0]        @ clear BSS location
	cmp	r0, r1          @ are we at the end yet
	add	r0, r0, #4      @ increment clear index pointer
	bne	clear_bss_loop  @ keep clearing till at end

	mov	r0, #0

	/* Data and Instruction sync and invalidate I-cache */
	dsb
	isb

	ldr lr, =halt
	ldr pc, =main	
halt:
	b halt

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@
@  corrupt: r0, r1, r2, r3
@
.align  2
flash_cache_all:

        /* disable MMU stuff and caches */
        mrc     p15, 0, r0, c1, c0, 0
        orr     r0, r0, #0x00002000      /* clear bits 13 (--V-)   */
        bic     r0, r0, #0x00000007      /* clear bits 2:0 (-CAM)  */
        orr     r0, r0, #0x00000002      /* set bit 1 (--A-) Align */
        orr     r0, r0, #0x00000800      /* set bit 12 (Z---) BTB  */
        mcr     p15, 0, r0, c1, c0, 0

        /*
         * Invalidate L1 I/D
         */
        mov     r0, #0                   /* set up for MCR */
        mcr     p15, 0, r0, c8, c7, 0    /* invalidate TLBs */
        mcr     p15, 0, r0, c7, c5, 0    /* invalidate icache */

        /* Invalidate L1 D-cache */
        mcr     p15, 2, r0, c0, c0, 0    /* select L1 data cache */
        mrc     p15, 1, r3, c0, c0, 0    /* Read Current Cache Size Identification Register */
        ldr     r1, =0x1ff
        and     r3, r1, r3, LSR #13      /* r3 = (number of sets -1) */
        mov     r0, #0
way_loop:
        mov     r1, #0                  /* r1->set counter */
line_loop:
        mov     r2, r0, LSL #30
        orr     r2, r1, LSL #5           /* r2->set/way cache-op format */
        mcr     p15, 0, r2, c7, c6, 2    /* Invalidate line described by r2 */
        add     r1, r1, #1               /* Increment set counter */
        cmp     r1, r3                   /* Check if the last set is reached... */
        ble     line_loop                /* if not, continue the set_loop */
        add     r0, r0, #1               /* else, Increment way counter */
        cmp     r0, #4                   /* Check if the last way is reached... */
        blt     way_loop                 /* if not, continue the way_loop */

        mov     pc, lr

.align	2
.globl _text_end
_text_end: .word __text_end



