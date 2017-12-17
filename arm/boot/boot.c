/******************************************************************************
 *    Copyright (C) 2014 Hisilicon STB Development Dept
 *    All rights reserved.
 * ***
 *    Create by Cai Zhiyong
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *   http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
******************************************************************************/

#include <early_uart.h>
#include <asm/io.h>
#include <boot.h>
#include <irq.h>
#include <stdio.h>
#include <string.h>
#include <trace.h>
#include <cpuinfo.h>


#define TIMERx_LOAD                                        0x00
#define TIMERx_VALUE                                       0x04
#define TIMERx_CONTROL                                     0x08
#define TIMERx_CONTROL_SIZE                     (1 << 1)
#define TIMERx_CONFROL_256PRE                   (2 << 2)
#define TIMERx_CONTROL_INTR_EN                  (1 << 5)
#define TIMERx_CONTROL_CYC                      (1 << 6)
#define TIMERx_CONTROL_EN                       (1 << 7)
#define TIMERx_INTR_CLR                                    0x0c

#define GPIO5_DIR      (*(volatile unsigned long *)0xF8004400)
#define GPIO5_1_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<1))) //LED D16
#define GPIO5_3_DATA      (*(volatile unsigned long *)(0xF8004000 + (4L<<3))) //LED D15 blue for bluetooth

struct timer_ctrl_t {
	int init;
	uint32 base;
	int irqnr;    /* irq id */
	uint32 hz;    /* ticks percent usec */
	uint32 cycle; /* timer source cycle */
};

static struct timer_ctrl_t __timer_ctrl[8] = {
	{
		.init = 0,
		.base = TIMER0_BASE,
		.irqnr = TIMER0_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER1_BASE,
		.irqnr = TIMER1_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER2_BASE,
		.irqnr = TIMER2_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER3_BASE,
		.irqnr = TIMER3_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER4_BASE,
		.irqnr = TIMER4_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER5_BASE,
		.irqnr = TIMER5_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER6_BASE,
		.irqnr = TIMER6_IRQ_NR,
	}, {
		.init = 0,
		.base = TIMER7_BASE,
		.irqnr = TIMER7_IRQ_NR,
	}
};

#define GET_TIMER_CTRL(_index)   (&__timer_ctrl[_index])

#define TM_READ(_tmr, _a)              readl(_a + (_tmr)->base)
#define TM_WRITE(_tmr, _v, _a)         writel(_v, (_tmr)->base + _a)

#define FIVE_SECONDS               5000

static BOOL isLedOn = TRUE;

static void clock_source_irq_handle(void *arg)
{
	struct timer_ctrl_t *tmr = (struct timer_ctrl_t *)arg;
	TM_WRITE(tmr, 1, TIMERx_INTR_CLR);
        printf("timer irq:%d happened!\n", tmr->irqnr);

        if(isLedOn) {
            GPIO5_1_DATA = (GPIO5_1_DATA | (1L<<1));    // gpio5-1输出1，LED D16灭
            isLedOn = FALSE;
            printf("Led D16 off\n");
        } else
        {
            GPIO5_1_DATA = GPIO5_1_DATA & (~(1L<<1));    // gpio5-1输出0，LED D16点亮
            isLedOn = TRUE;
            printf("Led D16 on\n");
        }
}

void clock_source_init(int timer, int msec)
{
	struct timer_ctrl_t *tmr = GET_TIMER_CTRL(timer);

	tmr->hz = OSC_FREQ / 1000000;
	tmr->cycle = msec * tmr->hz * 1000;
	tmr->init = 1;

	TM_WRITE(tmr, 0, TIMERx_CONTROL);
	TM_WRITE(tmr, tmr->cycle, TIMERx_LOAD);
	TM_WRITE(tmr, 1, TIMERx_INTR_CLR);
	TM_WRITE(tmr,
		 (TIMERx_CONTROL_SIZE | TIMERx_CONTROL_CYC
		  | TIMERx_CONTROL_INTR_EN | TIMERx_CONTROL_EN),
		 TIMERx_CONTROL);

	irq_connect(tmr->irqnr, clock_source_irq_handle, (void *)tmr);
}

