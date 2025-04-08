TOOLCHAIN_DIR := /home/yangyu/.embedded_linux/lubancat
KERNEL_DIR := $(TOOLCHAIN_DIR)/kernel
ARCH := arm64
CROSS_COMPILE := aarch64-linux-gnu-
TARGET_IP := cat@192.168.1.132
TARGET_IP_PASSWD := temppwd
TARGET_IP_PORT := 22

export KERNEL_DIR ARCH CROSS_COMPILE TARGET_IP TARGET_IP_PASSWD TARGET_IP_PORT

SUBDIR := $(shell find . -maxdepth 1 -type d ! -name ".*")

all: ch1_char ch2_platform

ch1_char:
	$(MAKE) -C ch1_char

ch2_platform:
	$(MAKE) -C ch2_platform

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean ch1_char ch2_platform