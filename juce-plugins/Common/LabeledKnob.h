#pragma once
#include "JuceHeader.h"
#include "BasicKnob.h"

/*  LabeledKnob implements a very common scenario for audio plug-in GUIs: a knob
    with a text label centred below it. The label is made more useful by the ability
    to double-click it to transform it into an editor for the associated knob's value,
    and having the label automatically show the knob value while it is being adjusted.
    (Standard JUCE Sliders offer a similar capability, but unfortunately the standard
    Slider "text box" cannot also serve as an ordinary label.)

    Note the LabeledKnob object owns the label, but NOT the knob; a reference to the
    knob object is passed into the constructor. This allows any kind of knob derived
    from the base class BasicKnob to be used, and is an example of the "Decorator"
    pattern (https://en.wikipedia.org/wiki/Decorator_pattern).

    Implementation of the necessary mouse interaction is complicated. The essence
    is the use of a MouseListener object to correctly capture mouse-enter, -exit,
    and -move events for the LabeledKnob Component AND all its child components.
*/

class LabeledKnob : public Component, protected Timer
{
    friend class LKMouseListener;

public:
    LabeledKnob(const String& lblText, BasicKnob& theKnob, const int knobInset = 0);
    virtual ~LabeledKnob();

    void resized() override;

protected:
    void timerCallback() override;
    void clearMouseOvers();
    void updateMouseOvers(const MouseEvent&);
    void updateLabel(const MouseEvent&);

private:
    BasicKnob& knob;
    const int insetAmount;
    Label label;
    String labelText;
    bool mouseOverKnob, mouseOverLabel;

    class LKMouseListener : public MouseListener
    {
    public:
        LKMouseListener(LabeledKnob& lk) : owner(lk) {}

        void mouseEnter(const MouseEvent&) override;
        void mouseExit(const MouseEvent&) override;
        void mouseMove(const MouseEvent&) override;

    private:
        LabeledKnob& owner;
    };
    LKMouseListener mouseListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledKnob)
};
