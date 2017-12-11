/* This file is auto export, DO NOT modify. */
#ifndef _CONFIGH_
#define _CONFIGH_

#include "platform.h"


#define ARCH                                      "armv7-a"
#define BOOT_REG0                                 "bin/BOOT_0.reg"
#define CONFIG_ARCH_HI3798CV2X                    y
#define CONFIG_AUXIMAGE                           "bin/AUXCODE.img"
#define CONFIG_BOOTARGS_1G                        "mem=1G mmz=ddr,0,0,420M"
#define CONFIG_BOOTARGS_2G                        "mem=2G mmz=ddr,0,0,580M"
#define CONFIG_BOOTARGS_512M                      "mem=512M mmz=ddr,0,0,215M"
#define CONFIG_BOOTARGS_MERGE                     y
#define CONFIG_BOOTHEAD_GAP                       0
#define CONFIG_BOOTROM_SUPPORT                    y
#define CONFIG_CACHE_ARMV7                        y
#define CONFIG_CMDLINE                            y
#define CONFIG_CPU_HI3798CV2X                     y
#define CONFIG_DDR_TRAINING_V2                    y
#define CONFIG_DEBUG                              y
#define CONFIG_DEFAULT_BOOTCMD                    ""
#define CONFIG_ENV_FROM                           0x80000
#define CONFIG_ENV_RANGE                          0x20000
#define CONFIG_ENV_SIZE                           0x10000
#define CONFIG_EXT4_SPARSE                        y
#define CONFIG_GENERIC_MMC                        y
#define CONFIG_GENERIC_NAND                       y
#define CONFIG_GENERIC_SFC                        y
#define CONFIG_GENERIC_SPI_NAND                   y
#define CONFIG_GPIO_LIGHT_SET                     0x29
#define CONFIG_HIMCI_V200                         1
#define CONFIG_MMC_BOOT_ADDR                      0
#define CONFIG_MMC_POWER_OFF_TIMEOUT              50
#define CONFIG_MMC_POWER_ON_TIMEROUT              50
#define CONFIG_MMC_RESET_HIGH_TIMEROUT            300
#define CONFIG_MMC_RESET_LOW_TIMEOUT              10
#define CONFIG_NAND_MAX_CHIP                      1
#define CONFIG_PRINT                              y
#define CONFIG_RESERVE_MEM_GAP                    0x100000
#define CONFIG_RESERVE_MEMORY_DEBUG               y
#define CONFIG_RESERVE_MEMORY_GAP                 0x8000000
#define CONFIG_RESERVE_MEMORY_SIZE                0x8000000
#define CONFIG_SHOW_REG_INFO                      y
#define CONFIG_START_TIMEOUT                      2
#define CONFIG_UART_P101X                         y
#define CONFIG_UART                               y
#define CONFIG_UNCMOP_MAX_SIZE                    0x100000
#define CONFIG_UNCOMP_ADDR                        0x1100000
#define CONFIG_UNIFIED_BOOT                       y
#define CONFIG_VERIFY_KERNEL                      y
#define PATCHLEVEL                                0
#define PLATFORM                                  hi3798cv2x
#define SUBLEVEL                                  0
#define TEXT_BASE                                 0x00C00000
#define V                                         1
#define VERSION                                   1
#define HOSTNAME                                  "ubuntu"
#define USER                                      "weberfeng"
#endif

