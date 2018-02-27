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

#include "DragonflyReverbPlugin.hpp"
#include "DistrhoPluginInfo.h"

// #define NUM_PARAMS 12
#define NUM_PRESETS 24
#define PARAM_NAME_LENGTH 16
#define PARAM_SYMBOL_LENGTH 8
#define PRESET_NAME_LENGTH 24

START_NAMESPACE_DISTRHO

typedef struct {
  const char *name;
  const char *symbol;
  float range_min;
  float range_def;
  float range_max;
  const char *unit;
} Param;

/*
typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;
*/

// enum {
//   , EARLY_LEVEL, LATE_LEVEL,
//   SIZE, PREDELAY, DIFFUSE,
//   LOW_CUT, LOW_XOVER, LOW_MULT,
//   HIGH_CUT, HIGH_XOVER, HIGH_MULT
// };

static Param params[paramCount] = {
  {"Dry Level",       "dry_level",    0.0f,   50.0f,   100.0f,   "%"},
  {"Early Level",     "early_level",  0.0f,   50.0f,   100.0f,   "%"},
  {"Late Level",      "late_level",   0.0f,   50.0f,   100.0f,   "%"},
  {"Size",            "size",         8.0f,   40.0f,   100.0f,   "m"},
  {"Predelay",        "delay",        0.0f,   14.0f,   100.0f,  "ms"},
  {"Diffuse",         "diffuse",      0.0f,   80.0f,   100.0f,   "%"},
  {"Low Cut",         "low_cut",      0.0f,    4.0f,   100.0f,  "Hz"},
  {"Low Crossover",   "low_xo",     100.0f,  600.0f,  1000.0f,  "Hz"},
  {"Low Decay Mult",  "low_mult",     0.1f,    1.5f,     4.0f,   "X"},
  {"High Cut",        "high_cut",  2000.0f, 7500.0f, 20000.0f,  "Hz"},
  {"High Crossover",  "high_xo",   1000.0f, 4500.0f, 20000.0f,  "Hz"},
  {"High Decay Mult", "high_mult",    0.1f,    0.4f,     2.0f,   "X"}
};

