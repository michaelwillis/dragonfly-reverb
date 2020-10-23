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

        style = mallocNew!Style(_client);

        tabOnImage = loadOwnedImage(cast(ubyte[])(import("tab_on.png")));
        tabOffImage = loadOwnedImage(cast(ubyte[])(import("tab_off.png")));

        this.modalContainer = mallocNew!UIElement(context(), flagRaw);
        this.modalContainer.position = box2i(16, 136, 624, 464);
        addChild(this.modalContainer);

        int knobFrames = 101;

        addChild(style.largeKnob(context(), paramMix, 294, 20));

        UILabel mixNameLabel = mallocNew!UILabel(context(), "Mix", style.font, 16, style.highlight);
        mixNameLabel.position = box2i(294, 2, 294 + 52, 20);
        addChild(mixNameLabel);
        
        UINumericLabel mixValueLabel = mallocNew!UINumericLabel(
          context(), cast(FloatParameter) _client.param(paramMix), "%3.0f%%", style.font, 14, style.highlight);
        mixValueLabel.position = box2i(294, 76, 294 + 52, 90);
        addChild(mixValueLabel);

        presetsTab = mallocNew!UITab(context(), "Presets", tabOffImage, tabOnImage, style.font, 16, style.textColor, style.highlight);
        presetsTab.position = box2i(20, 104, 120, 126);
        presetsTab.setSelected(true);
        addChild(presetsTab);
                
        effectsTab = mallocNew!UITab(context(), "Effects", tabOffImage, tabOnImage, style.font, 16, style.textColor, style.highlight);
        effectsTab.position = box2i(120, 104, 220, 126);
        addChild(effectsTab);
        
        creditsTab = mallocNew!UITab(context(), "Credits", tabOffImage, tabOnImage, style.font, 16, style.textColor, style.highlight);
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

          UILabel effect1Label = mallocNew!UILabel(context(), "Effect 1:", style.font, 14, style.highlight, HorizontalAlignment.left);
          effect1Label.position = box2i(16, 136, 72, 150);
          this.modalContainer.addChild(effect1Label);

          UISelectBox effect1AlgorithmSelect = mallocNew!UISelectBox(
            context(), cast(EnumParameter) _client.param(paramEffect1Algorithm), style.font, 14, true);
          effect1AlgorithmSelect.position = box2i(76, 136, 320, 150);
          this.modalContainer.addChild(effect1AlgorithmSelect);

          UIEffect effect1 = mallocNew!UIEffect(context(), _client, style.font, style.smallKnobImage, style.textColor, style.highlight);
          effect1.position = box2i(16, 136, 304, 464);
          // effect1.selectEffect(); // TODO, set this based on effect enum param
          this.modalContainer.addChild(effect1);

          // TODO: Add second effect
          immutable int sliderWidth = 146;
          immutable int sliderHeight = 16;
          immutable int sliderFrames = 143;
          immutable int sliderMargin = 2;

          UILabel effect1SendLabel = mallocNew!UILabel(context(), "Send Effect 2", style.font, 14, style.textColor, HorizontalAlignment.left);
          effect1SendLabel.position = box2i(16, 424, 112, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendLabel);

          GainSlider effect1SendSlider = mallocNew!GainSlider(
            context(), cast(GainParameter) _client.param(paramEffect1Send), style.sliderImage, sliderFrames, sliderMargin);
          effect1SendSlider.position = box2i(110, 424, 110 + sliderWidth, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendSlider);

          UINumericLabel effect1SendValueLabel = mallocNew!UINumericLabel(
            context(), cast(FloatParameter) _client.param(paramEffect1Send), "%3.1f dB", style.font, 14, style.highlight);
          effect1SendValueLabel.position = box2i(260, 424, 316, 424 + sliderHeight);
          this.modalContainer.addChild(effect1SendValueLabel);

          UILabel effect1GainLabel = mallocNew!UILabel(context(), "Effect 1 Gain", style.font, 14, style.textColor, HorizontalAlignment.left);
          effect1GainLabel.position = box2i(16, 448, 112, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainLabel);

          GainSlider effect1GainSlider = mallocNew!GainSlider(
            context(), cast(GainParameter) _client.param(paramEffect1Gain), style.sliderImage, sliderFrames, sliderMargin);
          effect1GainSlider.position = box2i(110, 448, 110 + sliderWidth, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainSlider);

          UINumericLabel effect1GainValueLabel = mallocNew!UINumericLabel(
            context(), cast(FloatParameter) _client.param(paramEffect1Gain), "%3.1f dB", style.font, 14, style.highlight);
          effect1GainValueLabel.position = box2i(260, 448, 316, 448 + sliderHeight);
          this.modalContainer.addChild(effect1GainValueLabel);

        } else if (mode == UIMode.CREDITS){
          auto lines = makeVec!string();
          lines ~= "Dragonfly Reverb is a free multipurpose reverb plugin.";
          lines ~= "";
          lines ~= "Michael Willis - Primary developer and project lead";
          lines ~= "Rob van den Berg - Initial graphic design";

          foreach(i, line; lines) {
            UILabel label = mallocNew!UILabel(context(), line, style.font, 16, style.textColor, HorizontalAlignment.left);
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

    OwnedImage!RGBA tabOnImage, tabOffImage;

    Style style;
}

class Style {
public:
nothrow:
@nogc:
  DragonflyReverbClient client;

  Font font;
  OwnedImage!RGBA smallKnobImage, largeKnobImage, sliderImage;

  immutable int largeKnobSize = 52; // Pixels
  immutable int largeKnobFrames = 101;

  immutable int smallKnobSize = 32; // Pixels
  immutable int smallKnobFrames = 101;

  immutable RGBA textColor = RGBA(200, 200, 200, 255);
  immutable RGBA highlight = RGBA(230, 240, 255, 255);

  this(DragonflyReverbClient client) {
    this.client = client;

    smallKnobImage = loadOwnedImage(cast(ubyte[])(import("knob.png")));
    largeKnobImage = loadOwnedImage(cast(ubyte[])(import("knob-52.png")));
    sliderImage = loadOwnedImage(cast(ubyte[])(import("slider.png")));
    font = mallocNew!Font(cast(ubyte[])( import("Vera.ttf") ));
  }

  UIFilmstripKnob largeKnob(UIContext context, int paramIndex, int x, int y) {
    UIFilmstripKnob knob = mallocNew!UIFilmstripKnob(context,
      cast(FloatParameter) client.param(paramIndex), largeKnobImage, largeKnobFrames);
    knob.position = box2i(x, y, x + largeKnobSize, y + largeKnobSize);
    return knob;
  }
}