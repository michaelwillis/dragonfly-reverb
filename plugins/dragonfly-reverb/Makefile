#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = DragonflyReverb

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	DragonflyReverbPlugin.cpp \
	DragonflyReverbDSP.cpp

FILES_UI  = \
	DragonflyReverbUI.cpp \
	DragonflyReverbArtwork.cpp \
	NotoSans_Regular.ttf.cpp \
	DragonflyReverbDSP.cpp \
	Spectrogram.cpp \
	LabelledKnob.cpp \
	fft.c

# --------------------------------------------------------------
# Do some magic

include ../../dpf/Makefile.plugins.mk

BUILD_CXX_FLAGS := ../../freeverb3/*.cpp.o ../../freeverb3/*.c.o -I../../freeverb3 $(BUILD_CXX_FLAGS)

# --------------------------------------------------------------
# Link dependencies

LINK_OPTS += -lm

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
