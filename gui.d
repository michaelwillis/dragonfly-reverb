module gui;

import dplug.gui;
import dplug.flatwidgets;
import dplug.client;
import dplug.canvas;

import widgets.effect;
import widgets.gainslider;
import widgets.label;
import widgets.select;
import widgets.tab;

import main;

enum UIMode {
  PRESETS,
  EFFECTS,
  CREDITS
}

class DragonflyReverbGUI : FlatBackgroundGUI!("background.png")
{
public:
nothrow:
@nogc:

    DragonflyReverbClient _client;

    UITab presetsTab, effectsTab, creditsTab;

    this(DragonflyReverbClient client)
    {
        _client = client;
        super(640, 480);
        setUpdateMargin(0);

        style = mallocNew!Style(_client, context());

        this.modalContainer = mallocNew!UIElement(context(), flagRaw);
        this.modalContainer.position = box2i(16, 136, 624, 464);
        addChild(this.modalContainer);

        style.largeKnob(this, paramMix, "Mix", "%3.0f%%", 294, 4);

        presetsTab = mallocNew!UITab(style, "Presets", 16);
        presetsTab.position = box2i(20, 104, 120, 126);
        presetsTab.setSelected(true);
        addChild(presetsTab);
                
        effectsTab = mallocNew!UITab(style, "Effects", 16);
        effectsTab.position = box2i(120, 104, 220, 126);
        addChild(effectsTab);
        
        creditsTab = mallocNew!UITab(style, "Credits", 16);
        creditsTab.position = box2i(220, 104, 320, 126);
        addChild(creditsTab); 
    }

    override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate) {
      if (presetsTab.position.contains(x, y)) {
        selectMode(UIMode.PRESETS);
      } else if (effectsTab.position.contains(x, y)) {
        selectMode(UIMode.EFFECTS);
      } else if (creditsTab.position.contains(x, y)) {
        selectMode(UIMode.CREDITS);
      } else {
        return true;
      }

      return false;
    }

    void selectMode(UIMode mode) {

      presetsTab.setSelected(mode == UIMode.PRESETS);
      effectsTab.setSelected(mode == UIMode.EFFECTS);
      creditsTab.setSelected(mode == UIMode.CREDITS);

      context().focused = null; // Avoid segfault, context tries to call onFocusExit()

      removeChild(this.modalContainer);
      this.modalContainer.destroyFree();
      this.modalContainer = mallocNew!UIElement(context(), 1);
      this.modalContainer.position = box2i(16, 136, 624, 464);
      
        if (mode == UIMode.PRESETS) {
          
        } else if (mode == UIMode.EFFECTS) {
          EnumParameter effect1Algorithm = cast(EnumParameter) _client.param(paramEffect1Algorithm);
          auto effect1Widget = mallocNew!UIEffect(style, _client, true);
          style.place(modalContainer, 0, 0, 288, 328, effect1Widget);

          style.place(modalContainer, 0, 0, 58, 16,
            mallocNew!UILabel(style, "Effect 1:", 14, HorizontalAlignment.left));
          style.place(modalContainer, 60, 0, 248, 16, mallocNew!UISelectBox(style, effect1Algorithm, 14, true));

          effect1Widget.selectEffect(effect1Algorithm.value()); // TODO: listen for change

          // TODO: Add second effect

        } else if (mode == UIMode.CREDITS){
          auto lines = makeVec!string();
          lines ~= "Dragonfly Reverb is a free multipurpose reverb plugin.";
          lines ~= "";
          lines ~= "Michael Willis - Primary developer and project lead";
          lines ~= "Rob van den Berg - Initial graphic design";

          foreach(i, line; lines) {
            UILabel label = mallocNew!UILabel(style, line, 16, HorizontalAlignment.left);
            label.position = box2i(32, cast(int) (152 + (i * 20)), 608, cast(int) (172 + (i * 20)));
            this.modalContainer.addChild(label);    
          }

        }

        this.addChild(modalContainer);
        setDirtyWhole(); 
    }

    // this struct object should not be since it contains everything rasterizer-related
    Canvas canvas;

