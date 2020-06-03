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

#pragma once
#include "JuceHeader.h"
#include "DragonflyRoomParameters.h"
#include "freeverb/earlyref.hpp"
#include "freeverb/progenitor2.hpp"

#define NUM_BANKS 6
#define PRESETS_PER_BANK 5

class DragonflyRoomProcessor    : public AudioProcessor
                                , public AudioProcessorValueTreeState::Listener
                                , public ChangeBroadcaster
{
public:
    DragonflyRoomProcessor();
    ~DragonflyRoomProcessor();

    // How the plugin describes itself to the host
    const String getName() const override { return "DragonflyRoom"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    //bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    bool hasEditor() const override { return true; }
    AudioProcessorEditor* createEditor() override;

    // Multiple simultaneously-loaded presets aka "programs"
    int getNumPrograms() override { return NUM_BANKS * PRESETS_PER_BANK; }
    int getCurrentProgram() override { return currentProgramIndex; }
    void setCurrentProgram(int) override;
    const String getProgramName(int) override;
    void changeProgramName(int, const String&) override {}
    void populateProgramsComboBox(ComboBox&);

    // Actual audio processing
    void prepareToPlay (double sampleRate, int maxSamplesPerBlock) override;
    void releaseResources() override;
    void reset() override;
    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    // Enabling the host to save and restore state
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

public:
    // Plugin's AudioProcessorValueTreeState
    AudioProcessorValueTreeState valueTreeState;

    // Application's view of the AudioProcessorValueTreeState, including working parameter values
    DragonflyRoomParameters parameters;
    
    // Respond to parameter changes
    void parameterChanged(const String&, float) override;

private:
    fv3::iir_1st_f input_lpf_0, input_lpf_1, input_hpf_0, input_hpf_1;

    fv3::earlyref_f early;
    fv3::progenitor2_f late;

    void setInputLPF(float freq);
    void setInputHPF(float freq);

    AudioSampleBuffer dryBuffer;
    AudioSampleBuffer filteredInputBuffer;
    AudioSampleBuffer earlyOutputBuffer;

    float sampleRateHz;
    int currentProgramIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DragonflyRoomProcessor)
};
