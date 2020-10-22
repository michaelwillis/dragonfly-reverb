module gui;

import dplug.gui;
import dplug.flatwidgets;
import dplug.client;
import dplug.canvas;

import widgets.effect;
import widgets.gainslider;
import widgets.label;
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
    Font _font;

    UITab presetsTab, effectsTab, creditsTab;

    this(DragonflyReverbClient client)
    {
        _client = client;
        super(640, 480); // size
        _font = mallocNew!Font(cast(ubyte[])( import("Vera.ttf") ));
        setUpdateMargin(0);

        this.modalContainer = mallocNew!UIElement(context(), flagRaw);
        this.modalContainer.position = box2i(16, 136, 624, 464);
        addChild(this.modalContainer);

        OwnedImage!RGBA smallKnobImage = loadOwnedImage(cast(ubyte[])(import("knob.png")));
        OwnedImage!RGBA knob52Image = loadOwnedImage(cast(ubyte[])(import("knob-52.png")));
        OwnedImage!RGBA switchOnImage = loadOwnedImage(cast(ubyte[])(import("switchOn.png")));
        OwnedImage!RGBA switchOffImage = loadOwnedImage(cast(ubyte[])(import("switchOff.png")));
        sliderImage = loadOwnedImage(cast(ubyte[])(import("slider.png")));

        tabOnImage = loadOwnedImage(cast(ubyte[])(import("tab_on.png")));
        tabOffImage = loadOwnedImage(cast(ubyte[])(import("tab_off.png")));

        int knobFrames = 101;

        UIFilmstripKnob mixKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramMix), knob52Image, knobFrames);
        mixKnob.position = box2i(294, 20, 294 + 52, 20 + 52);
        addChild(mixKnob);

        UILabel mixNameLabel = mallocNew!UILabel(context(), "Mix", _font, 16, highlight);
        mixNameLabel.position = box2i(294, 2, 294 + 52, 20);
        addChild(mixNameLabel);
        
        UINumericLabel mixValueLabel = mallocNew!UINumericLabel(
          context(), cast(FloatParameter) _client.param(paramMix), "%3.0f%%", _font, 14, highlight);
        mixValueLabel.position = box2i(294, 76, 294 + 52, 90);
        addChild(mixValueLabel);

        presetsTab = mallocNew!UITab(context(), "Presets", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
        presetsTab.position = box2i(20, 104, 120, 126);
        presetsTab.setSelected(true);
        addChild(presetsTab);
                
        effectsTab = mallocNew!UITab(context(), "Effects", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
        effectsTab.position = box2i(120, 104, 220, 126);
        addChild(effectsTab);
        
        creditsTab = mallocNew!UITab(context(), "Credits", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
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
          UIEffect effect1 = mallocNew!UIEffect(context(), _client, _font, smallKnobImage);
          effect1.position = box2i(16, 136, 304, 464);
          // effect1.selectEffect(); // TODO, set this based on effect enum param
          this.modalContainer.addChild(effect1);

          // TODO: Add second effect
          immutable int sliderWidth = 146;
          immutable int sliderHeight = 16;
          immutable int sliderFrames = 143;
          immutable int sliderMargin = 2;

          UILabel effect1SendLabel = mallocNew!UILabel(context(), "Send Effect 2", _font, 14, textColor, HorizontalAlignment.left);
          effect1SendLabel.position = box2i(12, 424, 112, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendLabel);

          GainSlider effect1SendSlider = mallocNew!GainSlider(
            context(), cast(GainParameter) _client.param(paramEffect1Send), sliderImage, sliderFrames, sliderMargin);
          effect1SendSlider.position = box2i(108, 424, 108 + sliderWidth, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendSlider);

          UINumericLabel effect1SendValueLabel = mallocNew!UINumericLabel(
            context(), cast(FloatParameter) _client.param(paramEffect1Send), "%3.1f dB", _font, 14, highlight);
          effect1SendValueLabel.position = box2i(260, 424, 316, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendValueLabel);

          UILabel effect1GainLabel = mallocNew!UILabel(context(), "Effect 1 Gain", _font, 14, textColor, HorizontalAlignment.left);
          effect1GainLabel.position = box2i(12, 448, 112, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainLabel);

          GainSlider effect1GainSlider = mallocNew!GainSlider(
            context(), cast(GainParameter) _client.param(paramEffect1Gain), sliderImage, sliderFrames, sliderMargin);
          effect1GainSlider.position = box2i(108, 448, 108 + sliderWidth, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainSlider);

          UINumericLabel effect1GainValueLabel = mallocNew!UINumericLabel(
            context(), cast(FloatParameter) _client.param(paramEffect1Gain), "%3.1f dB", _font, 14, highlight);
          effect1GainValueLabel.position = box2i(260, 448, 316, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainValueLabel);

        } else if (mode == UIMode.CREDITS){
          auto lines = makeVec!string();
          lines ~= "Dragonfly Reverb is a free multipurpose reverb plugin.";
          lines ~= "";
          lines ~= "Michael Willis - Primary developer and project lead";
          lines ~= "Rob van den Berg - Initial graphic design";

          foreach(i, line; lines) {
            UILabel label = mallocNew!UILabel(context(), line, _font, 16, textColor, HorizontalAlignment.left);
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
    RGBA textColor = RGBA(200, 200, 200, 255);
    RGBA highlight = RGBA(230, 240, 255, 255);

    UIElement modalContainer;

    OwnedImage!RGBA tabOnImage, tabOffImage, smallKnobImage, sliderImage;
}
