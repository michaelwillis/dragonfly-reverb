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
    model.setwidth(0.8f);
    model.setLRDelay(0.3f);
    model.setLRCrossApFreq(750.0f, 4.0f);
    model.setDiffusionApFreq(150.0f, 4.0f);

    model.loadPresetReflection(0);
    model.setRSFactor(0.1f * parameters.size);
    model.setoutputhpf(parameters.lowCut);
    model.setoutputlpf(parameters.highCut);
    model.setSampleRate(44100.0f);
}

// Respond to parameter changes
void DragonflyERProcessor::parameterChanged(const String& paramID, float value)
{
    // List of preferred preset reflection program indices
    static long programs[] = { 2, 18, 0, 19, 1, 13, 14, 21 };

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
