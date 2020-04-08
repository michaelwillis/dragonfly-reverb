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

#include "DragonflyRoomParameters.h"

// Dry Level
const String DragonflyRoomParameters::dryLevelID = "dryLevel";
const String DragonflyRoomParameters::dryLevelName = TRANS("Dry Level");
const String DragonflyRoomParameters::dryLevelLabel = "%";
const float DragonflyRoomParameters::dryLevelMin = 0.0f;
const float DragonflyRoomParameters::dryLevelMax = 100.0f;
const float DragonflyRoomParameters::dryLevelDefault = 80.0f;
const float DragonflyRoomParameters::dryLevelStep = 0.0f;
// Early Level
const String DragonflyRoomParameters::earlyLevelID = "earlyLevel";
const String DragonflyRoomParameters::earlyLevelName = TRANS("Early Level");
const String DragonflyRoomParameters::earlyLevelLabel = "%";
const float DragonflyRoomParameters::earlyLevelMin = 0.0f;
const float DragonflyRoomParameters::earlyLevelMax = 100.0f;
const float DragonflyRoomParameters::earlyLevelDefault = 10.0f;
const float DragonflyRoomParameters::earlyLevelStep = 0.0f;
// Early Send
const String DragonflyRoomParameters::earlySendID = "earlySend";
const String DragonflyRoomParameters::earlySendName = TRANS("Early Send");
const String DragonflyRoomParameters::earlySendLabel = "%";
const float DragonflyRoomParameters::earlySendMin = 0.0f;
const float DragonflyRoomParameters::earlySendMax = 100.0f;
const float DragonflyRoomParameters::earlySendDefault = 20.0f;
const float DragonflyRoomParameters::earlySendStep = 0.0f;
// Late Level
const String DragonflyRoomParameters::lateLevelID = "lateLevel";
const String DragonflyRoomParameters::lateLevelName = TRANS("Late Level");
const String DragonflyRoomParameters::lateLevelLabel = "%";
const float DragonflyRoomParameters::lateLevelMin = 0.0f;
const float DragonflyRoomParameters::lateLevelMax = 100.0f;
const float DragonflyRoomParameters::lateLevelDefault = 20.0f;
const float DragonflyRoomParameters::lateLevelStep = 0.0f;
// Size
const String DragonflyRoomParameters::sizeID = "size";
const String DragonflyRoomParameters::sizeName = TRANS("Size");
const String DragonflyRoomParameters::sizeLabel = "m";
const float DragonflyRoomParameters::sizeMin = 8.0f;
const float DragonflyRoomParameters::sizeMax = 32.0f;
const float DragonflyRoomParameters::sizeDefault = 12.0f;
const float DragonflyRoomParameters::sizeStep = 0.0f;
// Width
const String DragonflyRoomParameters::widthID = "width";
const String DragonflyRoomParameters::widthName = TRANS("Width");
const String DragonflyRoomParameters::widthLabel = "%";
const float DragonflyRoomParameters::widthMin = 50.0f;
const float DragonflyRoomParameters::widthMax = 150.0f;
const float DragonflyRoomParameters::widthDefault = 100.0f;
const float DragonflyRoomParameters::widthStep = 0.0f;
// Predelay
const String DragonflyRoomParameters::predelayID = "predelay";
const String DragonflyRoomParameters::predelayName = TRANS("Predelay");
const String DragonflyRoomParameters::predelayLabel = "ms";
const float DragonflyRoomParameters::predelayMin = 0.0f;
const float DragonflyRoomParameters::predelayMax = 100.0f;
const float DragonflyRoomParameters::predelayDefault = 8.0f;
const float DragonflyRoomParameters::predelayStep = 0.0f;
// Decay
const String DragonflyRoomParameters::decayID = "decay";
const String DragonflyRoomParameters::decayName = TRANS("Decay");
const String DragonflyRoomParameters::decayLabel = "sec";
const float DragonflyRoomParameters::decayMin = 0.1f;
const float DragonflyRoomParameters::decayMax = 10.0f;
const float DragonflyRoomParameters::decayDefault = 0.4f;
const float DragonflyRoomParameters::decayStep = 0.0f;
// Diffuse
const String DragonflyRoomParameters::diffuseID = "diffuse";
const String DragonflyRoomParameters::diffuseName = TRANS("Diffuse");
const String DragonflyRoomParameters::diffuseLabel = "%";
const float DragonflyRoomParameters::diffuseMin = 0.0f;
const float DragonflyRoomParameters::diffuseMax = 100.0f;
const float DragonflyRoomParameters::diffuseDefault = 70.0f;
const float DragonflyRoomParameters::diffuseStep = 0.0f;
// Spin
const String DragonflyRoomParameters::spinID = "spin";
const String DragonflyRoomParameters::spinName = TRANS("Spin");
const String DragonflyRoomParameters::spinLabel = "Hz";
const float DragonflyRoomParameters::spinMin = 0.0f;
const float DragonflyRoomParameters::spinMax = 5.0f;
const float DragonflyRoomParameters::spinDefault = 0.8f;
const float DragonflyRoomParameters::spinStep = 0.0f;
// Wander
const String DragonflyRoomParameters::wanderID = "wander";
const String DragonflyRoomParameters::wanderName = TRANS("Wander");
const String DragonflyRoomParameters::wanderLabel = "%";
const float DragonflyRoomParameters::wanderMin = 0.0f;
const float DragonflyRoomParameters::wanderMax = 100.0f;
const float DragonflyRoomParameters::wanderDefault = 40.0f;
const float DragonflyRoomParameters::wanderStep = 0.0f;
// High Cut
const String DragonflyRoomParameters::highCutID = "highCut";
const String DragonflyRoomParameters::highCutName = TRANS("High Cut");
const String DragonflyRoomParameters::highCutLabel = "Hz";
const float DragonflyRoomParameters::highCutMin = 1000.0f;
const float DragonflyRoomParameters::highCutMax = 16000.0f;
const float DragonflyRoomParameters::highCutDefault = 16000.0f;
const float DragonflyRoomParameters::highCutStep = 0.0f;
// Early Damp
const String DragonflyRoomParameters::earlyDampID = "earlyDamp";
const String DragonflyRoomParameters::earlyDampName = TRANS("Early Damp");
const String DragonflyRoomParameters::earlyDampLabel = "Hz";
const float DragonflyRoomParameters::earlyDampMin = 1000.0f;
const float DragonflyRoomParameters::earlyDampMax = 16000.0f;
const float DragonflyRoomParameters::earlyDampDefault = 10000.0f;
const float DragonflyRoomParameters::earlyDampStep = 0.0f;
// Late Damp
const String DragonflyRoomParameters::lateDampID = "lateDamp";
const String DragonflyRoomParameters::lateDampName = TRANS("Late Damp");
const String DragonflyRoomParameters::lateDampLabel = "Hz";
const float DragonflyRoomParameters::lateDampMin = 1000.0f;
const float DragonflyRoomParameters::lateDampMax = 16000.0f;
const float DragonflyRoomParameters::lateDampDefault = 9400.0f;
const float DragonflyRoomParameters::lateDampStep = 0.0f;
// Low Boost
const String DragonflyRoomParameters::lowBoostID = "lowBoost";
const String DragonflyRoomParameters::lowBoostName = TRANS("Low Boost");
const String DragonflyRoomParameters::lowBoostLabel = "%";
const float DragonflyRoomParameters::lowBoostMin = 0.0f;
const float DragonflyRoomParameters::lowBoostMax = 100.0f;
const float DragonflyRoomParameters::lowBoostDefault = 50.0f;
const float DragonflyRoomParameters::lowBoostStep = 0.0f;
// Boost Freq
const String DragonflyRoomParameters::boostFreqID = "boostFreq";
const String DragonflyRoomParameters::boostFreqName = TRANS("Boost Freq");
const String DragonflyRoomParameters::boostFreqLabel = "Hz";
const float DragonflyRoomParameters::boostFreqMin = 50.0f;
const float DragonflyRoomParameters::boostFreqMax = 1050.0f;
const float DragonflyRoomParameters::boostFreqDefault = 600.0f;
const float DragonflyRoomParameters::boostFreqStep = 0.0f;
// Low Cut
const String DragonflyRoomParameters::lowCutID = "lowCut";
const String DragonflyRoomParameters::lowCutName = TRANS("Low Cut");
const String DragonflyRoomParameters::lowCutLabel = "Hz";
const float DragonflyRoomParameters::lowCutMin = 0.0f;
const float DragonflyRoomParameters::lowCutMax = 200.0f;
const float DragonflyRoomParameters::lowCutDefault = 4.0f;
const float DragonflyRoomParameters::lowCutStep = 0.0f;

