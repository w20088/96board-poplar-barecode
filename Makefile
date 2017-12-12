################################################################################
#    Copyright (C) 2014 Hisilicon STB Development Dept
#    All rights reserved.
#####
#    Create by Cai Zhiyong
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#    http://www.apache.org/licenses/LICENSE-2.0
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
################################################################################
ARCH ?= armv7-a
CROSS_COMPILE  ?= arm-histbv310-linux-
ifeq ($(CROSS_COMPILE),)
$(error You should define CROSS_COMPILE!)
endif
BOOT           = fastboot
V              = 0

################################################################################

TOPDIR     := .
DEFTARGET  := $(MAKECMDGOALS)
PLATFORM   :=
PHONYS     :=
TEMPS      :=
CC         := $(CROSS_COMPILE)gcc
AR         := $(CROSS_COMPILE)ar
LD         := $(CROSS_COMPILE)ld
OBJCOPY    := $(CROSS_COMPILE)objcopy
OBJDUMP    := $(CROSS_COMPILE)objdump
NM         := $(CROSS_COMPILE)nm
RM         := rm
DEFCONFIG  := $(TOPDIR)/configs/config

sinclude $(DEFCONFIG)

ARFLAGS    := rcs
LDFLAGS    :=
CFLAGS     := -Wall -nostdinc -isystem $(TOPDIR)/include \
	-I$(TOPDIR)/arm/$(PLATFORM)/include \
	-I$(TOPDIR)/arm/include \
	-Os -fno-strict-aliasing -mapcs-frame \
	-fno-common -fno-builtin -ffreestanding \
	-pipe -marm  -mabi=aapcs-linux \
	-mno-thumb-interwork -march=$(ARCH)

ASFLAGS    := $(CFLAGS)
ZBOOT      := $(BOOT)z
BUILDOBJ   := build-obj

ifeq ($(V),1)
    Q     :=
    QUIET :=
else
    Q     := @
    QUIET := -s
endif

AUXIMAGE =  $(TOPDIR)/auxcode_sign.img
AUXCODE  := $(shell if [ -d $(TOPDIR)/../auxcode ]; then \
			echo $(TOPDIR)/../auxcode; \
		else if [ -d $(TOPDIR)/auxcode ]; then \
			echo $(TOPDIR)/auxcode; \
		fi;fi;)

CFLAGS += -DCONFIG_AUXIMAGE=\"$(AUXIMAGE)\"

MKFLAGS    := $(QUIET) -f $(TOPDIR)/scripts/Makefile.build
CLFLAGS    := $(QUIET) -f $(TOPDIR)/scripts/Makefile.clean
export MKFLAGS CLFLAGS

SHELL      := $(shell if [ -x "$${BASH}" ]; then echo $${BASH}; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	     else echo sh; fi; \
	fi)

export CC AR LD OBJCOPY OBJDUMP RM ARFLAGS LDFLAGS CFLAGS ASFLAGS SHELL
export PLATFORM TOPDIR BOOT ZBOOT V Q BUILDOBJ DEFCONFIG MAKE RM NM

EXPORTCONFIG := ${TOPDIR}/scripts/export.sh

