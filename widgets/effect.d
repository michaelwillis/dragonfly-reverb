/**
Effect container UI element
*/
module widgets.effect;

import dplug.client;
import dplug.gui.element;

import main; // Maybe TODO - move params enum out of main?
import widgets.select;
import dsp.effects;

class UIEffect : UIElement {
public:
nothrow:
@nogc:

  this(UIContext context, Client client, Font font, OwnedImage!RGBA knobImage) {
    super(context, flagRaw);
    this.client = client;
    this.font = font;

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
      UISelectBox earlySelect = mallocNew!UISelectBox(context(), cast(EnumParameter) client.param(paramEffect1EarlyReflectionPattern), font, 14);
      earlySelect.position = box2i(16, 216, 116, 316);
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
}