AudioProcessorValueTreeState::ParameterLayout DragonflyRoomParameters::createParameterLayout()
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
        earlyLevelID, earlyLevelName,
        NormalisableRange<float>(earlyLevelMin, earlyLevelMax, earlyLevelStep), earlyLevelDefault,
        earlyLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        earlySendID, earlySendName,
        NormalisableRange<float>(earlySendMin, earlySendMax, earlySendStep), earlySendDefault,
        earlySendLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lateLevelID, lateLevelName,
        NormalisableRange<float>(lateLevelMin, lateLevelMax, lateLevelStep), lateLevelDefault,
        lateLevelLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
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
        diffuseID, diffuseName,
        NormalisableRange<float>(diffuseMin, diffuseMax, diffuseStep), diffuseDefault,
        diffuseLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        spinID, spinName,
        NormalisableRange<float>(spinMin, spinMax, spinStep), spinDefault,
        spinLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        wanderID, wanderName,
        NormalisableRange<float>(wanderMin, wanderMax, wanderStep), wanderDefault,
        wanderLabel,
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
        earlyDampID, earlyDampName,
        NormalisableRange<float>(earlyDampMin, earlyDampMax, earlyDampStep), earlyDampDefault,
        earlyDampLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lateDampID, lateDampName,
        NormalisableRange<float>(lateDampMin, lateDampMax, lateDampStep), lateDampDefault,
        lateDampLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lowBoostID, lowBoostName,
        NormalisableRange<float>(lowBoostMin, lowBoostMax, lowBoostStep), lowBoostDefault,
        lowBoostLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        boostFreqID, boostFreqName,
        NormalisableRange<float>(boostFreqMin, boostFreqMax, boostFreqStep), boostFreqDefault,
        boostFreqLabel,
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

    return { params.begin(), params.end() };
}

