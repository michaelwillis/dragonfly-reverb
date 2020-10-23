/**
Select Box UI Element
*/
module widgets.select;

import dplug.gui.element;
import dplug.client.params;
import dplug.core.nogc;
import dplug.core.vec;
import dplug.window.window : getCharFromKey;

private import core.stdc.stdlib : malloc, free;
private import core.stdc.stdio : snprintf, printf;
private import core.stdc.string : strcmp, strlen;

class UISelectBox : UIElement, IParameterListener {
public:
nothrow:
@nogc:

  this(UIContext context, EnumParameter param, Font font, int textSize, bool horizontal = false) {
    super(context, flagRaw);
    _font = font;
    _textSize = textSize;
    _horizontal = horizontal;
    _param = param;
    // TODO: Unify textColor and highlightColor for all widgets
    _textColor = RGBA(200, 200, 200, 255);
    _highlightColor = RGBA(255, 255, 255, 255);
    _backgroundColor = RGBA(0, 0, 0, 255);

    _param.addListener(this);
  }

  ~this() {
    _param.removeListener(this);
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {

    float textPosX = 0, textPosY = 0.5 * _textSize;
    
    foreach(dirtyRect; dirtyRects) {
      auto cropped = rawMap.cropImageRef(dirtyRect);

      for (int i = 0; i <= _param.maxValue(); i++) {
        vec2f positionInDirty = vec2f(textPosX, textPosY) - dirtyRect.min;
        
        cropped.fillText(
          _font, _param.getValueString(i),
          _textSize, 0.5, i == _param.value() ? _highlightColor : _textColor,
          positionInDirty.x, positionInDirty.y, HorizontalAlignment.left);

        if (_horizontal) {
          auto box = _font.measureText(_param.getValueString(i), _textSize, 0.5);
          textPosX += box.width + 12;
        } else {
          // TODO: Get rid of magic number 1.2, make row height explicit
          textPosY += 1.2 * _textSize;
        }
      }
    }
  }

  override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate) {

    int value = -1;

    if (_horizontal) {
      int left = 4;
      for (int i = 0; i <= _param.maxValue(); i++) {
          auto box = _font.measureText(_param.getValueString(i), _textSize, 0.5);
          int right = left + box.width + 12;
          if (x >= left && x < right) {
            value = i;
            break;
          }
          left = right;
      }
    } else {
      // TODO: Get rid of magic number 1.2, make row height explicit
      value = cast(int) ((cast(float)y) / (_textSize * 1.2));
    }

    if (value < 0 || value > _param.maxValue()) {
      return false;
    }

    _param.beginParamEdit();
    _param.setFromGUI(value);
    _param.endParamEdit();

    setDirtyWhole();
    return true;
  }

  override void onParameterChanged(Parameter sender) {
    setDirtyWhole(); // TODO: Only dirty old and new selection?
  }

  override void onBeginParameterEdit(Parameter sender) { }

  override void onEndParameterEdit(Parameter sender) { }

private:
  EnumParameter _param;

  Font _font;
  int _textSize;
  bool _horizontal;
  RGBA _textColor;
  RGBA _highlightColor;  
  RGBA _backgroundColor;
}
