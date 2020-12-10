module dsp.room;

import dplug.dsp.delayline;
import dplug.dsp.iir;

import dsp.effects;
import dsp.filters;

import std.math;

immutable float DECAY_0 = 0.237, DECAY_1 = 0.938, DECAY_2 = 0.844, DECAY_3 = 0.906;
immutable float DIFFUSE_1 = 0.375, DIFFUSE_2 = 0.312, DIFFUSE_3 = 0.406, DIFFUSE_4 = 0.25;

immutable float[] INPUT_ALLPASS_DELAY_LEFT = [
  0.01808, 0.01568, 0.01272, 0.01017, 0.00639, 0.00475, 0.00422, 0.00358, 0.00319, 0.00217
];

immutable float[] INPUT_ALLPASS_DELAY_RIGHT = [
  0.01767, 0.01603, 0.01219, 0.01067, 0.00692, 0.00475, 0.00410, 0.00384, 0.00325, 0.00232
];

immutable float[] CROSS_ALLPASS_DELAY_LEFT = [0.01260, 0.00999, 0.00773, 0.00510];
immutable float[] CROSS_ALLPASS_DELAY_RIGHT = [0.0131, 0.00949, 0.00724, 0.00560];

immutable float CROSSFEED = 0.40;
immutable int INPUT_DAMP_FREQ = 20000;
immutable int DAMP_FREQ = 9000;

final class RoomEffect : NoEffect
{
public:
nothrow:
@nogc:
  this() {
    lpfL_in_59_60.initialize();
    lpfR_in_64_65.initialize();
    lpfLdamp_11_12.initialize();
    lpfRdamp_13_14.initialize();

    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();

    setDiffusion(0.78);

    allpassL_15_16.setFeedback(DIFFUSE_1);
    allpassR_19_20.setFeedback(DIFFUSE_1);

    allpassL_17_18.setFeedback(DIFFUSE_2);
    allpassR_21_22.setFeedback(DIFFUSE_2);

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

      // Input Diffusion
      for(int i = 0; i < 10; i ++) { // TODO: Get rid of magic number 10
        // Maybe modulate these like ProG does?
	      outL = inputAllpassL[i].process(outL);
	      outR = inputAllpassR[i].process(outR);
	    }

      // Crossfeed Diffusion
      float crossL = outL, crossR = outR;
      for(int i = 0; i < 4; i ++) {  // TODO: Get rid of magic number 4
	      crossL = crossAllpassL[i].process(crossL);
	      crossR = crossAllpassR[i].process(crossR);
	    }

      outL = lpfL_in_59_60.nextSample(outL + CROSSFEED * crossR, inputDampCoefficient);
      outR = lpfR_in_64_65.nextSample(outR + CROSSFEED * crossL, inputDampCoefficient);

      // Another crossfeed, this one with decay based on the rt60
      outL += loopdecay * delayR_58.sampleFull(cast(int) (0.00047 * sampleRate * size));
      outR += loopdecay * delayL_37.sampleFull(cast(int) (0.04607 * sampleRate * size));

      outL = allpassL_17_18.process(allpassL_15_16.process(lpfLdamp_11_12.nextSample(outL, dampCoefficient)));
      outR = allpassR_21_22.process(allpassR_19_20.process(lpfRdamp_13_14.nextSample(outR, dampCoefficient)));

      delayL_23.feedSample(outL);
      delayL_31.feedSample(allpass2L_25_27.process(delayL_23.sampleFull(cast(int) (0.03092 * sampleRate * size))));
      delayL_37.feedSample(allpass3L_34_37.process(delayL_31.sampleFull(cast(int) (0.01008 * sampleRate * size))));

      delayR_40.nextSample(outR);
      delayR_49.nextSample(allpass2R_43_45.process(delayR_40.sampleFull(cast(int) (0.04278 * sampleRate * size))));
      delayR_58.feedSample(allpass3R_52_55.process(delayR_49.sampleFull(cast(int) (0.01465 * sampleRate * size))));

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

    for(long i = 0; i < 10; i ++) {
      inputAllpassL[i].setSize(cast(int) (INPUT_ALLPASS_DELAY_LEFT[i] * sampleRate * maxSize));
      inputAllpassR[i].setSize(cast(int) (INPUT_ALLPASS_DELAY_RIGHT[i] * sampleRate * maxSize));
    }

    for(long i = 0; i < 4; i ++) {
      crossAllpassL[i].setSize(cast(int) (CROSS_ALLPASS_DELAY_LEFT[i] * sampleRate * maxSize));
      crossAllpassR[i].setSize(cast(int) (CROSS_ALLPASS_DELAY_RIGHT[i] * sampleRate * maxSize));
    }

    allpassL_15_16.setSize(cast(int) (0.01808 * sampleRate * maxSize));
    allpassL_17_18.setSize(cast(int) (0.01568 * sampleRate * maxSize));
    allpassR_19_20.setSize(cast(int) (0.01767 * sampleRate * maxSize));
    allpassR_21_22.setSize(cast(int) (0.01603 * sampleRate * maxSize));

    allpass2L_25_27.setSize(cast(int) (0.01272 * sampleRate * maxSize), cast(int) (0.01016 * sampleRate * maxSize));
    allpass2R_43_45.setSize(cast(int) (0.01219 * sampleRate * maxSize), cast(int) (0.01067 * sampleRate * maxSize));
    allpass3L_34_37.setSize(cast(int) (0.00639 * sampleRate * maxSize), cast(int) (0.00422 * sampleRate * maxSize), cast(int) (0.00358 * sampleRate * maxSize));
    allpass3R_52_55.setSize(cast(int) (0.00692 * sampleRate * maxSize), cast(int) (0.00410 * sampleRate * maxSize), cast(int) (0.00384 * sampleRate * maxSize));

    inputDampCoefficient = biquadRBJLowPass(INPUT_DAMP_FREQ, sampleRate);
    dampCoefficient = biquadRBJLowPass(DAMP_FREQ, sampleRate);
    highCutCoefficient = biquadRBJLowPass(highCut, sampleRate);
    lowCutCoefficient = biquadRBJHighPass(lowCut, sampleRate);

    // Reset all biquads
    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();

    delayL_23.resize(cast(int) (0.03092 * sampleRate * maxSize));
    delayL_31.resize(cast(int) (0.01008 * sampleRate * maxSize));
    delayL_37.resize(cast(int) (0.04607 * sampleRate * maxSize));

    delayR_40.resize(cast(int) (0.04278 * sampleRate * maxSize));
    delayR_49.resize(cast(int) (0.01465 * sampleRate * maxSize));
    delayR_58.resize(cast(int) (0.00047 * sampleRate * maxSize));

    // Reset delays on allpass filters to accomodate new sample rate
    setSize(this.size); 
  }

