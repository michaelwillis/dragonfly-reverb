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
