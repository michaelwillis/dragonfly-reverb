/**
Text Label
*/
module widgets.label;

import dplug.gui.element;
import dplug.client;
import dplug.core.nogc;
import dplug.core.vec;
import dplug.window.window : getCharFromKey;
import std.string;

import gui;

private import core.stdc.stdlib : malloc, free;
private import core.stdc.stdio : snprintf, printf;
private import core.stdc.string : strcmp, strlen;

class UILabel : UIElement
{
public:
nothrow:
@nogc:

  this(Style style, const char[] text, int textSize, HorizontalAlignment alignment = HorizontalAlignment.center) {
    super(style.context, flagRaw);
    this.style = style;
    this.text = text;
    this.textSize = textSize;
    this.alignment = alignment;
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {
    float textPosx = alignment == HorizontalAlignment.center ? position.width * 0.5f : 0.0f;
    float textPosy = position.height * 0.5f;

    foreach(dirtyRect; dirtyRects)
    {
      auto cropped = rawMap.cropImageRef(dirtyRect);
      vec2f positionInDirty = vec2f(textPosx, textPosy) - dirtyRect.min;

      cropped.fillText(style.font, text, textSize, 0.5, style.highlight, positionInDirty.x, positionInDirty.y, alignment);
    }
  }

private:
  Style style;
  const char[] text;
  int textSize;
  HorizontalAlignment alignment;
}

class UINumericLabel : UIElement, IParameterListener {
public:
nothrow:
@nogc:

  this(UIContext context, FloatParameter param, const char[] text, Font font, int textSize, RGBA textColor = RGBA(230, 230, 230, 255)) {
    super(context, flagRaw);
    _param = param;
    _text = text;
    _font = font;
    _textSize = textSize;
    _textColor = textColor;

    _param.addListener(this);
  }

  ~this() {
    _param.removeListener(this);
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {
    float textPosx = position.width * 0.5f;
    float textPosy = position.height * 0.5f;

    char[16] buffer;
    snprintf(buffer.ptr, 16, _text.ptr, _param.value());

    foreach(dirtyRect; dirtyRects) {
      auto cropped = rawMap.cropImageRef(dirtyRect);
      vec2f positionInDirty = vec2f(textPosx, textPosy) - dirtyRect.min;

      cropped.fillText(_font, fromStringz(buffer.ptr), _textSize, 0.5, _textColor, positionInDirty.x, positionInDirty.y);
    }
  }

  override void onParameterChanged(Parameter sender) {
    setDirtyWhole();
  }

  override void onBeginParameterEdit(Parameter sender) { }

  override void onEndParameterEdit(Parameter sender) { }

private:

  FloatParameter _param;
  const char[] _text;
  Font _font;
  int _textSize;
  RGBA _textColor;
}
