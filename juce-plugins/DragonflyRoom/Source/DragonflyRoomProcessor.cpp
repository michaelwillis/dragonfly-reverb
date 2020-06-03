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

#include "DragonflyRoomProcessor.h"
#include "DragonflyRoomEditor.h"

// Increase the late level by approx 8dB
#define LATE_GAIN 2.5f

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DragonflyRoomProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DragonflyRoomProcessor::createEditor()
{
    return new DragonflyRoomEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
DragonflyRoomProcessor::DragonflyRoomProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("DragonflyRoom"), DragonflyRoomParameters::createParameterLayout())
    , parameters(valueTreeState, this)
    , sampleRateHz(44100.0f)
{
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();

    early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    early.setMuteOnChange(false);
    early.setdryr(0.0f);   // mute dry signal
    early.setwet(0.0f);    // 0dB
    early.setwidth(0.8f);
    early.setLRDelay(0.3f);
    early.setLRCrossApFreq(750.0f, 4.0f);
    early.setDiffusionApFreq(150.0f, 4.0f);
    early.setSampleRate(44100.0f);

    late.setMuteOnChange(false);
    late.setwet(0); // 0dB
    late.setdryr(0); // mute dry signal
    late.setwidth(1.0);
    late.setSampleRate(sampleRateHz);

    early.setRSFactor(0.1f * parameters.size);
    late.setRSFactor(0.1f * parameters.size);
    late.setbassboost(5.0f * parameters.lowBoost / powf(parameters.decay, 1.5f) * (0.1f * parameters.size));
    early.setwidth(parameters.width / 1.2f);
    late.setwidth(parameters.width);
    late.setPreDelay(parameters.predelay);
    late.setrt60(parameters.decay);
    late.setidiffusion1(parameters.diffuse / 1.2f);
    late.setodiffusion1(parameters.diffuse / 1.2f);
    late.setspin(parameters.spin);
    late.setspin2(std::sqrtf(100.0f - (10.0f - parameters.spin) * (10.0f - parameters.spin)) / 2.0f);
    late.setwander(parameters.wander / 2.0f + 0.1f);
    late.setwander2(parameters.wander / 2.0f + 0.1f);
    early.setoutputlpf(parameters.earlyDamp);
    late.setdamp(parameters.lateDamp);
    late.setoutputdamp(parameters.lateDamp);
    late.setdamp2(parameters.boostFreq);

    setCurrentProgram(0);
}

// built-in presets aka "programs"
struct Preset
{
    const char* name;
    struct { 
        float dryLevel;
        float earlyLevel;
        float earlySend;
        float lateLevel;
        float size;
        float width;
        float predelay;
        float decay;
        float diffuse;
        float spin;
        float wander;
        float highCut;
        float earlyDamp;
        float lateDamp;
        float lowBoost;
        float boostFreq;
        float lowCut;
    } params;
};

typedef struct {
    const char* name;
    const Preset presets[PRESETS_PER_BANK];
} Bank;

