#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = DragonflyRoomReverb

# --------------------------------------------------------------
# Files to build

FILES_COMMON  = DSP.cpp \
	../../common/kiss_fft/kiss_fft.c \
	../../common/kiss_fft/kiss_fftr.c 

ifneq ($(SYSTEM_FREEVERB3),true)
FILES_COMMON += \
	../../common/freeverb/allpass.cpp \
	../../common/freeverb/biquad.cpp \
	../../common/freeverb/comb.cpp \
	../../common/freeverb/delay.cpp \
	../../common/freeverb/delayline.cpp \
	../../common/freeverb/earlyref.cpp \
	../../common/freeverb/efilter.cpp \
	../../common/freeverb/nrev.cpp \
	../../common/freeverb/nrevb.cpp \
	../../common/freeverb/progenitor.cpp \
	../../common/freeverb/progenitor2.cpp \
	../../common/freeverb/revbase.cpp \
	../../common/freeverb/slot.cpp \
	../../common/freeverb/strev.cpp \
	../../common/freeverb/utils.cpp \
	../../common/freeverb/zrev.cpp \
	../../common/freeverb/zrev2.cpp
endif

FILES_DSP = $(FILES_COMMON) \
	Plugin.cpp

FILES_UI  = $(FILES_COMMON) \
	Artwork.cpp \
	UI.cpp \
	AbstractUI.cpp \
	../../common/LabelledKnob.cpp \
	../../common/Selection.cpp \
	../../common/Spectrogram.cpp \
	../../common/Bitstream_Vera_Sans_Regular.cpp

# --------------------------------------------------------------
# Check for optimization support

include ../../common/optimization/Makefile.optimization.mk

# --------------------------------------------------------------
# Do some magic

FILE_BROWSER_DISABLED = true
SKIP_RTAUDIO_FALLBACK = true
include ../../dpf/Makefile.plugins.mk

# --------------------------------------------------------------
# Build dependencies

BUILD_CXX_FLAGS += -I../../common -DLIBFV3_FLOAT
ifeq ($(SYSTEM_FREEVERB3),true)
BUILD_CXX_FLAGS += -DLIBSRATE1
BUILD_CXX_FLAGS += $(shell $(PKG_CONFIG) --cflags freeverb3-3)
endif

# TODO handle throw deprecation for freeverb
BUILD_CXX_FLAGS += -Wno-deprecated

# --------------------------------------------------------------
# Link dependencies

ifeq ($(SYSTEM_FREEVERB3),true)
LINK_OPTS += $(shell $(PKG_CONFIG) --libs freeverb3-3) -lsamplerate
endif

LINK_OPTS += -lm

# --------------------------------------------------------------
# Enable all possible plugin types

TARGETS = jack lv2_sep vst2 vst3 clap

all: $(TARGETS)

# --------------------------------------------------------------
