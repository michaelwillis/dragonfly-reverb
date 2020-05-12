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

#include "BasicLookAndFeel.h"
#include "BasicKnob.h"

// Change the look of JUCE's "rotary sliders" so they're more like traditional knobs. This code is adapted
// from the example at https://www.juce.com/doc/tutorial_look_and_feel_customisation.

void BasicLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle,
    Slider& slider)
{
    // basic geometry
    const float radius = jmin(width / 2, height / 2) - 10.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // fill
    Colour fillColour = findColour(Slider::rotarySliderFillColourId);
    auto bkp = dynamic_cast<BasicKnob*>(&slider);
    if (bkp) fillColour = bkp->bodyColour;
    g.setColour(fillColour);
    g.fillEllipse(rx, ry, rw, rw);

    // outline
    g.setColour(findColour(Slider::rotarySliderOutlineColourId));
    g.drawEllipse(rx, ry, rw, rw, 3.0f);

    // pointer
    Path p;
    const float pointerLength = radius * 0.5f;
    const float pointerThickness = 3.0f;
    p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
    p.applyTransform(AffineTransform::rotation(angle).translated(centreX, centreY));
    g.setColour(findColour(Slider::rotarySliderOutlineColourId));
    g.fillPath(p);
}

Colour BasicLookAndFeel::darkBlue = Colour(52, 87, 155);
Colour BasicLookAndFeel::orange = Colour(201, 83, 47);
Colour BasicLookAndFeel::green = Colour(41, 151, 85);
Colour BasicLookAndFeel::lightBlue = Colour(63, 176, 201);//Colour(41, 187, 161);
Colour BasicLookAndFeel::purple = Colour(117, 73, 178);
