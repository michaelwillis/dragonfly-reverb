include Makefile.mk

OBJS = \
	kiss_fft/kiss_fft.c.o \
	kiss_fft/kiss_fftr.c.o

ifneq ($(SYSTEM_FREEVERB3),true)
OBJS += \
	freeverb/allpass.cpp.o \
	freeverb/biquad.cpp.o \
	freeverb/comb.cpp.o \
	freeverb/delay.cpp.o \
	freeverb/delayline.cpp.o \
	freeverb/earlyref.cpp.o \
	freeverb/efilter.cpp.o \
	freeverb/nrev.cpp.o \
	freeverb/nrevb.cpp.o \
	freeverb/progenitor.cpp.o \
	freeverb/progenitor2.cpp.o \
	freeverb/revbase.cpp.o \
	freeverb/slot.cpp.o \
	freeverb/strev.cpp.o \
	freeverb/utils.cpp.o \
	freeverb/zrev.cpp.o \
	freeverb/zrev2.cpp.o \
	kiss_fft/kiss_fft.c.o \
	kiss_fft/kiss_fftr.c.o
endif

all: $(OBJS)

ifneq ($(SYSTEM_FREEVERB3),true)
freeverb/%.cpp.o: freeverb/%.cpp
	$(CXX) $< $(BUILD_CXX_FLAGS) -I. -DLIBFV3_FLOAT -Wno-unused-parameter -c -o $@
endif

kiss_fft/%.c.o: kiss_fft/%.c
	$(CC) $< $(BUILD_C_FLAGS) -I. -c -o $@

clean:
	rm -f *.[od] */*.[od]
