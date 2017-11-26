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
#define NUM_PRESETS 7
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

enum {
  DRY_LEVEL, EARLY_LEVEL, EARLY_SIZE, EARLY_WIDTH, EARLY_LPF, EARLY_SEND,
  LATE_LEVEL, LATE_PREDELAY, LATE_DECAY, LATE_SIZE, LATE_WIDTH, LATE_LPF,
  DIFFUSE, LOW_XOVER, LOW_MULT, HIGH_XOVER, HIGH_MULT, SPIN, WANDER
};

static Param params[NUM_PARAMS] = {
  {"Dry Level",       "dry",          0.0f,   50.0f,   100.0f,   "%"},
  {"Early Level",     "e_lev",        0.0f,   50.0f,   100.0f,   "%"},
  {"Early Size",      "e_size",       1.0f,    5.0f,    10.0f,   "m"},
  {"Early Width",     "e_width",     10.0f,   40.0f,   200.0f,   "%"},
  {"Early Low Pass",  "e_lpf",     2000.0f, 7500.0f, 20000.0f,  "Hz"},
  {"Early Send",      "e_send",       0.0f,   20.0f,   100.0f,   "%"},
  {"Late Level",      "l_level",      0.0f,   50.0f,   100.0f,   "%"},
  {"Late Predelay",   "l_delay",      0.0f,   14.0f,   100.0f,  "ms"},
  {"Late Decay Time", "l_time",       0.1f,    2.0f,    10.0f, "sec"},
  {"Late Size",       "l_size",      10.0f,   40.0f,   100.0f,   "m"},
  {"Late Width",      "l_width",     10.0f,   90.0f,   200.0f,   "%"},
  {"Late Low Pass",   "l_lpf",     2000.0f, 7500.0f, 20000.0f,  "Hz"},
  {"Diffuse",         "diffuse",      0.0f,   80.0f,   100.0f,   "%"},
  {"Low Crossover",   "lo_xo",      100.0f,  600.0f,  1000.0f,  "Hz"},
  {"Low Decay Mult",  "lo_mult",      0.1f,    1.5f,     4.0f,   "X"},
  {"High Crossover",  "hi_xo",     1000.0f, 4500.0f, 20000.0f,  "Hz"},
  {"High Decay Mult", "hi_mult",      0.1f,    0.4f,     2.0f,   "X"},
  {"Spin",            "spin",         0.1f,    3.0f,     5.0f,  "Hz"},
  {"Wander",          "wander",       0.0f,   15.0f,    30.0f,  "ms"}
};

static Preset presets[NUM_PRESETS] = {
  //                 dry, e_lev, e_size, e_width, e_lpf, e_send, l_level, l_delay, l_time, l_size, l_width, l_lpf, diffuse, lo_xo, lo_mult, hi_xo, hi_mult, spin, wander
  {"Room",        { 50.0,  50.0,    3.0,    80.0, 18000,   40.0,    50.0,     5.0,    0.4,   10.0,   100.0, 12000,    75.0,   400,     0.9,  6000,    0.90,  0.3, 27.0}},
  {"Studio",      { 50.0,  50.0,    5.0,    90.0,  6000,   20.0,    50.0,     5.0,    0.8,   12.0,    90.0,  6000,    50.0,   200,     1.2,  5000,    0.80,  2.5,  7.0}},
  {"Chamber",     { 50.0,  50.0,    5.0,    40.0,  9000,   60.0,    50.0,    12.0,    1.2,   16.0,    60.0,  8000,    80.0,   500,     1.5,  5500,    0.30,  3.3, 15.0}},
  {"Small Hall",  { 50.0,  50.0,    3.0,    40.0,  6000,   20.0,    50.0,    12.0,    1.3,   18.0,   100.0,  5500,    90.0,   500,     1.2,  4000,    0.35,  2.5, 13.0}},
  {"Medium Hall", { 50.0,  50.0,    5.0,    60.0,  7500,   20.0,    50.0,    15.0,    1.7,   30.0,   120.0,  6500,    90.0,   600,     1.5,  4500,    0.40,  2.8, 16.0}},
  {"Large Hall",  { 50.0,  50.0,    6.0,    80.0,  5000,   20.0,    50.0,    20.0,    3.2,   42.0,   100.0,  7500,    85.0,   600,     2.4,  4500,    0.30,  2.9, 22.0}},
  {"Cathedral",   { 50.0,  50.0,    8.0,   120.0,  5000,   60.0,    50.0,    30.0,    4.8,   58.0,   200.0,  6500,    85.0,   850,     3.0,  4000,    0.60,  3.1, 18.0}}
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
      case     DRY_LEVEL: return dry_level * 100.0;
      case   EARLY_LEVEL: return early_level * 100.0;
      case    EARLY_SIZE: return early.getRSFactor() * 7.0;
      case   EARLY_WIDTH: return early.getwidth() * 100.0;
      case     EARLY_LPF: return early.getoutputlpf();
      case    EARLY_SEND: return early_send * 100.0;
      case    LATE_LEVEL: return late_level * 100.0;
      case LATE_PREDELAY: return late.getPreDelay();
      case    LATE_DECAY: return late.getrt60();
      case     LATE_SIZE: return late.getRSFactor() * 80.0;
      case    LATE_WIDTH: return late.getwidth() * 100.0;
      case      LATE_LPF: return late.getoutputlpf();
      case       DIFFUSE: return late.getidiffusion1() * 100.0;
      case     LOW_XOVER: return late.getxover_low();
      case      LOW_MULT: return late.getrt60_factor_low();
      case    HIGH_XOVER: return late.getxover_high();
      case     HIGH_MULT: return late.getrt60_factor_high();
      case          SPIN: return late.getspin();
      case        WANDER: return late.getwander();
    }

    return 0.0;
}

void DragonflyReverbPlugin::setParameterValue(uint32_t index, float value)
{
    switch(index) {
      case     DRY_LEVEL: dry_level        = (value / 100.0); break;
      case   EARLY_LEVEL: early_level      = (value / 100.0); break;
      case    EARLY_SIZE: early.setRSFactor  (value / 7.0);   break;
      case   EARLY_WIDTH: early.setwidth     (value / 100.0); break;
      case     EARLY_LPF: early.setoutputlpf (value);         break;
      case    EARLY_SEND: early_send       = (value / 100.0); break;
      case    LATE_LEVEL: late_level       = (value / 100.0); break;
      case LATE_PREDELAY: late.setPreDelay   (value);         break;
      case    LATE_DECAY: late.setrt60       (value);         break;
      case     LATE_SIZE: late.setRSFactor   (value / 80.0);  break;
      case    LATE_WIDTH: late.setwidth      (value / 100.0); break;
      case      LATE_LPF: late.setoutputlpf  (value);         break;
      case       DIFFUSE: late.setidiffusion1(value / 100.0 * 0.75);
                          late.setapfeedback (value / 100.0 * 0.75);
                                                              break;
      case     LOW_XOVER: late.setxover_low  (value);         break;
      case      LOW_MULT: late.setrt60_factor_low(value);     break;
      case    HIGH_XOVER: late.setxover_high (value);         break;
      case     HIGH_MULT: late.setrt60_factor_high(value);    break;
      case          SPIN: late.setspin       (value);         break;
      case        WANDER: late.setwander     (value);         break;
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
