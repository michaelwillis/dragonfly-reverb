/*
 * Dragonfly Reverb, a hall-style reverb plugin
 * Copyright (c) 2018 Michael Willis
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

#include "DistrhoPlugin.hpp"
#include "freeverb/earlyref.hpp"
#include "freeverb/zrev2.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

class DragonflyReverbPlugin : public Plugin
{
public:
    DragonflyReverbPlugin();

protected:
    // -------------------------------------------------------------------
    // Information

    const char* getLabel() const noexcept override
    {
        return "Dragonfly Reverb";
    }

    const char* getDescription() const override
    {
        return "A hall-style reverb based on freeverb3 algorithms";
    }

    const char* getMaker() const noexcept override
    {
        return "Michael Willis";
    }

    const char* getHomePage() const override
    {
        return "https://github.com/michaelwillis/dragonfly-reverb";
    }

    const char* getLicense() const noexcept override
    {
        return "GPL v2";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(0, 0, 1);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('d', 'f', 'r', '1');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initProgramName(uint32_t index, String& programName) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;
    void  loadProgram(uint32_t index) override;

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void run(const float** inputs, float** outputs, uint32_t frames) override;

    // -------------------------------------------------------------------
    // Callbacks

    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------

private:

    static const uint32_t BUFFER_SIZE = 256;

    float dry_level;
    float early_level;
    float early_send;
    float late_level;

    float size;
    float delay;
    float diffuse;
    float low_cut;
    float low_xover;
    float low_mult;
    float high_cut;
    float high_xover;
    float high_mult;

    fv3::earlyref_f early;
    fv3::zrev2_f late;

    float early_out_buffer[2][BUFFER_SIZE];
    float late_in_buffer[2][BUFFER_SIZE];
    float late_out_buffer[2][BUFFER_SIZE];

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragonflyReverbPlugin)
};

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
