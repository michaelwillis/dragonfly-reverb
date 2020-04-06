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
