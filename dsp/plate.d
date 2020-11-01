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
  float outL, outR, tmpL, tmpR;
    for (int f = 0; f < frames; f++) {
      // TODO: Try true stereo input, with two DCCut instances
      hpf = 0.5 * inDCC.process(leftIn[f] + rightIn[f]) - 0.5 * hpf;
      outL = outR = tmpL = tmpR = hpf;

      outL += apfeedback * lastL;
      lastL += -1 * apfeedback * outL;
      for(int i = 0; i < combL.length; i++) outL += combL[i].process(tmpL);
      // TODO: Implement left allpass filters!!
      outL = lDCC.process(outL);

      outR += apfeedback * lastR;
      lastR += -1 * apfeedback * outR;
      for(int i = 0; i < combR.length; i++) outR += combR[i].process(tmpR);
      // TODO: Implement right allpass filters!!
      outR = rDCC.process(outR);

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

    inDCC.setCutOnFreq(8.0, sampleRate);
    lDCC.setCutOnFreq(8.0, sampleRate);
    rDCC.setCutOnFreq(8.0, sampleRate);

    inDCC.mute();
    lDCC.mute();
    rDCC.mute();

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

  float hpf = 0.0, lpfL = 0.0, lpfR = 0.0, lastL = 0.0, lastR = 0.0;

  DCCut inDCC, lDCC, rDCC;
  FeedbackCombFilter[18] combL, combR;
  Delayline!float predelayL, predelayR;
}