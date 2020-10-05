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

class UISelectBox : UIElement {
public:
nothrow:
@nogc:

  this(UIContext context, Font font, int textSize, EnumParameter param) {
    super(context, flagRaw);
    _font = font;
    _textSize = textSize;
    _param = param;
    _textColor = RGBA(200, 200, 200, 255);
    _highlightColor = RGBA(255, 255, 255, 255);
    _backgroundColor = RGBA(0, 0, 0, 255);
    _selectedIndex = 0;
  }

  ~this() {

  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {

    float textPosX = 5;
    
    foreach(dirtyRect; dirtyRects) {
      auto cropped = rawMap.cropImageRef(dirtyRect);
      // cropped.fillAll(_backgroundColor);

      for (int i = 0; i <= _param.maxValue(); i++) {
        // TODO: Get rid of magic number 1.2, make row height explicit
        float textPosY = ((cast(float)i) * 1.2 + 1) * _textSize;
        vec2f positionInDirty = vec2f(textPosX, textPosY) - dirtyRect.min;
        
        cropped.fillText(
          _font, _param.getValueString(i),
          _textSize, 0.5, i == _selectedIndex ? _highlightColor : _textColor,
          positionInDirty.x, positionInDirty.y,
          HorizontalAlignment.left,
          VerticalAlignment.baseline);
      }
    }
  }

  override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate) {
    // TODO: is x, y absolute or relative?

    _selectedIndex = cast(int) ((cast(float)y) / (_textSize * 1.2));
    setDirtyWhole();
    return true;
  }
    
private:
  Font _font;
  int _textSize;
  RGBA _textColor;
  RGBA _highlightColor;  
  RGBA _backgroundColor;

  EnumParameter _param;
  int _selectedIndex;
}
