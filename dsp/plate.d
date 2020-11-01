module dsp.plate;

import dplug.dsp.delayline;

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

  }
  override void mute() {
    // TODO: There may be a better way to do this that doesn't reallocate the delay buffers.
    reset(this.sampleRate, this.maxFrames);
  }

  override void processAudio(float[] leftIn, float[] rightIn, float[] leftOut, float[] rightOut, int frames) {

    for (int f = 0; f < frames; f++) {

      hpfL = 0.5 * inDCCutLeft.process(leftIn[f]) - 0.5 * hpfL;
      hpfR = 0.5 * inDCCutRight.process(rightIn[f]) - 0.5 * hpfR;

      float outL = hpfL + apfeedback * lastL;
      lastL += -1 * apfeedback * outL;
      for(int i = 0; i < combL.length; i++) outL += combL[i].process(hpfL);
      // TODO: Implement left allpass filters!!
      outL = outDCCutLeft.process(outL);

      float outR = hpfR + apfeedback * lastR;
      lastR += -1 * apfeedback * outR;
      for(int i = 0; i < combR.length; i++) outR += combR[i].process(hpfR);
      // TODO: Implement right allpass filters!!
      outR = outDCCutRight.process(outR);

      // TODO: Implement width!
      predelayL.feedSample(lastL);
      predelayR.feedSample(lastR);

      int predelaySamples = cast(int)(predelaySeconds * sampleRate);

      // TODO: Implement high cut and low cut
      leftOut[f] = predelayL.sampleFull(predelaySamples);
      rightOut[f] = predelayL.sampleFull(predelaySamples);

      lastL = outL;
      lastR = outR;
    }
  }

  override void reset(double sampleRate, int maxFrames) {
    this.sampleRate = sampleRate;
    this.maxFrames = maxFrames;

    inDCCutLeft.setCutOnFreq(8.0, sampleRate);
    inDCCutRight.setCutOnFreq(8.0, sampleRate);
    outDCCutLeft.setCutOnFreq(8.0, sampleRate);
    outDCCutRight.setCutOnFreq(8.0, sampleRate);

    inDCCutLeft.mute();
    inDCCutRight.mute();
    outDCCutLeft.mute();
    outDCCutRight.mute();

    int maxPredelaySamples = cast(int) (maxPredelaySeconds * sampleRate);
    predelayL.resize(maxPredelaySamples);
    predelayR.resize(maxPredelaySamples);

    // TODO: Make the sizes prime numbers
    // TODO: Offset the right channel like nrevb
    for(int i = 0; i < combL.length; i++) {
      combL[i].setSize(cast(int)(combSizes[i] * sampleRate));
    }

    for(int i = 0; i < combR.length; i++) {
      combR[i].setSize(cast(int)(combSizes[i] * sampleRate));
    }

    setDecaySeconds(decaySeconds);
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

private:
  immutable float maxPredelaySeconds = 0.100;
  immutable float apfeedback = 0.2;

  double sampleRate;
  int maxFrames;

  float predelaySeconds = 0.01, decaySeconds = 0.3;

  float hpfL = 0.0, lpfL = 0.0, lastL = 0.0;
  float hpfR = 0.0, lpfR = 0.0, lastR = 0.0;

  DCCut inDCCutLeft, inDCCutRight, outDCCutLeft, outDCCutRight;
  FeedbackCombFilter[18] combL, combR;
  Delayline!float predelayL, predelayR;
}