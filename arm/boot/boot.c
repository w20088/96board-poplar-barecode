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
#include <stdio.h>
#include <string.h>
#include <trace.h>
#include <cpuinfo.h>

int arm_start(void)
{

    int c;
    early_printf( "\r\n\r\nminiboot %d.%d.%d (%s@%s) (%s %s)\r\n\r\n",
        VERSION, PATCHLEVEL, SUBLEVEL,
            USER, HOSTNAME, __TIME__, __DATE__);


#ifdef CONFIG_UART
    uart_init(NULL);
#endif

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
