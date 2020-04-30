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
#include "ParameterListeners.h"

class DragonflyPlateParameters
{
public:
    // Id's are symbolic names, Names are human-friendly names for GUI.
    // Labels are supplementary, typically used for units of measure.
    static const String dryLevelID, dryLevelName, dryLevelLabel;
    static const float dryLevelMin, dryLevelMax, dryLevelDefault, dryLevelStep;
    static const String wetLevelID, wetLevelName, wetLevelLabel;
    static const float wetLevelMin, wetLevelMax, wetLevelDefault, wetLevelStep;
    static const String algIndexID, algIndexName, algIndexLabel;
    static const int algIndexEnumCount, algIndexDefault;
    static const String widthID, widthName, widthLabel;
    static const float widthMin, widthMax, widthDefault, widthStep;
    static const String predelayID, predelayName, predelayLabel;
    static const float predelayMin, predelayMax, predelayDefault, predelayStep;
    static const String decayID, decayName, decayLabel;
    static const float decayMin, decayMax, decayDefault, decayStep;
    static const String lowCutID, lowCutName, lowCutLabel;
    static const float lowCutMin, lowCutMax, lowCutDefault, lowCutStep;
    static const String highCutID, highCutName, highCutLabel;
    static const float highCutMin, highCutMax, highCutDefault, highCutStep;
    static const String dampenID, dampenName, dampenLabel;
    static const float dampenMin, dampenMax, dampenDefault, dampenStep;
    static AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

public:
    DragonflyPlateParameters(AudioProcessorValueTreeState& vts,
                             AudioProcessorValueTreeState::Listener* processor);
    ~DragonflyPlateParameters();

    static void populateAlgorithmsComboBox(ComboBox&);
    static void populatePresetsComboBox(ComboBox&);

    void detachControls();
    void attachControls(
        Slider& dryLevelKnob,
        Slider& wetLevelKnob,
        ComboBox& algIndexCombo,
        Slider& widthKnob,
        Slider& predelayKnob,
        Slider& decayKnob,
        Slider& lowCutKnob,
        Slider& highCutKnob,
        Slider& dampenKnob );

    // working parameter values
    float dryLevel;
    float wetLevel;
    enum Algorithm { simple, nested, tank };
    Algorithm algIndex;
    float width;
    float predelay;
    float decay;
    float lowCut;
    float highCut;
    float dampen;

private:
    // Reference to AudioProcessorValueTreeState object that owns the parameter objects
    AudioProcessorValueTreeState& valueTreeState;

    // Pointer to processor as an AudioProcessorValueTreeState::Listener
    AudioProcessorValueTreeState::Listener* processorAsListener;

    // Attachment objects link GUI controls to parameters
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dryLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> wetLevelAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> algIndexAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> widthAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> predelayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCutAttachment;
    std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> dampenAttachment;

    // Listener objects link parameters to working variables
    FloatListener dryLevelListener;
    FloatListener wetLevelListener;
    EnumListener<Algorithm> algIndexListener;
    FloatListener widthListener;
    FloatListener predelayListener;
    FloatListener decayListener;
    FloatListener lowCutListener;
    FloatListener highCutListener;
    FloatListener dampenListener;
};
