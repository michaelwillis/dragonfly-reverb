#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

# all: libs dgl plugins gen
all: dgl plugins gen

# --------------------------------------------------------------

# libs:
#	 $(MAKE) -C freeverb3

dgl:
ifeq ($(HAVE_DGL),true)
	$(MAKE) -C dpf/dgl
endif

# plugins: libs dgl
plugins: dgl
	$(MAKE) all -C plugins/dragonfly-reverb

ifneq ($(CROSS_COMPILING),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/dragonfly-reverb
	rm freeverb3/libsamplerate2/*.d
	rm freeverb3/libsamplerate2/*.o
	rm freeverb3/freeverb/*.d
	rm freeverb3/freeverb/*.o
	rm -rf bin build

# --------------------------------------------------------------

.PHONY: plugins
