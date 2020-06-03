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

#include "DragonflyPlateParameters.h"

// Dry Level
const String DragonflyPlateParameters::dryLevelID = "dryLevel";
const String DragonflyPlateParameters::dryLevelName = TRANS("Dry Level");
const String DragonflyPlateParameters::dryLevelLabel = "%";
const float DragonflyPlateParameters::dryLevelMin = 0.0f;
const float DragonflyPlateParameters::dryLevelMax = 100.0f;
const float DragonflyPlateParameters::dryLevelDefault = 85.0f;
const float DragonflyPlateParameters::dryLevelStep = 0.0f;
// Wet Level
const String DragonflyPlateParameters::wetLevelID = "wetLevel";
const String DragonflyPlateParameters::wetLevelName = TRANS("Wet Level");
const String DragonflyPlateParameters::wetLevelLabel = "%";
const float DragonflyPlateParameters::wetLevelMin = 0.0f;
const float DragonflyPlateParameters::wetLevelMax = 100.0f;
const float DragonflyPlateParameters::wetLevelDefault = 15.0f;
const float DragonflyPlateParameters::wetLevelStep = 0.0f;
// Algorithm
const String DragonflyPlateParameters::algIndexID = "algIndex";
const String DragonflyPlateParameters::algIndexName = TRANS("Algorithm");
const String DragonflyPlateParameters::algIndexLabel = "";
const int DragonflyPlateParameters::algIndexEnumCount = 3;
const int DragonflyPlateParameters::algIndexDefault = 1;
// Width
const String DragonflyPlateParameters::widthID = "width";
const String DragonflyPlateParameters::widthName = TRANS("Width");
const String DragonflyPlateParameters::widthLabel = "%";
const float DragonflyPlateParameters::widthMin = 50.0f;
const float DragonflyPlateParameters::widthMax = 150.0f;
const float DragonflyPlateParameters::widthDefault = 130.0f;
const float DragonflyPlateParameters::widthStep = 0.0f;
// Predelay
const String DragonflyPlateParameters::predelayID = "predelay";
const String DragonflyPlateParameters::predelayName = TRANS("Predelay");
const String DragonflyPlateParameters::predelayLabel = "ms";
const float DragonflyPlateParameters::predelayMin = 0.0f;
const float DragonflyPlateParameters::predelayMax = 500.0f;
const float DragonflyPlateParameters::predelayDefault = 125.0f;
const float DragonflyPlateParameters::predelayStep = 0.0f;
// Decay
const String DragonflyPlateParameters::decayID = "decay";
const String DragonflyPlateParameters::decayName = TRANS("Decay");
const String DragonflyPlateParameters::decayLabel = "sec";
const float DragonflyPlateParameters::decayMin = 0.1f;
const float DragonflyPlateParameters::decayMax = 10.0f;
const float DragonflyPlateParameters::decayDefault = 3.0f;
const float DragonflyPlateParameters::decayStep = 0.0f;
// Low Cut
const String DragonflyPlateParameters::lowCutID = "lowCut";
const String DragonflyPlateParameters::lowCutName = TRANS("Low Cut");
const String DragonflyPlateParameters::lowCutLabel = "Hz";
const float DragonflyPlateParameters::lowCutMin = 0.0f;
const float DragonflyPlateParameters::lowCutMax = 3000.0f;
const float DragonflyPlateParameters::lowCutDefault = 750.0f;
const float DragonflyPlateParameters::lowCutStep = 0.0f;
// High Cut
const String DragonflyPlateParameters::highCutID = "highCut";
const String DragonflyPlateParameters::highCutName = TRANS("High Cut");
const String DragonflyPlateParameters::highCutLabel = "Hz";
const float DragonflyPlateParameters::highCutMin = 1000.0f;
const float DragonflyPlateParameters::highCutMax = 16000.0f;
const float DragonflyPlateParameters::highCutDefault = 16000.0f;
const float DragonflyPlateParameters::highCutStep = 0.0f;
// Dampen
const String DragonflyPlateParameters::dampenID = "dampen";
const String DragonflyPlateParameters::dampenName = TRANS("Dampen");
const String DragonflyPlateParameters::dampenLabel = "Hz";
const float DragonflyPlateParameters::dampenMin = 1000.0f;
const float DragonflyPlateParameters::dampenMax = 16000.0f;
const float DragonflyPlateParameters::dampenDefault = 16000.0f;
const float DragonflyPlateParameters::dampenStep = 0.0f;

