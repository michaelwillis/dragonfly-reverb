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

#define NUM_PRESETS 24

START_NAMESPACE_DISTRHO

DragonflyReverbPlugin::DragonflyReverbPlugin() : Plugin(paramCount, NUM_PRESETS, 0), dsp(getSampleRate(), false) {
  loadProgram(defaultPreset);
}

// -----------------------------------------------------------------------
// Init

void DragonflyReverbPlugin::initParameter(uint32_t index, Parameter& parameter) {
  if (index < paramCount) {
    parameter.hints      = kParameterIsAutomable;
    parameter.name       = params[index].name;
    parameter.symbol     = params[index].symbol;
    parameter.ranges.min = params[index].range_min;
    parameter.ranges.def = presets[defaultPreset].params[index];
    parameter.ranges.max = params[index].range_max;
    parameter.unit       = params[index].unit;
  }
}

void DragonflyReverbPlugin::initProgramName(uint32_t index, String& programName) {
  programName = presets[index].name;
}

// -----------------------------------------------------------------------
// Internal data

float DragonflyReverbPlugin::getParameterValue(uint32_t index) const {
  return dsp.getParameterValue(index);
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value) {
  dsp.setParameterValue(index, value);
}

void DragonflyReverbPlugin::loadProgram(uint32_t index) {
    const float *preset = presets[index].params;
    for (uint32_t param_index = 0; param_index < paramCount; param_index++) {
        setParameterValue(param_index, preset[param_index]);
    }
}

// -----------------------------------------------------------------------
// Process

void DragonflyReverbPlugin::activate() {
  sampleRateChanged(getSampleRate());
}

void DragonflyReverbPlugin::run(const float** inputs, float** outputs, uint32_t frames) {
  dsp.run(inputs, outputs, frames);
}

// -----------------------------------------------------------------------
// Callbacks

void DragonflyReverbPlugin::sampleRateChanged(double newSampleRate) {
  dsp.sampleRateChanged(newSampleRate);
}

// -----------------------------------------------------------------------

Plugin* createPlugin() {
    return new DragonflyReverbPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
