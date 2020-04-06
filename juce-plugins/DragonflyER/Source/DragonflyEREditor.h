#pragma once
#include "JuceHeader.h"
#include "DragonflyERProcessor.h"
#include "BasicLookAndFeel.h"
#include "InfoImage.h"
#include "IconButton.h"
#include "LabeledKnob.h"

class DragonflyEREditor  : public AudioProcessorEditor
{
public:
    DragonflyEREditor (DragonflyERProcessor&);
    virtual ~DragonflyEREditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DragonflyERProcessor& processor;

    Image headerImage;
    InfoImage infoImage;

    GroupComponent mainGroup;
    IconButton infoButton;

    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob wetLevelKnob; LabeledKnob labeledWetLevelKnob;
    ComboBox progIndexCombo; Label progIndexLabel;
    BasicKnob sizeKnob; LabeledKnob labeledSizeKnob;
    BasicKnob widthKnob; LabeledKnob labeledWidthKnob;
    BasicKnob lowCutKnob; LabeledKnob labeledLowCutKnob;
    BasicKnob highCutKnob; LabeledKnob labeledHighCutKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyEREditor)
};
