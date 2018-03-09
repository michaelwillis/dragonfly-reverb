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
#include <iostream>

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
  if (index < paramCount) {
    return newParams[index];
  }

  return 0.0;
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value)
{
  if (index < paramCount) {
    newParams[index] = value;
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
  for (uint32_t index = 0; index < paramCount; index++) {
    if (d_isNotEqual(oldParams[index], newParams[index])) {
      oldParams[index] = newParams[index];
      float value = newParams[index];

      switch(index) {
        case     paramDry_level: dry_level        = (value / 100.0); break;
        case   paramEarly_level: early_level      = (value / 100.0); break;
        case    paramLate_level: late_level       = (value / 100.0); break;
        case          paramSize: early.setRSFactor  (value / 50.0);
                                 late.setRSFactor   (value / 100.0);
                                 late.setrt60       (value / 20.0);  break;
        case      paramPredelay: late.setPreDelay   (value);         break;
        case       paramDiffuse: late.setidiffusion1(value / 140.0);
                                 late.setapfeedback (value / 140.0);
        case       paramLow_cut: early.setoutputhpf (value);
                                 late.setoutputhpf  (value);         break;
        case     paramLow_xover: late.setxover_low  (value);         break;
        case      paramLow_mult: late.setrt60_factor_low(value);     break;
        case      paramHigh_cut: early.setoutputlpf (value);
                                 late.setoutputlpf  (value);         break;
        case    paramHigh_xover: late.setxover_high (value);         break;
        case     paramHigh_mult: late.setrt60_factor_high(value);    break;
      }
    }
  }


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
