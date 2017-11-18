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

typedef struct {
  const char *name;
  const char *symbol;
  float range_min;
  float range_def;
  float range_max;
  const char *unit;
} Param;

typedef struct {
  const char *name;
  const float params[NUM_PARAMS];
} Preset;

static Param params[NUM_PARAMS] = {
  {"Dry Level",       "dry",          0.0f,   50.0f,   100.0f,   "%"},
  {"Early Level",     "e_lev",        0.0f,   50.0f,   100.0f,   "%"},
  {"Early Size",      "e_size",       1.0f,   10.0f,     5.0f,   "m"},
  {"Early Width",     "e_width",     10.0f,   40.0f,   100.0f,   "%"},
  {"Early Low Pass",  "e_lpf",     2000.0f, 7500.0f, 20000.0f,  "Hz"},
  {"Early Send",      "e_send",       0.0f,   20.0f,   100.0f,   "%"},
  {"Late Level",      "l_level",      0.0f,   50.0f,   100.0f,   "%"},
  {"Late Predelay",   "l_delay",      0.0f,   14.0f,   100.0f,  "ms"},
  {"Late Decay Time", "l_time",       0.1f,    2.0f,    10.0f, "sec"},
  {"Late Size",       "l_size",      10.0f,   40.0f,   100.0f,   "m"},
  {"Late Width",      "l_width",     10.0f,   90.0f,   100.0f,   "%"},
  {"Late Low Pass",   "l_lpf",     2000.0f, 7500.0f, 20000.0f,  "Hz"},
  {"Diffuse",         "diffuse",      0.0f,   80.0f,   100.0f,   "%"},
  {"Low Crossover",   "lo_xo",      100.0f,  600.0f,  1000.0f,  "Hz"},
  {"Low Decay Mult",  "lo_mult",      0.1f,    1.5f,     2.0f,   "X"},
  {"High Crossover",  "hi_xo",     1000.0f, 4500.0f, 20000.0f,  "Hz"},
  {"High Decay Mult", "hi_mult",      0.1f,    0.4f,     1.0f,   "X"},
  {"Spin",            "spin",         0.1f,    3.0f,    10.0f,  "Hz"},
  {"Wander",          "wander",       0.0f,   15.0f,    50.0f,  "ms"}
};

static Preset presets[NUM_PRESETS] = {
  //                 dry, e_lev, e_size, e_width, e_lpf, e_send, l_level, l_delay, l_time, l_size, l_width, l_lpf, diffuse, lo_xo, lo_mult, hi_xo, hi_mult, spin, wander
  {"Medium Hall", { 50.0,  50.0,    5.0,    40.0,  7500,   20.0,    50.0,    15.0,    1.7,   30.0,   100.0,  5500,    90.0,   600,     1.5,  4500,    0.35,  3.0, 15.0}},
  {"Large Hall",  { 50.0,  50.0,    6.0,    80.0,  5000,   30.0,    50.0,    20.0,    2.8,   40.0,   100.0,  6500,    90.0,   600,     2.4,  4500,    0.35,  2.0, 20.0}}
};

// -----------------------------------------------------------------------

DragonflyReverbPlugin::DragonflyReverbPlugin() : Plugin(NUM_PARAMS, NUM_PRESETS, 0) // 0 states
{
    early.loadPresetReflection(FV3_EARLYREF_PRESET_1);
    early.setMuteOnChange(true);
    early.setdryr(0); // mute dry signal
    early.setwet(0); // 0dB
    early.setLRDelay(0.3);
    early.setLRCrossApFreq(750, 4);
    early.setDiffusionApFreq(150, 4);
    early.setSampleRate(getSampleRate());
    early.setoutputhpf(4.0);

    late.setMuteOnChange(true);
    late.setwet(0); // 0dB
    late.setdryr(0); // mute dry signal
    late.setSampleRate(getSampleRate());
    late.setoutputhpf(4.0);
    late.setlfo1freq(0.9);
    late.setlfo2freq(1.3);
    late.setlfofactor(0.3);
    late.setspinfactor(0.3);

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
  programName = presets[index].name;
}

// -----------------------------------------------------------------------
// Internal data

float DragonflyReverbPlugin::getParameterValue(uint32_t index) const
{
    switch(index) {
      case  0: return dry_level * 100.0;
      case  1: return early_level * 100.0;
      case  2: return early.getRSFactor() * 7.0;
      case  3: return early.getwidth() * 100.0;
      case  4: return early.getoutputlpf();
      case  5: return early_send * 100.0;
      case  6: return late_level * 100.0;
      case  7: return late.getPreDelay();
      case  8: return late.getrt60();
      case  9: return late.getRSFactor() * 80.0;
      case 10: return late.getwidth() * 100.0;
      case 11: return late.getoutputlpf();
      case 12: return late.getidiffusion1() * 100.0;
      case 13: return late.getxover_low();
      case 14: return late.getrt60_factor_low();
      case 15: return late.getxover_high();
      case 16: return late.getrt60_factor_high();
      case 17: return late.getspin();
      case 18: return late.getwander();
    }

    return 0.0;
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value)
{
    switch(index) {
      case  0:     dry_level    = (value / 100.0); break;
      case  1:   early_level    = (value / 100.0); break;
      case  2: early.setRSFactor  (value / 7.0);   break;
      case  3: early.setwidth     (value / 100.0); break;
      case  4: early.setoutputlpf(value);          break;
      case  5:    early_send    = (value / 100.0); break;
      case  6:    late_level    = (value / 100.0); break;
      case  7: late.setPreDelay   (value);         break;
      case  8: late.setrt60       (value);         break;
      case  9: late.setRSFactor   (value / 80.0);  break;
      case 10: late.setwidth      (value / 100.0); break;
      case 11: late.setoutputlpf  (value);         break;
      case 12:
        late.setidiffusion1(value / 100.0 * 0.75);
        late.setapfeedback(value / 100.0 * 0.75);
        break;
      case 13: late.setxover_low  (value);         break;
      case 14: late.setrt60_factor_low(value);     break;
      case 15: late.setxover_high (value);         break;
      case 16: late.setrt60_factor_high(value);    break;
      case 17: late.setspin       (value);         break;
      case 18: late.setwander     (value);         break;
    }
}

void DragonflyReverbPlugin::loadProgram(uint32_t index)
{
    const float *preset = presets[index].params;
    for (uint32_t param_index = 0; param_index < NUM_PARAMS; param_index++)
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
