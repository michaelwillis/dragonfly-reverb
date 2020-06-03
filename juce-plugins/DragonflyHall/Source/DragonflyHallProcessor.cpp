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

#include "DragonflyHallProcessor.h"
#include "DragonflyHallEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DragonflyHallProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DragonflyHallProcessor::createEditor()
{
    return new DragonflyHallEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
DragonflyHallProcessor::DragonflyHallProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("DragonflyHall"), DragonflyHallParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
    early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    early.setMuteOnChange(false);
    early.setdryr(0.0f);    // mute dry signal
    early.setwet(0.0f);     // 0dB
    early.setwidth(0.8f);
    early.setLRDelay(0.3f);
    early.setLRCrossApFreq(750.0f, 4.0f);
    early.setDiffusionApFreq(150.0f, 4.0f);

    late.setMuteOnChange(false);
    late.setwet(0.0f);      // 0dB
    late.setdryr(0.0f);     // mute dry signal
    late.setwidth(1.0f);

    early.setRSFactor(parameters.size / 10.0f);
    late.setRSFactor(parameters.size / 80.0f);
    early.setwidth(0.01f * parameters.width);
    late.setwidth(0.01f * parameters.width);
    late.setPreDelay(parameters.predelay);
    late.setidiffusion1(parameters.diffuse / 140.0f);
    late.setapfeedback(parameters.diffuse / 140.0f);
    early.setoutputhpf(parameters.lowCut);
    late.setoutputhpf(parameters.lowCut);
    late.setxover_low(parameters.lowCross);
    late.setrt60_factor_low(parameters.lowMult);
    early.setoutputlpf(parameters.highCut);
    late.setoutputlpf(parameters.highCut);
    late.setxover_high(parameters.highCross);
    late.setrt60_factor_high(parameters.highMult);
    late.setspin(parameters.spin);
    late.setwander(parameters.wander);
    late.setrt60(parameters.decay);
    late.setspinfactor(parameters.modulation / 100.0f);
    late.setlfofactor(parameters.modulation / 100.0f);

    setCurrentProgram(0);
}

// built-in presets aka "programs"
struct Preset
{
    const char* name;
    struct {
        float dryLevel;
        float earlyLevel;
        float lateLevel;
        float size;
        float width;
        float predelay;
        float diffuse;
        float lowCut;
        float lowCross;
        float lowMult;
        float highCut;
        float highCross;
        float highMult;
        float spin;
        float wander;
        float decay;
        float earlySend;
        float modulation;
    } params;
};

typedef struct {
    const char* name;
    const Preset presets[PRESETS_PER_BANK];
} Bank;

