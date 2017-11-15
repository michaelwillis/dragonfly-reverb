#!/usr/bin/make -f
# Makefile for DISTRHO Plugins #
# ---------------------------- #
# Created by falkTX
#

AR  ?= ar
CC  ?= gcc
CXX ?= g++

# --------------------------------------------------------------
# Fallback to Linux if no other OS defined

ifneq ($(HAIKU),true)
ifneq ($(MACOS),true)
ifneq ($(WIN32),true)
LINUX=true
endif
endif
endif

# --------------------------------------------------------------
# Set build and link flags

BASE_FLAGS = -Wall -Wextra -pipe
BASE_OPTS  = -O2 -ffast-math -mtune=generic -msse -msse2 -fdata-sections -ffunction-sections

ifneq ($(MACOS),true)
# MacOS doesn't support this
BASE_OPTS += -mfpmath=sse
endif

ifeq ($(MACOS),true)
# MacOS linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,-dead_strip -Wl,-dead_strip_dylibs
else
# Common linker flags
LINK_OPTS  = -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq ($(RASPPI),true)
# Raspberry-Pi optimization flags
BASE_OPTS  = -O2 -ffast-math -march=armv6 -mfpu=vfp -mfloat-abi=hard
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq ($(PANDORA),true)
# OpenPandora optimization flags
BASE_OPTS  = -O2 -ffast-math -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq ($(NOOPT),true)
# No optimization flags
BASE_OPTS  = -O2 -ffast-math -fdata-sections -ffunction-sections
endif

ifneq ($(WIN32),true)
# not needed for Windows
BASE_FLAGS += -fPIC -DPIC
endif

ifeq ($(DEBUG),true)
BASE_FLAGS += -DDEBUG -O0 -g
LINK_OPTS   =
else
BASE_FLAGS += -DNDEBUG $(BASE_OPTS) -fvisibility=hidden
CXXFLAGS   += -fvisibility-inlines-hidden
endif

BUILD_C_FLAGS   = $(BASE_FLAGS) -std=c99 -std=gnu99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=c++0x -std=gnu++0x $(CXXFLAGS) $(CPPFLAGS)
LINK_FLAGS      = $(LINK_OPTS) -Wl,--no-undefined $(LDFLAGS)

ifeq ($(MACOS),true)
# No C++11 support
BUILD_CXX_FLAGS = $(BASE_FLAGS) $(CXXFLAGS) $(CPPFLAGS)
LINK_FLAGS      = $(LINK_OPTS) $(LDFLAGS)
endif

# --------------------------------------------------------------
# Check for optional libs

ifeq ($(LINUX),true)
HAVE_DGL   = $(shell pkg-config --exists gl x11 && echo true)
HAVE_JACK  = $(shell pkg-config --exists jack   && echo true)
HAVE_LIBLO = $(shell pkg-config --exists liblo  && echo true)
endif

ifeq ($(MACOS),true)
HAVE_DGL = true
endif

ifeq ($(WIN32),true)
HAVE_DGL = true
endif

# --------------------------------------------------------------
# Set libs stuff

ifeq ($(HAVE_DGL),true)

ifeq ($(LINUX),true)
DGL_FLAGS = $(shell pkg-config --cflags gl x11)
DGL_LIBS  = $(shell pkg-config --libs gl x11)
endif

ifeq ($(MACOS),true)
DGL_LIBS  = -framework OpenGL -framework Cocoa
endif

ifeq ($(WIN32),true)
DGL_LIBS  = -lopengl32 -lgdi32
endif

endif # HAVE_DGL

# --------------------------------------------------------------
# Set app extension

ifeq ($(WIN32),true)
APP_EXT = .exe
endif

# --------------------------------------------------------------
# Set shared lib extension

LIB_EXT = .so

ifeq ($(MACOS),true)
LIB_EXT = .dylib
endif

ifeq ($(WIN32),true)
LIB_EXT = .dll
endif

# --------------------------------------------------------------
# Set shared library CLI arg

SHARED = -shared

ifeq ($(MACOS),true)
SHARED = -dynamiclib
endif

# --------------------------------------------------------------
