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
