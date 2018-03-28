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

OBJS_DSP = \
	DragonflyReverbPlugin.cpp.o \
	DragonflyReverbDSP.cpp.o

OBJS_UI  = \
	DragonflyReverbUI.cpp.o \
	DragonflyReverbArtwork.cpp.o \
	NotoSans_Regular.ttf.cpp.o \
	DragonflyReverbDSP.cpp.o \
	Spectrogram.cpp.o

# --------------------------------------------------------------
# Do some magic

include ../Makefile.mk

# --------------------------------------------------------------
# Link dependencies

LINK_OPTS += -lfftw3f -lm

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(HAVE_JACK),true)
TARGETS += jack
endif

ifeq ($(HAVE_DGL),true)
TARGETS += lv2_sep
else
TARGETS += lv2_dsp
endif

TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