static Bank banks[NUM_BANKS] = {
  {
    "Unify Custom", {           // dry,  early, late,    size, width, delay, diffuse, low cut, low xo,  low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Natural Hall",          { 80.0,  10.0,  16.4,    22.0, 130.0,   0.0,   100.0,       0, 1200.0,       1.5,   7300.0,  1000.0,  0.620609,  0.0,   40.0, 2.77599,  20.0, 10.0  }},
      {"Drum Hall",             { 80.0,  12.0,  13.0, 19.2813, 100.0,   7.5,    60.0, 106.103, 879.891, 0.847375,   5853.2,  1000.0,  0.620609,  0.0,   40.0, 2.77599,  20.0, 10.0  }},
      {"String Hall",           {  0.0, 22.42, 100.0,    42.0, 100.0, 15.44,   100.0,     0.0, 1200.0,   1.52622,  8690.08,  7158.91,      1.2, 4.23,  17.52,     1.4,  20.0, 45.0  }},
      {"Mysterious Hall",       {  0.0, 65.28, 100.0,    40.0, 150.0,  20.0,    60.0,   26.76,  600.0,       2.5,  8848.75,  5052.81,      0.2,  3.0,   35.0,    5.09, 100.0, 30.0  }},
      {"Fantasy Hall",          {  0.0, 22.42, 100.0,    42.1, 150.0, 15.44,   100.0,   253.4, 1200.0,   1.52622,  3033.91,  9080.55, 0.620609, 1.25,  25.08,  5.6777,  20.0, 75.86 }},
    }
  },
  {
    "Rooms", {                   // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Bright Room",            { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    90.0,     4.0,    500,     0.80,    16000,    7900,      0.75,  1.0, 25.0,     0.6,    20.0,    30.0 }},
      {"Clear Room",             { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    90.0,     4.0,    500,     0.90,    13000,    5800,      0.50,  1.0, 25.0,     0.6,    20.0,    30.0 }},
      {"Dark Room",              { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    50.0,     4.0,    500,     1.20,     7300,    4900,      0.35,  1.0, 25.0,     0.7,    20.0,    30.0 }},
      {"Small Chamber",          { 80.0,  10.0, 20.0, 16.0,  80.0,   8.0,    70.0,     4.0,    500,     1.10,     8200,    5500,      0.35,  1.2, 10.0,     0.8,    20.0,    20.0 }},
      {"Large Chamber",          { 80.0,  10.0, 20.0, 20.0,  80.0,   8.0,    90.0,     4.0,    500,     1.30,     7000,    4900,      0.25,  1.8, 12.0,     1.0,    20.0,    20.0 }},
    }
  },
  {
    "Studios", {                 // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Acoustic Studio",        { 80.0,  10.0, 20.0, 12.0,  90.0,   8.0,    75.0,     4.0,    450,     1.50,     7600,    4900,      0.80,  2.5,  7.0,     0.8,    20.0,    20.0 }},
      {"Electric Studio",        { 80.0,  10.0, 20.0, 12.0,  90.0,   6.0,    45.0,     4.0,    250,     1.25,     7600,    5800,      0.70,  2.5,  7.0,     0.9,    20.0,    30.0 }},
      {"Percussion Studio",      { 80.0,  10.0, 20.0, 12.0,  90.0,   6.0,    30.0,    20.0,    200,     1.75,     5800,    5200,      0.45,  2.5,  7.0,     0.7,    20.0,    10.0 }},
      {"Piano Studio",           { 80.0,  10.0, 20.0, 12.0,  80.0,   8.0,    40.0,    20.0,    600,     1.50,     8200,    5800,      0.50,  2.8, 10.0,     0.7,    20.0,    15.0 }},
      {"Vocal Studio",           { 80.0,  10.0, 20.0, 12.0,  90.0,   0.0,    60.0,     4.0,    400,     1.20,     5800,    5200,      0.40,  2.5,  7.0,     0.8,    20.0,    10.0 }},
    }
  },
  {
    "Small Halls", {             // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Small Bright Hall",      { 80.0,  10.0, 20.0, 24.0,  80.0,  12.0,    90.0,     4.0,    400,     1.10,    11200,    6250,      0.75,  2.5, 13.0,     1.3,    20.0,    15.0 }},
      {"Small Clear Hall",       { 80.0,  10.0, 20.0, 24.0, 100.0,   4.0,    90.0,     4.0,    500,     1.30,     7600,    5500,      0.50,  3.3, 15.0,     1.3,    20.0,    15.0 }},
      {"Small Dark Hall",        { 80.0,  10.0, 20.0, 24.0, 100.0,  12.0,    60.0,     4.0,    500,     1.50,     5800,    4000,      0.35,  2.5, 10.0,     1.5,    20.0,    15.0 }},
      {"Small Percussion Hall",  { 80.0,  10.0, 20.0, 24.0,  80.0,  12.0,    40.0,    20.0,    250,     2.00,     5200,    4000,      0.35,  2.0, 13.0,     1.1,    20.0,    10.0 }},
      {"Small Vocal Hall",       { 80.0,  10.0, 20.0, 24.0,  80.0,   4.0,    60.0,     4.0,    500,     1.25,     6250,    5200,      0.35,  3.1, 15.0,     1.2,    20.0,    10.0 }},
    }
  },
  {
    "Medium Halls", {            // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Medium Bright Hall",     { 80.0,  10.0, 20.0, 30.0, 100.0,  18.0,    90.0,     4.0,    400,     1.25,    10000,    6400,      0.60,  2.9, 15.0,     1.6,    20.0,    15.0 }},
      {"Medium Clear Hall",      { 80.0,  10.0, 20.0, 30.0, 100.0,   8.0,    90.0,     4.0,    500,     1.50,     7600,    5500,      0.50,  2.9, 15.0,     1.7,    20.0,    15.0 }},
      {"Medium Dark Hall",       { 80.0,  10.0, 20.0, 30.0, 100.0,  18.0,    60.0,     4.0,    500,     1.75,     5800,    4000,      0.40,  2.9, 15.0,     1.8,    20.0,    15.0 }},
      {"Medium Percussion Hall", { 80.0,  10.0, 20.0, 30.0,  80.0,  12.0,    40.0,    20.0,    300,     2.00,     5200,    4000,      0.35,  2.0, 12.0,     1.2,    20.0,    10.0 }},
      {"Medium Vocal Hall",      { 80.0,  10.0, 20.0, 32.0,  80.0,   8.0,    75.0,     4.0,    600,     1.50,     5800,    5200,      0.40,  2.8, 16.0,     1.3,    20.0,    10.0 }},
    }
  },
  {
    "Large Halls", {             // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Large Bright Hall",      { 80.0,  10.0, 20.0, 40.0, 100.0,  20.0,    90.0,     4.0,    400,     1.50,     8200,    5800,      0.50,  2.1, 20.0,     2.5,    20.0,    15.0 }},
      {"Large Clear Hall",       { 80.0,  10.0, 20.0, 40.0, 100.0,  12.0,    80.0,     4.0,    550,     2.00,     8200,    5200,      0.40,  2.1, 20.0,     2.8,    20.0,    15.0 }},
      {"Large Dark Hall",        { 80.0,  10.0, 20.0, 40.0, 100.0,  20.0,    60.0,     4.0,    600,     2.50,     6250,    2800,      0.20,  2.1, 20.0,     3.0,    20.0,    15.0 }},
      {"Large Vocal Hall",       { 80.0,  10.0, 20.0, 40.0,  80.0,  12.0,    80.0,     4.0,    700,     2.25,     6250,    4600,      0.30,  2.1, 17.0,     2.4,    20.0,    10.0 }},
      {"Great Hall",             { 80.0,  10.0, 20.0, 50.0,  90.0,  20.0,    95.0,     4.0,    750,     2.50,     5500,    4000,      0.30,  2.6, 22.0,     3.8,    20.0,    15.0 }},
    }
  }
};

