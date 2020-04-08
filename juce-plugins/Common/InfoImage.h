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
#include <JuceHeader.h>

class InfoImage : public ImageComponent
{
public:
    InfoImage() : ImageComponent() {}

    void mouseDown(const MouseEvent&) override
    {
        if (onMouseDown) onMouseDown();
    }

    std::function<void()> onMouseDown;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (InfoImage)
};
