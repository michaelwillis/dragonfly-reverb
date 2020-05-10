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

#include "DragonflyERParameters.h"

// Dry Level
const String DragonflyERParameters::dryLevelID = "dryLevel";
const String DragonflyERParameters::dryLevelName = TRANS("Dry Level");
const String DragonflyERParameters::dryLevelLabel = "%";
const float DragonflyERParameters::dryLevelMin = 0.0f;
const float DragonflyERParameters::dryLevelMax = 100.0f;
const float DragonflyERParameters::dryLevelDefault = 80.0f;
const float DragonflyERParameters::dryLevelStep = 0.0f;
// Wet Level
const String DragonflyERParameters::wetLevelID = "wetLevel";
const String DragonflyERParameters::wetLevelName = TRANS("Wet Level");
const String DragonflyERParameters::wetLevelLabel = "%";
const float DragonflyERParameters::wetLevelMin = 0.0f;
const float DragonflyERParameters::wetLevelMax = 100.0f;
const float DragonflyERParameters::wetLevelDefault = 20.0f;
const float DragonflyERParameters::wetLevelStep = 0.0f;
// Program
const String DragonflyERParameters::progIndexID = "progIndex";
const String DragonflyERParameters::progIndexName = TRANS("Program");
const String DragonflyERParameters::progIndexLabel = "";
const int DragonflyERParameters::progIndexEnumCount = 8;
const int DragonflyERParameters::progIndexDefault = 0;
// Size
const String DragonflyERParameters::sizeID = "size";
const String DragonflyERParameters::sizeName = TRANS("Size");
const String DragonflyERParameters::sizeLabel = "m";
const float DragonflyERParameters::sizeMin = 10.0f;
const float DragonflyERParameters::sizeMax = 60.0f;
const float DragonflyERParameters::sizeDefault = 20.0f;
const float DragonflyERParameters::sizeStep = 0.0f;
// Width
const String DragonflyERParameters::widthID = "width";
const String DragonflyERParameters::widthName = TRANS("Width");
const String DragonflyERParameters::widthLabel = "%";
const float DragonflyERParameters::widthMin = 50.0f;
const float DragonflyERParameters::widthMax = 150.0f;
const float DragonflyERParameters::widthDefault = 100.0f;
const float DragonflyERParameters::widthStep = 0.0f;
// Low Cut
const String DragonflyERParameters::lowCutID = "lowCut";
const String DragonflyERParameters::lowCutName = TRANS("Low Cut");
const String DragonflyERParameters::lowCutLabel = "Hz";
const float DragonflyERParameters::lowCutMin = 0.0f;
const float DragonflyERParameters::lowCutMax = 3000.0f;
const float DragonflyERParameters::lowCutDefault = 50.0f;
const float DragonflyERParameters::lowCutStep = 0.0f;
// High Cut
const String DragonflyERParameters::highCutID = "highCut";
const String DragonflyERParameters::highCutName = TRANS("High Cut");
const String DragonflyERParameters::highCutLabel = "Hz";
const float DragonflyERParameters::highCutMin = 1000.0f;
const float DragonflyERParameters::highCutMax = 16000.0f;
const float DragonflyERParameters::highCutDefault = 10000.0f;
const float DragonflyERParameters::highCutStep = 0.0f;

AudioProcessorValueTreeState::ParameterLayout DragonflyERParameters::createParameterLayout()
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
        progIndexID, progIndexName,
        0, progIndexEnumCount - 1, progIndexDefault,
        progIndexLabel,
        [](int value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getIntValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        sizeID, sizeName,
        NormalisableRange<float>(sizeMin, sizeMax, sizeStep), sizeDefault,
        sizeLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        widthID, widthName,
        NormalisableRange<float>(widthMin, widthMax, widthStep), widthDefault,
        widthLabel,
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

    return { params.begin(), params.end() };
}

void DragonflyERParameters::populateProgramsComboBox(ComboBox& cb)
{
    int itemID = 0;
    cb.addItem("Abrupt Echo", ++itemID);
    cb.addItem("Backstage Pass", ++itemID);
    cb.addItem("Concert Venue", ++itemID);
    cb.addItem("Damaged Goods", ++itemID);
    cb.addItem("Elevator Pitch", ++itemID);
    cb.addItem("Floor Thirteen", ++itemID);
    cb.addItem("Garage Band", ++itemID);
    cb.addItem("Home Studio", ++itemID);
}