const String DragonflyHallProcessor::getProgramName(int progIndex)
{
    int bankIndex = progIndex / 5;
    progIndex = progIndex % 5;
    return String(banks[bankIndex].presets[progIndex].name);
}

void DragonflyHallProcessor::populateProgramsComboBox(ComboBox& cb)
{
    int itemID = 0;
    for (int bankIndex = 0; bankIndex < NUM_BANKS; bankIndex++)
    {
        cb.addSectionHeading(String(banks[bankIndex].name));
        for (int progIndex = 0; progIndex < PRESETS_PER_BANK; progIndex++)
            cb.addItem(String(banks[bankIndex].presets[progIndex].name), ++itemID);
    }
}

void DragonflyHallProcessor::setCurrentProgram(int progIndex)
{
    currentProgramIndex = progIndex = jlimit<int>(0, NUM_BANKS * PRESETS_PER_BANK - 1, progIndex);
    int bankIndex = progIndex / 5;
    progIndex = progIndex % 5;
    auto& preset = banks[bankIndex].presets[progIndex].params;

    // setValueNotifyingHost() requires NORMALIZED parameter values
    RangedAudioParameter* param;
#define SET_NORMALIZED(name, id) \
    param = valueTreeState.getParameter(DragonflyHallParameters:: id); \
    param->setValueNotifyingHost(param->convertTo0to1(preset. name))

    SET_NORMALIZED(dryLevel, dryLevelID);
    SET_NORMALIZED(earlyLevel, earlyLevelID);
    SET_NORMALIZED(earlySend, earlySendID);
    SET_NORMALIZED(lateLevel, lateLevelID);
    SET_NORMALIZED(size, sizeID);
    SET_NORMALIZED(width, widthID);
    SET_NORMALIZED(predelay, predelayID);
    SET_NORMALIZED(diffuse, diffuseID);
    SET_NORMALIZED(lowCut, lowCutID);
    SET_NORMALIZED(lowCross, lowCrossID);
    SET_NORMALIZED(lowMult, lowMultID);
    SET_NORMALIZED(highCut, highCutID);
    SET_NORMALIZED(highCross, highCrossID);
    SET_NORMALIZED(highMult, highMultID);
    SET_NORMALIZED(spin, spinID);
    SET_NORMALIZED(wander, wanderID);
    SET_NORMALIZED(decay, decayID);
    SET_NORMALIZED(modulation, modulationID);

    sendChangeMessage();
}

