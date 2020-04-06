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
