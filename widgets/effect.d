/**
Effect container UI element
*/
module widgets.effect;

import dplug.client;
import dplug.gui.element;

import main; // Maybe TODO - move params enum out of main?
import widgets.label;
import widgets.select;
import dsp.effects;

class UIEffect : UIElement {
public:
nothrow:
@nogc:

  this(UIContext context, Client client, Font font, OwnedImage!RGBA knobImage, RGBA textColor, RGBA highlight) {
    super(context, flagRaw);
    this.client = client;
    this.font = font;

    this.textColor = textColor;
    this.highlight = highlight;

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
      UILabel effect1Label = mallocNew!UILabel(context(), "Reflection Pattern", font, 14, highlight, HorizontalAlignment.left);
      effect1Label.position = box2i(16, 160, 148, 176);
      this.customControls.addChild(effect1Label);

      UISelectBox earlySelect = mallocNew!UISelectBox(context(), cast(EnumParameter) client.param(paramEffect1EarlyReflectionPattern), font, 14);
      earlySelect.position = box2i(16, 184, 148, 316);
      this.customControls.addChild(earlySelect);
      break;
    default:
      break;
    }

    addChild(customControls);
  }

private:
  UIElement customControls;
  Client client;
  Font font;
  OwnedImage!RGBA knobImage;
  RGBA textColor;
  RGBA highlight;
}
