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

#ifndef DISTRHO_PLUGIN_INFO_H_INCLUDED
#define DISTRHO_PLUGIN_INFO_H_INCLUDED

#define DISTRHO_PLUGIN_BRAND "Dragonfly"
#define DISTRHO_PLUGIN_NAME  "Dragonfly Reverb"
#define DISTRHO_PLUGIN_URI   "https://github.com/michaelwillis/dragonfly-reverb"

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_PROGRAMS 1
// #define DISTRHO_PLUGIN_WANT_STATE    1
// #define DISTRHO_UI_USE_NANOVG      1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

enum Parameters
{
    paramDry_level = 0,
    paramEarly_level, 
    paramLate_level,
    paramSize,
    paramPredelay,
    paramDiffuse,
    paramLow_cut,
    paramLow_xover,
    paramLow_mult,
    paramHigh_cut,
    paramHigh_xover,
    paramHigh_mult,
    paramCount
};

typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;


static Preset presets[24] = {
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
#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
