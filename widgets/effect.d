/**
Effect container UI element
*/
module widgets.effect;

import dplug.client;
import dplug.gui.element;

import main; // Maybe TODO - move params enum out of main?
import gui; // Maybe TODO - move style out of gui?
import widgets.label;
import widgets.select;
import dsp.effects;

class UIEffect : UIElement, IParameterListener {
public:
nothrow:
@nogc:

  this(Style style, Client client, bool firstEffect /* if false, this is effect #2 */ ) {
    super(style.context, flagRaw);
    this.style = style;
    this.client = client;
    this.firstEffect = firstEffect;

    if (firstEffect) {
      paramGain = paramEffect1Gain;
      paramAlgorithm = paramEffect1Algorithm;
      paramEarlyReflectionPattern = paramEffect1EarlyReflectionPattern;
      paramSize = paramEffect1Size;
      paramWidth = paramEffect1Width;
      paramPredelay = paramEffect1Predelay;
      paramDecay = paramEffect1Decay;
      paramDiffuse = paramEffect1Diffuse;
      paramModulation = paramEffect1Modulation;
      paramSpin = paramEffect1Spin;
      paramWander = paramEffect1Wander;
      paramHighCross = paramEffect1HighCross;
      paramHighMult = paramEffect1HighMult;
      paramHighCut = paramEffect1HighCut;
      paramHighCutHard = paramEffect1HighCutHard;
      paramLowCross = paramEffect1LowCross;
      paramLowMult = paramEffect1LowMult;
      paramLowCut = paramEffect1LowCut;
      paramLowCutHard = paramEffect1LowCutHard;
    } else {
      paramGain = paramEffect2Gain;
      paramAlgorithm = paramEffect2Algorithm;
      paramEarlyReflectionPattern = paramEffect2EarlyReflectionPattern;
      paramSize = paramEffect2Size;
      paramWidth = paramEffect2Width;
      paramPredelay = paramEffect2Predelay;
      paramDecay = paramEffect2Decay;
      paramDiffuse = paramEffect2Diffuse;
      paramModulation = paramEffect2Modulation;
      paramSpin = paramEffect2Spin;
      paramWander = paramEffect2Wander;
      paramHighCross = paramEffect2HighCross;
      paramHighMult = paramEffect2HighMult;
      paramHighCut = paramEffect2HighCut;
      paramHighCutHard = paramEffect2HighCutHard;
      paramLowCross = paramEffect2LowCross;
      paramLowMult = paramEffect2LowMult;
      paramLowCut = paramEffect2LowCut;
      paramLowCutHard = paramEffect2LowCutHard;
    }

    this.customControls = mallocNew!UIElement(context, flagRaw);
    addChild(this.customControls);

    client.param(paramAlgorithm).addListener(this);
  }

  ~this() {
    client.param(paramAlgorithm).removeListener(this);
  }

