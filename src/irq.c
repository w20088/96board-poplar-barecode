/******************************************************************************
 *    Copyright (C) 2014 Hisilicon STB Development Dept
 *    All rights reserved.
 * ***
 *    Create by Czyong
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

//#include <stdio.h>
//#include <boot.h>

/*****************************************************************************/
extern void uart_early_puts(const char * s);

void do_undefined_instruction (void)
{
	uart_early_puts("undefined instruction\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_software_interrupt (void)
{
	uart_early_puts("software interrupt\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_prefetch_abort (void)
{
	uart_early_puts("prefetch abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_data_abort (void)
{
	uart_early_puts("data abort\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_not_used (void)
{
	uart_early_puts("not used\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_fiq (void)
{
	uart_early_puts("fast interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}

void do_irq (void)
{
	uart_early_puts("interrupt request\r\n");
	uart_early_puts("Resetting CPU ...\r\n");
	//reset_cpu (0);
}
