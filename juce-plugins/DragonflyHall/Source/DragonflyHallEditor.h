#pragma once
#include "JuceHeader.h"
#include "DragonflyHallProcessor.h"
#include "BasicLookAndFeel.h"
#include "InfoImage.h"
#include "IconButton.h"
#include "LabeledKnob.h"

class DragonflyHallEditor   : public AudioProcessorEditor
                            , public ChangeListener
{
public:
    DragonflyHallEditor (DragonflyHallProcessor&);
    virtual ~DragonflyHallEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DragonflyHallProcessor& processor;

    Image headerImage;
    InfoImage infoImage;

    GroupComponent topGroup, middleGroup, bottomGroup;
    IconButton infoButton;
    ComboBox presetsCombo;

    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob earlyLevelKnob; LabeledKnob labeledEarlyLevelKnob;
    BasicKnob lateLevelKnob; LabeledKnob labeledLateLevelKnob;
    BasicKnob sizeKnob; LabeledKnob labeledSizeKnob;
    BasicKnob widthKnob; LabeledKnob labeledWidthKnob;
    BasicKnob predelayKnob; LabeledKnob labeledPredelayKnob;
    BasicKnob diffuseKnob; LabeledKnob labeledDiffuseKnob;
    BasicKnob lowCutKnob; LabeledKnob labeledLowCutKnob;
    BasicKnob lowCrossKnob; LabeledKnob labeledLowCrossKnob;
    BasicKnob lowMultKnob; LabeledKnob labeledLowMultKnob;
    BasicKnob highCutKnob; LabeledKnob labeledHighCutKnob;
    BasicKnob highCrossKnob; LabeledKnob labeledHighCrossKnob;
    BasicKnob highMultKnob; LabeledKnob labeledHighMultKnob;
    BasicKnob spinKnob; LabeledKnob labeledSpinKnob;
    BasicKnob wanderKnob; LabeledKnob labeledWanderKnob;
    BasicKnob decayKnob; LabeledKnob labeledDecayKnob;
    BasicKnob earlySendKnob; LabeledKnob labeledEarlySendKnob;
    BasicKnob modulationKnob; LabeledKnob labeledModulationKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyHallEditor)
};
