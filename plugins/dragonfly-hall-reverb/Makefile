#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = DragonflyHallReverb

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	Plugin.cpp \
	DSP.cpp

FILES_UI  = \
	UI.cpp \
	Artwork.cpp \
	DSP.cpp \
	../../common/AbstractUI.cpp \
	../../common/LabelledKnob.cpp \
	../../common/Spectrogram.cpp \
	../../common/NotoSans_Regular.ttf.cpp

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS := -I../../common $(BUILD_CXX_FLAGS)
BUILD_CXX_FLAGS := ../../common/kiss_fft/*.c.o $(BUILD_CXX_FLAGS)
ifneq ($(SYSTEM_FREEVERB3),true)
BUILD_CXX_FLAGS := ../../common/freeverb/*.cpp.o $(BUILD_CXX_FLAGS)
else
BUILD_CXX_FLAGS := $(shell pkg-config --cflags freeverb3-3) $(BUILD_CXX_FLAGS)
endif

ifneq ($(SYSTEM_LIBSAMPLERATE),true)
BUILD_CXX_FLAGS := ../../common/libsamplerate2/*.c.o $(BUILD_CXX_FLAGS)
endif

# --------------------------------------------------------------
# Link dependencies

ifeq ($(SYSTEM_LIBSAMPLERATE),true)
BUILD_C_FLAGS := -DLIBSRATE1 $(BUILD_C_FLAGS)
BUILD_CXX_FLAGS := -DLIBSRATE1 $(BUILD_CXX_FLAGS)
LINK_OPTS += -lsamplerate
endif

ifneq ($(SYSTEM_FREEVERB3),true)
LINK_OPTS += -lm
else
LINK_OPTS += -lm $(shell pkg-config --libs freeverb3-3)
endif


ifeq ($(WIN32),true)
LINK_OPTS += -static -static-libgcc -static-libstdc++
endif


# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_DGL),true)
ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif
endif

ifeq ($(HAVE_DGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
