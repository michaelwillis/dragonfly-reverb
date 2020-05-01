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

#include "DragonflyHallParameters.h"

// Dry Level
const String DragonflyHallParameters::dryLevelID = "dryLevel";
const String DragonflyHallParameters::dryLevelName = TRANS("Dry Level");
const String DragonflyHallParameters::dryLevelLabel = "%";
const float DragonflyHallParameters::dryLevelMin = 0.0f;
const float DragonflyHallParameters::dryLevelMax = 100.0f;
const float DragonflyHallParameters::dryLevelDefault = 80.0f;
const float DragonflyHallParameters::dryLevelStep = 0.0f;
// Early Level
const String DragonflyHallParameters::earlyLevelID = "earlyLevel";
const String DragonflyHallParameters::earlyLevelName = TRANS("Early Level");
const String DragonflyHallParameters::earlyLevelLabel = "%";
const float DragonflyHallParameters::earlyLevelMin = 0.0f;
const float DragonflyHallParameters::earlyLevelMax = 100.0f;
const float DragonflyHallParameters::earlyLevelDefault = 10.0f;
const float DragonflyHallParameters::earlyLevelStep = 0.0f;
// Late Level
const String DragonflyHallParameters::lateLevelID = "lateLevel";
const String DragonflyHallParameters::lateLevelName = TRANS("Late Level");
const String DragonflyHallParameters::lateLevelLabel = "%";
const float DragonflyHallParameters::lateLevelMin = 0.0f;
const float DragonflyHallParameters::lateLevelMax = 100.0f;
const float DragonflyHallParameters::lateLevelDefault = 20.0f;
const float DragonflyHallParameters::lateLevelStep = 0.0f;
// Size
const String DragonflyHallParameters::sizeID = "size";
const String DragonflyHallParameters::sizeName = TRANS("Size");
const String DragonflyHallParameters::sizeLabel = "m";
const float DragonflyHallParameters::sizeMin = 10.0f;
const float DragonflyHallParameters::sizeMax = 60.0f;
const float DragonflyHallParameters::sizeDefault = 24.0f;
const float DragonflyHallParameters::sizeStep = 0.0f;
// Width
const String DragonflyHallParameters::widthID = "width";
const String DragonflyHallParameters::widthName = TRANS("Width");
const String DragonflyHallParameters::widthLabel = "%";
const float DragonflyHallParameters::widthMin = 50.0f;
const float DragonflyHallParameters::widthMax = 150.0f;
const float DragonflyHallParameters::widthDefault = 100.0f;
const float DragonflyHallParameters::widthStep = 0.0f;
// Predelay
const String DragonflyHallParameters::predelayID = "predelay";
const String DragonflyHallParameters::predelayName = TRANS("Predelay");
const String DragonflyHallParameters::predelayLabel = "ms";
const float DragonflyHallParameters::predelayMin = 0.0f;
const float DragonflyHallParameters::predelayMax = 100.0f;
const float DragonflyHallParameters::predelayDefault = 4.0f;
const float DragonflyHallParameters::predelayStep = 0.0f;
// Diffuse
const String DragonflyHallParameters::diffuseID = "diffuse";
const String DragonflyHallParameters::diffuseName = TRANS("Diffuse");
const String DragonflyHallParameters::diffuseLabel = "%";
const float DragonflyHallParameters::diffuseMin = 0.0f;
const float DragonflyHallParameters::diffuseMax = 100.0f;
const float DragonflyHallParameters::diffuseDefault = 90.0f;
const float DragonflyHallParameters::diffuseStep = 0.0f;
// Low Cut
const String DragonflyHallParameters::lowCutID = "lowCut";
const String DragonflyHallParameters::lowCutName = TRANS("Low Cut");
const String DragonflyHallParameters::lowCutLabel = "Hz";
const float DragonflyHallParameters::lowCutMin = 0.0f;
const float DragonflyHallParameters::lowCutMax = 200.0f;
const float DragonflyHallParameters::lowCutDefault = 4.0f;
const float DragonflyHallParameters::lowCutStep = 0.0f;
// Low Cross
const String DragonflyHallParameters::lowCrossID = "lowCross";
const String DragonflyHallParameters::lowCrossName = TRANS("Low Cross");
const String DragonflyHallParameters::lowCrossLabel = "Hz";
const float DragonflyHallParameters::lowCrossMin = 200.0f;
const float DragonflyHallParameters::lowCrossMax = 1200.0f;
const float DragonflyHallParameters::lowCrossDefault = 500.0f;
const float DragonflyHallParameters::lowCrossStep = 0.0f;
// Low Mult
const String DragonflyHallParameters::lowMultID = "lowMult";
const String DragonflyHallParameters::lowMultName = TRANS("Low Mult");
const String DragonflyHallParameters::lowMultLabel = "";
const float DragonflyHallParameters::lowMultMin = 0.5f;
const float DragonflyHallParameters::lowMultMax = 2.5f;
const float DragonflyHallParameters::lowMultDefault = 1.3f;
const float DragonflyHallParameters::lowMultStep = 0.0f;
// High Cut
const String DragonflyHallParameters::highCutID = "highCut";
const String DragonflyHallParameters::highCutName = TRANS("High Cut");
const String DragonflyHallParameters::highCutLabel = "Hz";
const float DragonflyHallParameters::highCutMin = 1000.0f;
const float DragonflyHallParameters::highCutMax = 16000.0f;
const float DragonflyHallParameters::highCutDefault = 7600.0f;
const float DragonflyHallParameters::highCutStep = 0.0f;
// High Cross
const String DragonflyHallParameters::highCrossID = "highCross";
const String DragonflyHallParameters::highCrossName = TRANS("High Cross");
const String DragonflyHallParameters::highCrossLabel = "Hz";
const float DragonflyHallParameters::highCrossMin = 1000.0f;
const float DragonflyHallParameters::highCrossMax = 16000.0f;
const float DragonflyHallParameters::highCrossDefault = 5500.0f;
const float DragonflyHallParameters::highCrossStep = 0.0f;
// High Mult
const String DragonflyHallParameters::highMultID = "highMult";
const String DragonflyHallParameters::highMultName = TRANS("High Mult");
const String DragonflyHallParameters::highMultLabel = "";
const float DragonflyHallParameters::highMultMin = 0.2f;
const float DragonflyHallParameters::highMultMax = 1.2f;
const float DragonflyHallParameters::highMultDefault = 0.5f;
const float DragonflyHallParameters::highMultStep = 0.0f;
// Spin
const String DragonflyHallParameters::spinID = "spin";
const String DragonflyHallParameters::spinName = TRANS("Spin");
const String DragonflyHallParameters::spinLabel = "Hz";
const float DragonflyHallParameters::spinMin = 0.0f;
const float DragonflyHallParameters::spinMax = 10.0f;
const float DragonflyHallParameters::spinDefault = 3.3f;
const float DragonflyHallParameters::spinStep = 0.0f;
// Wander
const String DragonflyHallParameters::wanderID = "wander";
const String DragonflyHallParameters::wanderName = TRANS("Wander");
const String DragonflyHallParameters::wanderLabel = "ms";
const float DragonflyHallParameters::wanderMin = 0.0f;
const float DragonflyHallParameters::wanderMax = 40.0f;
const float DragonflyHallParameters::wanderDefault = 15.0f;
const float DragonflyHallParameters::wanderStep = 0.0f;
// Decay
const String DragonflyHallParameters::decayID = "decay";
const String DragonflyHallParameters::decayName = TRANS("Decay");
const String DragonflyHallParameters::decayLabel = "sec";
const float DragonflyHallParameters::decayMin = 0.1f;
const float DragonflyHallParameters::decayMax = 10.0f;
const float DragonflyHallParameters::decayDefault = 1.3f;
const float DragonflyHallParameters::decayStep = 0.0f;
// Early Send
const String DragonflyHallParameters::earlySendID = "earlySend";
const String DragonflyHallParameters::earlySendName = TRANS("Early Send");
const String DragonflyHallParameters::earlySendLabel = "%";
const float DragonflyHallParameters::earlySendMin = 0.0f;
const float DragonflyHallParameters::earlySendMax = 100.0f;
const float DragonflyHallParameters::earlySendDefault = 20.0f;
const float DragonflyHallParameters::earlySendStep = 0.0f;
// Modulation
const String DragonflyHallParameters::modulationID = "modulation";
const String DragonflyHallParameters::modulationName = TRANS("Modulation");
const String DragonflyHallParameters::modulationLabel = "%";
const float DragonflyHallParameters::modulationMin = 0.0f;
const float DragonflyHallParameters::modulationMax = 100.0f;
const float DragonflyHallParameters::modulationDefault = 15.0f;
const float DragonflyHallParameters::modulationStep = 0.0f;

