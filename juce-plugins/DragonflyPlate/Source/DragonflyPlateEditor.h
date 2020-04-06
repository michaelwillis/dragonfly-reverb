#pragma once
#include "JuceHeader.h"
#include "DragonflyPlateProcessor.h"
#include "BasicLookAndFeel.h"
#include "InfoImage.h"
#include "IconButton.h"
#include "LabeledKnob.h"

class DragonflyPlateEditor  : public AudioProcessorEditor
{
public:
    DragonflyPlateEditor (DragonflyPlateProcessor&);
    virtual ~DragonflyPlateEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DragonflyPlateProcessor& processor;

    Image headerImage;
    InfoImage infoImage;

    GroupComponent mainGroup;
    IconButton infoButton;

    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob wetLevelKnob; LabeledKnob labeledWetLevelKnob;
    ComboBox algIndexCombo;
    BasicKnob widthKnob; LabeledKnob labeledWidthKnob;
    BasicKnob predelayKnob; LabeledKnob labeledPredelayKnob;
    BasicKnob decayKnob; LabeledKnob labeledDecayKnob;
    BasicKnob lowCutKnob; LabeledKnob labeledLowCutKnob;
    BasicKnob highCutKnob; LabeledKnob labeledHighCutKnob;
    BasicKnob dampenKnob; LabeledKnob labeledDampenKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyPlateEditor)
};
