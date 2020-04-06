#pragma once
#include "JuceHeader.h"
#include "DragonflyERParameters.h"

#if JUCE_WINDOWS
#pragma warning(disable: 4290 4244 4305)
#endif
#include "freeverb/earlyref.hpp"

class DragonflyERProcessor  : public AudioProcessor
                            , public AudioProcessorValueTreeState::Listener
{
public:
    DragonflyERProcessor();
    ~DragonflyERProcessor();

    // How the plugin describes itself to the host
    const String getName() const override { return "DragonflyER"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    //bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    // Multiple simultaneously-loaded presets aka "programs" (not used)
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const String getProgramName(int) override { return {}; }
    void changeProgramName(int, const String&) override {}

    // Actual audio processing
    void prepareToPlay (double sampleRate, int maxSamplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    DragonflyERParameters parameters;
    
    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

private:
    fv3::earlyref_f model;

    AudioSampleBuffer dryBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyERProcessor)
};