AudioProcessorValueTreeState::ParameterLayout DragonflyHallParameters::createParameterLayout()
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
        diffuseID, diffuseName,
        NormalisableRange<float>(diffuseMin, diffuseMax, diffuseStep), diffuseDefault,
        diffuseLabel,
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
        lowCrossID, lowCrossName,
        NormalisableRange<float>(lowCrossMin, lowCrossMax, lowCrossStep), lowCrossDefault,
        lowCrossLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        lowMultID, lowMultName,
        NormalisableRange<float>(lowMultMin, lowMultMax, lowMultStep), lowMultDefault,
        lowMultLabel,
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
        highCrossID, highCrossName,
        NormalisableRange<float>(highCrossMin, highCrossMax, highCrossStep), highCrossDefault,
        highCrossLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));
    params.push_back(std::make_unique<AudioParameterFloat>(
        highMultID, highMultName,
        NormalisableRange<float>(highMultMin, highMultMax, highMultStep), highMultDefault,
        highMultLabel,
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
        decayID, decayName,
        NormalisableRange<float>(decayMin, decayMax, decayStep), decayDefault,
        decayLabel,
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
        modulationID, modulationName,
        NormalisableRange<float>(modulationMin, modulationMax, modulationStep), modulationDefault,
        modulationLabel,
        AudioProcessorParameter::genericParameter,
        [](float value, int maxLength) { return String(value).substring(0, maxLength); },
        [](const String& text) { return text.getFloatValue(); }));

    return { params.begin(), params.end() };
}

