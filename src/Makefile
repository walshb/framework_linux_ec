KERNELRELEASE ?= $(shell uname -r)
KERNELDIR ?= /lib/modules/$(KERNELRELEASE)/build
DEPMOD ?= depmod
DEPMODBASEDIR ?= /

all: module

module:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) V=1 modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

insmod:
	lsmod | grep -E '^(cros.ec|fwk.ec)' | grep -v '_proto' | awk '{print $$1}' | sort -r | xargs -rn1 rmmod
	lsmod | grep -E '^(cros.ec|fwk.ec)' | awk '{print $$1}' | xargs -rn1 rmmod
	insmod fwk_ec_proto.ko
	insmod fwk_ec.ko
	insmod fwk_ec_lpcs.ko
	insmod fwk_ec_dev.ko
	insmod fwk_ec_chardev.ko

module-install:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) V=1 modules_install
	$(DEPMOD) -b "$(DEPMODBASEDIR)" -a $(KERNELRELEASE)

install: module-install

.PHONY: all module clean module-install install
