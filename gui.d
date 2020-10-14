module gui;

import std.stdio;

import dplug.gui;
import dplug.flatwidgets;
import dplug.client;
import dplug.canvas;

import widgets.gainslider;
import widgets.select;
import widgets.label;

import main;

class DragonflyReverbGUI : FlatBackgroundGUI!("background.png")
{
public:
nothrow:
@nogc:

    DragonflyReverbClient _client;
    Font _font;

    this(DragonflyReverbClient client)
    {

        _client = client;
        super(640, 480); // size

        _font = mallocNew!Font(cast(ubyte[])( import("Vera.ttf") ));
        setUpdateMargin(0);

        OwnedImage!RGBA knobImage = loadOwnedImage(cast(ubyte[])(import("knob.png")));
        OwnedImage!RGBA knob52Image = loadOwnedImage(cast(ubyte[])(import("knob-52.png")));
        OwnedImage!RGBA switchOnImage = loadOwnedImage(cast(ubyte[])(import("switchOn.png")));
        OwnedImage!RGBA switchOffImage = loadOwnedImage(cast(ubyte[])(import("switchOff.png")));
        OwnedImage!RGBA sliderImage = loadOwnedImage(cast(ubyte[])(import("slider.png")));

        int knobFrames = 101;
        UIFilmstripKnob inputGainKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramInputGain), knobImage, knobFrames);

        UIFilmstripKnob clipKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramClip), knobImage, knobFrames);

        UIFilmstripKnob outputGainKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramOutputGain), knobImage, knobFrames);

        UIFilmstripKnob mixKnob = mallocNew!UIFilmstripKnob(
          context(), cast(FloatParameter) _client.param(paramMix), knob52Image, knobFrames);

        UIImageSwitch modeSwitch = mallocNew!UIImageSwitch(
          context(), cast(BoolParameter) _client.param(paramMode), switchOnImage, switchOffImage);

        UISelectBox earlySelect = mallocNew!UISelectBox(context(), cast(EnumParameter) _client.param(paramEffect1EarlyReflectionPattern), _font, 14);

        UILabel mixNameLabel = mallocNew!UILabel(context(), "Mix", _font, 16, highlight);
        UINumericLabel mixValueLabel = mallocNew!UINumericLabel(context(), cast(FloatParameter) _client.param(paramMix), "%3.0f%%", _font, 15, highlight);
        
        addChild(inputGainKnob);
        addChild(clipKnob);
        addChild(outputGainKnob);
        addChild(mixKnob);
        addChild(modeSwitch);
        addChild(earlySelect);
        addChild(mixNameLabel);
        addChild(mixValueLabel);        

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
        immutable int knobY1 = 101;
        immutable int knobY2 = 320;
        immutable int knobWidth = 32;
        immutable int knobHeight = 32;

        inputGainKnob.position = box2i(knobX1, knobY1, knobX1 + knobWidth, knobY1 + knobHeight);
        clipKnob.position = box2i(knobX2, knobY1, knobX2 + knobWidth, knobY1 + knobHeight);
        outputGainKnob.position = box2i(knobX1 + 24, knobY2, knobX1 + knobWidth + 24, knobY2 + knobHeight);
        mixNameLabel.position = box2i(294, 4, 294 + 52, 20);
        mixValueLabel.position = box2i(294, 76, 294 + 52, 92);        
        mixKnob.position = box2i(294, 22, 294 + 52, 22 + 52);
        earlySelect.position = box2i(0, 16, 100, 116);

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

    // this struct object should not be since it contains everything rasterizer-related
    Canvas canvas;

private:
    RGBA highlight = RGBA(230, 240, 255, 255);
}
