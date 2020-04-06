#include "LabeledKnob.h"

LabeledKnob::LabeledKnob(const String& lblText, BasicKnob& theKnob, const int knobInset)
    : knob(theKnob)
    , insetAmount(knobInset)
    , labelText(lblText)
    , mouseOverKnob(false)
    , mouseOverLabel(false)
    , mouseListener(*this)
{
    addAndMakeVisible(&knob);
    label.setText(labelText, dontSendNotification);
    label.setJustificationType(Justification::centred);
    label.setEditable(false, true, true);
    addAndMakeVisible(&label);
    addMouseListener(&mouseListener, true);

    knob.onValueChange = [this]()
    {
        if (mouseOverKnob || mouseOverLabel)
        {
            String vs = knob.getTextFromValue(knob.getValue());
            label.setText(vs, dontSendNotification);
            startTimer(3000);
        }
    };

    label.onTextChange = [this]()
    {
        knob.setValue(knob.getValueFromText(label.getText()));
    };
}

LabeledKnob::~LabeledKnob()
{
    removeMouseListener(&mouseListener);
}

void LabeledKnob::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromBottom(16));
    knob.setBounds(bounds.reduced(insetAmount));
}

void LabeledKnob::timerCallback()
{
    if (!mouseOverKnob && !mouseOverLabel && !label.isBeingEdited())
    {
        stopTimer();
        label.setText(labelText, dontSendNotification);
    }
}

void LabeledKnob::clearMouseOvers()
{
    mouseOverKnob = false;
    mouseOverLabel = false;
}

void LabeledKnob::updateMouseOvers(const MouseEvent& evt)
{
    MouseEvent mevt = evt.getEventRelativeTo(this);
    int knobDiameter = jmin(knob.getWidth(), knob.getHeight()) - 20;
    Rectangle<int> knobRect = knob.getBounds().withSizeKeepingCentre(knobDiameter, knobDiameter);
    knobRect.setHeight(knobDiameter + 10);
    mouseOverKnob = knobRect.contains(mevt.getPosition());
    mouseOverLabel = label.getBounds().contains(mevt.getPosition());
}

void LabeledKnob::updateLabel(const MouseEvent& evt)
{
    updateMouseOvers(evt);
    if (label.isBeingEdited()) return;

    if (mouseOverKnob || mouseOverLabel)
    {
        label.setText(knob.getTextFromValue(knob.getValue()), dontSendNotification);
    }
    else startTimer(3000);
}


void LabeledKnob::LKMouseListener::mouseEnter(const MouseEvent& evt)
{
    if (!owner.isEnabled()) return;
    owner.updateLabel(evt);
}

void LabeledKnob::LKMouseListener::mouseExit(const MouseEvent&)
{
    if (!owner.isEnabled()) return;
    owner.clearMouseOvers();
    owner.timerCallback();
}

void LabeledKnob::LKMouseListener::mouseMove(const MouseEvent& evt)
{
    if (!owner.isEnabled()) return;
    owner.updateLabel(evt);
}
