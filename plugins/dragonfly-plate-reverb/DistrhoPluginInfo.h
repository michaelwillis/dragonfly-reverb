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
#include <Param.hpp>

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND "Dragonfly"
#define DISTRHO_PLUGIN_NAME  "Dragonfly Plate Reverb"
#define DISTRHO_PLUGIN_URI   "urn:dragonfly:plate"

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_STATE    1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"


enum Algorithms {
  ALGORITHM_NREV = 0,
  ALGORITHM_NREV_B,
  ALGORITHM_STREV,
  ALGORITHM_COUNT
};

static char *algorithmNames[ALGORITHM_COUNT] = {
  "Simple",
  "Nested",
  "Tank"
};

enum Parameters
{
    paramDry = 0,
    paramWet,

    paramAlgorithm,

    paramWidth,
    paramPredelay,
    paramDecay,

    paramDiffuse,
    paramSpin,
    paramWander,

    paramLowCut,
    paramHighCut,
    paramDamp,

    paramCount
};

static Param PARAMS[paramCount] = {
  {paramDry,        "Dry Level",    "dry_level",      0.0f,   100.0f,   "%"},
  {paramWet,        "Wet Level",    "early_level",    0.0f,   100.0f,   "%"},
  {paramAlgorithm,  "Algorithm",    "algorithm",      0.0f,     2.0f,    ""},  
  {paramWidth,      "Width",        "width",         50.0f,   150.0f,   "%"},
  {paramPredelay,   "Predelay",     "predelay",       0.0f,   100.0f,  "ms"},
  {paramDecay,      "Decay",        "decay",          0.1f,    10.0f,   "s"},
  {paramDiffuse,    "Diffuse",      "diffuse",        0.0f,   100.0f,   "%"},
  {paramSpin,       "Spin",         "spin",           0.0f,     5.0f,  "Hz"},
  {paramWander,     "Wander",       "wander",         0.0f,   100.0f,   "%"},
  {paramLowCut,     "Low Cut",      "low_cut",        0.0f,   200.0f,  "Hz"},
  {paramHighCut,    "High Cut",     "high_cut",    1000.0f, 16000.0f,  "Hz"},
  {paramDamp,       "Dampen",       "early_damp",  1000.0f, 16000.0f,  "Hz"}
};

const int NUM_PRESETS = 8;

typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;

static Preset presets[NUM_PRESETS] = {
                  //  dry,  wet,        algorithm, width, delay, decay, diffuse, spin, wander, low cut, high cut,  damp
  {"Abrupt Plate", { 80.0, 20.0, ALGORITHM_NREV_B,   100,  20.0,   0.2,    90.0,  1.0,   60.0,      50,    10000,  7000 } },
  {"Bright Plate", { 80.0, 20.0, ALGORITHM_NREV_B,   100,   0.0,   0.4,    90.0,  1.0,   60.0,     200,    16000, 13000 } },
  {"Clear Plate",  { 80.0, 20.0, ALGORITHM_NREV_B,   100,   0.0,   0.6,    90.0,  1.0,   50.0,     100,    13000,  7000 } },
  {"Dark Plate",   { 80.0, 20.0, ALGORITHM_NREV_B,   100,   0.0,   0.8,    90.0,  0.8,   40.0,      50,     7000,  4000 } },
  {"Foil Tray",    { 80.0, 20.0,   ALGORITHM_NREV,    50,   0.0,   0.3,    90.0,  1.0,   60.0,     200,    16000, 13000 } },
  {"Metal Roof",   { 80.0, 20.0,   ALGORITHM_NREV,   120,  20.0,   0.5,    90.0,  1.0,   60.0,     100,    13000, 10000 } },
  {"Narrow Tank",  { 80.0, 20.0,  ALGORITHM_STREV,    60,  10.0,   0.6,    90.0,  2.0,   30.0,      50,    10000,  7000 } },
  {"Phat Tank",    { 80.0, 20.0,  ALGORITHM_STREV,   150,  10.0,   1.0,    90.0,  0.6,   60.0,      50,    10000,  4000 } }
};

const int DEFAULT_PRESET = 1;


#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