  void setPredelaySeconds(float predelaySeconds) {
      this.predelaySeconds = predelaySeconds;
  }

  void setDecaySeconds(float decaySeconds) {
    this.decaySeconds = decaySeconds;

    loopdecay = pow(10.0, log10(DECAY_0) * size / decaySeconds);
    float decay1 = pow(10.0, log10(DECAY_1) * size / decaySeconds);
    float decay2 = pow(10.0, log10(DECAY_2) * size / decaySeconds);
    float decay3 = pow(10.0, log10(DECAY_3) * size / decaySeconds);

    allpass2L_25_27.setDecay(decay1, decay2);
    allpass2R_43_45.setDecay(decay1, decay2);
    allpass3L_34_37.setDecay(decay1, decay1, decay2);
    allpass3R_52_55.setDecay(decay1, decay1, decay2);

    allpassL_15_16.setDecay(decay2);
    allpassR_19_20.setDecay(decay2);
    allpassL_17_18.setDecay(decay3);
    allpassR_21_22.setDecay(decay3);
  }

  void setSize(float size) {
    assert(size <= maxSize);
    assert(size >= 1.0);
    this.size = size;

    for(long i = 0; i < 10; i ++) {
      inputAllpassL[i].setDelay(cast(int) (INPUT_ALLPASS_DELAY_LEFT[i] * sampleRate * size));
      inputAllpassR[i].setDelay(cast(int) (INPUT_ALLPASS_DELAY_RIGHT[i] * sampleRate * size));
    }

    for(long i = 0; i < 4; i ++) {
      crossAllpassL[i].setDelay(cast(int) (CROSS_ALLPASS_DELAY_LEFT[i] * sampleRate * size));
      crossAllpassR[i].setDelay(cast(int) (CROSS_ALLPASS_DELAY_RIGHT[i] * sampleRate * size));
    }

    allpassL_15_16.setDelay(cast(int) (0.01808 * sampleRate * size));
    allpassL_17_18.setDelay(cast(int) (0.01568 * sampleRate * size));
    allpassR_19_20.setDelay(cast(int) (0.01767 * sampleRate * size));
    allpassR_21_22.setDelay(cast(int) (0.01603 * sampleRate * size));

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

  void setDiffusion(float diffusion) {

    for(long i = 0; i < 10; i ++)
    {
      inputAllpassL[i].setFeedback(-1.0 * diffusion);
      inputAllpassR[i].setFeedback(-1.0 * diffusion);
    }

    for(long i = 0; i < 4; i ++)
    {
      crossAllpassL[i].setFeedback(diffusion);
      crossAllpassR[i].setFeedback(diffusion);
    }
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

  float loopdecay = DECAY_0;

  Delayline!float 
    delayL_23, delayL_31, delayL_37,
    delayR_49, delayR_40, delayR_58;

  // More consistent name for delayR_ts?
  // _FV3_(delay) delayR_ts, delayR_41;

  FirstOrderFeedbackAllpassFilter[10] inputAllpassL, inputAllpassR;
  FirstOrderFeedbackAllpassFilter[4] crossAllpassL, crossAllpassR;

  FirstOrderFeedbackAllpassFilter allpassL_15_16, allpassR_19_20, allpassL_17_18, allpassR_21_22;

  SecondOrderFeedbackAllpassFilter allpass2L_25_27, allpass2R_43_45;
  ThirdOrderFeedbackAllpassFilter allpass3L_34_37, allpass3R_52_55;

  Delayline!float predelayL, predelayR;
  BiquadCoeff inputDampCoefficient, dampCoefficient, lowCutCoefficient, highCutCoefficient;

  BiquadDelay 
    lpfL_in_59_60, lpfR_in_64_65,
    lpfLdamp_11_12, lpfRdamp_13_14,
    lowCutL, lowCutR, highCutL, highCutR;
}