  void selectEffect(int effect) {
    context().focused = null; // Avoid segfault, context tries to call onFocusExit()
    removeChild(customControls);
    customControls.destroyFree();
    customControls = mallocNew!UIElement(context(), 1);
    style.place(this, 0, 0, 304, 320, customControls);

    if (effect != noEffect) {
      if (firstEffect) {
        style.gainSlider(customControls, paramEffect1Send, "Send Effect 2", "%3.1f dB", 0, 272);
      }

      style.gainSlider(customControls, paramGain, firstEffect ? "Effect 1 Gain" : "Effect 2 Gain", "%3.1f dB", 0, 296);
    }

    if (effect == earlyEffect) {
      style.selectBox(customControls, paramEarlyReflectionPattern, "Reflection Pattern", 0, 8);

      style.largeKnob(customControls, paramSize, "Size", "%2.0f m", 192, 8);
      style.largeKnob(customControls, paramWidth, "Width", "%3.0f%%", 192, 104);

      style.smallKnob(customControls, paramHighCut, "High Cut", "%5.0f Hz", 48, 202);
      style.smallKnob(customControls, paramLowCut, "Low Cut", "%5.0f Hz", 176, 202);
    } else if (effect == plateEffect) {
      // Divide into a 3 x 2 grid.  Width of 288 / 3 = 96 pixels per dial
      // Large dials are 52 pixels wide, 96 - 52 = 44, so each gets a margin of 22
      int x1 = 22, x2 = 96 + 22, x3 = 192 + 22;
      int y1 = 22, y2 = 96 + 22;

      style.largeKnob(customControls, paramPredelay, "Predelay", "%3.0f ms", x1, y1);
      style.largeKnob(customControls, paramWidth, "Width", "%3.0f%%", x2, y1);
      style.largeKnob(customControls, paramDecay, "Decay", "%2.1f s", x3, y1);

      style.largeKnob(customControls, paramLowCut, "Low Cut", "%5.0f Hz", x1, y2);
      style.largeKnob(customControls, paramHighCut, "High Cut", "%5.0f Hz", x2, y2);
    } else if (effect == roomEffect || effect == hallEffect) {

      int x1 = 0, x2 = 72, x3 = 144, x4 = 216;
      int y1 = 4, y2 = 70, y3 = 136, y4 = 202;

      style.smallKnob(customControls, paramPredelay, "Predelay", "%3.0f ms", x1, y1);
      style.smallKnob(customControls, paramSize, "Size", "%2.0f m", x2, y1);
      style.smallKnob(customControls, paramWidth, "Width", "%3.0f%%", x3, y1);
      style.smallKnob(customControls, paramDecay, "Decay", "%2.1f s", x4, y1);

      style.smallKnob(customControls, paramDiffuse, "Diffuse", "%3.0f%%", x1, y2);
      style.smallKnob(customControls, paramModulation, "Modulation", "%3.0f%%", x2, y2);
      style.smallKnob(customControls, paramSpin, "Spin", "%2.1f Hz", x3, y2);
      style.smallKnob(customControls, paramWander, "Wander", "%3.0f%%", x4, y2);

      style.smallKnob(customControls, paramHighCross, "High Cross", "%5.0f Hz", x1, y3);
      style.smallKnob(customControls, paramHighMult, "High Mult", "%1.1f X", x2, y3);
      style.smallKnob(customControls, paramHighCut, "High Cut", "%5.0f Hz", x3, y3);
      // style.smallKnob(customControls, paramLowCut, "Low Cut", "%5.0f Hz", x4, y3);

      style.smallKnob(customControls, paramLowCross, "Low Cross", "%5.0f Hz", x1, y4);
      style.smallKnob(customControls, paramLowMult, "Low Mult", "%1.1f X", x2, y4);
      style.smallKnob(customControls, paramLowCut, "Low Cut", "%5.0f Hz", x3, y4);
      // style.smallKnob(customControls, paramLowCut, "Low Cut", "%5.0f Hz", x4, y4);
    }
  }

  override void onParameterChanged(Parameter sender) {
    if (sender.index() == paramAlgorithm) {
      selectEffect((cast(EnumParameter) sender).value());
      setDirtyWhole(); // TODO: Only dirty old and new selection?
    }
  }

  override void onBeginParameterEdit(Parameter sender) { }

  override void onEndParameterEdit(Parameter sender) { }

private:
  Style style;
  Client client;
  immutable bool firstEffect;
  UIElement customControls;

  immutable int 
    paramGain,
    paramAlgorithm,
    paramEarlyReflectionPattern,
    paramSize,
    paramWidth,
    paramPredelay,
    paramDecay,
    paramDiffuse,
    paramModulation,
    paramSpin,
    paramWander,
    paramHighCross,
    paramHighMult,
    paramHighCut,
    paramHighCutHard,
    paramLowCross,
    paramLowMult,
    paramLowCut,
    paramLowCutHard;

}