/*
static Preset presets[NUM_PRESETS] = {
                             // dry, e_lev, l_lev, size, delay, diffuse, low_cut, low_xo, low_mult, high_cut, high_xo, high_mult
  {"Bright Room",            { 80.0,  10.0,  10.0,  8.0,   4.0,    90.0,     4.0,    500,     0.80,    20000,    8000,   0.75}},
  {"Clear Room",             { 80.0,  10.0,  10.0,  8.0,   4.0,    90.0,     4.0,    500,     0.90,    16000,    6000,   0.50}},
  {"Dark Room",              { 80.0,  10.0,  10.0,  8.0,   4.0,    50.0,     4.0,    500,     1.20,     7000,    5000,   0.35}},
  {"Acoustic Studio",        { 85.0,   5.0,  10.0, 10.0,   8.0,    75.0,     4.0,    450,     1.50,     7500,    5000,   0.80}},
  {"Electric Studio",        { 85.0,   5.0,  10.0, 12.0,   6.0,    45.0,     4.0,    250,     1.25,     7500,    6000,   0.70}},
  {"Percussion Studio",      { 90.0,   0.0,  10.0, 12.0,   6.0,    20.0,    20.0,    200,     1.75,     6000,    5000,   0.45}},
  {"Vocal Studio",           { 90.0,   0.0,  10.0, 12.0,   0.0,    60.0,     4.0,    400,     1.20,     6000,    5000,   0.40}},
  {"Small Chamber",          { 75.0,  10.0,  15.0, 16.0,   8.0,    70.0,     4.0,    500,     1.10,     8000,    5500,   0.35}},
  {"Large Chamber",          { 75.0,  10.0,  15.0, 20.0,   8.0,    90.0,     4.0,    500,     1.30,     7000,    5000,   0.25}},
  {"Small Bright Hall",      { 75.0,  10.0,  15.0, 24.0,  12.0,    90.0,     4.0,    400,     1.10,    12000,    6000,   0.75}},
  {"Small Clear Hall",       { 75.0,  10.0,  15.0, 20.0,   4.0,    90.0,     4.0,    500,     1.30,     7500,    5500,   0.50}},
  {"Small Dark Hall",        { 75.0,  10.0,  15.0, 24.0,  12.0,    60.0,     4.0,    500,     1.50,     6000,    4000,   0.35}},
  {"Small Percussion Hall",  { 85.0,   0.0,  15.0, 24.0,  12.0,    30.0,    20.0,    250,     2.00,     5000,    4000,   0.35}},
  {"Small Vocal Hall",       { 85.0,   0.0,  15.0, 20.0,   4.0,    60.0,     4.0,    500,     1.25,     6000,    5000,   0.35}},
  {"Medium Bright Hall",     { 75.0,   5.0,  20.0, 36.0,  18.0,    90.0,     4.0,    400,     1.25,    10000,    6000,   0.60}},
  {"Medium Clear Hall",      { 75.0,   5.0,  20.0, 30.0,   8.0,    90.0,     4.0,    500,     1.50,     7500,    5500,   0.50}},
  {"Medium Dark Hall",       { 75.0,   5.0,  20.0, 36.0,  18.0,    60.0,     4.0,    500,     1.75,     6000,    4000,   0.40}},
  {"Medium Vocal Hall",      { 80.0,   0.0,  20.0, 30.0,   8.0,    75.0,     4.0,    600,     1.50,     6000,    5000,   0.40}},
  {"Large Bright Hall",      { 75.0,   0.0,  25.0, 48.0,  20.0,    90.0,     4.0,    400,     1.50,     8000,    6000,   0.50}},
  {"Large Clear Hall",       { 75.0,   0.0,  25.0, 40.0,  12.0,    80.0,     4.0,    550,     2.00,     8000,    5000,   0.40}},
  {"Large Dark Hall",        { 75.0,   0.0,  25.0, 48.0,  20.0,    60.0,     4.0,    600,     2.50,     6000,    3200,   0.20}},
  {"Large Vocal Hall",       { 75.0,   0.0,  25.0, 40.0,  12.0,    80.0,     4.0,    700,     2.25,     6000,    4500,   0.30}},
  {"Great Hall",             { 70.0,   0.0,  30.0, 60.0,  20.0,    95.0,     4.0,    750,     3.00,     5500,    4000,   0.30}},
  {"Cathedral",              { 70.0,   0.0,  30.0, 72.0,  24.0,    90.0,     4.0,    850,     2.25,     6000,    3200,   0.30}}
};
*/

// -----------------------------------------------------------------------

DragonflyReverbPlugin::DragonflyReverbPlugin() : Plugin(paramCount, NUM_PRESETS, 0) // 0 states
{
    early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    early.setMuteOnChange(false);
    early.setdryr(0); // mute dry signal
    early.setwet(0); // 0dB
    early.setwidth(0.8);
    early.setLRDelay(0.3);
    early.setLRCrossApFreq(750, 4);
    early.setDiffusionApFreq(150, 4);
    early.setSampleRate(getSampleRate());

    early_send = 0.20;

    late.setMuteOnChange(false);
    late.setwet(0); // 0dB
    late.setdryr(0); // mute dry signal
    late.setwidth(1.0);
    late.setSampleRate(getSampleRate());

    // set initial values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void DragonflyReverbPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    if (index < paramCount)
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
  programName = presets[index].name;
}

// -----------------------------------------------------------------------
// Internal data

