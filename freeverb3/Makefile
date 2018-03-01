include Makefile.mk

OBJS_FV3 = \
	samplerate.c.o \
	samplerate_common.c.o \
	src_common.c.o \
	src_linear.c.o \
	src_sinc.c.o \
	src_zoh.c.o \
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

all: $(OBJS_FV3)

%.c.o: libsamplerate2/%.c
	$(CC) $< $(BUILD_C_FLAGS) -I. -DLIBSRATE2_FLOAT -c -o $@

%.cpp.o: freeverb/%.cpp
	$(CXX) $< $(BUILD_CXX_FLAGS) -I. -DLIBSRATE2_FLOAT -DLIBFV3_FLOAT -Wno-unused-parameter -c -o $@

clean:
	rm -f *.d *.o
