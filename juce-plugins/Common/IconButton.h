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

/*  IconButton is an icon that is clickable like a button, and remembers any modifier keys
    held down when it was clicked, so the onClick() handler can access them.
*/

class IconButton   : public DrawableButton
{
public:
    IconButton(const char* iconSvg, Colour fillColour, Colour normalColour, Colour overColour, Colour downColour);
    virtual ~IconButton() = default;

    void paint(Graphics&) override;

    void clicked(const ModifierKeys& modifiers) override
    {
        mods = modifiers;
        Button::clicked();
    }

    ModifierKeys mods;

private:
    Colour backgroundColour;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (IconButton)
};