static Bank banks[NUM_BANKS] = {
   {
     "Unify Custom", {       // dry, early, e. send, late, size,  width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Natural Wood",       { 85.0,  10.0,    20.0, 20.0, 15.0,  100.0,  15.0,  0.15,    80.0,  0.0,   20.0,    16000,       5200,    4000.0,       70.0,       850,  167.0 }},
      {"Perc Room",          { 85.0,  15.0,    20.0, 13.0, 13.0,   90.0,  25.0,   0.3,    70.0,  0.4,   40.0,    16000,      16000,   10434.5,       20.0,       600,  110.0 }},
      {"Glass Walls",        { 85.0,  10.0,    20.0, 20.0, 24.0,   90.0,  25.0, 0.327,    84.0,  0.4,   40.0,    15000,      14000,   12000.0,       60.0,       600,   60.0 }},
      {"SuperLive",          { 80.0,  25.0,    60.0,  9.0,  8.3,   90.0,  25.0,  1.28,    25.0,  2.5,   80.0,    16000,      12000,    9000.0,       20.0,       500,   60.0 }},
      {"Long Room",          { 85.0,   5.0,    20.0, 12.0, 11.94, 130.0, 100.0,   4.3,   100.0,  2.0,   40.0,     9755,      16000,   10000.0,       90.0,       479,   60.0 }}
     }
   },
   {
     "Small", {              // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Small Bright Room",  { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.2,    60.0,  0.4,   40.0,    16000,      16000,     16000,       20.0,       600,    4.0 }},
      {"Small Clear Room",   { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.2,    60.0,  0.8,   40.0,    16000,      11200,     10000,       40.0,       600,    4.0 }},
      {"Small Dark Room",    { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.3,    70.0,  1.6,   20.0,    16000,       6400,      5500,       60.0,      1000,    4.0 }},
      {"Small Drum Room",    { 80.0,  10.0,    20.0, 20.0,  9.0,  90.0,   8.0,   0.2,    24.0,  2.1,   10.0,    16000,       8200,      7000,       40.0,       400,    4.0 }},
      {"Small Vocal Room",   { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   0.0,   0.3,    86.0,  2.4,   12.0,    16000,       7600,      6400,       20.0,       400,    4.0 }}
     }
  },
  {
    "Medium", {              // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Medium Bright Room", { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.4,    60.0,  0.4,   40.0,    16000,      16000,     14000,       25.0,       600,    4.0 }},
      {"Medium Clear Room",  { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.4,    70.0,  0.8,   40.0,    16000,      10000,      9400,       50.0,       600,    4.0 }},
      {"Medium Dark Room",   { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.5,    70.0,  1.6,   20.0,    16000,       5800,      4600,       70.0,      1000,    4.0 }},
      {"Medium Drum Room",   { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,  12.0,   0.4,    32.0,  2.4,   10.0,    16000,       8000,      6000,       50.0,       300,    4.0 }},
      {"Medium Vocal Room",  { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   2.0,   0.6,    92.0,  2.7,   12.0,    16000,       8000,      6000,       25.0,       400,    4.0 }}
    }
  },
  {
    "Large", {               // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Large Bright Room",  { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.7,    70.0,  0.4,   40.0,    16000,      16000,     14000,       30.0,       600,    4.0 }},
      {"Large Clear Room",   { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.7,    80.0,  0.4,   40.0,    16000,       9400,      8500,       60.0,       600,    4.0 }},
      {"Large Dark Room",    { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.8,    80.0,  1.6,   20.0,    16000,       5200,      4000,       80.0,      1000,    4.0 }},
      {"Large Drum Room",    { 80.0,  10.0,    20.0, 20.0, 14.0, 100.0,  16.0,   0.7,    40.0,  2.7,   10.0,    16000,       8000,      5000,       60.0,       300,    4.0 }},
      {"Large Vocal Room",   { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,   4.0,   1.0,    98.0,  3.1,   12.0,    16000,       7000,      5000,       30.0,       400,    4.0 }}
    }
  },
  {
    "Halls", {               // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Bright Hall",        { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.6,    78.0,  0.4,   40.0,    16000,      16000,     14000,       30.0,       600,    4.0 }},
      {"Clear Hall",         { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.6,    84.0,  0.4,   40.0,    16000,      14000,     12000,       60.0,       600,    4.0 }},
      {"Dark Hall",          { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.9,    80.0,  1.6,   20.0,    16000,       8000,      6000,       80.0,      1000,    4.0 }},
      {"Percussion Hall",    { 80.0,  10.0,    20.0, 20.0, 20.0, 100.0,  16.0,   1.5,    40.0,  2.7,   10.0,    16000,       7000,      5000,       60.0,       300,    4.0 }},
      {"Vocal Hall",         { 80.0,  10.0,    20.0, 20.0, 25.0, 100.0,   4.0,   2.0,    98.0,  3.1,   12.0,    16000,       6000,      5000,       30.0,       400,    4.0 }}
    }
  },
  {
    "Effects", {             // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Bright Plate",       { 80.0,  10.0,    20.0, 20.0, 10.0, 100.0,   0.0,   1.8,    78.0,  1.6,   40.0,    16000,      16000,     15000,       10.0,      1000,    4.0}},
      {"Dark Plate",         { 80.0,  10.0,    20.0, 20.0, 10.0, 100.0,   0.0,   2.0,    78.0,  0.4,   20.0,    16000,       5200,      4000,       40.0,       500,    4.0}},
      {"Brick Wall",         { 80.0,  10.0,    20.0, 20.0, 20.0,  80.0,  80.0,   0.4,    12.0,  0.2,    5.0,    16000,      16000,     15000,       10.0,       200,    4.0}},
      {"Echo Chamber",       { 80.0,  10.0,    20.0, 20.0, 12.0, 120.0,  10.0,   4.0,    24.0,  2.3,   80.0,    16000,      12000,      9000,       20.0,       500,    4.0}},
      {"Long Tunnel",        { 80.0,  10.0,    20.0, 20.0, 25.0,  50.0,   0.0,   8.0,    90.0,  0.4,   10.0,    16000,       8000,      6000,       50.0,       500,    4.0}},
    }
  }
};