#define     GPIO_0_ADDR 0xF8B20000
#define     GPIO_1_ADDR 0xF8B21000
#define     GPIO_2_ADDR 0xF8B22000
#define     GPIO_3_ADDR 0xF8B23000
#define     GPIO_4_ADDR 0xF8B24000
#define     GPIO_5_ADDR 0xF8004000
#define     GPIO_6_ADDR 0xF8B26000
#define     GPIO_7_ADDR 0xF8B27000
#define     GPIO_8_ADDR 0xF8B28000
#define     GPIO_9_ADDR 0xF8B29000
#define     GPIO_10_ADDR 0xF8B2A000
#define     GPIO_11_ADDR 0xF8B2B000
#define     GPIO_12_ADDR 0xF8B2C000

static unsigned int  g_GpioPhyAddr[13]={GPIO_0_ADDR,GPIO_1_ADDR,GPIO_2_ADDR,GPIO_3_ADDR,GPIO_4_ADDR,\
                                                    GPIO_5_ADDR,GPIO_6_ADDR,GPIO_7_ADDR,GPIO_8_ADDR,GPIO_9_ADDR,\
                                                    GPIO_10_ADDR,GPIO_11_ADDR,GPIO_12_ADDR};
static int gpio_irq_num = 0;
static void gpio_source_irq_handle(void *arg)
{
        int *irqNo = (int *)arg;
        unsigned int GpioUsrAddr = g_GpioPhyAddr[*irqNo - 108];
        (*(volatile unsigned long *)(GpioUsrAddr + 0x41C)) = 0xff; //clear int bit
        printf("gpio irq:%d happened! address:0x%x\n", *irqNo, GpioUsrAddr);
        if(isLedOn) {
            GPIO5_3_DATA =  (GPIO5_3_DATA | (1L<<3));    // gpio5-1输出1，LED D15灭
            isLedOn = FALSE;
            printf("Led D15 off\n");
        } else
        {
            GPIO5_3_DATA =  GPIO5_3_DATA & (~(1L<<3));    // gpio5-1输出0，LED D15点亮
            isLedOn = TRUE;
            printf("Led D15 on\n");
        }
}

void gpio_source_init(int gpioNo)
{
    unsigned int GpioUsrAddr;
    int groupNo=0;
    int bitNo=0;
    unsigned long GpioValue;

    groupNo = gpioNo / 8;
    bitNo = gpioNo % 8;
    GpioUsrAddr = g_GpioPhyAddr[groupNo];
    printf("gpio %d address: 0x%x\n", gpioNo, GpioUsrAddr);
    (*(volatile unsigned long *)(GpioUsrAddr + 0x41C)) = 0xff; //clear int bit
    GpioValue = (*(volatile unsigned long *)(GpioUsrAddr + 0x404));
    GpioValue &= (~(1 << bitNo)); //edge triger
    (*(volatile unsigned long *)(GpioUsrAddr + 0x404)) = GpioValue;
    GpioValue = (*(volatile unsigned long *)(GpioUsrAddr + 0x40C));
    GpioValue |= (1 << bitNo);   // up edge trigger
    (*(volatile unsigned long *)(GpioUsrAddr + 0x40C)) = GpioValue;
    GpioValue = (*(volatile unsigned long *)(GpioUsrAddr + 0x410));
    GpioValue |= (1 << bitNo); //interrupt enable
    (*(volatile unsigned long *)(GpioUsrAddr + 0x410)) = GpioValue;
    gpio_irq_num = 108 + groupNo;
    irq_connect(gpio_irq_num, gpio_source_irq_handle, (void *)&gpio_irq_num);
}


int arm_start(void)
{

    int c;

    GPIO5_DIR = GPIO5_DIR | (1L<<1);    // 设置gpio5-1为输出口
    GPIO5_DIR = GPIO5_DIR | (1L<<3);    // 设置gpio5-3为输出口
#ifdef CONFIG_UART
    uart_init(NULL);
#endif

    irq_init();
    clock_source_init(CONFIG_CLOCK_SOURCE, FIVE_SECONDS);
    gpio_source_init(31); //gpio3-7
    cpu_enable_irq();

    printf("Hello World! %s\n", "The first printf");
    while(1)
    {
        c = getc();
        if (c == '\r')
        {
            putc('\n');
        }
        putc(c);
    }

	return 0;
}
