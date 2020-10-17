/**
Tab UI Element

Right now just an image + text that can be updated to select/unselect
*/

module widgets.tab;

import dplug.gui.element;

class UITab : UIElement {
public:
nothrow:
@nogc:

  this(
    UIContext context, const char[] text,
    OwnedImage!RGBA offImage, OwnedImage!RGBA onImage,
    Font font, int textSize, RGBA offColor, RGBA onColor) {

    super(context, flagRaw);
    this.text = text;
    this.onImage = onImage;
    this.offImage = offImage;
    this.font = font;
    this.textSize = textSize;
    this.offColor = offColor;
    this.onColor = onColor;
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {
    auto imageRef = selected ? onImage.toRef() : offImage.toRef;
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
      croppedRawOut.fillText(font, text, textSize, 0.5, selected ? onColor : offColor, positionInDirty.x, positionInDirty.y);
    }
  }

  void setSelected(bool selected) {
    this.selected = selected;
    setDirtyWhole();
  }

private:
  OwnedImage!RGBA onImage, offImage;  
  const char[] text;
  Font font;
  int textSize;
  RGBA onColor, offColor;

  bool selected;
}
