/**
Select Box UI Element
*/
module widgets.select;

import dplug.gui.element;
import dplug.client.params;
import dplug.core.nogc;
import dplug.core.vec;
import dplug.window.window : getCharFromKey;

import gui;

private import core.stdc.stdlib : malloc, free;
private import core.stdc.stdio : snprintf, printf;
private import core.stdc.string : strcmp, strlen;

class UISelectBox : UIElement, IParameterListener {
public:
nothrow:
@nogc:

  this(Style style, EnumParameter param, int textSize, bool horizontal = false) {
    super(style.context, flagRaw);
    this.style = style;
    this.textSize = textSize;
    this.horizontal = horizontal;
    this.param = param;
    this.param.addListener(this);
  }

  ~this() {
    this.param.removeListener(this);
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {

    float textPosX = 0, textPosY = 0.5 * textSize;
    
    foreach(dirtyRect; dirtyRects) {
      auto cropped = rawMap.cropImageRef(dirtyRect);

      for (int i = 0; i <= param.maxValue(); i++) {
        vec2f positionInDirty = vec2f(textPosX, textPosY) - dirtyRect.min;
        
        cropped.fillText(style.font, param.getValueString(i),
          textSize, 0.5, i == param.value() ? style.highlight : style.textColor,
          positionInDirty.x, positionInDirty.y, HorizontalAlignment.left);

        if (horizontal) {
          auto box = style.font.measureText(param.getValueString(i), textSize, 0.5);
          textPosX += box.width + 12;
        } else {
          // TODO: Get rid of magic number 1.2, make row height explicit
          textPosY += 1.2 * textSize;
        }
      }
    }
  }

  override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate) {

    int value = -1;

    if (horizontal) {
      int left = 4;
      for (int i = 0; i <= param.maxValue(); i++) {
          auto box = style.font.measureText(param.getValueString(i), textSize, 0.5);
          int right = left + box.width + 12;
          if (x >= left && x < right) {
            value = i;
            break;
          }
          left = right;
      }
    } else {
      // TODO: Get rid of magic number 1.2, make row height explicit
      value = cast(int) ((cast(float)y) / (textSize * 1.2));
    }

    if (value < 0 || value > param.maxValue()) {
      return false;
    }

    param.beginParamEdit();
    param.setFromGUI(value);
    param.endParamEdit();

    setDirtyWhole();
    return true;
  }

  override void onParameterChanged(Parameter sender) {
    setDirtyWhole(); // TODO: Only dirty old and new selection?
  }

  override void onBeginParameterEdit(Parameter sender) { }

  override void onEndParameterEdit(Parameter sender) { }

private:
  Style style;
  EnumParameter param;
  int textSize;
  bool horizontal;
}
