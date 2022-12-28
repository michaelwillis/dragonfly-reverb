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

#include "Plugin.hpp"
#include "DistrhoPluginInfo.h"
#include "DragonflyVersion.h"

START_NAMESPACE_DISTRHO

DragonflyReverbPlugin::DragonflyReverbPlugin() : Plugin(paramCount, 0, 1), dsp(getSampleRate()) {
  preset = DEFAULT_PRESET;
}

// -----------------------------------------------------------------------
// Init

void DragonflyReverbPlugin::initAudioPort(bool input, uint32_t index, AudioPort& port) {
  // treat audio ports as stereo
  port.groupId = kPortGroupStereo;

  // everything else is as default
  Plugin::initAudioPort(input, index, port);
}

void DragonflyReverbPlugin::initParameter(uint32_t index, Parameter& parameter) {
  if (index < paramCount) {
    parameter.hints      = kParameterIsAutomatable;
    parameter.name       = PARAMS[index].name;
    parameter.symbol     = PARAMS[index].symbol;
    parameter.ranges.min = PARAMS[index].range_min;
    parameter.ranges.def = presets[DEFAULT_PRESET].params[index];
    parameter.ranges.max = PARAMS[index].range_max;
    parameter.unit       = PARAMS[index].unit;
    if (index == paramAlgorithm) {
      parameter.hints   |= kParameterIsInteger;
      parameter.enumValues.count = ALGORITHM_COUNT;
      parameter.enumValues.restrictedMode = true;
      ParameterEnumerationValue* const values = new ParameterEnumerationValue[ALGORITHM_COUNT];
      parameter.enumValues.values = values;
      for (int i=0; i<ALGORITHM_COUNT; ++i) {
        values[i].label = algorithmNames[i];
        values[i].value = i;
      }
    }
  }
}

void DragonflyReverbPlugin::initState(uint32_t index, State& state) {
  if (index == 0) {
    state.key = "preset";
    state.defaultValue = "Clear Plate";
  }
}

// -----------------------------------------------------------------------
// Internal data

float DragonflyReverbPlugin::getParameterValue(uint32_t index) const {
  return dsp.getParameterValue(index);
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value) {
  dsp.setParameterValue(index, value);
}

void DragonflyReverbPlugin::setState(const char* key, const char* value) {
  if (std::strcmp(key, "preset") == 0) {
    for (int p = 0; p < NUM_PRESETS; p++) {
      if (std::strcmp(value, presets[p].name) == 0) {
        preset = p;
      }
    }
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