AudioProcessorValueTreeState::ParameterLayout DragonflyPlateParameters::createParameterLayout()
{
    std::vector<std::unique_ptr<RangedAudioParameter>> params;

    params.push_back(std::make_unique<AudioParameterFloat>(
        dryLevelID, dryLevelName,
        NormalisableRange<float>(dryLevelMin, dryLevelMax, dryLevelStep), dryLevelDefault,
        dryLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        wetLevelID, wetLevelName,
        NormalisableRange<float>(wetLevelMin, wetLevelMax, wetLevelStep), wetLevelDefault,
        wetLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterInt>(
        algIndexID, algIndexName,
        0, algIndexEnumCount - 1, algIndexDefault,
        algIndexLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        widthID, widthName,
        NormalisableRange<float>(widthMin, widthMax, widthStep), widthDefault,
        widthLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        predelayID, predelayName,
        NormalisableRange<float>(predelayMin, predelayMax, predelayStep), predelayDefault,
        predelayLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        decayID, decayName,
        NormalisableRange<float>(decayMin, decayMax, decayStep), decayDefault,
        decayLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lowCutID, lowCutName,
        NormalisableRange<float>(lowCutMin, lowCutMax, lowCutStep), lowCutDefault,
        lowCutLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        highCutID, highCutName,
        NormalisableRange<float>(highCutMin, highCutMax, highCutStep), highCutDefault,
        highCutLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        dampenID, dampenName,
        NormalisableRange<float>(dampenMin, dampenMax, dampenStep), dampenDefault,
        dampenLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

void DragonflyPlateParameters::populateAlgorithmsComboBox(ComboBox& cb)
{
    int itemID = 0;
    cb.addItem("Simple", ++itemID);
    cb.addItem("Nested", ++itemID);
    cb.addItem("Tank", ++itemID);
}

void DragonflyPlateParameters::populatePresetsComboBox(ComboBox& cb)
{
    int itemID = 0;
    cb.addItem("Abrupt Plate", ++itemID);
    cb.addItem("Bright Plate", ++itemID);
    cb.addItem("Clear Plate", ++itemID);
    cb.addItem("Dark Plate", ++itemID);
    cb.addItem("Foil Tray", ++itemID);
    cb.addItem("Metal Roof", ++itemID);
    cb.addItem("Narrow Tank", ++itemID);
    cb.addItem("Phat Tank", ++itemID);
}

DragonflyPlateParameters::DragonflyPlateParameters(AudioProcessorValueTreeState& vts,
                                                   AudioProcessorValueTreeState::Listener* processor)
    : dryLevel(0.01f * dryLevelDefault)
    , wetLevel(0.01f * wetLevelDefault)
    , algIndex(Algorithm(algIndexDefault))
    , width(0.01f * widthDefault)
    , predelay(predelayDefault)
    , decay(decayDefault)
    , lowCut(lowCutDefault)
    , highCut(highCutDefault)
    , dampen(dampenDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , dryLevelListener(dryLevel, 0.01f)
    , wetLevelListener(wetLevel, 0.01f)
    , algIndexListener(algIndex)
    , widthListener(width, 0.01f)
    , predelayListener(predelay)
    , decayListener(decay)
    , lowCutListener(lowCut)
    , highCutListener(highCut)
    , dampenListener(dampen)
{
    valueTreeState.addParameterListener(dryLevelID, processorAsListener);
    valueTreeState.addParameterListener(wetLevelID, processorAsListener);
    valueTreeState.addParameterListener(algIndexID, processorAsListener);
    valueTreeState.addParameterListener(widthID, processorAsListener);
    valueTreeState.addParameterListener(predelayID, processorAsListener);
    valueTreeState.addParameterListener(decayID, processorAsListener);
    valueTreeState.addParameterListener(lowCutID, processorAsListener);
    valueTreeState.addParameterListener(highCutID, processorAsListener);
    valueTreeState.addParameterListener(dampenID, processorAsListener);

    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.addParameterListener(algIndexID, &algIndexListener);
    valueTreeState.addParameterListener(widthID, &widthListener);
    valueTreeState.addParameterListener(predelayID, &predelayListener);
    valueTreeState.addParameterListener(decayID, &decayListener);
    valueTreeState.addParameterListener(lowCutID, &lowCutListener);
    valueTreeState.addParameterListener(highCutID, &highCutListener);
    valueTreeState.addParameterListener(dampenID, &dampenListener);
}

DragonflyPlateParameters::~DragonflyPlateParameters()
{
    detachControls();
    
    valueTreeState.removeParameterListener(dryLevelID, processorAsListener);
    valueTreeState.removeParameterListener(wetLevelID, processorAsListener);
    valueTreeState.removeParameterListener(algIndexID, processorAsListener);
    valueTreeState.removeParameterListener(widthID, processorAsListener);
    valueTreeState.removeParameterListener(predelayID, processorAsListener);
    valueTreeState.removeParameterListener(decayID, processorAsListener);
    valueTreeState.removeParameterListener(lowCutID, processorAsListener);
    valueTreeState.removeParameterListener(highCutID, processorAsListener);
    valueTreeState.removeParameterListener(dampenID, processorAsListener);

    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.removeParameterListener(algIndexID, &algIndexListener);
    valueTreeState.removeParameterListener(widthID, &widthListener);
    valueTreeState.removeParameterListener(predelayID, &predelayListener);
    valueTreeState.removeParameterListener(decayID, &decayListener);
    valueTreeState.removeParameterListener(lowCutID, &lowCutListener);
    valueTreeState.removeParameterListener(highCutID, &highCutListener);
    valueTreeState.removeParameterListener(dampenID, &dampenListener);
}

void DragonflyPlateParameters::detachControls()
{
    dryLevelAttachment.reset(nullptr);
    wetLevelAttachment.reset(nullptr);
    algIndexAttachment.reset(nullptr);
    widthAttachment.reset(nullptr);
    predelayAttachment.reset(nullptr);
    decayAttachment.reset(nullptr);
    lowCutAttachment.reset(nullptr);
    highCutAttachment.reset(nullptr);
    dampenAttachment.reset(nullptr);
}

void DragonflyPlateParameters::attachControls(
    Slider& dryLevelKnob,
    Slider& wetLevelKnob,
    ComboBox& algIndexCombo,
    Slider& widthKnob,
    Slider& predelayKnob,
    Slider& decayKnob,
    Slider& lowCutKnob,
    Slider& highCutKnob,
    Slider& dampenKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    wetLevelAttachment.reset(new SlAt(valueTreeState, wetLevelID, wetLevelKnob));
    algIndexAttachment.reset(new CbAt(valueTreeState, algIndexID, algIndexCombo));
    widthAttachment.reset(new SlAt(valueTreeState, widthID, widthKnob));
    predelayAttachment.reset(new SlAt(valueTreeState, predelayID, predelayKnob));
    decayAttachment.reset(new SlAt(valueTreeState, decayID, decayKnob));
    lowCutAttachment.reset(new SlAt(valueTreeState, lowCutID, lowCutKnob));
    highCutAttachment.reset(new SlAt(valueTreeState, highCutID, highCutKnob));
    dampenAttachment.reset(new SlAt(valueTreeState, dampenID, dampenKnob));
}
