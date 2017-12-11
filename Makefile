CC      = arm-histbv310-linux-gcc
LD      = arm-histbv310-linux-ld
AR      = arm-histbv310-linux-ar
OBJCOPY = arm-histbv310-linux-objcopy
OBJDUMP = arm-histbv310-linux-objdump

CFLAGS 		:= -Wall -O2 -Iinclude
CPPFLAGS   	:= -nostdinc -nostdlib -fno-builtin

objs := start.o \
        early_irq.o \
        early_uart.o \
        tail.o \
        src/irq.o \
        src/serial_pl01x.o \
        src/main.o

fastboot.bin: $(objs)
	${LD} -Tfastboot.lds -o fastboot.elf $^
	${OBJCOPY} -O binary -S fastboot.elf $@
	${OBJDUMP} -D -m arm fastboot.elf > fastboot.dis
	
%.o:%.c
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o:%.S
	${CC} $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.bin *.elf *.dis
	find . -name *.o | xargs rm -f {}
	
