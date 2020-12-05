module dsp.room;

import dplug.dsp.delayline;
import dplug.dsp.iir;

import dsp.effects;
import dsp.filters;

import std.math;

immutable float DECAY_0 = 0.237, DECAY_1 = 0.938, DECAY_2 = 0.844, DECAY_3 = 0.906;
immutable float DIFFUSE_1 = 0.375, DIFFUSE_2 = 0.312, DIFFUSE_3 = 0.406, DIFFUSE_4 = 0.25;

final class RoomEffect : NoEffect
{
public:
nothrow:
@nogc:
  this() {
    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();

    // allpassmL_15_16.setfeedback(DIFFUSE_1);
    // allpassmR_19_20.setfeedback(DIFFUSE_1);

    // allpassmL_17_18.setfeedback(DIFFUSE_2);
    // allpassmR_21_22.setfeedback(DIFFUSE_2);

    allpass2L_25_27.setFeedback(DIFFUSE_4, DIFFUSE_3);
    allpass2R_43_45.setFeedback(DIFFUSE_4, DIFFUSE_3);
    allpass3L_34_37.setFeedback(DIFFUSE_4, DIFFUSE_4, DIFFUSE_3);
    allpass3R_52_55.setFeedback(DIFFUSE_4, DIFFUSE_4, DIFFUSE_3);
  }


  override void mute() {
    // TODO: There may be a better way to do this that doesn't reallocate the delay buffers.
    reset(this.sampleRate, this.maxFrames);
  }

  override void processAudio(float[] leftIn, float[] rightIn, float[] leftOut, float[] rightOut, int frames) {
    for (int f = 0; f < frames; f++) {
      float outL = leftIn[f];
      float outR = rightIn[f];

      delayL_37.feedSample(
        allpass3L_34_37.process(
          delayL_31.nextSample(
            allpass2L_25_27.process(
              delayL_23.nextSample(outL)))));

      delayR_58.feedSample(
        allpass3R_52_55.process(
          delayR_49.nextSample(
            allpass2R_43_45.process(
              delayR_40.nextSample(outR)))));

      float dOut = delayL_23.sampleFull(cast(int) (0.00003 * sampleRate)) * 0.469
        + (delayL_31.sampleFull(cast(int) (0.00117 * sampleRate))
        - delayR_49.sampleFull(cast(int) (0.00563 * sampleRate))
        + delayL_31.sampleFull(cast(int) (0.00808 * sampleRate))
	      - delayR_40.sampleFull(cast(int) (0.00322 * sampleRate))
        - delayR_49.sampleFull(cast(int) (0.01371 * sampleRate))) * 0.219
        + (delayL_37.sampleFull(cast(int) (0.04607 * sampleRate))
        + allpass2L_25_27.getz1(cast(int) (0.00355 * sampleRate))
        + allpass2L_25_27.getz2(cast(int) (0.01407 * sampleRate))
        - allpass2R_43_45.getz2(cast(int) (0.00302 * sampleRate))
        + allpass3L_34_37.getz1(cast(int) (0.00076 * sampleRate))
        + allpass3L_34_37.getz2(cast(int) (0.02286 * sampleRate))
        + allpass3L_34_37.getz3(cast(int) (0.03516 * sampleRate))
        - allpass3R_52_55.getz2(cast(int) (0.00908 * sampleRate))) * 0.064
        + delayL_37.sampleFull(cast(int) (0.02286 * sampleRate)) * 0.045;

      float bOut = delayR_40.sampleFull(cast(int) (0.01832 * sampleRate)) * 0.469
	      + (delayR_49.sampleFull(cast(int) (0.01371 * sampleRate))
        - delayL_31.sampleFull(cast(int) (0.00914 * sampleRate))
        + delayR_49.sampleFull(cast(int) (0.00070 * sampleRate))
        - delayL_37.sampleFull(cast(int) (0.00105 * sampleRate))
	      - delayL_23.sampleFull(cast(int) (0.02315 * sampleRate))
        - delayL_31.sampleFull(cast(int) (0.00554 * sampleRate))) * 0.219
	      + (delayR_58.sampleFull(cast(int) (0.00023 * sampleRate))
        + allpass2R_43_45.getz1(cast(int) (0.00029 * sampleRate))
        + allpass2R_43_45.getz2(cast(int) (0.01052 * sampleRate))
        - allpass2L_25_27.getz2(cast(int) (0.00088 * sampleRate))
	      + allpass3R_52_55.getz1(cast(int) (0.00029 * sampleRate))
        + allpass3R_52_55.getz2(cast(int) (0.00319 * sampleRate))
        + allpass3R_52_55.getz3(cast(int) (0.03839 * sampleRate))
	      - allpass3L_34_37.getz2(cast(int) (0.02344 * sampleRate))) * 0.064
        + delayR_58.sampleFull(cast(int) (0.00029 * sampleRate)) * 0.045;

      // TODO: Comb filters
      outL = dOut;
      outR = bOut;

      predelayL.feedSample(widthMult1 * outL + widthMult2 * outR);
      predelayR.feedSample(widthMult1 * outR + widthMult2 * outL);

      int predelaySamples = cast(int)(predelaySeconds * sampleRate);

      leftOut[f] = highCutL.nextSample(
        lowCutL.nextSample(
          predelayL.sampleFull(predelaySamples),
          lowCutCoefficient),
        highCutCoefficient);

      rightOut[f] = highCutR.nextSample(
        lowCutR.nextSample(
          predelayR.sampleFull(predelaySamples),
          lowCutCoefficient),
        highCutCoefficient);
    }
  }

