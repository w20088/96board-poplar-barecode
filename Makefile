
CC      = arm-histbv310-linux-gcc
LD      = arm-histbv310-linux-ld
AR      = arm-histbv310-linux-ar
OBJCOPY = arm-histbv310-linux-objcopy
OBJDUMP = arm-histbv310-linux-objdump

CFLAGS 		:= -Wall -O2 
CPPFLAGS   	:= -nostdinc -nostdlib -fno-builtin

objs := start.o uart.o tail.o

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
	