INIT_REG := $(wildcard $(TOPDIR)/*.reg $(TOPDIR)/.reg)
export INIT_REG

################################################################################
SRCDIR      :=
CONFIGINC   := $(TOPDIR)/include/config.h
DEFMAKEFILE := $(TOPDIR)/configs/Makefile
BOOTPATH    := arm/$(PLATFORM)/boot/

ifdef CONFIG_UNIFIED_BOOT
LINKLDS     := arm/boot_unify.lds
endif

PLATFORMLDS := arm/$(PLATFORM)/boot/platform.lds
export LINKLDS PLATFORMLDS

#LIBS        := arm/ libs/ drivers/
LIBS        := src/
#ZLIBS       := arm/lib/compiler/
ZLIBS       :=

LIBS  := $(addsuffix $(BUILDOBJ).o,$(addprefix $(TOPDIR)/,$(LIBS)))
ZLIBS := $(addsuffix $(BUILDOBJ).o,$(addprefix $(TOPDIR)/,$(ZLIBS)))


DEFLATE     := $(TOPDIR)/$(TOOLPATH)/deflate
MKBOOTARGS  := $(TOPDIR)/$(TOOLPATH)/mkbootargs

ifeq ($(CONFIG_PRODUCT_WITH_BOOT),y)
    LIBAPPS := $(TOPDIR)/../product/libproduct.a
else
    LIBAPPS :=
endif

INPUTFLAGS = ${MAKEFLAGS}
export INPUTFLAGS

ALL_CONFIG   := $(notdir $(wildcard $(TOPDIR)/configs/*_config))
ALL_PLATRORM := $(subst _config,,$(ALL_CONFIG))

include $(TOPDIR)/scripts/Makefile.define
################################################################################
sinclude $(DEFMAKEFILE)
TGTCONFIG := $(TOPDIR)/configs/$(DEFTARGET)_config

PHONYS += help
help:
	@echo "make [platform] [options]"
	@echo "platforms:"
	@for ix in $(ALL_PLATRORM); do ( \
		_MSG="    $${ix}                                        "; MSG=$${_MSG:0:40}; \
		echo -n "$${MSG}"; head -n 1 ./configs/$${ix}_config; \
	) done
	@echo ;

################################################################################
TEMPS += $(BOOT).bin $(BOOT).text $(BOOT).srec
PHONYS += $(BOOT).bin
$(BOOT).bin: $(BOOT).elf 
	$(call show_cmd,OBJCOPY,$@)
	$(Q)$(OBJCOPY) -O srec $(BOOT).elf $(BOOT).srec
	$(Q)$(OBJCOPY) -j .text -O binary $(BOOT).elf $(BOOT).text
	$(Q)$(OBJCOPY)  -R .comment --gap-fill=0xff -O binary $(BOOT).elf $@
	$(call show_cmd,BOOT,$@)
	@echo "Config File:  $(DEFTARGET)"
	@echo "Register:     $(notdir $(INIT_REG))"
	@echo "Entry Point:  $(TEXT_BASE)"
	@echo "  Image $@ is ready"

TEMPS += $(ZBOOT).elf $(ZBOOT).bin data.z $(ZBOOT).text \
	$(ZBOOT).srec $(BOOT).elf

$(BOOT).elf: $(CONFIGINC) $(LIBS) $(LINKLDS) $(PLATFORMLDS) $(DDRLIBS) force
	$(Q)$(MAKE) $(MKFLAGS) SRCDIR="$(BOOTPATH)" LIBS="$(LIBS) $(LIBAPPS)" CONFIG_AUXIMAGE=\"$(AUXIMAGE)\" \
		$(BOOT).elf

%/$(BUILDOBJ).o: $(CONFIGINC) force
	$(Q)$(MAKE) $(MKFLAGS) SRCDIR="$(@D)/" $@

$(DEFCONFIG): $(TGTCONFIG)
	$(call show_cmd,GEN,$(DEFCONFIG))
	$(Q)$(SHELL) ${EXPORTCONFIG} export "$(TGTCONFIG)" "$(DEFCONFIG)" "$(CONFIGINC)" INPUTFLAGS

$(DEFMAKEFILE): $(TGTCONFIG)
	$(call show_cmd,GEN,$(DEFMAKEFILE))
	$(call export_make,$(DEFTARGET))

$(ALL_CONFIG):
	$(call show_cmd,GEN,$(DEFCONFIG))
	$(Q)$(SHELL) ${EXPORTCONFIG} export "$(TOPDIR)/configs/$(@)" "$(DEFCONFIG)" "$(CONFIGINC)" INPUTFLAGS
	$(call show_cmd,GEN,$(DEFMAKEFILE))
	$(call export_make,$(subst _config,,$@))

force:;


################################################################################

PHONYS += clean
clean: $(addsuffix .clean,$(LIBS) $(ZLIBS) $(DDRLIBS))
	$(Q)$(RM) -f $(TEMPS)
	$(Q)test ! -d "$(BOOTPATH)" || $(MAKE) $(CLFLAGS) SRCDIR="$(BOOTPATH)" clean

%/$(BUILDOBJ).o.clean:
	$(call show_cmd,CLEAN,$(@D))
	$(Q)$(MAKE) $(CLFLAGS) SRCDIR="$(@D)/" clean

PHONYS += distclean
distclean: clean
	$(Q)$(RM) -f $(DEFCONFIG) $(CONFIGINC) $(DEFMAKEFILE)

################################################################################

.PHONY: $(PHONYS)
################################################################################