  override void reset(double sampleRate, int maxFrames) {
    this.sampleRate = sampleRate;
    this.maxFrames = maxFrames;

    int maxPredelaySamples = cast(int) (maxPredelaySeconds * sampleRate);
    predelayL.resize(maxPredelaySamples);
    predelayR.resize(maxPredelaySamples);

    setDecaySeconds(decaySeconds);

    // TODO: Make these prime!
    allpass2L_25_27.setSize(cast(int) (0.01272 * sampleRate * maxSize), cast(int) (0.01016 * sampleRate * maxSize));
    allpass2R_43_45.setSize(cast(int) (0.01219 * sampleRate * maxSize), cast(int) (0.01067 * sampleRate * maxSize));
    allpass3L_34_37.setSize(cast(int) (0.00639 * sampleRate * maxSize), cast(int) (0.00422 * sampleRate * maxSize), cast(int) (0.00358 * sampleRate * maxSize));
    allpass3R_52_55.setSize(cast(int) (0.00692 * sampleRate * maxSize), cast(int) (0.00410 * sampleRate * maxSize), cast(int) (0.00384 * sampleRate * maxSize));

    highCutCoefficient = biquadRBJLowPass(highCut, sampleRate);
    lowCutCoefficient = biquadRBJHighPass(lowCut, sampleRate);

    // Reset all biquads
    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();

    // Reset delays on allpass filters to accomodate new sample rate
    setSize(this.size); 
  }

  void setPredelaySeconds(float predelaySeconds) {
      this.predelaySeconds = predelaySeconds;
  }

  float getDecaySeconds() {
    return decaySeconds;
  }

  void setDecaySeconds(float decaySeconds) {
    this.decaySeconds = decaySeconds;

    // loopdecay = pow(10.0, log10(DECAY_0) * size / decaySeconds),
    float decay1 = pow(10.0, log10(DECAY_1) * size / decaySeconds);
    float decay2 = pow(10.0, log10(DECAY_2) * size / decaySeconds);
    float decay3 = pow(10.0, log10(DECAY_3) * size / decaySeconds);

    allpass2L_25_27.setDecay(decay1, decay2);
    allpass2R_43_45.setDecay(decay1, decay2);
    allpass3L_34_37.setDecay(decay1, decay1, decay2);
    allpass3R_52_55.setDecay(decay1, decay1, decay2);

    // allpassmL_15_16.setDecay(decay2);
    // allpassmR_19_20.setDecay(decay2);

    // allpassmL_17_18.setDecay(decay3);
    // allpassmR_21_22.setDecay(decay3);
  }