const String DragonflyRoomProcessor::getProgramName(int progIndex)
{
    int bankIndex = progIndex / 5;
    progIndex = progIndex % 5;
    return String(banks[bankIndex].presets[progIndex].name);
}

void DragonflyRoomProcessor::populateProgramsComboBox(ComboBox& cb)
{
    int itemID = 0;
    for (int bankIndex = 0; bankIndex < NUM_BANKS; bankIndex++)
    {
        cb.addSectionHeading(String(banks[bankIndex].name));
        for (int progIndex = 0; progIndex < PRESETS_PER_BANK; progIndex++)
            cb.addItem(String(banks[bankIndex].presets[progIndex].name), ++itemID);
    }
}

void DragonflyRoomProcessor::setCurrentProgram(int progIndex)
{
    currentProgramIndex = progIndex = jlimit<int>(0, NUM_BANKS * PRESETS_PER_BANK - 1, progIndex);
    int bankIndex = progIndex / PRESETS_PER_BANK;
    progIndex = progIndex % PRESETS_PER_BANK;
    auto& preset = banks[bankIndex].presets[progIndex].params;

    // setValueNotifyingHost() requires NORMALIZED parameter values
    RangedAudioParameter* param;
#define SET_NORMALIZED(name, id) \
    param = valueTreeState.getParameter(DragonflyRoomParameters:: id); \
    param->setValueNotifyingHost(param->convertTo0to1(preset. name))

    SET_NORMALIZED(dryLevel, dryLevelID);
    SET_NORMALIZED(earlyLevel, earlyLevelID);
    SET_NORMALIZED(earlySend, earlySendID);
    SET_NORMALIZED(size, sizeID);
    SET_NORMALIZED(predelay, predelayID);
    SET_NORMALIZED(decay, decayID);
    SET_NORMALIZED(diffuse, diffuseID);
    SET_NORMALIZED(spin, spinID);
    SET_NORMALIZED(wander, wanderID);
    SET_NORMALIZED(highCut, highCutID);
    SET_NORMALIZED(earlyDamp, earlyDampID);
    SET_NORMALIZED(lateDamp, lateDampID);
    SET_NORMALIZED(lowBoost, lowBoostID);
    SET_NORMALIZED(boostFreq, boostFreqID);
    SET_NORMALIZED(lowCut, lowCutID);

    sendChangeMessage();
}

// Respond to parameter changes
void DragonflyRoomProcessor::parameterChanged(const String& paramID, float value)
{
    if (paramID == DragonflyRoomParameters::sizeID)
    {
        const ScopedLock processBlockLock(getCallbackLock());   // lock out processBlock calls while making changes
        early.setRSFactor(0.1f * value);
        late.setRSFactor(0.1f * value);
        late.setbassboost(parameters.lowBoost / 20.0f / powf(parameters.decay, 1.5f) * (0.1f * value));
    }
    else if (paramID == DragonflyRoomParameters::widthID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        early.setwidth(value / 120.0f);
        late.setwidth(value / 100.0f);
    }
    else if (paramID == DragonflyRoomParameters::predelayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setPreDelay(value);
    }
    else if (paramID == DragonflyRoomParameters::decayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setrt60(value);
        late.setbassboost(parameters.lowBoost / 20.0f / powf(value, 1.5f) * (0.1f * parameters.size));
    }
    else if (paramID == DragonflyRoomParameters::diffuseID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setidiffusion1(value / 120.0f);
        late.setodiffusion1(value / 120.0f);
    }
    else if (paramID == DragonflyRoomParameters::spinID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setspin(value);
        late.setspin2(std::sqrtf(100.0f - (10.0f - value) * (10.0f - value)) / 2.0f);
    }
    else if (paramID == DragonflyRoomParameters::wanderID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setwander(value / 200.0f + 0.1f);
        late.setwander2(value / 200.0f + 0.1f);
    }
    else if (paramID == DragonflyRoomParameters::highCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        setInputLPF(value);
    }
    else if (paramID == DragonflyRoomParameters::earlyDampID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        early.setoutputlpf(value);
    }
    else if (paramID == DragonflyRoomParameters::lateDampID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setdamp(value);
        late.setoutputdamp(value);
    }
    else if (paramID == DragonflyRoomParameters::lowBoostID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setbassboost(value / 20.0f / powf(parameters.decay, 1.5f) * (0.1f * parameters.size));
    }
    else if (paramID == DragonflyRoomParameters::boostFreqID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setdamp2(value);
    }
    else if (paramID == DragonflyRoomParameters::lowCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        setInputHPF(value);
    }
}