private:
    UIElement modalContainer;

    Style style;
}

class Style {
public:
nothrow:
@nogc:
  DragonflyReverbClient client;
  UIContext context;

  Font font;
  OwnedImage!RGBA smallKnobImage, largeKnobImage, sliderImage, tabOnImage, tabOffImage;

  immutable int largeKnobSize = 52; // Pixels
  immutable int largeKnobFrames = 101;

  immutable int smallKnobSize = 32; // Pixels
  immutable int smallKnobFrames = 101;

  immutable int sliderWidth = 146, sliderHeight = 16;
  immutable int sliderFrames = 143, sliderMargin = 2;
  
  immutable RGBA textColor = RGBA(200, 200, 200, 255);
  immutable RGBA highlight = RGBA(230, 240, 255, 255);

  this(DragonflyReverbClient client, UIContext context) {
    this.client = client;
    this.context = context;

    font = mallocNew!Font(cast(ubyte[])( import("Vera.ttf") ));

    smallKnobImage = loadOwnedImage(cast(ubyte[])(import("knob.png")));
    largeKnobImage = loadOwnedImage(cast(ubyte[])(import("knob-52.png")));
    sliderImage    = loadOwnedImage(cast(ubyte[])(import("slider.png")));

    tabOnImage     = loadOwnedImage(cast(ubyte[])(import("tab_on.png")));
    tabOffImage    = loadOwnedImage(cast(ubyte[])(import("tab_off.png")));
  }

  void selectBox(UIElement parent, int paramIndex, const char[] name, int x, int y) {
    place(parent, x, y, 128, 16, mallocNew!UILabel(this, name, 14, HorizontalAlignment.left));
    place(parent, x, y + 24, 128, 136, mallocNew!UISelectBox(this, cast(EnumParameter) client.param(paramIndex), 14));
  }
  void largeKnob(UIElement parent, int paramIndex, const char[] name, const char[] format, int x, int y) {
    FloatParameter param = cast(FloatParameter) client.param(paramIndex);
    place(parent, x, y, largeKnobSize, 16, mallocNew!UILabel(this, name, 14));
    place(parent, x, y + 18, largeKnobSize, largeKnobSize,
      mallocNew!UIFilmstripKnob(context, param, largeKnobImage, largeKnobFrames));
    place(parent, x, y + 72, largeKnobSize, 16, mallocNew!UINumericLabel(this, param, format, 14));
  }
  
  void smallKnob(UIElement parent, int paramIndex, const char[] name, const char[] format, int x, int y) {
    FloatParameter param = cast(FloatParameter) client.param(paramIndex);
    place(parent, x, y, largeKnobSize, 16, mallocNew!UILabel(this, name, 14));
    place(parent, x, y + 18, smallKnobSize, smallKnobSize,
      mallocNew!UIFilmstripKnob(context, param, smallKnobImage, smallKnobFrames));
    place(parent, x, y + smallKnobSize + 20, largeKnobSize, 16, mallocNew!UINumericLabel(this, param, format, 14));
  }

  void gainSlider(UIElement parent, int paramIndex, const char[] name, const char[] format, int x, int y) {
    GainParameter param = cast(GainParameter) client.param(paramIndex);
    place(parent, x, y, 96, sliderHeight, mallocNew!UILabel(this, name, 14, HorizontalAlignment.left));
    place(parent, x + 94, y, sliderWidth, sliderHeight,
      mallocNew!GainSlider(context, param, sliderImage, sliderFrames, sliderMargin));
    place(parent, x + 244, y, 56, sliderHeight, mallocNew!UINumericLabel(this, param, format, 14));
  }

  void place(UIElement parent, int x, int y, int width, int height, UIElement child) {
    child.position = box2i(
      parent.position.min.x + x,
      parent.position.min.y + y,
      parent.position.min.x + x + width,
      parent.position.min.y + y + height);
    parent.addChild(child);
  }
}