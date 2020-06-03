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

#include "DragonflyPlateProcessor.h"
#include "DragonflyPlateEditor.h"

NRev::NRev() : fv3::nrev_f() { }

void NRev::setDampLpf(float value) {
    dampLpf = limFs2(value);
    dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
    dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
}

void NRev::mute() {
    fv3::nrev_f::mute();
    dampLpfL.mute();
    dampLpfR.mute();
}

void NRev::setFsFactors() {
    fv3::nrev_f::setFsFactors();
    setDampLpf(dampLpf);
}

void NRev::processloop2(long count, float* inputL, float* inputR, float* outputL, float* outputR) {
    float outL, outR;
    while (count-- > 0)
    {
        outL = outR = 0;
        hpf = damp3_1 * inDCC(*inputL + *inputR) - damp3 * hpf;
        UNDENORMAL(hpf);

        hpf *= FV3_NREV_SCALE_WET;

        for (long i = 0; i < FV3_NREV_NUM_COMB; i++) outL += combL[i]._process(hpf);
        for (long i = 0; i < 3; i++) outL = allpassL[i]._process_ov(outL);
        lpfL = dampLpfL(damp2 * lpfL + damp2_1 * outL); UNDENORMAL(lpfL);
        outL = allpassL[3]._process_ov(lpfL);
        outL = allpassL[5]._process_ov(outL);
        outL = delayWL(lLDCC(outL));

        for (long i = 0; i < FV3_NREV_NUM_COMB; i++) outR += combR[i]._process(hpf);
        for (long i = 0; i < 3; i++) outR = allpassR[i]._process_ov(outR);
        lpfR = dampLpfR(damp2 * lpfR + damp2_1 * outR); UNDENORMAL(lpfR);
        outR = allpassR[3]._process_ov(lpfR);
        outR = allpassR[6]._process_ov(outR);
        outR = delayWR(lRDCC(outR));

        *outputL = outL * wet1 + outR * wet2 + delayL(*inputL) * dry;
        *outputR = outR * wet1 + outL * wet2 + delayR(*inputR) * dry;
        inputL++; inputR++; outputL++; outputR++;
    }
}


NRevB::NRevB() : fv3::nrevb_f() { }

void NRevB::setDampLpf(float value)
{
    dampLpf = limFs2(value);
    dampLpfL.setLPF_BW(dampLpf, getTotalSampleRate());
    dampLpfR.setLPF_BW(dampLpf, getTotalSampleRate());
}

void NRevB::mute() {
    fv3::nrevb_f::mute();
    dampLpfL.mute();
    dampLpfR.mute();
}

void NRevB::setFsFactors() {
    fv3::nrevb_f::setFsFactors();
    setDampLpf(dampLpf);
}

void NRevB::processloop2(long count, float* inputL, float* inputR, float* outputL, float* outputR) {
    float outL, outR, tmpL, tmpR;
    while (count-- > 0)
    {
        hpf = damp3_1 * inDCC.process(*inputL + *inputR) - damp3 * hpf; UNDENORMAL(hpf);
        outL = outR = tmpL = tmpR = hpf;

        outL += apfeedback * lastL;
        lastL += -1 * apfeedback * outL;

        for (long i = 0; i < FV3_NREV_NUM_COMB; i++) outL += combL[i]._process(tmpL);
        for (long i = 0; i < FV3_NREVB_NUM_COMB_2; i++) outL += comb2L[i]._process(tmpL);
        for (long i = 0; i < 3; i++) outL = allpassL[i]._process(outL);
        for (long i = 0; i < FV3_NREVB_NUM_ALLPASS_2; i++) outL = allpass2L[i]._process(outL);
        lpfL = dampLpfL(damp2 * lpfL + damp2_1 * outL); UNDENORMAL(lpfL);
        outL = allpassL[3]._process(lpfL); outL = allpassL[5]._process(outL);
        outL = lLDCC(outL);

        outR += apfeedback * lastR;
        lastR += -1 * apfeedback * outR;
        for (long i = 0; i < FV3_NREV_NUM_COMB; i++) outR += combR[i]._process(tmpR);
        for (long i = 0; i < FV3_NREVB_NUM_COMB_2; i++) outR += comb2R[i]._process(tmpR);
        for (long i = 0; i < 3; i++) outR = allpassR[i]._process(outR);
        for (long i = 0; i < FV3_NREVB_NUM_ALLPASS_2; i++) outR = allpass2R[i]._process(outR);
        lpfR = dampLpfR(damp2 * lpfR + damp2_1 * outR); UNDENORMAL(lpfR);
        outR = allpassR[3]._process(lpfR); outR = allpassL[6]._process(outR);
        outR = lRDCC(outR);

        lastL = FV3_NREVB_SCALE_WET * delayWL(lastL);
        lastR = FV3_NREVB_SCALE_WET * delayWR(lastR);
        *outputL = lastL * wet1 + lastR * wet2 + delayL(*inputL) * dry;
        *outputR = lastR * wet1 + lastL * wet2 + delayR(*inputR) * dry;
        lastL = outL; lastR = outR;
        inputL++; inputR++; outputL++; outputR++;
    }
}


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
    , sampleRateHz(44100.0f)
{
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();

    nrev.setSampleRate(sampleRateHz);
    nrevb.setSampleRate(sampleRateHz);
    strev.setSampleRate(sampleRateHz);

    nrev.setdryr(0.0f);
    nrev.setwetr(1.0f);
    nrev.setMuteOnChange(false);

    nrevb.setdryr(0.0f);
    nrevb.setwetr(1.0f);
    nrevb.setMuteOnChange(false);

    strev.setdryr(0.0f);
    strev.setwetr(1.0f);
    strev.setMuteOnChange(false);
    strev.setdccutfreq(6.0f);
    strev.setspinlimit(12.0f);
    strev.setspindiff(0.15f);

    model = &nrevb;

    nrev.setwidth(parameters.width / 120.0f);
    nrevb.setwidth(parameters.width / 120.0f);
    strev.setwidth(parameters.width / 120.0f);

    nrev.setPreDelay(parameters.predelay);
    nrevb.setPreDelay(parameters.predelay);
    strev.setPreDelay(parameters.predelay);

    nrev.setrt60(parameters.decay);
    nrevb.setrt60(parameters.decay);
    strev.setrt60(parameters.decay);

    nrev.setDampLpf(parameters.dampen);
    nrevb.setDampLpf(parameters.dampen);
    strev.setdamp(parameters.dampen);
    strev.setoutputdamp(fmaxf(parameters.dampen * 2.0f, 16000.0f));

    setCurrentProgram(0);
}