DragonflyHallParameters::DragonflyHallParameters(AudioProcessorValueTreeState& vts,
    AudioProcessorValueTreeState::Listener* processor)
    : dryLevel(0.01f * dryLevelDefault)
    , earlyLevel(0.01f * earlyLevelDefault)
    , lateLevel(0.01f * lateLevelDefault)
    , size(sizeDefault)
    , width(0.01f * widthDefault)
    , predelay(predelayDefault)
    , diffuse(0.01f * diffuseDefault)
    , lowCut(lowCutDefault)
    , lowCross(lowCrossDefault)
    , lowMult(lowMultDefault)
    , highCut(highCutDefault)
    , highCross(highCrossDefault)
    , highMult(highMultDefault)
    , spin(spinDefault)
    , wander(wanderDefault)
    , decay(decayDefault)
    , earlySend(0.01f * earlySendDefault)
    , modulation(0.01f * modulationDefault)
    , valueTreeState(vts)
    , processorAsListener(processor)
    , dryLevelListener(dryLevel, 0.01f)
    , earlyLevelListener(earlyLevel, 0.01f)
    , lateLevelListener(lateLevel, 0.01f)
    , sizeListener(size)
    , widthListener(width, 0.01f)
    , predelayListener(predelay)
    , diffuseListener(diffuse)
    , lowCutListener(lowCut)
    , lowCrossListener(lowCross)
    , lowMultListener(lowMult)
    , highCutListener(highCut)
    , highCrossListener(highCross)
    , highMultListener(highMult)
    , spinListener(spin)
    , wanderListener(wander)
    , decayListener(decay)
    , earlySendListener(earlySend, 0.01f)
    , modulationListener(modulation, 0.01f)
{
    valueTreeState.addParameterListener(dryLevelID, processorAsListener);
    valueTreeState.addParameterListener(earlyLevelID, processorAsListener);
    valueTreeState.addParameterListener(lateLevelID, processorAsListener);
    valueTreeState.addParameterListener(sizeID, processorAsListener);
    valueTreeState.addParameterListener(widthID, processorAsListener);
    valueTreeState.addParameterListener(predelayID, processorAsListener);
    valueTreeState.addParameterListener(diffuseID, processorAsListener);
    valueTreeState.addParameterListener(lowCutID, processorAsListener);
    valueTreeState.addParameterListener(lowCrossID, processorAsListener);
    valueTreeState.addParameterListener(lowMultID, processorAsListener);
    valueTreeState.addParameterListener(highCutID, processorAsListener);
    valueTreeState.addParameterListener(highCrossID, processorAsListener);
    valueTreeState.addParameterListener(highMultID, processorAsListener);
    valueTreeState.addParameterListener(spinID, processorAsListener);
    valueTreeState.addParameterListener(wanderID, processorAsListener);
    valueTreeState.addParameterListener(decayID, processorAsListener);
    valueTreeState.addParameterListener(earlySendID, processorAsListener);
    valueTreeState.addParameterListener(modulationID, processorAsListener);

    valueTreeState.addParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.addParameterListener(earlyLevelID, &earlyLevelListener);
    valueTreeState.addParameterListener(lateLevelID, &lateLevelListener);
    valueTreeState.addParameterListener(sizeID, &sizeListener);
    valueTreeState.addParameterListener(widthID, &widthListener);
    valueTreeState.addParameterListener(predelayID, &predelayListener);
    valueTreeState.addParameterListener(diffuseID, &diffuseListener);
    valueTreeState.addParameterListener(lowCutID, &lowCutListener);
    valueTreeState.addParameterListener(lowCrossID, &lowCrossListener);
    valueTreeState.addParameterListener(lowMultID, &lowMultListener);
    valueTreeState.addParameterListener(highCutID, &highCutListener);
    valueTreeState.addParameterListener(highCrossID, &highCrossListener);
    valueTreeState.addParameterListener(highMultID, &highMultListener);
    valueTreeState.addParameterListener(spinID, &spinListener);
    valueTreeState.addParameterListener(wanderID, &wanderListener);
    valueTreeState.addParameterListener(decayID, &decayListener);
    valueTreeState.addParameterListener(earlySendID, &earlySendListener);
    valueTreeState.addParameterListener(modulationID, &modulationListener);
}

