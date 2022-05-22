#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

include dpf/Makefile.base.mk

all: dgl plugins gen

# --------------------------------------------------------------

ifneq ($(CROSS_COMPILING),true)
CAN_GENERATE_TTL = true
else ifneq ($(EXE_WRAPPER),)
CAN_GENERATE_TTL = true
endif

dgl:
	$(MAKE) -C dpf/dgl

plugins: dgl
	$(MAKE) all -C plugins/dragonfly-hall-reverb
	$(MAKE) all -C plugins/dragonfly-room-reverb
	$(MAKE) all -C plugins/dragonfly-plate-reverb
	$(MAKE) all -C plugins/dragonfly-early-reflections

ifeq ($(CAN_GENERATE_TTL),true)
gen: plugins dpf/utils/lv2_ttl_generator
	@$(CURDIR)/dpf/utils/generate-ttl.sh

dpf/utils/lv2_ttl_generator:
	$(MAKE) -C dpf/utils/lv2-ttl-generator
else
gen:
endif

# --------------------------------------------------------------

clean:
	$(MAKE) clean -C dpf/dgl
	$(MAKE) clean -C dpf/utils/lv2-ttl-generator
	$(MAKE) clean -C plugins/dragonfly-hall-reverb
	$(MAKE) clean -C plugins/dragonfly-room-reverb
	$(MAKE) clean -C plugins/dragonfly-plate-reverb
	$(MAKE) clean -C plugins/dragonfly-early-reflections
	rm -rf bin build
	rm -f common/*.d common/*.o
	rm -f common/freeverb/*.d common/freeverb/*.o
	rm -f common/kiss_fft/*.d common/kiss_fft/*.o
	rm -f dpf/utils/lv2_ttl_generator.d

# --------------------------------------------------------------

.PHONY: plugins
