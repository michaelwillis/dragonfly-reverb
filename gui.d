module gui;

import std.stdio;

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
  CONTROLS,
  CREDITS
}

class DragonflyReverbGUI : FlatBackgroundGUI!("background.png")
{
public:
nothrow:
@nogc:

    DragonflyReverbClient _client;
    Font _font;

    UIMode mode;

    box2i presetsTabPosition  = box2i(20,  104, 120, 126);
    box2i controlsTabPosition = box2i(120, 104, 220, 126);
    box2i creditsTabPosition  = box2i(220, 104, 320, 126);

    UITab presetsTab, controlsTab, creditsTab;

    this(DragonflyReverbClient client)
    {

        _client = client;
        super(640, 480); // size
        _font = mallocNew!Font(cast(ubyte[])( import("Vera.ttf") ));
        setUpdateMargin(0);

        this.modalContainer = mallocNew!UIElement(context(), flagRaw);
        this.modalContainer.position = box2i(16, 136, 624, 464);
        addChild(this.modalContainer);

        OwnedImage!RGBA knobImage = loadOwnedImage(cast(ubyte[])(import("knob.png")));
        OwnedImage!RGBA knob52Image = loadOwnedImage(cast(ubyte[])(import("knob-52.png")));
        OwnedImage!RGBA switchOnImage = loadOwnedImage(cast(ubyte[])(import("switchOn.png")));
        OwnedImage!RGBA switchOffImage = loadOwnedImage(cast(ubyte[])(import("switchOff.png")));
        OwnedImage!RGBA sliderImage = loadOwnedImage(cast(ubyte[])(import("slider.png")));

        tabOnImage = loadOwnedImage(cast(ubyte[])(import("tab_on.png")));
        tabOffImage = loadOwnedImage(cast(ubyte[])(import("tab_off.png")));        

        int knobFrames = 101;
        UIFilmstripKnob inputGainKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramInputGain), knobImage, knobFrames);
        addChild(inputGainKnob);

        UIFilmstripKnob clipKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramClip), knobImage, knobFrames);
        addChild(clipKnob);

        UIFilmstripKnob outputGainKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramOutputGain), knobImage, knobFrames);
        addChild(outputGainKnob);

        UIFilmstripKnob mixKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramMix), knob52Image, knobFrames);
        addChild(mixKnob);

        UIImageSwitch modeSwitch = mallocNew!UIImageSwitch(
          context(), cast(BoolParameter) _client.param(paramMode), switchOnImage, switchOffImage);
        addChild(modeSwitch);

        UILabel mixNameLabel = mallocNew!UILabel(context(), "Mix", _font, 16, highlight);
        addChild(mixNameLabel);
        
        UINumericLabel mixValueLabel = mallocNew!UINumericLabel(context(), cast(FloatParameter) _client.param(paramMix), "%3.0f%%", _font, 14, highlight);
        addChild(mixValueLabel);

        presetsTab = mallocNew!UITab(context(), "Presets", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
        presetsTab.setSelected(true);
        addChild(presetsTab);
                
        controlsTab = mallocNew!UITab(context(), "Controls", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
        addChild(controlsTab);
        
        creditsTab = mallocNew!UITab(context(), "Credits", tabOffImage, tabOnImage, _font, 16, textColor, highlight);
        addChild(creditsTab);        

        int sliderFrames = 143;
        int sliderMargin = 2;
        GainSlider effect1GainSlider = mallocNew!GainSlider(context(), cast(GainParameter) _client.param(paramEffect1Gain), sliderImage, sliderFrames, sliderMargin);
        addChild(effect1GainSlider);

        UIElement sliderContainer = mallocNew!UIElement(context(), 1);
        sliderContainer.addChild(effect1GainSlider);
        addChild(sliderContainer);

        // Builds the UI hierarchy
        immutable int knobX1 = 70;
        immutable int knobX2 = 308;
        immutable int knobY1 = 200;
        immutable int knobY2 = 320;
        immutable int knobWidth = 32;
        immutable int knobHeight = 32;

        inputGainKnob.position = box2i(knobX1, knobY1, knobX1 + knobWidth, knobY1 + knobHeight);
        clipKnob.position = box2i(knobX2, knobY1, knobX2 + knobWidth, knobY1 + knobHeight);
        outputGainKnob.position = box2i(knobX1 + 24, knobY2, knobX1 + knobWidth + 24, knobY2 + knobHeight);
        mixNameLabel.position = box2i(294, 2, 294 + 52, 20);
        mixValueLabel.position = box2i(294, 76, 294 + 52, 90);
        mixKnob.position = box2i(294, 20, 294 + 52, 20 + 52);

        presetsTab.position = presetsTabPosition;
        controlsTab.position = controlsTabPosition;
        creditsTab.position = creditsTabPosition;

        immutable int sliderWidth = 146;
        immutable int sliderHeight = 16;

        sliderContainer.position = box2i(knobX1, knobY2 + 50, knobX1 + sliderWidth, knobY2 + 50 + sliderHeight);
        effect1GainSlider.position = box2i(50, 300, 50 + sliderWidth, 300 + sliderHeight);

        immutable int switchX = 380;
        immutable int switchY = 28;
        immutable int switchWidth = 51;
        immutable int switchHeight = 21;

        modeSwitch.position = box2i(switchX, switchY, switchX + switchWidth, switchY  + switchHeight);
    }

    override bool onMouseClick(int x, int y, int button, bool isDoubleClick, MouseState mstate)
    {
      // UIMode newMode = mode;

      if (presetsTab.position.contains(x, y)) {
        selectMode(0);
      } else if (controlsTab.position.contains(x, y)) {
        selectMode(1);
      } else if (creditsTab.position.contains(x, y)) {
        selectMode(2);
      } else {
        return true;
      }

      return false;
    }

    void selectMode(int mode) {
        presetsTab.setSelected(mode == 0);
        controlsTab.setSelected(mode == 1);
        creditsTab.setSelected(mode == 2);

        removeChild(this.modalContainer);
        this.modalContainer.destroyFree();
        this.modalContainer = mallocNew!UIElement(context(), 1);
        this.modalContainer.position = box2i(16, 136, 624, 464);
        
        if (mode == 0) {

        } else if (mode == 1) {
          UIEffect effect1 = mallocNew!UIEffect(context(), _client, _font);
          effect1.position = box2i(16, 136, 304, 464);
          this.modalContainer.addChild(effect1);

          // TODO: Add second effect
        } else {
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

    OwnedImage!RGBA tabOnImage, tabOffImage;
    UIElement modalContainer;
}
