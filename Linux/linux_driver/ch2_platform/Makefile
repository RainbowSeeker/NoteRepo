
obj-m := 1_led.o

EXTRA_CFLAGS += -Wno-declaration-after-statement -Wno-unused-variable

all: driver scp # app

driver:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(CURDIR) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) clean
	rm -f *App

app:
	$(CROSS_COMPILE)gcc -v

scp:
	sshpass -p $(TARGET_IP_PASSWD) scp -P $(TARGET_IP_PORT) *.ko $(TARGET_IP):~/driver

.PHONY: all clean scp app