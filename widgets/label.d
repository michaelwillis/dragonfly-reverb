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

  this(Style style, FloatParameter param, const char[] text, int textSize) {
    super(style.context, flagRaw);
    this.style = style;
    this.param = param;
    this.text = text;
    this.textSize = textSize;

    param.addListener(this);
  }

  ~this() {
    param.removeListener(this);
  }

  override void onDrawRaw(ImageRef!RGBA rawMap, box2i[] dirtyRects) {
    float textPosx = position.width * 0.5f;
    float textPosy = position.height * 0.5f;

    char[16] buffer;
    snprintf(buffer.ptr, 16, text.ptr, param.value());

    foreach(dirtyRect; dirtyRects) {
      auto cropped = rawMap.cropImageRef(dirtyRect);
      vec2f positionInDirty = vec2f(textPosx, textPosy) - dirtyRect.min;

      cropped.fillText(style.font, fromStringz(buffer.ptr), textSize, 0.5, style.textColor, positionInDirty.x, positionInDirty.y);
    }
  }

  override void onParameterChanged(Parameter sender) {
    setDirtyWhole();
  }

  override void onBeginParameterEdit(Parameter sender) { }

  override void onEndParameterEdit(Parameter sender) { }

private:
  Style style;
  FloatParameter param;
  const char[] text;
  int textSize;
}
