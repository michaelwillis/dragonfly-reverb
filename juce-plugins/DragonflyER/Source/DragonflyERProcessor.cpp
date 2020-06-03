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

#include "DragonflyERProcessor.h"
#include "DragonflyEREditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DragonflyERProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DragonflyERProcessor::createEditor()
{
    return new DragonflyEREditor(*this);
}

// List of preferred preset reflection program indices
static long programs[] = { 2, 18, 0, 19, 1, 13, 14, 21 };

// program names
static const char* progNames[] = {
    "Abrupt Echo",
    "Backstage Pass",
    "Concert Venue",
    "Damaged Goods",
    "Elevator Pitch",
    "Floor Thirteen",
    "Garage Band",
    "Home Studio"
};

void DragonflyERProcessor::populateProgramsComboBox(ComboBox& cb)
{
    cb.clear();
    for (int i = 0; i < 8; i++)
        cb.addItem(progNames[i], i + 1);
}

// Constructor: start off assuming stereo input, stereo output
DragonflyERProcessor::DragonflyERProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("DragonflyER"), DragonflyERParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
    model.setMuteOnChange(false);
    model.setdryr(0.0f);    // mute dry signal
    model.setwet(0.0f);     // 0dB
    model.setLRDelay(0.3f);
    model.setLRCrossApFreq(750.0f, 4.0f);
    model.setDiffusionApFreq(150.0f, 4.0f);

    model.setRSFactor(0.1f * parameters.size);
    model.setwidth(0.01f * parameters.width);
    model.setoutputhpf(parameters.lowCut);
    model.setoutputlpf(parameters.highCut);
    model.setSampleRate(44100.0f);

    // set initial
    RangedAudioParameter* param = valueTreeState.getParameter(DragonflyERParameters::progIndexID);
    param->setValueNotifyingHost(param->convertTo0to1(float(DragonflyERParameters::progIndexDefault)));
}

// Respond to parameter changes
void DragonflyERProcessor::parameterChanged(const String& paramID, float value)
{
    if (paramID == DragonflyERParameters::progIndexID)
    {
        const ScopedLock processBlockLock(getCallbackLock());   // lock out processBlock calls while making changes
        model.loadPresetReflection(programs[int(value)]);
    }
    else if (paramID == DragonflyERParameters::sizeID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        model.setRSFactor(0.1f * value);
    }
    else if (paramID == DragonflyERParameters::widthID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        model.setwidth(0.01f * value);
    }
    else if (paramID == DragonflyERParameters::lowCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        model.setoutputhpf(value);
    }
    else if (paramID == DragonflyERParameters::highCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        model.setoutputlpf(value);
    }
}

// Destructor
DragonflyERProcessor::~DragonflyERProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyERProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    model.setSampleRate(float(sampleRate));
}

// Audio processing finished; release any allocated memory
void DragonflyERProcessor::releaseResources()
{
}

// Silence everything
void DragonflyERProcessor::reset()
{
    model.mute();
}

// Process one buffer ("block") of data
void DragonflyERProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    dryBuffer.makeCopyOf(buffer, true);

    float* pInLeft = (float*)dryBuffer.getReadPointer(0);
    float* pInRight = (float*)dryBuffer.getReadPointer(1);
    float* pOutLeft = buffer.getWritePointer(0);
    float* pOutRight = buffer.getWritePointer(1);

    model.processreplace(pInLeft, pInRight, pOutLeft, pOutRight, numSamples);

    buffer.applyGain(parameters.wetLevel);
    buffer.addFrom(0, 0, dryBuffer, 0, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(1, 0, dryBuffer, 1, 0, numSamples, parameters.dryLevel);
}

// Called by the host when it needs to persist the current plugin state
void DragonflyERProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DragonflyERProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
