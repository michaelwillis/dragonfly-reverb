module dsp.plate;

import dplug.dsp.delayline;
import dplug.dsp.iir;

import dsp.effects;
import dsp.filters;

import std.math;

// Size in seconds, multiply by sample rate for number of samples
immutable float[] combSizes = [
  0.049023049,
  0.051987052,
  0.055887056,
  0.058461058,
  0.058539059,
  0.05986506,
  0.062439062,
  0.063843064,
  0.067782068,
  0.072813073,
  0.074256074,
  0.077961078,
  0.078780079,
  0.08006708,
  0.082680083,
  0.087789088,
  0.093561094,
  0.097851098
];

immutable float[] allpassSizes = [
  0.013533014,
  0.004407004,
  0.001443001,
  0.002223002,
  0.003393003,
  0.004017004
];

final class PlateEffect : NoEffect
{
public:
nothrow:
@nogc:
  this() {
    assert(combSizes.length == combL.length);
    assert(combSizes.length == combR.length);

    for(int i = 0; i < combL.length; i++) {
      combL[i].setDamp(0.1);
    }

    for(int i = 0; i < combR.length; i++) {
      combR[i].setDamp(0.1);
    }

    assert(allpassSizes.length == allpassL.length);
    assert(allpassSizes.length == allpassR.length);

    for(int i = 0; i < allpassL.length; i++) allpassL[i].setFeedback(0.5);
    for(int i = 0; i < allpassR.length; i++) allpassR[i].setFeedback(0.5);
    allpassLAfterLPF[0].setFeedback(0.5);
    allpassLAfterLPF[1].setFeedback(0.5);
    allpassRAfterLPF[0].setFeedback(0.5);
    allpassRAfterLPF[1].setFeedback(0.5);

    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();
  }
  override void mute() {
    // TODO: There may be a better way to do this that doesn't reallocate the delay buffers.
    reset(this.sampleRate, this.maxFrames);
  }

  override void processAudio(float[] leftIn, float[] rightIn, float[] leftOut, float[] rightOut, int frames) {
    for (int f = 0; f < frames; f++) {
      float outL = 0.0;
      float outR = 0.0;

      hpfL = 0.5 * leftIn[f] - 0.5 * hpfL;
      for(int i = 0; i < combL.length; i++) outL += combL[i].process(hpfL);
      for(int i = 0; i < allpassL.length; i++) outL = allpassL[i].process(outL);
      lpfL = 0.5 * lpfL + 0.5 * outL;
      outL = allpassLAfterLPF[0].process(lpfL);
      outL = allpassLAfterLPF[1].process(outL);

      hpfR = 0.5 * rightIn[f] - 0.5 * hpfR;
      for(int i = 0; i < combR.length; i++) outR += combR[i].process(hpfR);
      for(int i = 0; i < allpassR.length; i++) outR = allpassR[i].process(outR);
      lpfR = 0.5 * lpfR + 0.5 * outR;
      outR = allpassRAfterLPF[0].process(lpfR);
      outR = allpassRAfterLPF[1].process(outR);

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

    // TODO: Make the comb and allpass sizes prime numbers
    // TODO: Offset the right channel like nrevb
    for(int i = 0; i < combL.length; i++) combL[i].setSize(cast(int)(combSizes[i] * sampleRate));
    for(int i = 0; i < combR.length; i++) combR[i].setSize(cast(int)(combSizes[i] * sampleRate));
    for(int i = 0; i < allpassL.length; i++) allpassL[i].setSize(cast(int)(allpassSizes[i] * sampleRate));
    for(int i = 0; i < allpassR.length; i++) allpassR[i].setSize(cast(int)(allpassSizes[i] * sampleRate));

    allpassLAfterLPF[0].setSize(cast(int)(sampleRate * 59.0 / 25641));
    allpassLAfterLPF[1].setSize(cast(int)(sampleRate * 43.0 / 25641));
    allpassRAfterLPF[0].setSize(cast(int)(sampleRate * 59.0 / 25641));
    allpassRAfterLPF[1].setSize(cast(int)(sampleRate * 37.0 / 25641));

    setDecaySeconds(decaySeconds);

    highCutCoefficient = biquadRBJLowPass(highCut, sampleRate);
    lowCutCoefficient = biquadRBJHighPass(lowCut, sampleRate);

    // Reset all biquads
    lowCutL.initialize();
    lowCutR.initialize();
    highCutL.initialize();
    highCutR.initialize();
  }

  void setPredelaySeconds(float predelaySeconds) {
      this.predelaySeconds = predelaySeconds;
  }

  float getDecaySeconds() {
    return decaySeconds;
  }

  void setDecaySeconds(float decaySeconds) {
    this.decaySeconds = decaySeconds;

    for(int i = 0; i < combL.length; i++) {
      combL[i].setFeedback(pow(10.0, -3.0 * cast(float)combL[i].getSize() / (decaySeconds * sampleRate)));
    }

    for(int i = 0; i < combR.length; i++) {
      combR[i].setFeedback(pow(10.0, -3.0 * cast(float)combR[i].getSize() / (decaySeconds * sampleRate)));
    }
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
  immutable float maxPredelaySeconds = 0.100;
  immutable float apfeedback = 0.2;

  double sampleRate;
  int maxFrames;

  float predelaySeconds = 0.01, decaySeconds = 0.3;
  float lowCut = 1000.0, highCut = 16000.0;

  float widthMult1 = 1.0, widthMult2 = 0.0; // Multipliers for width

  float hpfL = 0.0, lpfL = 0.0, hpfR = 0.0, lpfR = 0.0;

  FeedbackCombFilter[18] combL, combR;
  FirstOrderFeedbackAllpassFilter[6] allpassL, allpassR;
  FirstOrderFeedbackAllpassFilter[2] allpassLAfterLPF;
  FirstOrderFeedbackAllpassFilter[2] allpassRAfterLPF;

  Delayline!float predelayL, predelayR;
  BiquadCoeff lowCutCoefficient, highCutCoefficient;
  BiquadDelay lowCutL, lowCutR, highCutL, highCutR;
}