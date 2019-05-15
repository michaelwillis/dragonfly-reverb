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

#include <cstdint>

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND "Dragonfly"
#define DISTRHO_PLUGIN_NAME  "Dragonfly Room Reverb"
#define DISTRHO_PLUGIN_URI   "urn:dragonfly:room"

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_STATE    1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

enum Parameters
{
    paramDry_level = 0,
    paramEarly_level,
    paramEarlySend,
    paramLate_level,
    paramOversample,
    paramWidth,
    paramPredelay,
    paramDecay,
    paramDiffuse,
    paramDampen,
    paramSpin,
    paramWander,
    paramBoost,
    paramBoostLPF,
    paramBoostBand,
    paramInputLPF,
    paramOutputLPF,
    paramCount
};

typedef struct {
  const char *name;
  const char *symbol;
  float range_min;
  float range_max;
  const char *unit;
} Param;

static Param params[paramCount] = {
  {"Dry Level",    "dry_level",      0.0f,   100.0f,   "%"},
  {"Early Level",  "early_level",    0.0f,   100.0f,   "%"},
  {"Early Send",   "early_send",     0.0f,   100.0f,   "%"},
  {"Late Level",   "late_level",     0.0f,   100.0f,   "%"},
  {"Oversample",   "oversample",     1.0f,     3.0f,   "x"},
  {"Width",        "width",         50.0f,   150.0f,   "%"},
  {"Predelay",     "predelay",       0.0f,   100.0f,  "ms"},
  {"Decay",        "decay",          0.1f,    10.0f,   "s"},
  {"Diffuse",      "diffuse",        0.0f,   100.0f,   "%"},
  {"Dampen",       "dampen",      1000.0f, 16000.0f,  "Hz"},
  {"Spin",         "spin",           0.0f,     5.0f,  "Hz"},
  {"Wander",       "wander",         0.0f,   100.0f,   "%"},
  {"Boost",        "boost",          0.0f,   100.0f,   "%"},
  {"Boost LPF",    "boost_freq",    50.0f,  1050.0f,  "Hz"},
  {"Boost Band",   "boost_band",     1.0f,     5.0f,   "x"},
  {"Input LPF",    "input_lpf",   1000.0f, 16000.0f,  "Hz"},
  {"Output LPF",   "output_lpf",  1000.0f, 16000.0f,  "Hz"}
};

const int NUM_BANKS = 5;
const int PRESETS_PER_BANK = 5;

typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;

typedef struct {
  const char *name;
  const Preset presets[PRESETS_PER_BANK];
} Bank;

