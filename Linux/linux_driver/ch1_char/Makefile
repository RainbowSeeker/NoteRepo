
obj-m := char.o misc.o timer.o

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
	sshpass -p "khadas" scp *.ko *App khadas@192.168.1.111:~/driver

.PHONY: all clean scp app