DragonflyHallParameters::~DragonflyHallParameters()
{
    detachControls();

    valueTreeState.removeParameterListener(dryLevelID, processorAsListener);
    valueTreeState.removeParameterListener(earlyLevelID, processorAsListener);
    valueTreeState.removeParameterListener(lateLevelID, processorAsListener);
    valueTreeState.removeParameterListener(sizeID, processorAsListener);
    valueTreeState.removeParameterListener(widthID, processorAsListener);
    valueTreeState.removeParameterListener(predelayID, processorAsListener);
    valueTreeState.removeParameterListener(diffuseID, processorAsListener);
    valueTreeState.removeParameterListener(lowCutID, processorAsListener);
    valueTreeState.removeParameterListener(lowCrossID, processorAsListener);
    valueTreeState.removeParameterListener(lowMultID, processorAsListener);
    valueTreeState.removeParameterListener(highCutID, processorAsListener);
    valueTreeState.removeParameterListener(highCrossID, processorAsListener);
    valueTreeState.removeParameterListener(highMultID, processorAsListener);
    valueTreeState.removeParameterListener(spinID, processorAsListener);
    valueTreeState.removeParameterListener(wanderID, processorAsListener);
    valueTreeState.removeParameterListener(decayID, processorAsListener);
    valueTreeState.removeParameterListener(earlySendID, processorAsListener);
    valueTreeState.removeParameterListener(modulationID, processorAsListener);

    valueTreeState.removeParameterListener(dryLevelID, &dryLevelListener);
    valueTreeState.removeParameterListener(earlyLevelID, &earlyLevelListener);
    valueTreeState.removeParameterListener(lateLevelID, &lateLevelListener);
    valueTreeState.removeParameterListener(sizeID, &sizeListener);
    valueTreeState.removeParameterListener(widthID, &widthListener);
    valueTreeState.removeParameterListener(predelayID, &predelayListener);
    valueTreeState.removeParameterListener(diffuseID, &diffuseListener);
    valueTreeState.removeParameterListener(lowCutID, &lowCutListener);
    valueTreeState.removeParameterListener(lowCrossID, &lowCrossListener);
    valueTreeState.removeParameterListener(lowMultID, &lowMultListener);
    valueTreeState.removeParameterListener(highCutID, &highCutListener);
    valueTreeState.removeParameterListener(highCrossID, &highCrossListener);
    valueTreeState.removeParameterListener(highMultID, &highMultListener);
    valueTreeState.removeParameterListener(spinID, &spinListener);
    valueTreeState.removeParameterListener(wanderID, &wanderListener);
    valueTreeState.removeParameterListener(decayID, &decayListener);
    valueTreeState.removeParameterListener(earlySendID, &earlySendListener);
    valueTreeState.removeParameterListener(modulationID, &modulationListener);
}

