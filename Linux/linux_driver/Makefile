TOOLCHAIN_DIR := /home/yangyu/.embedded_linux/khadas/fenix/build
KERNEL_DIR := $(TOOLCHAIN_DIR)/linux
ARCH := arm64
CROSS_COMPILE := $(TOOLCHAIN_DIR)/gcc-arm-aarch64-none-linux-gnu-12.2.rel1/bin/aarch64-none-linux-gnu-
TARGET_IP := khadas@192.168.1.111
TARGET_IP_PASSWD := khadas
TARGET_IP_PORT := 22

# TOOLCHAIN_DIR := /home/yangyu/.embedded_linux/qemu/buildroot-2024.02.6/output/build
# KERNEL_DIR := $(TOOLCHAIN_DIR)/linux-6.1.44
# ARCH := arm
# CROSS_COMPILE := $(TOOLCHAIN_DIR)/../host/bin/arm-buildroot-linux-gnueabihf-
# TARGET_IP := root@127.0.0.1
# TARGET_IP_PASSWD := 12134
# TARGET_IP_PORT := 2222

export KERNEL_DIR ARCH CROSS_COMPILE TARGET_IP TARGET_IP_PASSWD TARGET_IP_PORT

SUBDIR := $(shell find . -maxdepth 1 -type d ! -name ".*")

all: ch2_platform

ch1_char:
	$(MAKE) -C ch1_char

ch2_platform:
	$(MAKE) -C ch2_platform

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean ch1_char ch2_platform