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
#define DISTRHO_PLUGIN_NAME  "Dragonfly Early Reflections"
#define DISTRHO_PLUGIN_URI   "urn:dragonfly:early"

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_STATE    0

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

enum Parameters
{
    paramDry = 0,
    paramWet,
    paramProgram,
    paramSize,
    paramWidth,
    paramLowCut,
    paramHighCut,
    paramCount
};

static Param PARAMS[paramCount] = {
  {paramDry,        "Dry Level",    "dry_level",      0.0f,   100.0f,   "%"},
  {paramWet,        "Wet Level",    "early_level",    0.0f,   100.0f,   "%"},
  {paramProgram,    "Program",      "program",        0.0f,     6.0f,    ""},
  {paramSize,       "Size",         "size",          10.0f,    60.0f,   "m"},
  {paramWidth,      "Width",        "width",         50.0f,   150.0f,   "%"},
  {paramLowCut,     "Low Cut",      "low_cut",        0.0f,   200.0f,  "Hz"},
  {paramHighCut,    "High Cut",     "high_cut",    1000.0f, 16000.0f,  "Hz"}
};

const float DEFAULTS[paramCount] = {
  // dry,  wet, program, size, width, lowcut, highcut
    80.0, 20.0,       2, 20.0, 100.0,     50, 10000
};

const int PROGRAM_COUNT = 7;

typedef struct {
  const char *name;
  const int number; // freeverb's earlyref has numeric programs
} Program;

static Program programs[PROGRAM_COUNT] = {
  {"Abrupt Echo",     2},
  {"Backstage Pass", 18}, 
  {"Concert Venue",   0},
  {"Damaged Goods",  19},
  {"Elevator Pitch",  1},
  {"Floor Thirteen", 13},
  {"Garage Band",    14}
};

const int DEFAULT_PROGRAM = 2;


#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
