TOOLCHAIN_DIR := /home/yangyu/.embedded_linux/khadas/fenix/build
KERNEL_DIR := $(TOOLCHAIN_DIR)/linux
ARCH := arm64
CROSS_COMPILE := $(TOOLCHAIN_DIR)/gcc-arm-aarch64-none-linux-gnu-12.2.rel1/bin/aarch64-none-linux-gnu-
export KERNEL_DIR ARCH CROSS_COMPILE

SUBDIR := $(shell find . -maxdepth 1 -type d ! -name ".*")

all:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIR); do \
		$(MAKE) -C $$dir clean; \
	done

.PHONY: all clean