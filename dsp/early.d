module dsp.early;

import dplug.core.nogc;
import dplug.dsp.delayline;
import dplug.dsp.iir;
import dsp.effects;

enum : int {DELAY, GAIN};

struct ReflectionPattern
{
  immutable string name;
  immutable float[2][] left, right;
};

immutable ReflectionPattern[] patterns = [
  ReflectionPattern(
    "Abrupt Echo",
    [[0.0094,  1.35],
     [0.0126, -1.15],
     [0.0205,  1.15],
     [0.0213, -1.14]],
    [[0.0094,  1.36],
     [0.0134, -1.16],
     [0.0203, -1.0],
     [0.0230,  1.14]]),
  ReflectionPattern(
    "Backstage Pass",
    [[0.004350,  0.84],
     [0.009656,  0.833333],
     [0.012036, -0.733333],
     [0.015625,  1.023333],
     [0.019521,  0.69],
     [0.019531,  0.683333],
     [0.024615,  0.906667],
     [0.032896, -0.903333],
     [0.040786,  0.666667],
     [0.046208,  0.726667],
     [0.050464,  0.66]],
    [[0.004350,  0.85],
     [0.009640,  0.823333],
     [0.014000, -0.876667],
     [0.015312,  0.823333],
     [0.019276,  0.836667],
     [0.024344,  0.853333],
     [0.024406,  0.166667],
     [0.031292, -0.903333],
     [0.031354, -0.173333],
     [0.042391,  0.783333],
     [0.045625,  0.88],
     [0.045693,  0.14],
     [0.050609,  0.79]]),
  ReflectionPattern(
    "Concert Venue",
    [[0.0048, 0.841],
     [0.0218, 0.504],
     [0.0227, 0.491],
     [0.0270, 0.379],
     [0.027, 0.38],
     [0.0298, 0.346],
     [0.0458, 0.289],
     [0.0485, 0.272],
     [0.0572, 0.192],
     [0.0587, 0.193],
     [0.0595, 0.217],
     [0.0612, 0.181],
     [0.0707, 0.18],
     [0.0708, 0.181],
     [0.0726, 0.176],
     [0.0741, 0.142],
     [0.0753, 0.167],
     [0.0797, 0.134]],
    [[0.0048, 0.842],
     [0.0222, 0.506],
     [0.0233, 0.489],
     [0.0276, 0.382],
     [0.029, 0.3],
     [0.0288, 0.346],
     [0.0468, 0.29],
     [0.0475, 0.271],
     [0.0582, 0.193],
     [0.0577, 0.192],
     [0.0575, 0.217],
     [0.0622, 0.195],
     [0.0697, 0.192],
     [0.0718, 0.166],
     [0.0736, 0.186],
     [0.0751, 0.131],
     [0.0763, 0.168],
     [0.0817, 0.133]]),
  ReflectionPattern(
    "Damaged Goods",
    [[0.006531, 0.833333],
     [0.016114, 0.843333],
     [0.032687, 0.793333],
     [0.041682, 0.783333],
     [0.047917, 0.84],
     [0.055474, -0.793333],
     [0.059266, 0.86],
     [0.064068, 0.716667],
     [0.068786, -0.81],
     [0.075062, 0.733333]],
    [[0.006531, 0.833333],
     [0.015999, 0.863333],
     [0.025599, 0.846667],
     [0.034943, 0.846667],
     [0.04425, -0.836667],
     [0.050255, 0.826667],
     [0.062771, -0.86],
     [0.066646, 0.78],
     [0.066656, 0.776667],
     [0.071807, -0.89],
     [0.078354, -0.836667]]),
  ReflectionPattern(
    "Elevator Pitch",
    [[0.0204, 1.02],
     [0.0354, 0.818],
     [0.0399, 0.635],
     [0.0414, 0.719],
     [0.0699, 0.267],
     [0.0796, 0.242]],
    [[0.0204, 1.021],
     [0.0364, 0.82],
     [0.0389, 0.633],
     [0.0424, 0.722],
     [0.0709, 0.187],
     [0.0806, 0.243]]),
  ReflectionPattern(
    "Floor Thirteen",
    [[0.003268, 0.963333],
     [0.010603, -0.813333],
     [0.018526, 0.7],
     [0.02487, -0.65],
     [0.03774, -0.56],
     [0.048089, 0.526667],
     [0.053948, -0.5],
     [0.061333, 0.486667],
     [0.070073, -0.463333],
     [0.077125, 0.463333]],
    [[0.003118, 0.98],
     [0.010215, -0.856667],
     [0.018042, 0.723333],
     [0.023885, -0.66],
     [0.033062, 0.58],
     [0.033073, 0.58],
     [0.03312, 0.166667],
     [0.042307, 0.55],
     [0.051234, -0.506667],
     [0.059901, 0.476667],
     [0.06799, -0.466667],
     [0.076458, 0.46]]),
  ReflectionPattern(
    "Garage Band",
    [[0.001762, 0.993333],
     [0.008531, -0.873333],
     [0.02301, 0.663333],
     [0.042016, -0.543333],
     [0.055766, 0.503333],
     [0.059432, -0.486667],
     [0.064536, 0.48],
     [0.07124, -0.466667],
     [0.07649, 0.466667],
     [0.07925, -0.463333],
     [0.079292, -0.186667]],
    [[0.001696, 1.013333],
     [0.008927, 0.833333],
     [0.02326, -0.66],
     [0.03974, 0.546667],
     [0.039786, 0.19],
     [0.052427, 0.506667],
     [0.052479, 0.15],
     [0.053552, -0.506667],
     [0.062042, 0.493333],
     [0.06738, 0.473333],
     [0.067391, 0.473333],
     [0.07363, -0.463333],
     [0.07649, 0.473333]]),
  ReflectionPattern(
    "Home Studio",
    [[0.005974, 0.95],
     [0.014313, 0.756667],
     [0.02313, 0.68],
     [0.030646, 0.616667],
     [0.038906, 0.563333],
     [0.048729, 0.523333],
     [0.056156, 0.493333],
     [0.06451, 0.493333],
     [0.064521, 0.446667],
     [0.072859, 0.46],
     [0.081219, 0.456667]],
    [[0.006229, 0.893333],
     [0.007797, 0.146667],
     [0.01701, 0.733333],
     [0.025641, 0.656667],
     [0.033901, 0.576667],
     [0.040318, 0.543333],
     [0.051432, 0.51],
     [0.058495, 0.503333],
     [0.066859, 0.466667],
     [0.075224, 0.47],
     [0.085417, 0.48]])];

