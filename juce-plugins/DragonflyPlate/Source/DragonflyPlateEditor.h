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