  // TODO: Get rid of this hack, the main.d should cache known DSP param values.
  float getSize() {
    return this.size;
  }

  void setSize(float size) {
    assert(size <= maxSize);
    assert(size >= 1.0);
    this.size = size;

    // FIXME! This can allocate on the DSP thread.
    // Instead, preallocate the largest size possible
    // Then sample based on current room size.
    delayL_23.resize(cast(int) (0.03092 * sampleRate * size));
    delayL_31.resize(cast(int) (0.01008 * sampleRate * size));
    delayL_37.resize(cast(int) (0.04607 * sampleRate * size));

    delayR_40.resize(cast(int) (0.04278 * sampleRate * size));
    delayR_49.resize(cast(int) (0.01465 * sampleRate * size));
    delayR_58.resize(cast(int) (0.00047 * sampleRate * size));

    allpass2L_25_27.setDelay(cast(int) (0.01272 * sampleRate * size), cast(int) (0.01016 * sampleRate * size));
    allpass2R_43_45.setDelay(cast(int) (0.01219 * sampleRate * size), cast(int) (0.01067 * sampleRate * size));
    allpass3L_34_37.setDelay(cast(int) (0.00949 * sampleRate * size), cast(int) (0.00422 * sampleRate * size), cast(int) (0.00358 * sampleRate * size));
    allpass3L_34_37.setDelay(cast(int) (0.00949 * sampleRate * size), cast(int) (0.00410 * sampleRate * size), cast(int) (0.00384 * sampleRate * size));

    setDecaySeconds(this.decaySeconds);
  }

  void setWidth(float width) {
    widthMult1 = (1 + width) / 2;
    widthMult2 = (1 - width) / 2;
  }

  void setHighCut(float highCut) {
    this.highCut = highCut;
    highCutCoefficient = biquadRBJLowPass(highCut, sampleRate);
  }

  void setLowCut(float lowCut) {
    this.lowCut = lowCut;
    lowCutCoefficient = biquadRBJHighPass(lowCut, sampleRate);
  }

private:
  immutable float maxSize = 10.0;
  immutable float maxPredelaySeconds = 0.100;
  immutable float apfeedback = 0.2;

  double sampleRate;
  int maxFrames;

  float size = 1.0, predelaySeconds = 0.01, decaySeconds = 0.3;
  float lowCut = 1000.0, highCut = 16000.0;

  float widthMult1 = 1.0, widthMult2 = 0.0; // Multipliers for width

  float hpfL = 0.0, lpfL = 0.0, hpfR = 0.0, lpfR = 0.0;

  Delayline!float 
    delayL_23, delayL_31, delayL_37,
    delayR_49, delayR_40, delayR_58;

  // _FV3_(delay) /* delayR_66, */ delayR_49, delayR_ts, delayR_40, delayR_41, delayR_58;

  SecondOrderFeedbackAllpassFilter allpass2L_25_27, allpass2R_43_45;
  ThirdOrderFeedbackAllpassFilter allpass3L_34_37, allpass3R_52_55;

  Delayline!float predelayL, predelayR;
  BiquadCoeff lowCutCoefficient, highCutCoefficient;
  BiquadDelay lowCutL, lowCutR, highCutL, highCutR;
}

struct SecondOrderFeedbackAllpassFilter {
  void mute() nothrow @nogc {
    setSize(size1, size2); // Reallocates buffer and fills with zeros
  }

  float process(float input) nothrow @nogc {
    float sample1 = buffer1.sampleFull(delay1);
    float sample2 = buffer2.sampleFull(delay2);

    float output = decay2 * sample2;

    input += feedback2 * sample2;
    output -= input * feedback2;
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
    setDelay(0, 0); // Delay should be set after resize, but just in case
  }

  void setDelay(int delay1, int delay2) nothrow @nogc {
    assert(delay1 < size1);
    assert(delay2 < size2);
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
    setDelay(0, 0, 0); // Delay should be set after resize, but just in case
  }

  void setDelay(int delay1, int delay2, int delay3) nothrow @nogc {
    assert(delay1 < size1);
    assert(delay2 < size2);
    assert(delay3 < size3);
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