// Destructor
DragonflyRoomProcessor::~DragonflyRoomProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyRoomProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    sampleRateHz = float(sampleRate);
    early.setSampleRate(sampleRateHz);
    late.setSampleRate(sampleRateHz);
    setInputLPF(parameters.highCut);
    setInputHPF(parameters.lowCut);
}

// Audio processing finished; release any allocated memory
void DragonflyRoomProcessor::releaseResources()
{
}

// Silence everything
void DragonflyRoomProcessor::reset()
{
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();
    early.mute();
    late.mute();
}

// Process one buffer ("block") of data
void DragonflyRoomProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    dryBuffer.makeCopyOf(buffer, true);
    filteredInputBuffer.setSize(2, numSamples, true, false, true);
    earlyOutputBuffer.setSize(2, numSamples, true, false, true);

    // Compute LP filtered copy of input to filteredInputBuffer
    float* pInLeft = (float*)buffer.getReadPointer(0);
    float* pInRight = (float*)buffer.getReadPointer(1);
    float* pFiLeft = filteredInputBuffer.getWritePointer(0);
    float* pFiRight = filteredInputBuffer.getWritePointer(1);
    for (int i = 0; i < numSamples; i++)
    {
        pFiLeft[i] = input_lpf_0.process(input_hpf_0.process(pInLeft[i]));
        pFiRight[i] = input_lpf_1.process(input_hpf_1.process(pInRight[i]));
    }

    // filteredInputBuffer -> early -> earlyOutputBuffer
    float* pEarlyLeft = earlyOutputBuffer.getWritePointer(0);
    float* pEarlyLRight = earlyOutputBuffer.getWritePointer(1);
    early.processreplace(pFiLeft, pFiRight, pEarlyLeft, pEarlyLRight, numSamples);

    // re-use filteredInputBuffer as input for late
    filteredInputBuffer.addFrom(0, 0, earlyOutputBuffer, 0, 0, numSamples, parameters.earlySend);
    filteredInputBuffer.addFrom(1, 0, earlyOutputBuffer, 1, 0, numSamples, parameters.earlySend);

    // filteredInputBuffer -> late -> buffer
    float* pOutLeft = buffer.getWritePointer(0);
    float* pOutRight = buffer.getWritePointer(1);
    late.processreplace(pFiLeft, pFiRight, pOutLeft, pOutRight, numSamples);

    // compute final output mix
    buffer.applyGain(LATE_GAIN * parameters.lateLevel);
    buffer.addFrom(0, 0, dryBuffer, 0, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(1, 0, dryBuffer, 1, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(0, 0, earlyOutputBuffer, 0, 0, numSamples, parameters.earlyLevel);
    buffer.addFrom(1, 0, earlyOutputBuffer, 1, 0, numSamples, parameters.earlyLevel);
}

void DragonflyRoomProcessor::setInputLPF(float freq)
{
    freq = jlimit<float>(0.0f, 0.5f * sampleRateHz, freq);
    input_lpf_0.setLPF_BW(freq, sampleRateHz);
    input_lpf_1.setLPF_BW(freq, sampleRateHz);
}

void DragonflyRoomProcessor::setInputHPF(float freq)
{
    freq = jlimit<float>(0.0f, 0.5f * sampleRateHz, freq);
    input_hpf_0.setHPF_BW(freq, sampleRateHz);
    input_hpf_1.setHPF_BW(freq, sampleRateHz);
}

// Called by the host when it needs to persist the current plugin state
void DragonflyRoomProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DragonflyRoomProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
