
obj-m := 1_led.o 2_input.o 3_i2c.o 4_spi.o

DTS_FILE := dts/lubancat4.dts

CPPFLAGS := -nostdinc -undef -x assembler-with-cpp
CPPFLAGS += -I $(shell pwd)/dts
CPPFLAGS += -I $(KERNEL_DIR)/include

EXTRA_CFLAGS += -Wno-declaration-after-statement -Wno-unused-variable

all: driver dtb app scp

driver:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -f *App

app:
	$(CROSS_COMPILE)gcc -o spiApp test/spiApp.c

scp:
	sshpass -p $(TARGET_IP_PASSWD) scp -P $(TARGET_IP_PORT) *.ko *.dtb *App $(TARGET_IP):~/driver

dtb: $(DTS_FILE)
	cpp $(CPPFLAGS) $< | dtc -I dts -O dtb -o $(shell basename $< .dts).dtb

# dtc -I dtb -O dts -H both -o lubancat4.dts ch2_platform/rk3588s-lubancat-4.dtb

.PHONY: all clean scp app

