#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include Makefile.mk

all: libs plugins gen

# --------------------------------------------------------------

libs:
	$(MAKE) -C freeverb3
ifeq ($(HAVE_DGL),true)
	$(MAKE) -C dpf/dgl
endif

plugins: libs
	$(MAKE) all -C plugins/dragonfly-reverb

gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/dpf/utils/generate-vst-bundles.sh
endif

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator

# --------------------------------------------------------------

clean:
ifeq ($(HAVE_DGL),true)
	$(MAKE) clean -C dpf/dgl
endif
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/dragonfly-reverb

# --------------------------------------------------------------

.PHONY: plugins
