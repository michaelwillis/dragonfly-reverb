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
	$(wildcard ../../freeverb3/libsamplerate2/*.c) \
	$(wildcard ../../freeverb3/freeverb/*.cpp) \
	DragonflyReverbPlugin.cpp \
	DragonflyReverbDSP.cpp \
	# ../../freeverb3/libsamplerate2/samplerate.c	\
	# ../../freeverb3/libsamplerate2/src_common.c	\
	# ../../freeverb3/libsamplerate2/src_sinc.c \
	# ../../freeverb3/libsamplerate2/samplerate_common.c \
	# ../../freeverb3/libsamplerate2/src_linear.c \
	# ../../freeverb3/libsamplerate2/src_zoh.c \
	# ../../freeverb3/freeverb/*.cpp

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

BUILD_CXX_FLAGS := ../../freeverb3/freeverb/*.cpp.o ../../freeverb3/libsamplerate2/*.c.o -I../../freeverb3 $(BUILD_CXX_FLAGS)
BUILD_C_FLAGS += -DLIBSRATE2_FLOAT -DHAVE_LRINT -DHAVE_LRINTF
BUILD_CXX_FLAGS += -DLIBFV3_FLOAT -DHAVE_LRINT -DHAVE_LRINTF -I../../freeverb3

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