final class EarlyEffect : NoEffect
{
public:
nothrow:
@nogc:
  this()
  {
    float maxDelaySeconds = 0;

    foreach (pattern; patterns)
    {
      foreach (delay; pattern.left[DELAY]) {
        if (delay > maxDelaySeconds) maxDelaySeconds = delay;
      }
      foreach (delay; pattern.right[DELAY]) {
        if (delay > maxDelaySeconds) maxDelaySeconds = delay;
      }
    }

    this.maxDelaySeconds = maxDelaySeconds;

    diffuseAllpassL.initialize();
    diffuseAllpassR.initialize();
  }
  
  override void processAudio(float[] leftIn, float[] rightIn,
                             float[] leftOut, float[] rightOut,
                             int frames)
  {
    debugLog("Dragonfly Reverb 4 - Process Audio!");
    // TODO:
    // * Predelay?
    // * Cross-channel delay/allpass
    // * Width
    // * LPF/HPF

    for (int f = 0; f < frames; f++)
    {
      delayLineL.feedSample(leftIn[f]);
      delayLineR.feedSample(rightIn[f]);

      float left = 0;
      float right = 0;

      for(int i = 0; i < tapLengthL; i++)
      {
        left += delayLineL.sampleFull(leftDelays[i]) * leftGains[i];
      }
      
      for(int i = 0; i < tapLengthR; i++)
      {
        right += delayLineR.sampleFull(rightDelays[i]) * rightGains[i];        
      }
      
      leftOut[f]  = diffuseAllpassL.nextSample(left,  diffuseAllpassCoefficient);
      rightOut[f] = diffuseAllpassR.nextSample(right, diffuseAllpassCoefficient);
    }
  }

  override void reset(double sampleRate, int maxFrames)
  {
    this.sampleRate = sampleRate;
    int maxDelaySamples = cast(int) (maxDelaySeconds * sampleRate);
    delayLineL.resize(maxDelaySamples);
    delayLineR.resize(maxDelaySamples);

    diffuseAllpassCoefficient = biquadRBJAllPass(150, sampleRate);

    // Recalculate current reflection pattern due to new sample rate
    selectReflectionPattern(this.selectedReflectionPattern); 
  }

  void selectReflectionPattern(int selectReflectionPattern)
  {
    this.selectedReflectionPattern = selectedReflectionPattern;
    ReflectionPattern pattern = patterns[selectedReflectionPattern];

    tapLengthL = pattern.left.length;
    tapLengthR = pattern.right.length;

    for(int i = 0; i < tapLengthL; i++)
    {
      leftDelays[i] = cast(int) (pattern.left[i][DELAY] * sampleRate);
      leftGains[i] = pattern.left[i][GAIN];
    }

    for(int i = 0; i < tapLengthR; i++)
    {
      rightDelays[i] = cast(int) (pattern.right[i][DELAY] * sampleRate);
      rightGains[i] = pattern.right[i][GAIN];
    }
  }

private:
  immutable float maxDelaySeconds;

  int selectedReflectionPattern = 2;
  double sampleRate;

  long tapLengthL = 0;
  long tapLengthR = 0;
  int[20] leftDelays;
  int[20] rightDelays;
  float[20] leftGains;
  float[20] rightGains;
  
  Delayline!float delayLineL, delayLineR;

  BiquadCoeff diffuseAllpassCoefficient;
  BiquadDelay diffuseAllpassL, diffuseAllpassR;
}