// built-in presets aka "programs"
#define NUM_PRESETS 9
enum AlgorithmIndex { ALGORITHM_NREV, ALGORITHM_NREV_B, ALGORITHM_STREV };

struct Preset
{
    String name;
    float dryLevel, wetLevel;
    AlgorithmIndex algIndex;
    float width;
    float delay;
    float decay;
    float lowCut, highCut, dampFreq;
};

static Preset presets[NUM_PRESETS] = {
    //                 dry,   wet,       algorithm,   width, delay, decay, low cut, high cut,     damp
    {"Unify Custom", 85.0f, 15.0f, ALGORITHM_NREV_B, 130.0f, 125.0f,  3.0f, 750.0f, 16000.0f, 16000.0f },
    {"Abrupt Plate", 80.0f, 20.0f, ALGORITHM_NREV_B, 100.0f, 20.0f,   0.2f,  50.0f, 10000.0f,  7000.0f },
    {"Bright Plate", 80.0f, 20.0f, ALGORITHM_NREV_B, 100.0f,  0.0f,   0.4f, 200.0f, 16000.0f, 13000.0f },
    {"Clear Plate",  80.0f, 20.0f, ALGORITHM_NREV_B, 100.0f,  0.0f,   0.6f, 100.0f, 13000.0f,  7000.0f },
    {"Dark Plate",   80.0f, 20.0f, ALGORITHM_NREV_B, 100.0f,  0.0f,   0.8f,  50.0f,  7000.0f,  4000.0f },
    {"Foil Tray",    80.0f, 20.0f,   ALGORITHM_NREV,  50.0f,  0.0f,   0.3f, 200.0f, 16000.0f, 13000.0f },
    {"Metal Roof",   80.0f, 20.0f,   ALGORITHM_NREV, 120.0f, 20.0f,   0.5f, 100.0f, 13000.0f, 10000.0f },
    {"Narrow Tank",  80.0f, 20.0f,  ALGORITHM_STREV,  60.0f, 10.0f,   0.6f,  50.0f, 10000.0f,  7000.0f },
    {"Phat Tank",    80.0f, 20.0f,  ALGORITHM_STREV, 150.0f, 10.0f,   1.0f,  50.0f, 10000.0f,  4000.0f }
};