static Bank banks[NUM_BANKS] = {
  {
    "Small", {               // dry, early, e. send, late, oversample, width, delay, decay, diffuse, damp lpf, spin, wander, bass boost, boost freq, boost band, in lpf, out lpf
      {"Small Bright Room",  { 80.0,  10.0,    20.0, 20.0,        1.0,  90.0,   4.0,   0.2,    60.0,    16000,  0.4,   40.0,       20.0,        600,        4.0,  16000,   16000}},
      {"Small Clear Room",   { 80.0,  10.0,    20.0, 20.0,        1.0,  90.0,   4.0,   0.2,    60.0,    12500,  0.8,   40.0,       40.0,        600,        4.0,  16000,   16000}},
      {"Small Dark Room",    { 80.0,  10.0,    20.0, 20.0,        1.0,  90.0,   4.0,   0.3,    70.0,     9000,  1.6,   20.0,       60.0,       1000,        4.0,  16000,   16000}},
      {"Small Drum Room",    { 80.0,  10.0,    20.0, 20.0,        1.0,  90.0,   8.0,   0.2,    24.0,     7000,  2.1,   10.0,       40.0,        400,        4.0,  16000,   16000}},
      {"Small Vocal Room",   { 80.0,  10.0,    20.0, 20.0,        2.0,  90.0,   0.0,   0.3,    86.0,     7000,  2.4,   12.0,       20.0,        400,        4.0,  16000,   16000}}
    }
  },
  {
    "Medium", {              // dry, early, e. send, late, oversample, width, delay, decay, diffuse, damp lpf, spin, wander, bass boost, boost freq
      {"Medium Bright Room", { 80.0,  10.0,    20.0, 20.0,        1.0, 100.0,   8.0,   0.4,    60.0,    14000,  0.4,   40.0,       25.0,        600,        4.0,  16000,   16000}},
      {"Medium Clear Room",  { 80.0,  10.0,    20.0, 20.0,        1.0, 100.0,   8.0,   0.4,    70.0,    12000,  0.8,   40.0,       50.0,        600,        4.0,  16000,   16000}},
      {"Medium Dark Room",   { 80.0,  10.0,    20.0, 20.0,        1.0, 100.0,   8.0,   0.5,    70.0,     8000,  1.6,   20.0,       70.0,       1000,        4.0,  16000,   16000}},
      {"Medium Drum Room",   { 80.0,  10.0,    20.0, 20.0,        1.0, 100.0,  12.0,   0.4,    32.0,     6000,  2.4,   10.0,       50.0,        300,        4.0,  16000,   16000}},
      {"Medium Vocal Room",  { 80.0,  10.0,    20.0, 20.0,        2.0, 100.0,   2.0,   0.6,    92.0,     6000,  2.7,   12.0,       25.0,        400,        4.0,  16000,   16000}}
    }
  },
  {
    "Large", {               // dry, early, e. send, late, oversample, width, delay, decay, diffuse, damp lpf, spin, wander, bass boost, boost freq
      {"Large Bright Room",  { 80.0,  10.0,    20.0, 20.0,        2.0, 100.0,  12.0,   0.7,    70.0,    14000,  0.4,   40.0,       30.0,        600,        4.0,  16000,   16000}},
      {"Large Clear Room",   { 80.0,  10.0,    20.0, 20.0,        2.0, 100.0,  12.0,   0.7,    80.0,    12000,  0.4,   40.0,       60.0,        600,        4.0,  16000,   16000}},
      {"Large Dark Room",    { 80.0,  10.0,    20.0, 20.0,        2.0, 100.0,  12.0,   0.8,    80.0,     7000,  1.6,   20.0,       80.0,       1000,        4.0,  16000,   16000}},
      {"Large Drum Room",    { 80.0,  10.0,    20.0, 20.0,        1.0, 100.0,  16.0,   0.7,    40.0,     5000,  2.7,   10.0,       60.0,        300,        4.0,  16000,   16000}},
      {"Large Vocal Room",   { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,   4.0,   1.0,    98.0,     5000,  3.1,   12.0,       30.0,        400,        4.0,  16000,   16000}}
    }
  },
  {
    "Halls", {               // dry, early, e. send, late, oversample, width, delay, decay, diffuse, damp lpf, spin, wander, bass boost, boost freq
      {"Bright Hall",        { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,  12.0,   1.6,    78.0,    14000,  0.4,   40.0,       30.0,        600,        4.0,  16000,   16000}},
      {"Clear Hall",         { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,  12.0,   1.6,    84.0,    12000,  0.4,   40.0,       60.0,        600,        4.0,  16000,   16000}},
      {"Dark Hall",          { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,  12.0,   1.9,    80.0,     6000,  1.6,   20.0,       80.0,       1000,        4.0,  16000,   16000}},
      {"Percussion Hall",    { 80.0,  10.0,    20.0, 20.0,        2.0, 100.0,  16.0,   1.5,    40.0,     5000,  2.7,   10.0,       60.0,        300,        4.0,  16000,   16000}},
      {"Vocal Hall",         { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,   4.0,   2.0,    98.0,     5000,  3.1,   12.0,       30.0,        400,        4.0,  16000,   16000}}
    }
  },
  {
    "Effects", {
      {"Bright Plate",       { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,   0.0,   1.8,    78.0,    15000,  1.6,   40.0,       10.0,       1000,        4.0,  16000,   16000}},
      {"Dark Plate",         { 80.0,  10.0,    20.0, 20.0,        3.0, 100.0,   0.0,   2.0,    78.0,     9000,  0.4,   20.0,       40.0,        500,        4.0,  16000,   16000}},
      {"Brick Wall",         { 80.0,  10.0,    20.0, 20.0,        1.0,  80.0,  80.0,   0.4,    12.0,    15000,  0.2,    5.0,       10.0,        200,        4.0,  16000,   16000}},
      {"Echo Chamber",       { 80.0,  10.0,    20.0, 20.0,        3.0, 120.0,  10.0,   4.0,    24.0,     9000,  2.3,   80.0,       20.0,        500,        4.0,  16000,   16000}},
      {"Long Tunnel",        { 80.0,  10.0,    20.0, 20.0,        1.0,  50.0,   0.0,   8.0,    90.0,     6000,  0.4,   10.0,       50.0,        500,        4.0,  16000,   16000}},
    }
  }
};

const int DEFAULT_BANK   = 2; // Medium Rooms
const int DEFAULT_PRESET = 1; // Second preset in each bank


#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
