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
      float output = buffer.sampleFull(size - 1);
      filterstore = (output * damp2) + (filterstore * damp1);
      buffer.feedSample(input + (filterstore * feedback));
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
    buffer.resize(size);
  }

  int size;
  float feedback = 0, filterstore = 0, damp1 = 0, damp2 = 1;
  Delayline!float buffer;
}


struct FeedforwardCombFilter {
  void mute() nothrow @nogc {
    setSize(size); // Reallocates buffer and fills with zeros
  }

  float process(float input, float feedforward) nothrow @nogc {
    float output = buffer.sampleFull(size - 1) * feedforward + input;
    buffer.feedSample(input);
    return output;
  }

  void setSize(int size) nothrow @nogc {
    assert(size > 1);
    this.size = size;
    buffer.resize(size);

    // Delay can be customized afterward if needed, but this
    // is a sensible default and prevents buffer overrun.
    setDelay(size);
  }

  void setDelay(int delay) nothrow @nogc {
    assert(delay <= size);
    this.delay = delay;
  }

  int size, delay;
  Delayline!float buffer;
}


struct FirstOrderFeedbackAllpassFilter {
  void mute() nothrow @nogc {
    setSize(size); // Reallocates buffer and fills with zeros
  }

  float process(float input) nothrow @nogc {
      immutable float sample = buffer.sampleFull(delay);
      input += feedback * sample;
      buffer.feedSample(input);
      return decay * sample - feedback * input;
  }
  
  void setFeedback(float feedback) nothrow @nogc {
      this.feedback = feedback;
  }

  void setDecay(float decay) nothrow @nogc {
    assert(decay < 1.0);
  }

  void setDelay(int delay) nothrow @nogc {
    assert(delay <= size);
    this.delay = delay;
  }

  void setSize(int size) nothrow @nogc {
    assert(size > 1);
    this.size = size;
    buffer.resize(size);

    // Delay can be customized afterward if needed, but this
    // is a sensible default and prevents buffer overrun.
    setDelay(size);
  }

  int size, delay;
  float decay = 1, feedback = 0;
  Delayline!float buffer;
}


struct SecondOrderFeedbackAllpassFilter {
  void mute() nothrow @nogc {
    setSize(size1, size2); // Reallocates buffer and fills with zeros
  }

  float process(float input) nothrow @nogc {
    float sample1 = buffer1.sampleFull(delay1);
    float sample2 = buffer2.sampleFull(delay2);

    input += feedback2 * sample2;
    float output = decay2 * sample2 - input * feedback2;
    input += feedback1 * sample1;

    buffer2.feedSample(decay1 * sample1 - input * feedback1);
    buffer1.feedSample(input);

    return output;
  }

  void setFeedback(float feedback1, float feedback2) nothrow @nogc {
      this.feedback1 = feedback1;
      this.feedback2 = feedback2;
  }

  void setDecay(float decay1, float decay2) nothrow @nogc {
    assert(decay1 < 1.0);
    assert(decay2 < 1.0);
    this.decay1 = decay1;
    this.decay2 = decay2;
  }

  void setSize(int size1, int size2) nothrow @nogc {
    assert(size1 > 1);
    assert(size2 > 1);
    this.size1 = size1;
    this.size2 = size2;
    buffer1.resize(size1);
    buffer2.resize(size2);

    // Delay can be customized afterward if needed, but this
    // is a sensible default and prevents buffer overrun.
    setDelay(size1, size2);
  }

  void setDelay(int delay1, int delay2) nothrow @nogc {
    assert(delay1 <= size1);
    assert(delay2 <= size2);
    this.delay1 = delay1;
    this.delay2 = delay2;
  }

  float getz1(int index) nothrow @nogc {
    return buffer1.sampleFull(index);
  }

  float getz2(int index) nothrow @nogc {
    return buffer2.sampleFull(index);
  }

  int size1, size2, delay1, delay2;
  float decay1, decay2, feedback1, feedback2;
  Delayline!float buffer1, buffer2;
}


struct ThirdOrderFeedbackAllpassFilter {
  void mute() nothrow @nogc {
    setSize(size1, size2, size3); // Reallocates buffer and fills with zeros
  }

  float process(float input) nothrow @nogc {
    float sample1 = buffer1.sampleFull(delay1);
    float sample2 = buffer2.sampleFull(delay2);
    float sample3 = buffer3.sampleFull(delay3);

    input += feedback3 * sample3;
    float output = decay3 * sample3 - feedback3 * input;

    input += feedback2 * sample2;
    buffer3.feedSample(decay2 * sample2 - feedback2 * input);

    input += feedback1 * sample1;
    buffer2.feedSample(decay1 * sample1 - feedback1 * input);

    buffer1.feedSample(input);

    return output;
  }

  void setFeedback(float feedback1, float feedback2, float feedback3) nothrow @nogc {
    assert(feedback1 < 1.0);
    assert(feedback2 < 1.0);
    assert(feedback3 < 1.0);
    this.feedback1 = feedback1;
    this.feedback2 = feedback2;
    this.feedback3 = feedback3;
  }

  void setDecay(float decay1, float decay2, float decay3) nothrow @nogc {
    assert(decay1 < 1.0);
    assert(decay2 < 1.0);
    assert(decay3 < 1.0);
    this.decay1 = decay1;
    this.decay2 = decay2;
    this.decay3 = decay3;
  }

  void setSize(int size1, int size2, int size3) nothrow @nogc {
    assert(size1 > 1);
    assert(size2 > 1);
    assert(size3 > 1);
    this.size1 = size1;
    this.size2 = size2;
    this.size3 = size3;
    buffer1.resize(size1);
    buffer2.resize(size2);
    buffer3.resize(size3);

    // Delay can be customized afterward if needed, but this
    // is a sensible default and prevents buffer overrun.
    setDelay(size1, size2, size3);
  }

  void setDelay(int delay1, int delay2, int delay3) nothrow @nogc {
    assert(delay1 <= size1);
    assert(delay2 <= size2);
    assert(delay3 <= size3);
    this.delay1 = delay1;
    this.delay2 = delay2;
    this.delay3 = delay3;
  }

  float getz1(int index) nothrow @nogc {
    return buffer1.sampleFull(index);
  }

  float getz2(int index) nothrow @nogc {
    return buffer2.sampleFull(index);
  }

  float getz3(int index) nothrow @nogc {
    return buffer3.sampleFull(index);
  }

  int size1, size2, size3;
  int delay1, delay2, delay3;
  float decay1, decay2, decay3;
  float feedback1, feedback2, feedback3;
  Delayline!float buffer1, buffer2, buffer3;
}