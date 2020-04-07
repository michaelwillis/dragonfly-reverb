#include "DragonflyRoomProcessor.h"
#include "DragonflyRoomEditor.h"

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
{
}

// Respond to parameter changes
void DragonflyRoomProcessor::parameterChanged(const String&, float)
{
}

// Destructor
DragonflyRoomProcessor::~DragonflyRoomProcessor()
{
}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyRoomProcessor::prepareToPlay (double /*sampleRate*/, int /*maxSamplesPerBlock*/)
{
}

// Audio processing finished; release any allocated memory
void DragonflyRoomProcessor::releaseResources()
{
}

// Process one buffer ("block") of data
void DragonflyRoomProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;
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