void DragonflyPlateProcessor::setCurrentProgram(int newProgramIndex)
{
    currentProgramIndex = jlimit<int>(0, NUM_PRESETS - 1, newProgramIndex);
    Preset& preset = presets[currentProgramIndex];

    // setValueNotifyingHost() requires NORMALIZED parameter values
    RangedAudioParameter* param;
#define SET_NORMALIZED(name) \
    param = valueTreeState.getParameter(DragonflyPlateParameters:: ## name ## ID); \
    param->setValueNotifyingHost(param->convertTo0to1(preset. ## name))
#define SET_NORMALIZED2(name, id) \
    param = valueTreeState.getParameter(DragonflyPlateParameters:: id); \
    param->setValueNotifyingHost(param->convertTo0to1(preset. ## name))

    SET_NORMALIZED(dryLevel);
    SET_NORMALIZED(wetLevel);
    SET_NORMALIZED(algIndex);
    SET_NORMALIZED(width);
    SET_NORMALIZED2(delay, predelayID);
    SET_NORMALIZED(decay);
    SET_NORMALIZED(lowCut);
    SET_NORMALIZED(highCut);
    SET_NORMALIZED2(dampFreq, dampenID);
}

const String DragonflyPlateProcessor::getProgramName(int programIndex)
{
    programIndex = jlimit<int>(0, NUM_PRESETS - 1, programIndex);
    return presets[programIndex].name;
}


// Respond to parameter changes
void DragonflyPlateProcessor::parameterChanged(const String& paramID, float value)
{
    if (paramID == DragonflyPlateParameters::algIndexID)
    {
        const ScopedLock processBlockLock(getCallbackLock());   // lock out processBlock calls while making changes
        fv3::revbase_f* previous = model;
        int algorithm = value;
        if (algorithm == ALGORITHM_NREV) model = &nrev;
        else if (algorithm == ALGORITHM_NREV_B) model = &nrevb;
        else if (algorithm == ALGORITHM_STREV) model = &strev;
        if (model != previous) previous->mute();
    }
    else if (paramID == DragonflyPlateParameters::widthID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        nrev.setwidth(value / 120.0f);
        nrevb.setwidth(value / 120.0f);
        strev.setwidth(value / 120.0f);
    }
    else if (paramID == DragonflyPlateParameters::predelayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        nrev.setPreDelay(value);
        nrevb.setPreDelay(value);
        strev.setPreDelay(value);
    }
    else if (paramID == DragonflyPlateParameters::decayID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        nrev.setrt60(value);
        nrevb.setrt60(value);
        strev.setrt60(value);
    }
    else if (paramID == DragonflyPlateParameters::lowCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        setInputHPF(value);
    }
    else if (paramID == DragonflyPlateParameters::highCutID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        setInputLPF(value);
    }
    else if (paramID == DragonflyPlateParameters::dampenID)
    {
        const ScopedLock processBlockLock(getCallbackLock());
        nrev.setDampLpf(value);
        nrevb.setDampLpf(value);
        strev.setdamp(value);
        strev.setoutputdamp(fmaxf(value * 2.0f, 16000.0f));
    }
}

// Destructor
DragonflyPlateProcessor::~DragonflyPlateProcessor()
{
}

// Silence everything
void DragonflyPlateProcessor::reset()
{

}

// Prepare to process audio (always called at least once before processBlock)
void DragonflyPlateProcessor::prepareToPlay (double sampleRate, int /*maxSamplesPerBlock*/)
{
    sampleRateHz = float(sampleRate);
    nrev.setSampleRate(sampleRateHz);
    nrevb.setSampleRate(sampleRateHz);
    strev.setSampleRate(sampleRateHz);
    setInputLPF(parameters.highCut);
    setInputHPF(parameters.lowCut);
}

// Audio processing finished; release any allocated memory
void DragonflyPlateProcessor::releaseResources()
{
    input_lpf_0.mute();
    input_lpf_1.mute();
    input_hpf_0.mute();
    input_hpf_1.mute();
    nrev.mute();
    nrevb.mute();
    strev.mute();
}

// Process one buffer ("block") of data
void DragonflyPlateProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer&)
{
    ScopedNoDenormals noDenormals;

    int numSamples = buffer.getNumSamples();
    dryBuffer.makeCopyOf(buffer, true);
    filteredInputBuffer.setSize(2, numSamples, true, false, true);

    float* pInLeft = (float*)buffer.getReadPointer(0);
    float* pInRight = (float*)buffer.getReadPointer(1);
    float* pFiLeft = filteredInputBuffer.getWritePointer(0);
    float* pFiRight = filteredInputBuffer.getWritePointer(1);

    for (int i = 0; i < numSamples; i++)
    {
        pFiLeft[i] = input_lpf_0.process(input_hpf_0.process(pInLeft[i]));
        pFiRight[i] = input_lpf_1.process(input_hpf_1.process(pInRight[i]));
    }

    float* pOutLeft = buffer.getWritePointer(0);
    float* pOutRight = buffer.getWritePointer(1);
    model->processreplace(pFiLeft, pFiRight, pOutLeft, pOutRight, numSamples);

    buffer.applyGain(parameters.wetLevel);
    buffer.addFrom(0, 0, dryBuffer, 0, 0, numSamples, parameters.dryLevel);
    buffer.addFrom(1, 0, dryBuffer, 1, 0, numSamples, parameters.dryLevel);
}

void DragonflyPlateProcessor::setInputLPF(float freq)
{
    freq = jlimit<float>(0.0f, 0.5f * sampleRateHz, freq);
    input_lpf_0.setLPF_BW(freq, sampleRateHz);
    input_lpf_1.setLPF_BW(freq, sampleRateHz);
}

void DragonflyPlateProcessor::setInputHPF(float freq)
{
    freq = jlimit<float>(0.0f, 0.5f * sampleRateHz, freq);
    input_hpf_0.setHPF_BW(freq, sampleRateHz);
    input_hpf_1.setHPF_BW(freq, sampleRateHz);
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
