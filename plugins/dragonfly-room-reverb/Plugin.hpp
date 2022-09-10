/*
 * Dragonfly Reverb, copyright (c) 2019 Michael Willis, Rob van den Berg
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
#include "DragonflyVersion.h"
#include "DSP.hpp"
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
        return "Dragonfly Room Reverb";
    }

    const char* getDescription() const override
    {
        return "A room reverb based on Freeverb3 ProG";
    }

    const char* getMaker() const noexcept override
    {
        return "Michael Willis";
    }

    const char* getHomePage() const override
    {
        return "https://michaelwillis.github.io/dragonfly-reverb";
    }

    const char* getLicense() const noexcept override
    {
        return "http://www.gnu.org/licenses/gpl-3.0.html";
    }

    uint32_t getVersion() const noexcept override
    {
        return d_version(MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);
    }

    int64_t getUniqueId() const noexcept override
    {
        return d_cconst('d', 'f', 'r', '2');
    }

    // -------------------------------------------------------------------
    // Init

    void initParameter(uint32_t index, Parameter& parameter) override;
    void initState(uint32_t index, State& state) override;

    // -------------------------------------------------------------------
    // Internal data

    float getParameterValue(uint32_t index) const override;
    void  setParameterValue(uint32_t index, float value) override;

    void setState(const char* key, const char* value);

    // -------------------------------------------------------------------
    // Process

    void activate() override;
    void run(const float** inputs, float** outputs, uint32_t frames) override;

    // -------------------------------------------------------------------
    // Callbacks

    void sampleRateChanged(double newSampleRate) override;

    // -------------------------------------------------------------------

private:
    DragonflyReverbDSP dsp;
    int bank;
    int preset;
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragonflyReverbPlugin)
};

END_NAMESPACE_DISTRHO
