
ifeq ($(OS),Windows_NT)
    MACHINE = WIN32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
    	ARCH = AMD64
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        ARCH = IA32
    endif
else
    UNAME_M := $(shell uname -m)
    ifeq ($(UNAME_M),x86_64)
        ARCH = AMD64
    endif
    ifneq ($(filter %86,$(UNAME_M)),)
        ARCH += IA32
    endif
    ifneq ($(filter arm%,$(UNAME_M)),)
        ARCH += ARM
    endif
endif

ifeq ($(ARCH), AMD64)
USE_PLUGIN_SIMD = true
CXXFLAGS += -mavx -mfma -DUSE_PLUGIN_SIMD
endif

# --------------------------
# FIle Inclusion
ifeq ($(USE_PLUGIN_SIMD), true)
FILES_COMMON  += \
	../../common/optimization/optimization.cpp \
	../../common/optimization/default.cpp

ifeq ($(ARCH), AMD64)
FILES_COMMON  += \
	../../common/optimization/avx.cpp
endif

endif

