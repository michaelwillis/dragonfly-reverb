/**
Tab UI Element

Right now just an image + text that can be updated to select/unselect
*/

module widgets.tab;

import dplug.gui.element;
import gui;

class UITab : UIElement {
public:
nothrow:
@nogc:

  this(Style style, const char[] text, int textSize) {
    super(style.context, flagRaw);
    this.style = style;
    this.text = text;
    this.textSize = textSize;
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {
    auto imageRef = selected ? style.tabOnImage.toRef() : style.tabOffImage.toRef;
    foreach(dirtyRect; dirtyRects) {
      auto croppedRawIn = imageRef.cropImageRef(dirtyRect);
      auto croppedRawOut = rawMap.cropImageRef(dirtyRect);

      int w = dirtyRect.width;
      int h = dirtyRect.height;

      for(int j = 0; j < h; ++j)
      {
        RGBA[] inputRaw = croppedRawIn.scanline(j);
        RGBA[] outputRaw = croppedRawOut.scanline(j);
        
        for(int i = 0; i < w; ++i)
        {
          outputRaw[i] = inputRaw[i];
        }
      }

      vec2f positionInDirty = vec2f(position.width * 0.5f, position.height * 0.5f) - dirtyRect.min;
      auto color = selected ? style.highlight : style.textColor;
      croppedRawOut.fillText(style.font, text, textSize, 0.5, color, positionInDirty.x, positionInDirty.y);
    }
  }

  void setSelected(bool selected) {
    this.selected = selected;
    setDirtyWhole();
  }

private:
  Style style;
  const char[] text;
  int textSize;

  bool selected;
}
