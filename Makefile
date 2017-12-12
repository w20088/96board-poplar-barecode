CC      = arm-histbv310-linux-gcc
LD      = arm-histbv310-linux-ld
AR      = arm-histbv310-linux-ar
OBJCOPY = arm-histbv310-linux-objcopy
OBJDUMP = arm-histbv310-linux-objdump

CFLAGS          := -Wall -O2 -Iinclude
CPPFLAGS        := -nostdinc -nostdlib -fno-builtin
LDFLAGS         :=

objs := start.o \
        uart.o \
        src/uart_p101x.o \
        src/console.o \
        src/compiler/udivsi3.o \
        src/compiler/aeabi_uidivmod.o \
        src/compiler/udivmodsi4.o \
        src/compiler/divsi3.o \
        src/compiler/aeabi_idivmod.o \
        src/compiler/divmodsi4.o \
        src/compiler/aeabi_uldivmod.o \
        src/compiler/udivmoddi4.o \
        src/compiler/64bit.o \
        src/libc/crc32.o \
        src/libc/util.o \
        src/libc/strtoul.o \
        src/libc/strncpy.o \
        src/libc/strtok.o \
        src/libc/rand.o \
        src/libc/crc16.o \
        src/libc/memmove.o \
        src/libc/printf.o \
        src/libc/strlen.o \
        src/libc/memcmp.o \
        src/main.o \
        tail.o

fastboot.bin: $(objs)
	${LD} $(LDFLAGS) -Tfastboot.lds -o fastboot.elf $^
	${OBJCOPY} -O binary -S fastboot.elf $@
	${OBJDUMP} -D -m arm fastboot.elf > fastboot.dis
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.bin *.elf *.dis
	find . -name *.o | xargs rm -f {}
	
