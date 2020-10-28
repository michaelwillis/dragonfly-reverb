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

  this(Style style, Client client) {
    super(style.context, flagRaw);
    this.style = style;
    this.client = client;

    this.customControls = mallocNew!UIElement(context, flagRaw);
    addChild(this.customControls);

    selectEffect(earlyEffect);
  }

  void selectEffect(int effect) {
    removeChild(this.customControls);
    this.customControls.destroyFree();
    this.customControls = mallocNew!UIElement(context(), 1);
    this.customControls.position = box2i(16, 136, 624, 464);

    switch(effect) {
    case noEffect:
      // Nothing
      break;
      
    case earlyEffect:
      UILabel effect1Label = mallocNew!UILabel(style, "Reflection Pattern", 14, HorizontalAlignment.left);
      effect1Label.position = box2i(16, 160, 148, 176);
      customControls.addChild(effect1Label);

      UISelectBox earlySelect = mallocNew!UISelectBox(
        style, cast(EnumParameter) client.param(paramEffect1EarlyReflectionPattern), 14);
      earlySelect.position = box2i(16, 184, 148, 316);
      customControls.addChild(earlySelect);

      style.largeKnob(customControls, paramEffect1Size, "Size", "%2.0f m", 206, 160);
      style.largeKnob(customControls, paramEffect1Width, "Width", "%3.0f%%", 206, 256);

      style.smallKnob(customControls, paramEffect1HighCut, "High Cut", "%5.0f dB", 64, 336);
      style.smallKnob(customControls, paramEffect1LowCut, "Low Cut", "%5.0f dB", 192, 336);
      break;
    default:
      break;
    }

    addChild(customControls);
  }

private:
  Style style;
  Client client;
  UIElement customControls;
}
