@/******************************************************************************
@ *    Copyright (c) 2009-2016 by Hisi.
@ *    All rights reserved.
@ * ***
@ *    Create by Lyj. 2014-05-13
@ *
@******************************************************************************/

#define CONFIG_REG_SIZE				(0x2000)

.section .tail,#alloc

_checked_area_reserve:
	.fill 0x10,1,0

.align	8 /* align to 0x100 */
.globl _checked_area_end
_checked_area_end:

/*
 * +-----------------------------------------+
 * | 8. Boot Signature                       |
 * +-----------------------------------------+
 */
_boot_sign_area:
	.fill 0x100,1,0

.globl _total_boot_area_end
 _total_boot_area_end:
 
/*
 * +-----------------------------------------+
 * | 9. Reg list                             |
 * +-----------------------------------------+
 */
.align	9 /* align to 0x200 */
.globl reg_list_start
reg_list_start:
_boot_reg0:
boot_reg0:
	.incbin "bin/BOOT_0.reg"
.= boot_reg0 + CONFIG_REG_SIZE

boot_reg1:
	.incbin	"bin/BOOT_1.reg"
.= boot_reg1 + CONFIG_REG_SIZE

boot_reg2:
	.incbin "bin/BOOT_2.reg"
.= boot_reg2 + CONFIG_REG_SIZE
