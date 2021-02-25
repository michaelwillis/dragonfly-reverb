#!/usr/bin/make -f
# Makefile for dgl #
# ---------------- #
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

BASE_FLAGS = -Wall -Wextra -pipe -MD -MP
BASE_OPTS  = -O2 -fdata-sections -ffunction-sections

ifeq ($(CPU_I386_OR_X86_64),true)
BASE_OPTS += -mtune=generic -msse -msse2
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
BASE_OPTS  = -O2 -march=armv6 -mfpu=vfp -mfloat-abi=hard
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
endif

ifeq ($(NOOPT),true)
# No optimization flags
BASE_OPTS  = -O2 -fdata-sections -ffunction-sections
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

BUILD_C_FLAGS   = $(BASE_FLAGS) -std=c99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=c++11 $(CXXFLAGS) $(CPPFLAGS)

ifeq ($(MACOS),true)
# 'no-undefined' is always enabled
LINK_FLAGS      = $(LINK_OPTS) $(LDFLAGS)
else
# Specify 'no-undefined'
LINK_FLAGS      = $(LINK_OPTS) -Wl,--no-undefined $(LDFLAGS)
endif

ifeq ($(MACOS_OLD),true)
# No C++11 support
BUILD_CXX_FLAGS = $(BASE_FLAGS) $(CXXFLAGS) $(CPPFLAGS)
endif

# --------------------------------------------------------------
# Strict test build

ifeq ($(TESTBUILD),true)
BASE_FLAGS += -Werror -Wcast-qual -Wconversion -Wformat -Wformat-security -Wredundant-decls -Wshadow -Wstrict-overflow -fstrict-overflow -Wundef -Wwrite-strings
BASE_FLAGS += -Wpointer-arith -Wabi -Winit-self -Wuninitialized -Wstrict-overflow=5
# BASE_FLAGS += -Wfloat-equal
ifeq ($(CC),clang)
BASE_FLAGS += -Wdocumentation -Wdocumentation-unknown-command
BASE_FLAGS += -Weverything -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-exit-time-destructors -Wno-float-equal
else
BASE_FLAGS += -Wcast-align -Wunsafe-loop-optimizations
endif
ifneq ($(MACOS),true)
BASE_FLAGS += -Wmissing-declarations -Wsign-conversion
ifneq ($(CC),clang)
BASE_FLAGS += -Wlogical-op
endif
endif
CFLAGS     += -Wold-style-definition -Wmissing-declarations -Wmissing-prototypes -Wstrict-prototypes
CXXFLAGS   += -Weffc++ -Wnon-virtual-dtor -Woverloaded-virtual
endif

# --------------------------------------------------------------
# Set libs stuff

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

# --------------------------------------------------------------
