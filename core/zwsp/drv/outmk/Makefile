export CROSS_COMPILE ?= aarch64-mix210-linux-
export MEM_SIZE ?= 8G
export KER_SRC = $(CURDIR)/linux-4.19.90/
OUTPUT_DIR ?= $(CURDIR)/output
UBOOU_DEFCONFIG = ss928v100_emmc_defconfig
LINUX_DEFCONFIG = ss928v100_emmc_defconfig
BUSYBOX_DEFCONFIG = ss928v100_defconfig

PHONY += all
all:prepare u-boot atf rootfs

PHONY += clean
clean:u-boot_clean sample_clean kernel_clean rootfs_clean busybox_clean
	rm -rf $(OUTPUT_DIR) || exit 0

PHONY += clean
prepare:
	mkdir -p $(OUTPUT_DIR)

########################## u-boot #############################
PHONY += u-boot
u-boot:prepare
	$(MAKE) -C u-boot DEFCONFIG=$(UBOOU_DEFCONFIG)
	cp u-boot/output/* $(OUTPUT_DIR)/
	cp u-boot/uEnv/env_append.txt $(OUTPUT_DIR)/
	rm $(OUTPUT_DIR)/fw* || exit 0
PHONY += u-boot_clean
u-boot_clean:
	$(MAKE) -C u-boot clean
PHONY += ubootcfg
ubootcfg:
	$(MAKE) -C u-boot ubootcfg

########################## sample #############################
PHONY += sample
sample:
	cd externed_device_sample && ln -sf ../mpp mpp
	$(MAKE) -C externed_device_sample
PHONY += sample_clean
sample_clean:
	$(MAKE) -C externed_device_sample clean
	rm externed_device_sample/mpp || exit 0

########################## kernel #############################
PHONY += kernel
kernel:
	$(MAKE) -C linux-4.19.90 ARCH=arm64 $(LINUX_DEFCONFIG)
	$(MAKE) -C linux-4.19.90 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) uImage
PHONY += kernel_clean
kernel_clean:
	$(MAKE) -C linux-4.19.90 ARCH=arm64 clean
PHONY += kernelcfg
kernelcfg:
	$(MAKE) -C linux-4.19.90 ARCH=arm64 $(LINUX_DEFCONFIG)
	$(MAKE) -C linux-4.19.90 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) menuconfig
	cp linux-4.19.90/.config linux-4.19.90/arch/arm64/configs/$(LINUX_DEFCONFIG)
PHONY += modules
modules:
	$(MAKE) -C linux-4.19.90 ARCH=arm64 $(LINUX_DEFCONFIG)
	$(MAKE) -C linux-4.19.90 ARCH=arm64 CROSS_COMPILE=$(CROSS_COMPILE) modules

########################## modulers #############################
PHONY += modules_outtree
export MODULES_COMMON=$(CURDIR)/modules/common.mk
modules_outtree:
	for dir in $$(ls -d modules/*/); do\
		cd $$dir && $(MAKE) ; \
	done
modules_outtree_clean:
	for dir in $$(ls -d modules/*/); do\
		cd $$dir && $(MAKE) clean; \
	done

########################## atf #############################
PHONY += atf
atf:prepare kernel
	cd trusted-firmware-a-2.2 && \
	chmod +x mk_ss928v100.sh && \
	./mk_ss928v100.sh && \
	cp build/ss928v100/release/fip.bin $(OUTPUT_DIR)/kernel

########################## busybox #############################
PHONY += busybox
busybox:
	$(MAKE) -C busybox-1.31.1 $(BUSYBOX_DEFCONFIG)
	$(MAKE) -C busybox-1.31.1 CROSS_COMPILE=$(CROSS_COMPILE)
	$(MAKE) -C busybox-1.31.1 install
PHONY += busybox_clean
busybox_clean:
	$(MAKE) -C busybox-1.31.1 clean
PHONY += busyboxcfg
busyboxcfg:
	$(MAKE) -C busybox-1.31.1 $(BUSYBOX_DEFCONFIG)
	$(MAKE) -C busybox-1.31.1 CROSS_COMPILE=$(CROSS_COMPILE) menuconfig
	cp busybox-1.31.1/.config busybox-1.31.1/configs/$(BUSYBOX_DEFCONFIG)

########################## rootfs #############################
PHONY += rootfs
ROOTFS_BUILD_DIR=$(OUTPUT_DIR)/rootfs
rootfs: busybox sample u-boot tools
	make -C tools 
	@mkdir -p $(ROOTFS_BUILD_DIR)
	@echo "[rootfs install]"
	@./rootfs/install.sh $(CURDIR) $(ROOTFS_BUILD_DIR)
	@echo "[generate ext4]"
	@./rootfs/gen_ext4.sh $(ROOTFS_BUILD_DIR) $(OUTPUT_DIR)/rootfs.ext4
rootfs_clean:
	@rm -rf $(ROOTFS_BUILD_DIR) || exit 0
	@rm -rf $(OUTPUT_DIR)/rootfs.ext4 || exit 0

.PHONY:$(PHONY)
