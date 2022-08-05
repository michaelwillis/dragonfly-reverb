/*
 * Dragonfly Reverb, a hall-style reverb plugin
 * Copyright (c) 2018 Michael Willis, Rob van den Berg
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
#define DISTRHO_PLUGIN_NAME  "Dragonfly Hall Reverb"
#define DISTRHO_PLUGIN_URI   "https://github.com/michaelwillis/dragonfly-reverb"

#define DISTRHO_PLUGIN_HAS_UI          1
#define DISTRHO_PLUGIN_IS_RT_SAFE      1
#define DISTRHO_PLUGIN_NUM_INPUTS      2
#define DISTRHO_PLUGIN_NUM_OUTPUTS     2
#define DISTRHO_PLUGIN_WANT_STATE      1
#define DISTRHO_PLUGIN_WANT_FULL_STATE 1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"

enum Parameters
{
    paramDry = 0,
    paramEarly,
    paramLate,
    paramSize,
    paramWidth,
    paramPredelay,
    paramDiffuse,
    paramLowCut,
    paramLowXover,
    paramLowMult,
    paramHighCut,
    paramHighXover,
    paramHighMult,
    paramSpin,
    paramWander,
    paramDecay,
    paramEarlySend,
    paramModulation,
    paramCount
};

static const Param PARAMS[paramCount] = {
  {paramDry,        "Dry Level",   "dry_level",    0.0f,   100.0f,   "%"},
  {paramEarly,      "Early Level", "early_level",  0.0f,   100.0f,   "%"},
  {paramLate,       "Late Level",  "late_level",   0.0f,   100.0f,   "%"},
  {paramSize,       "Size",        "size",        10.0f,    60.0f,   "m"},
  {paramWidth,      "Width",       "width",       50.0f,   150.0f,   "%"},
  {paramPredelay,   "Predelay",    "delay",        0.0f,   100.0f,  "ms"},
  {paramDiffuse,    "Diffuse",     "diffuse",      0.0f,   100.0f,   "%"},
  {paramLowCut,     "Low Cut",     "low_cut",      0.0f,   200.0f,  "Hz"},
  {paramLowXover,   "Low Cross",   "low_xo",     200.0f,  1200.0f,  "Hz"},
  {paramLowMult,    "Low Mult",    "low_mult",     0.5f,     2.5f,   "X"},
  {paramHighCut,    "High Cut",    "high_cut",  1000.0f, 16000.0f,  "Hz"},
  {paramHighXover,  "High Cross",  "high_xo",   1000.0f, 16000.0f,  "Hz"},
  {paramHighMult,   "High Mult",   "high_mult",    0.2f,     1.2f,   "X"},
  {paramSpin,       "Spin",        "spin",         0.0f,    10.0f,  "Hz"},
  {paramWander,     "Wander",      "wander",       0.0f,    40.0f,  "ms"},
  {paramDecay,      "Decay",       "decay",        0.1f,    10.0f,   "s"},
  {paramEarlySend,  "Early Send",  "early_send",   0.0f,   100.0f,   "%"},
  {paramModulation, "Modulation",  "modulation",   0.0f,   100.0f,   "%"}
};

static const int NUM_BANKS = 5;
static const int PRESETS_PER_BANK = 5;

typedef struct {
  const char *name;
  const float params[paramCount];
} Preset;

typedef struct {
  const char *name;
  const Preset presets[PRESETS_PER_BANK];
} Bank;

static const Bank banks[NUM_BANKS] = {
  {
    "Rooms", {                   // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Bright Room",            { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    90.0,     4.0,    500,     0.80,    16000,    7900,      0.75,  1.0, 25.0,     0.6,    20.0,    30.0 }},
      {"Clear Room",             { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    90.0,     4.0,    500,     0.90,    13000,    5800,      0.50,  1.0, 25.0,     0.6,    20.0,    30.0 }},
      {"Dark Room",              { 80.0,  10.0, 20.0, 10.0,  90.0,   4.0,    50.0,     4.0,    500,     1.20,     7300,    4900,      0.35,  1.0, 25.0,     0.7,    20.0,    30.0 }},
      {"Small Chamber",          { 80.0,  10.0, 20.0, 16.0,  80.0,   8.0,    70.0,     4.0,    500,     1.10,     8200,    5500,      0.35,  1.2, 10.0,     0.8,    20.0,    20.0 }},
      {"Large Chamber",          { 80.0,  10.0, 20.0, 20.0,  80.0,   8.0,    90.0,     4.0,    500,     1.30,     7000,    4900,      0.25,  1.8, 12.0,     1.0,    20.0,    20.0 }},
    }
  },
  {
    "Studios", {                 // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Acoustic Studio",        { 80.0,  10.0, 20.0, 12.0,  90.0,   8.0,    75.0,     4.0,    450,     1.50,     7600,    4900,      0.80,  2.5,  7.0,     0.8,    20.0,    20.0 }},
      {"Electric Studio",        { 80.0,  10.0, 20.0, 12.0,  90.0,   6.0,    45.0,     4.0,    250,     1.25,     7600,    5800,      0.70,  2.5,  7.0,     0.9,    20.0,    30.0 }},
      {"Percussion Studio",      { 80.0,  10.0, 20.0, 12.0,  90.0,   6.0,    30.0,    20.0,    200,     1.75,     5800,    5200,      0.45,  2.5,  7.0,     0.7,    20.0,    10.0 }},
      {"Piano Studio",           { 80.0,  10.0, 20.0, 12.0,  80.0,   8.0,    40.0,    20.0,    600,     1.50,     8200,    5800,      0.50,  2.8, 10.0,     0.7,    20.0,    15.0 }},
      {"Vocal Studio",           { 80.0,  10.0, 20.0, 12.0,  90.0,   0.0,    60.0,     4.0,    400,     1.20,     5800,    5200,      0.40,  2.5,  7.0,     0.8,    20.0,    10.0 }},
    }
  },
  {
    "Small Halls", {             // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Small Bright Hall",      { 80.0,  10.0, 20.0, 24.0,  80.0,  12.0,    90.0,     4.0,    400,     1.10,    11200,    6250,      0.75,  2.5, 13.0,     1.3,    20.0,    15.0 }},
      {"Small Clear Hall",       { 80.0,  10.0, 20.0, 24.0, 100.0,   4.0,    90.0,     4.0,    500,     1.30,     7600,    5500,      0.50,  3.3, 15.0,     1.3,    20.0,    15.0 }},
      {"Small Dark Hall",        { 80.0,  10.0, 20.0, 24.0, 100.0,  12.0,    60.0,     4.0,    500,     1.50,     5800,    4000,      0.35,  2.5, 10.0,     1.5,    20.0,    15.0 }},
      {"Small Percussion Hall",  { 80.0,  10.0, 20.0, 24.0,  80.0,  12.0,    40.0,    20.0,    250,     2.00,     5200,    4000,      0.35,  2.0, 13.0,     1.1,    20.0,    10.0 }},
      {"Small Vocal Hall",       { 80.0,  10.0, 20.0, 24.0,  80.0,   4.0,    60.0,     4.0,    500,     1.25,     6250,    5200,      0.35,  3.1, 15.0,     1.2,    20.0,    10.0 }},
    }
  },
  {
    "Medium Halls", {            // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Medium Bright Hall",     { 80.0,  10.0, 20.0, 30.0, 100.0,  18.0,    90.0,     4.0,    400,     1.25,    10000,    6400,      0.60,  2.9, 15.0,     1.6,    20.0,    15.0 }},
      {"Medium Clear Hall",      { 80.0,  10.0, 20.0, 30.0, 100.0,   8.0,    90.0,     4.0,    500,     1.50,     7600,    5500,      0.50,  2.9, 15.0,     1.7,    20.0,    15.0 }},
      {"Medium Dark Hall",       { 80.0,  10.0, 20.0, 30.0, 100.0,  18.0,    60.0,     4.0,    500,     1.75,     5800,    4000,      0.40,  2.9, 15.0,     1.8,    20.0,    15.0 }},
      {"Medium Percussion Hall", { 80.0,  10.0, 20.0, 30.0,  80.0,  12.0,    40.0,    20.0,    300,     2.00,     5200,    4000,      0.35,  2.0, 12.0,     1.2,    20.0,    10.0 }},
      {"Medium Vocal Hall",      { 80.0,  10.0, 20.0, 32.0,  80.0,   8.0,    75.0,     4.0,    600,     1.50,     5800,    5200,      0.40,  2.8, 16.0,     1.3,    20.0,    10.0 }},
    }
  },
  {
    "Large Halls", {             // dry, early, late, size, width, delay, diffuse, low cut, low xo, low mult, high cut, high xo, high mult, spin, wander, decay, e. send, modulation
      {"Large Bright Hall",      { 80.0,  10.0, 20.0, 40.0, 100.0,  20.0,    90.0,     4.0,    400,     1.50,     8200,    5800,      0.50,  2.1, 20.0,     2.5,    20.0,    15.0 }},
      {"Large Clear Hall",       { 80.0,  10.0, 20.0, 40.0, 100.0,  12.0,    80.0,     4.0,    550,     2.00,     8200,    5200,      0.40,  2.1, 20.0,     2.8,    20.0,    15.0 }},
      {"Large Dark Hall",        { 80.0,  10.0, 20.0, 40.0, 100.0,  20.0,    60.0,     4.0,    600,     2.50,     6250,    2800,      0.20,  2.1, 20.0,     3.0,    20.0,    15.0 }},
      {"Large Vocal Hall",       { 80.0,  10.0, 20.0, 40.0,  80.0,  12.0,    80.0,     4.0,    700,     2.25,     6250,    4600,      0.30,  2.1, 17.0,     2.4,    20.0,    10.0 }},
      {"Great Hall",             { 80.0,  10.0, 20.0, 50.0,  90.0,  20.0,    95.0,     4.0,    750,     2.50,     5500,    4000,      0.30,  2.6, 22.0,     3.8,    20.0,    15.0 }},
    }
  }
};

static const int DEFAULT_BANK   = 2; // Small Halls
static const int DEFAULT_PRESET = 1; // Second preset in each bank

#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
