module dsp.filters;

import std.math;

import dplug.dsp.delayline;

struct DCCut {
  float gain = 0.9999;
  float y1 = 0, y2 = 0;

  void mute() nothrow @nogc {
      y1 = y2 = 0;
  }

  float process(float input) nothrow @nogc {
      float output = input - y1 + gain * y2;
      y1 = input;
      y2 = output;
      return output;
  }

  void setA(float a) nothrow @nogc {
      this.gain = a;
  }

  void setCutOnFreq(float freq, float sampleRate) nothrow @nogc {
    immutable float sqrt3 = sqrt(3.0);
    immutable float angle = 2.0 * PI * freq / sampleRate;
    gain = (sqrt3 - 2.0 * sin(angle)) / (sin(angle) + sqrt3 * cos(angle));
  }
}

struct FeedbackCombFilter {
  void mute() nothrow @nogc {
    filterstore = 0;
    setSize(size); // Reallocates buffer and fills with zeros
  }

  float process(float input) nothrow @nogc {
      float output = delay.sampleFull(size - 1);
      filterstore = (output * damp2) + (filterstore * damp1);
      delay.feedSample(input + (filterstore * feedback));
      return output;
  }

  void setDamp(float damp) nothrow @nogc {
    damp1 = damp;
    damp2 = 1.0 - damp;
  }

  void setFeedback(float feedback) nothrow @nogc {
      this.feedback = feedback;
  }

  int getSize() nothrow @nogc {
    return size;
  }

  void setSize(int size) nothrow @nogc {
    assert(size > 1);
    this.size = size;
    delay.resize(size);
  }

  int size;
  float feedback = 0, filterstore = 0, damp1 = 0, damp2 = 1;
  Delayline!float delay;
}