/*
 * Dragonfly Reverb
 * copyright (c) 2019-2020 Michael Willis, Rob van den Berg, Shane Dunne
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

#pragma once
#include "JuceHeader.h"

/*  One of the most serious omissions in the JUCE library is a standard "knob" control.
    JUCE offers a "rotary" style Slider, but this is not satisfactory. The BasicKnob
    and BasicLookAndFeel classes are based on techniques presented in the JUCE Tutorial
    https://docs.juce.com/master/tutorial_look_and_feel_customisation.html
*/

class BasicKnob : public Slider
{
    friend class BasicLookAndFeel;

public:
    BasicKnob(float minValue, float maxValue, const String& unit = {});
    virtual ~BasicKnob() = default;

    // Slider
    double getValueFromText(const String& text) override;
    String getTextFromValue(double value) override;

protected:
    String unitOfMeasure;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicKnob)
};
