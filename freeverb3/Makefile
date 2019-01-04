include Makefile.mk

OBJS_FV3 = \
	allpass.cpp.o \
	biquad.cpp.o \
	comb.cpp.o \
	delay.cpp.o \
	delayline.cpp.o \
	earlyref.cpp.o \
	efilter.cpp.o \
	revbase.cpp.o \
	src.cpp.o \
	slot.cpp.o \
	utils.cpp.o \
	zrev.cpp.o \
	zrev2.cpp.o

ifneq ($(SYSTEM_LIBSAMPLERATE),true)
OBJS_FV3 += \
	samplerate.c.o \
	samplerate_common.c.o \
	src_common.c.o \
	src_linear.c.o \
	src_sinc.c.o \
	src_zoh.c.o
endif

all: $(OBJS_FV3)

ifneq ($(SYSTEM_LIBSAMPLERATE),true)
%.c.o: libsamplerate2/%.c
	$(CC) $< $(BUILD_C_FLAGS) -I. -DLIBSRATE2_FLOAT -c -o $@
endif

%.cpp.o: freeverb/%.cpp
ifneq ($(SYSTEM_LIBSAMPLERATE),true)
	$(CXX) $< $(BUILD_CXX_FLAGS) -I. -DLIBSRATE2_FLOAT -DLIBFV3_FLOAT -Wno-unused-parameter -c -o $@
else
	$(CXX) $< $(BUILD_CXX_FLAGS) -I. -DLIBSRATE1 -DLIBFV3_FLOAT -Wno-unused-parameter -c -o $@
endif

clean:
	rm -f *.d *.o