// Respond to parameter changes
void DragonflyHallProcessor::parameterChanged(const String& paramID, float value)
{
    if (paramID == DragonflyHallParameters::sizeID)
    {
        const ScopedLock processBlockLock(getCallbackLock());   // lock out processBlock calls while making changes
        early.setRSFactor(value / 10.0f);
        late.setRSFactor(value / 80.0f);
    }
    else if (paramID == DragonflyHallParameters::widthID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        early.setwidth(0.01f * value);
        late.setwidth(0.01f * value);
    }
    else if (paramID == DragonflyHallParameters::predelayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setPreDelay(value);
    }
    else if (paramID == DragonflyHallParameters::diffuseID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setidiffusion1(value / 140.0f);
        late.setapfeedback(value / 140.0f);
    }
    else if (paramID == DragonflyHallParameters::lowCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        early.setoutputhpf(value);
        late.setoutputhpf(value);
    }
    else if (paramID == DragonflyHallParameters::lowCrossID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setxover_low(value);
    }
    else if (paramID == DragonflyHallParameters::lowMultID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setrt60_factor_low(value);
    }
    else if (paramID == DragonflyHallParameters::highCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        early.setoutputlpf(value);
        late.setoutputlpf(value);
    }
    else if (paramID == DragonflyHallParameters::highCrossID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setxover_high(value);
    }
    else if (paramID == DragonflyHallParameters::highMultID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setrt60_factor_high(value);
    }
    else if (paramID == DragonflyHallParameters::spinID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setspin(value);
    }
    else if (paramID == DragonflyHallParameters::wanderID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setwander(value);
    }
    else if (paramID == DragonflyHallParameters::decayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setrt60(value);
    }
    else if (paramID == DragonflyHallParameters::modulationID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        late.setspinfactor(value / 100.0f);
        late.setlfofactor(value / 100.0f);
    }
}

// Destructor
DragonflyHallProcessor::~DragonflyHallProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyHallProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    float sampleRateHz = float(sampleRate);
    early.setSampleRate(sampleRateHz);
    late.setSampleRate(sampleRateHz);
}

// Audio processing finished; release any allocated memory
void DragonflyHallProcessor::releaseResources()
{
}

// Silence everything
void DragonflyHallProcessor::reset()
{
    early.mute();
    late.mute();
}

// Process one buffer ("block") of data
void DragonflyHallProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    dryBuffer.makeCopyOf(buffer, true);
    earlyOutputBuffer.setSize(2, numSamples, true, false, true);

    // buffer -> early -> earlyOutputBuffer
    float* pInLeft = (float*)buffer.getReadPointer(0);
    float* pInRight = (float*)buffer.getReadPointer(1);
    float* pEarlyLeft = earlyOutputBuffer.getWritePointer(0);
    float* pEarlyLRight = earlyOutputBuffer.getWritePointer(1);
    early.processreplace(pInLeft, pInRight, pEarlyLeft, pEarlyLRight, numSamples);

    // prepare lateInputBuffer
    lateInputBuffer.makeCopyOf(buffer, true);
    lateInputBuffer.addFrom(0, 0, earlyOutputBuffer, 0, 0, numSamples, parameters.earlySend);
    lateInputBuffer.addFrom(1, 0, earlyOutputBuffer, 1, 0, numSamples, parameters.earlySend);

    // lateOutputBuffer -> late -> buffer
    float* pLateLeft = (float*)buffer.getReadPointer(0);
    float* pLateRight = (float*)buffer.getReadPointer(1);
    float* pOutLeft = buffer.getWritePointer(0);
    float* pOutRight = buffer.getWritePointer(1);
    late.processreplace(pLateLeft, pLateRight, pOutLeft, pOutRight, numSamples);

    // compute final output mix
    buffer.applyGain(parameters.lateLevel);
    buffer.addFrom(0, 0, dryBuffer, 0, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(1, 0, dryBuffer, 1, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(0, 0, earlyOutputBuffer, 0, 0, numSamples, parameters.earlyLevel);
    buffer.addFrom(1, 0, earlyOutputBuffer, 1, 0, numSamples, parameters.earlyLevel);
}

// Called by the host when it needs to persist the current plugin state
void DragonflyHallProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DragonflyHallProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
