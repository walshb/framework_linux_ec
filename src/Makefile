VERSION=4.0.0

KERNELRELEASE ?= $(shell uname -r)
KERNELDIR ?= /lib/modules/$(KERNELRELEASE)/build
PREFIX ?= /usr
DESTDIR ?=
SRCDIR ?= $(PREFIX)/src
DKMSDIR ?= $(SRCDIR)/fwk_ec-$(VERSION)
DEPMOD ?= depmod
DEPMODBASEDIR ?= /

all: module

module:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) V=1 modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) clean

insmod:
	lsmod | grep -E '^(cros.ec|fwk.ec)' | grep -v '_proto' | awk '{print $$1}' | sort -r | xargs -rn1 rmmod
	lsmod | grep -E '^(cros.ec|fwk.ec)' | awk '{print $$1}' | xargs -rn1 rmmod
	insmod fwk_ec_proto.ko
	insmod fwk_ec.ko
	insmod fwk_ec_lpcs.ko
	insmod fwk_ec_dev.ko
	insmod fwk_ec_chardev.ko

module-install:
	$(MAKE) -C $(KERNELDIR) M=$(CURDIR) V=1 modules_install
	$(DEPMOD) -b "$(DEPMODBASEDIR)" -a $(KERNELRELEASE)

install: module-install

DKMS_SOURCES := $(filter-out %.mod.c, $(wildcard *.c *.h)) Kbuild dkms.conf REVISION

dkms-install:
	install -d -m 755 $(DESTDIR)$(DKMSDIR)
	install -v -m 644 $(DKMS_SOURCES) $(DESTDIR)$(DKMSDIR)/

.PHONY: all module clean insmod module-install install dkms-install