DragonflyERParameters::DragonflyERParameters(AudioProcessorValueTreeState& vts,
                                             AudioProcessorValueTreeState::Listener* processor)
    : dryLevel(0.01f * dryLevelDefault)
    , wetLevel(0.01f * wetLevelDefault)
    , progIndex(Program(progIndexDefault))
    , size(sizeDefault)
    , width(0.01f * widthDefault)
    , lowCut(lowCutDefault)
    , highCut(highCutDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , dryLevelListener(dryLevel, 0.01f)
    , wetLevelListener(wetLevel, 0.01f)
    , progIndexListener(progIndex)
    , sizeListener(size)
    , widthListener(width, 0.01f)
    , lowCutListener(lowCut)
    , highCutListener(highCut)
{
    valueTreeState.addParameterListener(dryLevelID, processorAsListener);
    valueTreeState.addParameterListener(wetLevelID, processorAsListener);
    valueTreeState.addParameterListener(progIndexID, processorAsListener);
    valueTreeState.addParameterListener(sizeID, processorAsListener);
    valueTreeState.addParameterListener(widthID, processorAsListener);
    valueTreeState.addParameterListener(lowCutID, processorAsListener);
    valueTreeState.addParameterListener(highCutID, processorAsListener);

    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.addParameterListener(progIndexID, &progIndexListener);
    valueTreeState.addParameterListener(sizeID, &sizeListener);
    valueTreeState.addParameterListener(widthID, &widthListener);
    valueTreeState.addParameterListener(lowCutID, &lowCutListener);
    valueTreeState.addParameterListener(highCutID, &highCutListener);
}

DragonflyERParameters::~DragonflyERParameters()
{
    detachControls();
    
    valueTreeState.removeParameterListener(dryLevelID, processorAsListener);
    valueTreeState.removeParameterListener(wetLevelID, processorAsListener);
    valueTreeState.removeParameterListener(progIndexID, processorAsListener);
    valueTreeState.removeParameterListener(sizeID, processorAsListener);
    valueTreeState.removeParameterListener(widthID, processorAsListener);
    valueTreeState.removeParameterListener(lowCutID, processorAsListener);
    valueTreeState.removeParameterListener(highCutID, processorAsListener);

    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(wetLevelID, &wetLevelListener);
    valueTreeState.removeParameterListener(progIndexID, &progIndexListener);
    valueTreeState.removeParameterListener(sizeID, &sizeListener);
    valueTreeState.removeParameterListener(widthID, &widthListener);
    valueTreeState.removeParameterListener(lowCutID, &lowCutListener);
    valueTreeState.removeParameterListener(highCutID, &highCutListener);
}

void DragonflyERParameters::detachControls()
{
    dryLevelAttachment.reset(nullptr);
    wetLevelAttachment.reset(nullptr);
    progIndexAttachment.reset(nullptr);
    sizeAttachment.reset(nullptr);
    widthAttachment.reset(nullptr);
    lowCutAttachment.reset(nullptr);
    highCutAttachment.reset(nullptr);
}

void DragonflyERParameters::attachControls(
    Slider& dryLevelKnob,
    Slider& wetLevelKnob,
    ComboBox& progIndexCombo,
    Slider& sizeKnob,
    Slider& widthKnob,
    Slider& lowCutKnob,
    Slider& highCutKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    wetLevelAttachment.reset(new SlAt(valueTreeState, wetLevelID, wetLevelKnob));
    progIndexAttachment.reset(new CbAt(valueTreeState, progIndexID, progIndexCombo));
    sizeAttachment.reset(new SlAt(valueTreeState, sizeID, sizeKnob));
    widthAttachment.reset(new SlAt(valueTreeState, widthID, widthKnob));
    lowCutAttachment.reset(new SlAt(valueTreeState, lowCutID, lowCutKnob));
    highCutAttachment.reset(new SlAt(valueTreeState, highCutID, highCutKnob));
}
