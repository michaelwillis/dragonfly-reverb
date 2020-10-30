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

class UIEffect : UIElement {
public:
nothrow:
@nogc:

  this(Style style, Client client, bool firstEffect /* if false, this is effect #2 */ ) {
    super(style.context, flagRaw);
    this.style = style;
    this.client = client;
    this.firstEffect = firstEffect;

    this.customControls = mallocNew!UIElement(context, flagRaw);
    addChild(this.customControls);
  }

  void selectEffect(int effect) {
    removeChild(this.customControls);
    this.customControls.destroyFree();
    this.customControls = mallocNew!UIElement(context(), 1);
    this.customControls.position = box2i(16, 136, 624, 464);

    if (effect != noEffect) {
      if (firstEffect) {
        style.gainSlider(this, paramEffect1Send, "Send Effect 2", "%3.1f dB", 0, 288);
      }

      style.gainSlider(this, paramEffect1Gain, "Effect 1 Gain", "%3.1f dB", 0, 312);
    }

    switch(effect) {
    case noEffect:
      // Nothing
      break;
      
    case earlyEffect:
      style.selectBox(customControls, paramEffect1EarlyReflectionPattern, "Reflection Pattern", 0, 24);

      style.largeKnob(customControls, paramEffect1Size, "Size", "%2.0f m", 192, 24);
      style.largeKnob(customControls, paramEffect1Width, "Width", "%3.0f%%", 192, 120);

      style.smallKnob(customControls, paramEffect1HighCut, "High Cut", "%5.0f Hz", 48, 200);
      style.smallKnob(customControls, paramEffect1LowCut, "Low Cut", "%5.0f Hz", 176, 200);
      break;
    default:
      break;
    }

    addChild(customControls);
  }

private:
  Style style;
  Client client;
  immutable bool firstEffect;
  UIElement customControls;
}
