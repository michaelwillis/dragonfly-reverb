/*
 * Dragonfly Reverb, copyright (c) 2022 Michael Willis
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

#include <juce_audio_processors/juce_audio_processors.h>

#include <AvailabilityMacros.h>

#define createPlugin createStaticPlugin
#include "src/DistrhoPluginInternal.hpp"
#include "src/DistrhoUIInternal.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class ParameterFromDPF : public juce::AudioProcessorParameter
{
    PluginExporter& plugin;
    const ParameterEnumerationValues& enumValues;
    const ParameterRanges& ranges;
    const uint32_t hints;
    const uint index;
    bool* const updatedPtr;
    mutable juce::StringArray dpfValueStrings;

public:
    ParameterFromDPF(PluginExporter& plugin_, const uint index_, bool* const updatedPtr_)
        : plugin(plugin_),
          enumValues(plugin_.getParameterEnumValues(index_)),
          ranges(plugin_.getParameterRanges(index_)),
          hints(plugin_.getParameterHints(index_)),
          index(index_),
          updatedPtr(updatedPtr_) {}

    void setValueNotifyingHostFromDPF(const float newValue)
    {
        setValueNotifyingHost(ranges.getNormalizedValue(newValue));
        *updatedPtr = false;
    }

protected:
    float getValue() const override
    {
        return ranges.getNormalizedValue(plugin.getParameterValue(index));
    }

    void setValue(const float newValue) override
    {
        *updatedPtr = true;
        plugin.setParameterValue(index, ranges.getUnnormalizedValue(newValue));
    }

    float getDefaultValue() const override
    {
        return ranges.getNormalizedValue(plugin.getParameterDefault(index));
    }

    juce::String getName(const int maximumStringLength) const override
    {
        if (maximumStringLength <= 0)
            return juce::String(plugin.getParameterName(index).buffer());

        return juce::String(plugin.getParameterName(index).buffer(), static_cast<size_t>(maximumStringLength));
    }

    juce::String getLabel() const override
    {
        return plugin.getParameterUnit(index).buffer();
    }

    int getNumSteps() const override
    {
        if (hints & kParameterIsBoolean)
            return 2;

        if (enumValues.restrictedMode)
            return enumValues.count;

        if (hints & kParameterIsInteger)
            return ranges.max - ranges.min;

        return juce::AudioProcessorParameter::getNumSteps();
    }

    bool isDiscrete() const override
    {
        if (hints & (kParameterIsBoolean|kParameterIsInteger))
            return true;

        if (enumValues.restrictedMode)
            return true;

        return false;
    }

    bool isBoolean() const override
    {
        return (hints & kParameterIsBoolean) != 0x0;
    }

    juce::String getText(const float normalizedValue, const int maximumStringLength) const override
    {
        float value = ranges.getUnnormalizedValue(normalizedValue);

        if (hints & kParameterIsBoolean)
        {
            const float midRange = ranges.min + (ranges.max - ranges.min) * 0.5f;
            value = value > midRange ? ranges.max : ranges.min;
        }
        else if (hints & kParameterIsInteger)
        {
            value = std::round(value);
        }

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (d_isEqual(enumValues.values[i].value, value))
                {
                    if (maximumStringLength <= 0)
                        return juce::String(enumValues.values[i].label);

                    return juce::String(enumValues.values[i].label, static_cast<size_t>(maximumStringLength));
                }
            }
        }

        juce::String text;
        if (hints & kParameterIsInteger)
            text = juce::String(static_cast<int>(value));
        else
            text = juce::String(value);

        if (maximumStringLength <= 0)
            return text;

        return juce::String(text.toRawUTF8(), static_cast<size_t>(maximumStringLength));
    }

    float getValueForText(const juce::String& text) const override
    {
        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (text == enumValues.values[i].label.buffer())
                    return ranges.getNormalizedValue(enumValues.values[i].value);
            }
        }

        float value;
        if (hints & kParameterIsInteger)
            value = std::atoi(text.toRawUTF8());
        else
            value = std::atof(text.toRawUTF8());

        return ranges.getFixedAndNormalizedValue(value);
    }

    bool isAutomatable() const override
    {
        return (hints & kParameterIsAutomatable) != 0x0;
    }

    juce::String getCurrentValueAsText() const override
    {
        const float value = plugin.getParameterValue(index);

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
            {
                if (d_isEqual(enumValues.values[i].value, value))
                    return juce::String(enumValues.values[i].label);
            }
        }

        if (hints & kParameterIsInteger)
            return juce::String(static_cast<int>(value));

        return juce::String(value);
    }

    juce::StringArray getAllValueStrings() const override
    {
        if (dpfValueStrings.size() != 0)
            return dpfValueStrings;

        if (enumValues.restrictedMode)
        {
            for (uint32_t i=0; i < enumValues.count; ++i)
                dpfValueStrings.add(enumValues.values[i].label.buffer());

            return dpfValueStrings;
        }

        if (hints & kParameterIsBoolean)
        {
            if (hints & kParameterIsInteger)
            {
                dpfValueStrings.add(juce::String(static_cast<int>(ranges.min)));
                dpfValueStrings.add(juce::String(static_cast<int>(ranges.max)));
            }
            else
            {
                dpfValueStrings.add(juce::String(ranges.min));
                dpfValueStrings.add(juce::String(ranges.max));
            }
        }
        else if (hints & kParameterIsInteger)
        {
            const int imin = static_cast<int>(ranges.min);
            const int imax = static_cast<int>(ranges.max);

            for (int i=imin; i<=imax; ++i)
                dpfValueStrings.add(juce::String(i));
        }

        return dpfValueStrings;
    }
};

// --------------------------------------------------------------------------------------------------------------------

// unused
static constexpr const writeMidiFunc nullWriteMidiFunc = nullptr;

// unused
static constexpr const requestParameterValueChangeFunc nullRequestParameterValueChangeFunc = nullptr;

// only needed for headless builds, which this wrapper never builds for
static constexpr const updateStateValueFunc nullUpdateStateValueFunc = nullptr;

// DSP/processor implementation
class DragonflyWrapperProcessor : public juce::AudioProcessor
{
    friend class DragonflyWrapperEditor;

    PluginExporter plugin;
    TimePosition timePosition;
    const uint32_t parameterCount;

    juce::AudioProcessorParameter* bypassParameter;
    bool* updatedParameters;

public:
    DragonflyWrapperProcessor()
        : plugin(this, nullWriteMidiFunc, nullRequestParameterValueChangeFunc, nullUpdateStateValueFunc),
          parameterCount(plugin.getParameterCount()),
          bypassParameter(nullptr),
          updatedParameters(nullptr)
    {
        if (const double sampleRate = getSampleRate())
            if (sampleRate > 0.0)
                plugin.setSampleRate(sampleRate);

        if (const int samplesPerBlock = getBlockSize())
            if (samplesPerBlock > 0)
                plugin.setBufferSize(static_cast<uint32_t>(samplesPerBlock));

        if (parameterCount != 0)
        {
            updatedParameters = new bool[parameterCount];
            std::memset(updatedParameters, 0, sizeof(bool)*parameterCount);

            for (uint i=0; i<parameterCount; ++i)
            {
                ParameterFromDPF* const param = new ParameterFromDPF(plugin, i, updatedParameters + i);
                addParameter(param);

                if (plugin.getParameterDesignation(i) == kParameterDesignationBypass)
                    bypassParameter = param;
            }
        }
    }

    ~DragonflyWrapperProcessor() override
    {
        delete[] updatedParameters;
    }

protected:
    const juce::String getName() const override
    {
        return plugin.getName();
    }

    juce::StringArray getAlternateDisplayNames() const override
    {
        return juce::StringArray(plugin.getLabel());
    }

    void prepareToPlay(const double sampleRate, const int samplesPerBlock) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(samplesPerBlock > 0,);

        plugin.deactivateIfNeeded();
        plugin.setSampleRate(sampleRate);
        plugin.setBufferSize(static_cast<uint32_t>(samplesPerBlock));
        plugin.activate();
    }

    void releaseResources() override
    {
        plugin.deactivateIfNeeded();
    }

    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override
    {
        const int numSamples = buffer.getNumSamples();
        DISTRHO_SAFE_ASSERT_INT_RETURN(numSamples > 0, numSamples, midiMessages.clear());
        DISTRHO_SAFE_ASSERT_RETURN(buffer.getNumChannels() >= 2,);

        const float* audioBufferIn[18] = {};
        float* audioBufferOut[18] = {};

        for (int i=buffer.getNumChannels(); --i >= 0;)
        {
            audioBufferIn[i] = buffer.getReadPointer(i);
            audioBufferOut[i] = buffer.getWritePointer(i);
        }

        plugin.run(audioBufferIn, audioBufferOut, static_cast<uint32_t>(numSamples));
    }

    // fix compiler warning
    void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override {}

    double getTailLengthSeconds() const override
    {
        return 0.0;
    }

    bool acceptsMidi() const override
    {
        return true;
    }

    bool producesMidi() const override
    {
        return true;
    }

    juce::AudioProcessorParameter* getBypassParameter() const override
    {
        return bypassParameter;
    }

    juce::AudioProcessorEditor* createEditor() override;

    bool hasEditor() const override
    {
        return true;
    }

    int getNumPrograms() override
    {
        return 1;
    }

    int getCurrentProgram() override
    {
        return 0;
    }

    void setCurrentProgram(int) override
    {
    }

    const juce::String getProgramName(int) override
    {
        return "Default";
    }

    void changeProgramName(int, const juce::String&) override
    {
    }

    void getStateInformation(juce::MemoryBlock& destData) override
    {
        juce::XmlElement xmlState("DragonflyState");

        for (uint32_t i=0; i<parameterCount; ++i)
            xmlState.setAttribute(plugin.getParameterSymbol(i).buffer(), plugin.getParameterValue(i));

        for (uint32_t i=0, stateCount=plugin.getStateCount(); i<stateCount; ++i)
        {
            const String& key(plugin.getStateKey(i));
            xmlState.setAttribute(key.buffer(), plugin.getStateValue(key).buffer());
        }

        copyXmlToBinary(xmlState, destData);
    }

    void setStateInformation(const void* const data, const int sizeInBytes) override
    {
        std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
        DISTRHO_SAFE_ASSERT_RETURN(xmlState.get() != nullptr,);

        const juce::Array<juce::AudioProcessorParameter*>& parameters(getParameters());

        for (uint32_t i=0; i<parameterCount; ++i)
        {
            const double value = xmlState->getDoubleAttribute(plugin.getParameterSymbol(i).buffer(),
                                                              plugin.getParameterDefault(i));
            const float normalizedValue = plugin.getParameterRanges(i).getFixedAndNormalizedValue(value);
            parameters.getUnchecked(static_cast<int>(i))->setValueNotifyingHost(normalizedValue);
        }

        for (uint32_t i=0, stateCount=plugin.getStateCount(); i<stateCount; ++i)
        {
            const String& key(plugin.getStateKey(i));
            const juce::String value = xmlState->getStringAttribute(key.buffer(),
                                                                    plugin.getStateDefaultValue(i).buffer());
            plugin.setState(key, value.toRawUTF8());
        }
    }

};

// --------------------------------------------------------------------------------------------------------------------

// unused
static constexpr const sendNoteFunc nullSendNoteFunc = nullptr;

// unwanted, juce file dialogs are ugly
static constexpr const fileRequestFunc nullFileRequestFunc = nullptr;

// UI/editor implementation
class DragonflyWrapperEditor : public juce::AudioProcessorEditor,
                              private juce::Timer
{
    DragonflyWrapperProcessor& dragonflyProcessor;

    UIExporter* ui;
    void* const dspPtr;

public:
    DragonflyWrapperEditor(DragonflyWrapperProcessor& dragonflyProc)
        : juce::AudioProcessorEditor(dragonflyProc),
          dragonflyProcessor(dragonflyProc),
          ui(nullptr),
          dspPtr(dragonflyProc.plugin.getInstancePointer())
    {
        setOpaque(true);
        setResizable(true, false);
        // setResizeLimits(648, 538, -1, -1);
        setSize(1228, 666);

        startTimer(1000.0 / 60.0);
    }

    ~DragonflyWrapperEditor() override
    {
        stopTimer();
        delete ui;
    }

protected:
    void timerCallback() override
    {
        if (ui == nullptr)
            return;

        for (uint32_t i=0; i<dragonflyProcessor.parameterCount; ++i)
        {
            if (dragonflyProcessor.updatedParameters[i])
            {
                dragonflyProcessor.updatedParameters[i] = false;
                ui->parameterChanged(i, dragonflyProcessor.plugin.getParameterValue(i));
            }
        }

        repaint();
    }

    void paint(juce::Graphics&) override
    {
        if (ui == nullptr)
        {
            juce::ComponentPeer* const peer = getPeer();
            DISTRHO_SAFE_ASSERT_RETURN(peer != nullptr,);

            void* const nativeHandle = peer->getNativeHandle();
            DISTRHO_SAFE_ASSERT_RETURN(nativeHandle != nullptr,);

            ui = new UIExporter(this,
                (uintptr_t)nativeHandle,
                dragonflyProcessor.getSampleRate(),
                editParamFunc,
                setParamFunc,
                setStateFunc,
                nullSendNoteFunc,
                setSizeFunc,
                nullFileRequestFunc,
                nullptr, // bundlePath
                dspPtr,
                0.0 // scaleFactor
            );

            if (dragonflyProcessor.wrapperType == juce::AudioProcessor::wrapperType_Standalone)
            {
                const double scaleFactor = ui->getScaleFactor();
                ui->setWindowOffset(4 * scaleFactor, 30 * scaleFactor);
            }
        }

        ui->plugin_idle();
    }

private:
    static void editParamFunc(void* const ptr, const uint32_t index, const bool started)
    {
        DragonflyWrapperEditor* const editor = static_cast<DragonflyWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        DragonflyWrapperProcessor& dragonflyProcessor(editor->dragonflyProcessor);

        if (started)
            dragonflyProcessor.getParameters().getUnchecked(static_cast<int>(index))->beginChangeGesture();
        else
            dragonflyProcessor.getParameters().getUnchecked(static_cast<int>(index))->endChangeGesture();
    }

    static void setParamFunc(void* const ptr, const uint32_t index, const float value)
    {
        DragonflyWrapperEditor* const editor = static_cast<DragonflyWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        DragonflyWrapperProcessor& dragonflyProcessor(editor->dragonflyProcessor);
        const juce::Array<juce::AudioProcessorParameter*>& parameters(dragonflyProcessor.getParameters());
        juce::AudioProcessorParameter* const parameter = parameters.getUnchecked(static_cast<int>(index));
        static_cast<ParameterFromDPF*>(parameter)->setValueNotifyingHostFromDPF(value);
    }

    static void setStateFunc(void* const ptr, const char* const key, const char* const value)
    {
        DragonflyWrapperEditor* const editor = static_cast<DragonflyWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

        DragonflyWrapperProcessor& dragonflyProcessor(editor->dragonflyProcessor);
        dragonflyProcessor.plugin.setState(key, value);
    }

    static void setSizeFunc(void* const ptr, uint width, uint height)
    {
        DragonflyWrapperEditor* const editor = static_cast<DragonflyWrapperEditor*>(ptr);
        DISTRHO_SAFE_ASSERT_RETURN(editor != nullptr,);

       #ifdef DISTRHO_OS_MAC
        UIExporter* const ui = editor->ui;
        DISTRHO_SAFE_ASSERT_RETURN(ui != nullptr,);

        const double scaleFactor = ui->getScaleFactor();
        width /= scaleFactor;
        height /= scaleFactor;
       #endif

        editor->setSize(static_cast<int>(width), static_cast<int>(height));
    }
};

juce::AudioProcessorEditor* DragonflyWrapperProcessor::createEditor()
{
    return new DragonflyWrapperEditor(*this);
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

juce::AudioProcessor* createPluginFilter()
{
    // set valid but dummy values
    d_nextBufferSize = 512;
    d_nextSampleRate = 48000.0;
    return new DISTRHO_NAMESPACE::DragonflyWrapperProcessor;
}

// --------------------------------------------------------------------------------------------------------------------
