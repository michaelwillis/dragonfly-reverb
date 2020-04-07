#pragma once
#include "JuceHeader.h"
#include "DragonflyRoomProcessor.h"
#include "BasicLookAndFeel.h"
#include "InfoImage.h"
#include "IconButton.h"
#include "LabeledKnob.h"

class DragonflyRoomEditor   : public AudioProcessorEditor
                            , public ChangeListener
{
public:
    DragonflyRoomEditor (DragonflyRoomProcessor&);
    virtual ~DragonflyRoomEditor();

    // Component
    void paint (Graphics&) override;
    void resized() override;

    // ChangeListener
    void changeListenerCallback(ChangeBroadcaster*) override;

private:
    SharedResourcePointer<BasicLookAndFeel> lookAndFeel;
    DragonflyRoomProcessor& processor;

    Image headerImage;
    InfoImage infoImage;

    GroupComponent topGroup, bottomGroup;
    IconButton infoButton;
    ComboBox presetsCombo;

    BasicKnob dryLevelKnob; LabeledKnob labeledDryLevelKnob;
    BasicKnob earlyLevelKnob; LabeledKnob labeledEarlyLevelKnob;
    BasicKnob earlySendKnob; LabeledKnob labeledEarlySendKnob;
    BasicKnob lateLevelKnob; LabeledKnob labeledLateLevelKnob;
    BasicKnob sizeKnob; LabeledKnob labeledSizeKnob;
    BasicKnob widthKnob; LabeledKnob labeledWidthKnob;
    BasicKnob predelayKnob; LabeledKnob labeledPredelayKnob;
    BasicKnob decayKnob; LabeledKnob labeledDecayKnob;
    BasicKnob diffuseKnob; LabeledKnob labeledDiffuseKnob;
    BasicKnob spinKnob; LabeledKnob labeledSpinKnob;
    BasicKnob wanderKnob; LabeledKnob labeledWanderKnob;
    BasicKnob highCutKnob; LabeledKnob labeledHighCutKnob;
    BasicKnob earlyDampKnob; LabeledKnob labeledEarlyDampKnob;
    BasicKnob lateDampKnob; LabeledKnob labeledLateDampKnob;
    BasicKnob lowBoostKnob; LabeledKnob labeledLowBoostKnob;
    BasicKnob boostFreqKnob; LabeledKnob labeledBoostFreqKnob;
    BasicKnob lowCutKnob; LabeledKnob labeledLowCutKnob;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyRoomEditor)
};
