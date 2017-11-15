/*
 * DISTRHO MVerb, a DPF'ied MVerb.
 * Copyright (c) 2010 Martin Eastwood
 * Copyright (C) 2015 Filipe Coelho <falktx@falktx.com>
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

#include "DragonflyReverbPlugin.hpp"

#define NUM_PARAMS 19
#define NUM_PRESETS 2
#define PARAM_NAME_LENGTH 16
#define PARAM_SYMBOL_LENGTH 8
#define PRESET_NAME_LENGTH 20

START_NAMESPACE_DISTRHO

struct param {
  const char *name;
  const char *symbol;
  float range_min;
  float range_def;
  float range_max;
  const char *unit;
};

static param params[NUM_PARAMS] = {
  {"Dry Level",       "dry",        -70.0f, -10.0f,  10.0f,  "dB"},
  {"Early Level",     "e_lev",      -70.0f, -10.0f,  10.0f,  "dB"},
  {"Early Size",      "e_size",       1.0f,  10.0f,   5.0f,   "m"},
  {"Early Width",     "e_width",      0.1f,   0.4f,   1.0f,   "X"},
  {"Early Low Pass",  "e_lpf",        2.0f,   7.5f,  20.0f, "kHz"},
  {"Early Send",      "e_send",       0.0f,   0.2f,   1.0f,   "X"},
  {"Late Level",      "l_level",    -70.0f, -10.0f,  10.0f,  "dB"},
  {"Late Predelay",   "l_delay",      0.0f,  14.0f, 100.0f,  "ms"},
  {"Late Decay Time", "l_time",       0.1f,   2.0f,  10.0f, "sec"},
  {"Late Size",       "l_size",      10.0f,  40.0f, 100.0f,   "m"},
  {"Late Width",      "l_width",      0.1f,   0.9f,   1.0f,   "X"},
  {"Late Low Pass",   "l_lpf",        2.0f,   7.5f,  20.0f, "kHz"},
  {"Diffuse",         "diffuse",      0.0f,   0.8f,   1.0f,   "X"},
  {"Low Crossover",   "lo_xo",        0.1f,   0.6f,   1.0f, "kHz"},
  {"Low Decay Mult",  "lo_mult",      0.0f,   1.5f,   2.0f,   "X"},
  {"High Crossover",  "hi_xo",        1.0f,   4.5f,  20.0f, "kHz"},
  {"High Decay Mult", "hi_mult",      0.0f,   0.4f,   1.0f,   "X"},
  {"Spin",            "spin",         0.1f,   3.0f,  10.0f,  "Hz"},
  {"Wander",          "wander",       0.0f,  15.0f,  50.0f,  "ms"}
};

/*
static char presetNames[NUM_PRESETS][PRESET_NAME_LENGTH] = {
  "Room",
  "Bright Room",
  "Dark Room",

  "Studio",

  "Chamber",

  "Small Hall",
  "Small Bright Hall",
  "Small Dark Hall",
  "Small Vocal Hall",

  "Medium Hall",
  "Medium Bright Hall",
  "Medium Dark Hall",
  "Medium Vocal Hall",

  "Large Hall",
  "Large Bright Hall",
  "Large Dark Hall",
  "Large Vocal Hall",

  "Cathedral"
};
*/

static const char* preset_names[NUM_PRESETS] = {
  "Medium Hall",
  "Large Hall"
};

static float presets[NUM_PRESETS][NUM_PARAMS] = {
  // dry, e_lev, e_size, e_width, e_lpf, e_send, l_level, l_delay, l_time, l_size, l_width, l_lpf, diffuse, lo_xo, lo_mult, hi_xo, hi_mult, spin, wander
  {-10.0, -10.0,    5.0,    0.4,    7.5,    0.2,   -10.0,    15.0,    1.7,   30.0,     1.0,   5.5,     0.9,   0.6,     1.5,   4.5,    0.35,  3.0, 15.0},
  {-10.0, -10.0,    6.0,    0.8,    5.0,    0.2,   -10.0,    20.0,    2.8,   40.0,     1.0,   6.5,     0.9,   4.5,     2.4,   4.5,    0.35,  2.0, 20.0}
};

// -----------------------------------------------------------------------

DragonflyReverbPlugin::DragonflyReverbPlugin() : Plugin(NUM_PARAMS, NUM_PRESETS, 0) // 0 states
{
    early = new fv3::earlyref_f();
    early->setMuteOnChange(true);
    early->setdryr(0);
    early->setwet(0); // 0dB
    early->setLRDelay(0.3);
    early->setLRCrossApFreq(750, 4);
    early->setDiffusionApFreq(150, 4);
    early->setSampleRate(FV3_REVBASE_DEFAULT_FS);

    late = new fv3::zrev2_f();
    late->setMuteOnChange(true);
    late->setdryr(0); // mute dry signal
    late->setSampleRate(FV3_REVBASE_DEFAULT_FS);

    // set initial values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void DragonflyReverbPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    if (index < NUM_PARAMS)
    {
      parameter.hints      = kParameterIsAutomable;
      parameter.name       = params[index].name;
      parameter.symbol     = params[index].symbol;
      parameter.ranges.min = params[index].range_min;
      parameter.ranges.def = params[index].range_def;
      parameter.ranges.max = params[index].range_max;
      parameter.unit       = params[index].unit;
    }
}

void DragonflyReverbPlugin::initProgramName(uint32_t index, String& programName)
{
    if (index < NUM_PRESETS) {
      programName = preset_names[index];
    }
}

// -----------------------------------------------------------------------
// Internal data

float DragonflyReverbPlugin::getParameterValue(uint32_t index) const
{
    // FIXME! Needs to get params
    return 0.0f;
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value)
{
    // FIXME! Needs to set params!
}

void DragonflyReverbPlugin::loadProgram(uint32_t index)
{
    float *preset = presets[index];
    for (uint32_t param_index = 0; param_index < NUM_PARAMS; param_index++)
    {
        setParameterValue(param_index, preset[param_index]);
    }

    // TODO: Reset reverb algorithms if necessary!
}

// -----------------------------------------------------------------------
// Process

void DragonflyReverbPlugin::activate()
{
    // FIXME: Reset reverb algorithms
}

void DragonflyReverbPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
    // FIXME: Do reverb processing here!
    late->processreplace(const_cast<float *>(inputs[0]),const_cast<float *>(inputs[1]),outputs[0],outputs[1],(long int)frames);
}

// -----------------------------------------------------------------------
// Callbacks

void DragonflyReverbPlugin::sampleRateChanged(double newSampleRate)
{
    early->setSampleRate(newSampleRate);
    late->setSampleRate(newSampleRate);

    // FIXME: Set sample rate on algorithms!
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new DragonflyReverbPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
