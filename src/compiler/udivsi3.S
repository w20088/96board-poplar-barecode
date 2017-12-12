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
 * Come from : LLVM compiler-rt-master/lib/arm/udivsi3.S
 * URL : https://github.com/llvm-mirror/compiler-rt/archive/master.zip
******************************************************************************/

/*===-- udivsi3.S - 32-bit unsigned integer divide ------------------------===//
 *
 *                     The LLVM Compiler Infrastructure
 *
 * This file is dual licensed under the MIT and the University of Illinois Open
 * Source Licenses. See LICENSE.TXT for details.
 *
 *===----------------------------------------------------------------------===//
 *
 * This file implements the __udivsi3 (32-bit unsigned integer divide) 
 * function for the ARM architecture.  A naive digit-by-digit computation is
 * employed for simplicity.
 *
 *===----------------------------------------------------------------------===*/

#include "assembly.h"

#define ESTABLISH_FRAME \
    push   {r7, lr}    ;\
    mov     r7,     sp
#define CLEAR_FRAME_AND_RETURN \
    pop    {r7, pc}

#define a r0
#define b r1
#define r r2
#define i r3
#define q ip
#define one lr

.syntax unified
.align 3
DEFINE_AEABI_FUNCTION_ALIAS(__aeabi_uidiv, __udivsi3)
DEFINE_COMPILERRT_FUNCTION(__udivsi3)
    ESTABLISH_FRAME
    clz     r2,     a
    tst     b,      b   // detect divide-by-zero
    clz     r3,     b
    mov     q,      #0
    beq     LOCAL_LABEL(return)    // return 0 if b is zero.
    mov     one,    #1
    subs    i,      r3, r2
    blt     LOCAL_LABEL(return)    // return 0 if MSB(a) < MSB(b)

LOCAL_LABEL(mainLoop):
    subs    r,      a,  b, lsl i
    orrhs   q,      q,one, lsl i
    movhs   a,      r
    subsne  i,      i, #1
    bhi     LOCAL_LABEL(mainLoop)

    subs    r,      a,  b
    orrhs   q,      #1

LOCAL_LABEL(return):
    mov     r0,     q
    CLEAR_FRAME_AND_RETURN
