CROSS_COMPILE ?= arm-linux-gnueabi-

CC=$(CROSS_COMPILE)gcc
LD=$(CROSS_COMPILE)ld
OBJCOPY=$(CROSS_COMPILE)objcopy
OBJDUMP=$(CROSS_COMPILE)objdump
CFLAGS := -march=armv7-a

all: fastboot.bin

fastboot.bin: l-loader
	$(OBJCOPY) -O binary $< $@
	${OBJDUMP} -D -m arm $< > l-loader.dis

l-loader: start.o l-loader.lds
	$(LD) -Bstatic -Tl-loader.lds start.o -o $@

start.o: start.S
	$(CC) -c -o $@ $< \
		$(CFLAGS)

l-loader.lds: l-loader.ld.in
	$(CPP) -P -o $@ - < $< $(CFLAGS)

clean:
	@rm -f *.o l-loader.lds l-loader fastboot.bin l-loader.dis
