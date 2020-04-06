#pragma once
#include "JuceHeader.h"

/*  One of the most serious omissions in the JUCE library is a standard "knob" control.
    JUCE offers a "rotary" style Slider, but this is not satisfactory. The BasicKnob
    and BasicLookAndFeel classes are based on techniques presented in the JUCE Tutorial
    https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
*/

class BasicLookAndFeel : public LookAndFeel_V4
{
public:
    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle,
        Slider& slider) override;
};
