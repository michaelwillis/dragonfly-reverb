module dsp.oscillator;

import std.math;

immutable int CORRECT_DRIFT_THRESHOLD = 10000;

struct SineOscillator {
  float re = 1, im = 0, arc_re = 0, arc_im = 0;
  long count = 0;

  void setFreq(float freq, float sampleRate) nothrow @nogc {
    assert(freq < sampleRate / 2.0);
    float theta = 2.0 * PI * (freq / sampleRate);
    arc_re = cos(theta);
    arc_im = sin(theta);
  }

  float opCall() nothrow @nogc {
    float output = im;
    float new_re = re * arc_re - im * arc_im;
    float new_im = re * arc_im + im * arc_re;

    re = new_re;
    im = new_im;

    // occasionally correct floating point drift
    if(count++ > CORRECT_DRIFT_THRESHOLD) {
      count = 0;
      float arc_r = sqrt(re * re + im * im);
      re /= arc_r;
      im /= arc_r;
    }

    return (output > 1.0) ? 1.0 : output < -1.0 ? -1.0 : output;
  }

}