DragonflyRoomParameters::DragonflyRoomParameters(AudioProcessorValueTreeState& vts,
                                           AudioProcessorValueTreeState::Listener* processor)
    : dryLevel(0.01f * dryLevelDefault)
    , earlyLevel(0.01f * earlyLevelDefault)
    , earlySend(0.01f * earlySendDefault)
    , lateLevel(0.01f * lateLevelDefault)
    , size(sizeDefault)
    , width(0.01f * widthDefault)
    , predelay(predelayDefault)
    , decay(decayDefault)
    , diffuse(0.01f * diffuseDefault)
    , spin(spinDefault)
    , wander(0.01f * wanderDefault)
    , highCut(highCutDefault)
    , earlyDamp(earlyDampDefault)
    , lateDamp(lateDampDefault)
    , lowBoost(0.01f * lowBoostDefault)
    , boostFreq(boostFreqDefault)
    , lowCut(lowCutDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , dryLevelListener(dryLevel, 0.01f)
    , earlyLevelListener(earlyLevel, 0.01f)
    , earlySendListener(earlySend, 0.01f)
    , lateLevelListener(lateLevel, 0.01f)
    , sizeListener(size)
    , widthListener(width, 0.01f)
    , predelayListener(predelay)
    , decayListener(decay)
    , diffuseListener(diffuse, 0.01f)
    , spinListener(spin)
    , wanderListener(wander, 0.01f)
    , highCutListener(highCut)
    , earlyDampListener(earlyDamp)
    , lateDampListener(lateDamp)
    , lowBoostListener(lowBoost, 0.01f)
    , boostFreqListener(boostFreq)
    , lowCutListener(lowCut)
{
    valueTreeState.addParameterListener(dryLevelID, processorAsListener);
    valueTreeState.addParameterListener(earlyLevelID, processorAsListener);
    valueTreeState.addParameterListener(earlySendID, processorAsListener);
    valueTreeState.addParameterListener(lateLevelID, processorAsListener);
    valueTreeState.addParameterListener(sizeID, processorAsListener);
    valueTreeState.addParameterListener(widthID, processorAsListener);
    valueTreeState.addParameterListener(predelayID, processorAsListener);
    valueTreeState.addParameterListener(decayID, processorAsListener);
    valueTreeState.addParameterListener(diffuseID, processorAsListener);
    valueTreeState.addParameterListener(spinID, processorAsListener);
    valueTreeState.addParameterListener(wanderID, processorAsListener);
    valueTreeState.addParameterListener(highCutID, processorAsListener);
    valueTreeState.addParameterListener(earlyDampID, processorAsListener);
    valueTreeState.addParameterListener(lateDampID, processorAsListener);
    valueTreeState.addParameterListener(lowBoostID, processorAsListener);
    valueTreeState.addParameterListener(boostFreqID, processorAsListener);
    valueTreeState.addParameterListener(lowCutID, processorAsListener);

    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(earlyLevelID, &earlyLevelListener);
    valueTreeState.addParameterListener(earlySendID, &earlySendListener);
    valueTreeState.addParameterListener(lateLevelID, &lateLevelListener);
    valueTreeState.addParameterListener(sizeID, &sizeListener);
    valueTreeState.addParameterListener(widthID, &widthListener);
    valueTreeState.addParameterListener(predelayID, &predelayListener);
    valueTreeState.addParameterListener(decayID, &decayListener);
    valueTreeState.addParameterListener(diffuseID, &diffuseListener);
    valueTreeState.addParameterListener(spinID, &spinListener);
    valueTreeState.addParameterListener(wanderID, &wanderListener);
    valueTreeState.addParameterListener(highCutID, &highCutListener);
    valueTreeState.addParameterListener(earlyDampID, &earlyDampListener);
    valueTreeState.addParameterListener(lateDampID, &lateDampListener);
    valueTreeState.addParameterListener(lowBoostID, &lowBoostListener);
    valueTreeState.addParameterListener(boostFreqID, &boostFreqListener);
    valueTreeState.addParameterListener(lowCutID, &lowCutListener);
}

DragonflyRoomParameters::~DragonflyRoomParameters()
{
    detachControls();
    
    valueTreeState.removeParameterListener(dryLevelID, processorAsListener);
    valueTreeState.removeParameterListener(earlyLevelID, processorAsListener);
    valueTreeState.removeParameterListener(earlySendID, processorAsListener);
    valueTreeState.removeParameterListener(lateLevelID, processorAsListener);
    valueTreeState.removeParameterListener(sizeID, processorAsListener);
    valueTreeState.removeParameterListener(widthID, processorAsListener);
    valueTreeState.removeParameterListener(predelayID, processorAsListener);
    valueTreeState.removeParameterListener(decayID, processorAsListener);
    valueTreeState.removeParameterListener(diffuseID, processorAsListener);
    valueTreeState.removeParameterListener(spinID, processorAsListener);
    valueTreeState.removeParameterListener(wanderID, processorAsListener);
    valueTreeState.removeParameterListener(highCutID, processorAsListener);
    valueTreeState.removeParameterListener(earlyDampID, processorAsListener);
    valueTreeState.removeParameterListener(lateDampID, processorAsListener);
    valueTreeState.removeParameterListener(lowBoostID, processorAsListener);
    valueTreeState.removeParameterListener(boostFreqID, processorAsListener);
    valueTreeState.removeParameterListener(lowCutID, processorAsListener);

    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(earlyLevelID, &earlyLevelListener);
    valueTreeState.removeParameterListener(earlySendID, &earlySendListener);
    valueTreeState.removeParameterListener(lateLevelID, &lateLevelListener);
    valueTreeState.removeParameterListener(sizeID, &sizeListener);
    valueTreeState.removeParameterListener(widthID, &widthListener);
    valueTreeState.removeParameterListener(predelayID, &predelayListener);
    valueTreeState.removeParameterListener(decayID, &decayListener);
    valueTreeState.removeParameterListener(diffuseID, &diffuseListener);
    valueTreeState.removeParameterListener(spinID, &spinListener);
    valueTreeState.removeParameterListener(wanderID, &wanderListener);
    valueTreeState.removeParameterListener(highCutID, &highCutListener);
    valueTreeState.removeParameterListener(earlyDampID, &earlyDampListener);
    valueTreeState.removeParameterListener(lateDampID, &lateDampListener);
    valueTreeState.removeParameterListener(lowBoostID, &lowBoostListener);
    valueTreeState.removeParameterListener(boostFreqID, &boostFreqListener);
    valueTreeState.removeParameterListener(lowCutID, &lowCutListener);
}

void DragonflyRoomParameters::detachControls()
{
    dryLevelAttachment.reset(nullptr);
    earlyLevelAttachment.reset(nullptr);
    earlySendAttachment.reset(nullptr);
    lateLevelAttachment.reset(nullptr);
    sizeAttachment.reset(nullptr);
    widthAttachment.reset(nullptr);
    predelayAttachment.reset(nullptr);
    decayAttachment.reset(nullptr);
    diffuseAttachment.reset(nullptr);
    spinAttachment.reset(nullptr);
    wanderAttachment.reset(nullptr);
    highCutAttachment.reset(nullptr);
    earlyDampAttachment.reset(nullptr);
    lateDampAttachment.reset(nullptr);
    lowBoostAttachment.reset(nullptr);
    boostFreqAttachment.reset(nullptr);
    lowCutAttachment.reset(nullptr);
}

void DragonflyRoomParameters::attachControls(
    Slider& dryLevelKnob,
    Slider& earlyLevelKnob,
    Slider& earlySendKnob,
    Slider& lateLevelKnob,
    Slider& sizeKnob,
    Slider& widthKnob,
    Slider& predelayKnob,
    Slider& decayKnob,
    Slider& diffuseKnob,
    Slider& spinKnob,
    Slider& wanderKnob,
    Slider& highCutKnob,
    Slider& earlyDampKnob,
    Slider& lateDampKnob,
    Slider& lowBoostKnob,
    Slider& boostFreqKnob,
    Slider& lowCutKnob )
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    earlyLevelAttachment.reset(new SlAt(valueTreeState, earlyLevelID, earlyLevelKnob));
    earlySendAttachment.reset(new SlAt(valueTreeState, earlySendID, earlySendKnob));
    lateLevelAttachment.reset(new SlAt(valueTreeState, lateLevelID, lateLevelKnob));
    sizeAttachment.reset(new SlAt(valueTreeState, sizeID, sizeKnob));
    widthAttachment.reset(new SlAt(valueTreeState, widthID, widthKnob));
    predelayAttachment.reset(new SlAt(valueTreeState, predelayID, predelayKnob));
    decayAttachment.reset(new SlAt(valueTreeState, decayID, decayKnob));
    diffuseAttachment.reset(new SlAt(valueTreeState, diffuseID, diffuseKnob));
    spinAttachment.reset(new SlAt(valueTreeState, spinID, spinKnob));
    wanderAttachment.reset(new SlAt(valueTreeState, wanderID, wanderKnob));
    highCutAttachment.reset(new SlAt(valueTreeState, highCutID, highCutKnob));
    earlyDampAttachment.reset(new SlAt(valueTreeState, earlyDampID, earlyDampKnob));
    lateDampAttachment.reset(new SlAt(valueTreeState, lateDampID, lateDampKnob));
    lowBoostAttachment.reset(new SlAt(valueTreeState, lowBoostID, lowBoostKnob));
    boostFreqAttachment.reset(new SlAt(valueTreeState, boostFreqID, boostFreqKnob));
    lowCutAttachment.reset(new SlAt(valueTreeState, lowCutID, lowCutKnob));
}