float DragonflyReverbPlugin::getParameterValue(uint32_t index) const
{
    switch(index) {
      case     paramDry_level: return dry_level * 100.0;
      case   paramEarly_level: return early_level * 100.0;
      case    paramLate_level: return late_level * 100.0;
      case          paramSize: return size;
      case      paramPredelay: return delay;
      case       paramDiffuse: return diffuse;
      case       paramLow_cut: return low_cut;
      case     paramLow_xover: return low_xover;
      case      paramLow_mult: return low_mult;
      case      paramHigh_cut: return late.getoutputlpf();
      case    paramHigh_xover: return high_xover;
      case     paramHigh_mult: return high_mult;
    }

    return 0.0;
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value)
{
    switch(index) {
      case     paramDry_level: dry_level        = (value / 100.0); break;
      case   paramEarly_level: early_level      = (value / 100.0); break;
      case    paramLate_level: late_level       = (value / 100.0); break;
      case          paramSize: size             = (value);
                          early.setRSFactor  (value / 50.0);
                          late.setRSFactor   (value / 100.0);
                          late.setrt60       (value / 20.0);  break;
      case      paramPredelay: delay            = (value);
                          late.setPreDelay   (value);         break;
      case       paramDiffuse: diffuse          = (value);
                          late.setidiffusion1(value / 140.0);
                          late.setapfeedback (value / 140.0);
      case       paramLow_cut: low_cut          = (value);
                          early.setoutputhpf (value);
                          late.setoutputhpf  (value);         break;
      case     paramLow_xover: low_xover        = (value);
                          late.setxover_low  (value);         break;
      case      paramLow_mult: low_mult         = (value);
                          late.setrt60_factor_low(value);     break;
      case      paramHigh_cut: high_cut         = (value);
                          early.setoutputlpf (value);
                          late.setoutputlpf  (value);         break;
      case    paramHigh_xover: high_xover       = (value);
                          late.setxover_high (value);         break;
      case     paramHigh_mult: high_mult        = (value);
                          late.setrt60_factor_high(value);    break;
    }
}

void DragonflyReverbPlugin::loadProgram(uint32_t index)
{
    const float *preset = presets[index].params;
    for (uint32_t param_index = 0; param_index < paramCount; param_index++)
    {
        setParameterValue(param_index, preset[param_index]);
    }
}

// -----------------------------------------------------------------------
// Process

void DragonflyReverbPlugin::activate()
{
  early.setSampleRate(getSampleRate());
  late.setSampleRate(getSampleRate());
}

void DragonflyReverbPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
  for (uint32_t offset = 0; offset < frames; offset += BUFFER_SIZE) {
    long int buffer_frames = frames - offset < BUFFER_SIZE ? frames - offset : BUFFER_SIZE;

    early.processreplace(
        const_cast<float *>(inputs[0] + offset),
        const_cast<float *>(inputs[1] + offset),
        early_out_buffer[0],
        early_out_buffer[1],
        buffer_frames
    );

    for (uint32_t i = 0; i < buffer_frames; i++) {
      late_in_buffer[0][i] = early_send * early_out_buffer[0][i] + inputs[0][offset + i];
      late_in_buffer[1][i] = early_send * early_out_buffer[1][i] + inputs[1][offset + i];
    }

    late.processreplace(
      const_cast<float *>(late_in_buffer[0]),
      const_cast<float *>(late_in_buffer[1]),
      late_out_buffer[0],
      late_out_buffer[1],
      buffer_frames
    );

    for (uint32_t i = 0; i < buffer_frames; i++) {
      outputs[0][offset + i] =
        dry_level   * inputs[0][offset + i]  +
        early_level * early_out_buffer[0][i] +
        late_level  * late_out_buffer[0][i];

      outputs[1][offset + i] =
        dry_level   * inputs[1][offset + i]  +
        early_level * early_out_buffer[1][i] +
        late_level  * late_out_buffer[1][i];
    }
  }
}

// -----------------------------------------------------------------------
// Callbacks

void DragonflyReverbPlugin::sampleRateChanged(double newSampleRate)
{
    early.setSampleRate(newSampleRate);
    late.setSampleRate(newSampleRate);
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new DragonflyReverbPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
