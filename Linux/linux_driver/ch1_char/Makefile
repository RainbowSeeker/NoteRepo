
obj-m := 1_char.o 2_misc.o 3_timer.o 4_tasklet_wq.o 5_gpio.o

EXTRA_CFLAGS += -Wno-declaration-after-statement -Wno-unused-variable

all: driver app scp

driver:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -f *App

app:
	$(CROSS_COMPILE)gcc test/charApp.c -o charApp
	$(CROSS_COMPILE)gcc test/miscApp.c -o miscApp

scp:
	sshpass -p $(TARGET_IP_PASSWD) scp -P $(TARGET_IP_PORT) *.ko *App $(TARGET_IP):~/driver

.PHONY: all clean scp app