module dsp.plate;

import dplug.dsp.delayline;

import dsp.effects;

final class PlateEffect : NoEffect
{
public:
nothrow:
@nogc:
  this() {

  }
  
  override void processAudio(float[] leftIn, float[] rightIn, float[] leftOut, float[] rightOut, int frames) {
    for (int f = 0; f < frames; f++) {
      predelayL.feedSample(leftIn[f]);
      predelayR.feedSample(rightIn[f]);

      int predelaySamples = cast(int)(predelaySeconds * sampleRate);
      float l = predelayL.sampleFull(predelaySamples);
      float r = predelayL.sampleFull(predelaySamples);

      leftOut[f] = l;
      rightOut[f] = r;
    }
  }

  override void reset(double sampleRate, int maxFrames) {
    this.sampleRate = sampleRate;
    this.maxFrames = maxFrames;

    int maxPredelaySamples = cast(int) (maxPredelaySeconds * sampleRate);
    predelayL.resize(maxPredelaySamples);
    predelayR.resize(maxPredelaySamples);
  }

  void setPredelaySeconds(float predelaySeconds) {
      this.predelaySeconds = predelaySeconds;
  }

private:
  immutable float maxPredelaySeconds = 0.100;

  double sampleRate;
  int maxFrames;

  float predelaySeconds = 0.01;

  Delayline!float predelayL, predelayR;
}