void DragonflyHallParameters::detachControls()
{
    dryLevelAttachment.reset(nullptr);
    earlyLevelAttachment.reset(nullptr);
    lateLevelAttachment.reset(nullptr);
    sizeAttachment.reset(nullptr);
    widthAttachment.reset(nullptr);
    predelayAttachment.reset(nullptr);
    diffuseAttachment.reset(nullptr);
    lowCutAttachment.reset(nullptr);
    lowCrossAttachment.reset(nullptr);
    lowMultAttachment.reset(nullptr);
    highCutAttachment.reset(nullptr);
    highCrossAttachment.reset(nullptr);
    highMultAttachment.reset(nullptr);
    spinAttachment.reset(nullptr);
    wanderAttachment.reset(nullptr);
    decayAttachment.reset(nullptr);
    earlySendAttachment.reset(nullptr);
    modulationAttachment.reset(nullptr);
}

void DragonflyHallParameters::attachControls(
    Slider& dryLevelKnob,
    Slider& earlyLevelKnob,
    Slider& lateLevelKnob,
    Slider& sizeKnob,
    Slider& widthKnob,
    Slider& predelayKnob,
    Slider& diffuseKnob,
    Slider& lowCutKnob,
    Slider& lowCrossKnob,
    Slider& lowMultKnob,
    Slider& highCutKnob,
    Slider& highCrossKnob,
    Slider& highMultKnob,
    Slider& spinKnob,
    Slider& wanderKnob,
    Slider& decayKnob,
    Slider& earlySendKnob,
    Slider& modulationKnob)
{
    using CbAt = AudioProcessorValueTreeState::ComboBoxAttachment;
    using SlAt = AudioProcessorValueTreeState::SliderAttachment;
    dryLevelAttachment.reset(new SlAt(valueTreeState, dryLevelID, dryLevelKnob));
    earlyLevelAttachment.reset(new SlAt(valueTreeState, earlyLevelID, earlyLevelKnob));
    lateLevelAttachment.reset(new SlAt(valueTreeState, lateLevelID, lateLevelKnob));
    sizeAttachment.reset(new SlAt(valueTreeState, sizeID, sizeKnob));
    widthAttachment.reset(new SlAt(valueTreeState, widthID, widthKnob));
    predelayAttachment.reset(new SlAt(valueTreeState, predelayID, predelayKnob));
    diffuseAttachment.reset(new SlAt(valueTreeState, diffuseID, diffuseKnob));
    lowCutAttachment.reset(new SlAt(valueTreeState, lowCutID, lowCutKnob));
    lowCrossAttachment.reset(new SlAt(valueTreeState, lowCrossID, lowCrossKnob));
    lowMultAttachment.reset(new SlAt(valueTreeState, lowMultID, lowMultKnob));
    highCutAttachment.reset(new SlAt(valueTreeState, highCutID, highCutKnob));
    highCrossAttachment.reset(new SlAt(valueTreeState, highCrossID, highCrossKnob));
    highMultAttachment.reset(new SlAt(valueTreeState, highMultID, highMultKnob));
    spinAttachment.reset(new SlAt(valueTreeState, spinID, spinKnob));
    wanderAttachment.reset(new SlAt(valueTreeState, wanderID, wanderKnob));
    decayAttachment.reset(new SlAt(valueTreeState, decayID, decayKnob));
    earlySendAttachment.reset(new SlAt(valueTreeState, earlySendID, earlySendKnob));
    modulationAttachment.reset(new SlAt(valueTreeState, modulationID, modulationKnob));
}
