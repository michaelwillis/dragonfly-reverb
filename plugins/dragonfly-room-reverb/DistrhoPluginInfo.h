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

#define MAJOR_VERSION 1
#define MINOR_VERSION 1
#define PATCH_VERSION 4
#define VERSION_SUFFIX ""

enum Parameters
{
    paramDry_level = 0,
    paramEarly_level,
    paramLate_level,
    paramOversample,
    paramWidth,
    paramPredelay,
    paramDecay,
    paramDiffuse,
    paramDampenFreq,
    paramSpin,
    paramWander,
    paramBoostFreq,
    paramBoostFactor,
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
  {"Late Level",   "late_level",     0.0f,   100.0f,   "%"},
  {"Oversample",   "oversample",     1.0f,     5.0f,   "x"},
  {"Width",        "width",         50.0f,   150.0f,   "%"},
  {"Predelay",     "predelay",       0.0f,   100.0f,  "ms"},
  {"Decay",        "decay",          0.5f,     2.5f,   "s"},
  {"Diffuse",      "diffuse",        0.0f,   100.0f,   "%"},
  {"Dampen Freq",  "dampen_freq", 5000.0f, 15000.0f,  "Hz"},
  {"Spin",         "spin",           0.0f,    10.0f,  "Hz"},
  {"Wander",       "wander",         0.0f,    40.0f,  "ms"},
  {"Boost Freq",   "boost_freq",    50.0f,  1050.0f,  "Hz"},
  {"Boost",        "boost_factor",   0.0f,   100.0f,   "%"}
};

const int PRESET_COUNT = 12;

typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;


static Preset presets[PRESET_COUNT] = {
                         // dry, early, late, oversample, width, delay, decay, diffuse, damp lpf, spin, wander, bass lpf, bass boost
  {"Tiny Bright Room",   { 80.0,  10.0, 20.0,        2.0,  80.0,   4.0,   0.6,    78.0,    15000,  0.4,   40.0,      300,     20.0}},
  {"Small Bright Room",  { 80.0,  10.0, 20.0,        2.0,  90.0,   8.0,   0.7,    78.0,    15000,  0.4,   40.0,      300,     20.0}},
  {"Medium Bright Room", { 80.0,  10.0, 20.0,        2.0, 100.0,  12.0,   0.9,    78.0,    15000,  0.4,   40.0,      400,     10.0}},
  {"Large Bright Room",  { 80.0,  10.0, 20.0,        2.0, 110.0,  16.0,   1.2,    78.0,    15000,  0.4,   40.0,      400,     10.0}},
  {"Tiny Dark Room",     { 80.0,  10.0, 20.0,        2.0,  80.0,   4.0,   0.6,    78.0,    10000,  1.6,   40.0,     1000,     30.0}},
  {"Small Dark Room",    { 80.0,  10.0, 20.0,        2.0,  90.0,   8.0,   0.7,    78.0,     9000,  1.6,   20.0,     1000,     30.0}},
  {"Medium Dark Room",   { 80.0,  10.0, 20.0,        2.0, 100.0,  12.0,   0.9,    78.0,     8000,  1.6,   20.0,     1000,     25.0}},
  {"Large Dark Room",    { 80.0,  10.0, 20.0,        2.0, 110.0,  16.0,   1.2,    78.0,     6000,  1.6,   20.0,     1000,     25.0}},
  {"Bright Plate",       { 80.0,  10.0, 20.0,        3.0, 100.0,   0.0,   1.2,    78.0,    15000,  0.4,   40.0,     1000,     10.0}},
  {"Dark Plate",         { 80.0,  10.0, 20.0,        3.0, 100.0,   0.0,   1.6,    78.0,     8000,  1.6,   20.0,      500,     20.0}},
  {"Abrupt Slapback",    { 80.0,  10.0, 20.0,        1.0,  80.0,  20.0,   0.5,     0.0,    12000,  0.8,   10.0,      500,     10.0}},
  {"Echo Chamber",       { 80.0,  10.0, 20.0,        1.0,  50.0,  10.0,   2.5,    10.0,     9000,  2.3,   80.0,      500,     20.0}},
};

const int DEFAULT_PRESET = 1;

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
