#include "DragonflyPlateProcessor.h"
#include "DragonflyPlateEditor.h"

// Instantiate this plugin
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DragonflyPlateProcessor();
}

// Instantiate this plugin's editor/GUI
AudioProcessorEditor* DragonflyPlateProcessor::createEditor()
{
    return new DragonflyPlateEditor(*this);
}

// Constructor: start off assuming stereo input, stereo output
DragonflyPlateProcessor::DragonflyPlateProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput  ("Input",  AudioChannelSet::stereo(), true)
                     .withOutput ("Output", AudioChannelSet::stereo(), true)
                     )
    , valueTreeState(*this, nullptr, Identifier("DragonflyPlate"), DragonflyPlateParameters::createParameterLayout())
    , parameters(valueTreeState, this)
{
}

// Respond to parameter changes
void DragonflyPlateProcessor::parameterChanged(const String&, float)
{
}

// Destructor
DragonflyPlateProcessor::~DragonflyPlateProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyPlateProcessor::prepareToPlay (double /*sampleRate*/, int /*maxSamplesPerBlock*/)
{
}

// Audio processing finished; release any allocated memory
void DragonflyPlateProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void DragonflyPlateProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
}

// Called by the host when it needs to persist the current plugin state
void DragonflyPlateProcessor::getStateInformation (MemoryBlock& destData)
{
    std::unique_ptr<XmlElement> xml(valueTreeState.state.createXml());
    copyXmlToBinary(*xml, destData);
}

// Called by the host before processing, when it needs to restore a saved plugin state
void DragonflyPlateProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
    if (xml && xml->hasTagName(valueTreeState.state.getType()))
    {
        valueTreeState.state = ValueTree::fromXml(*xml);
    }
}
