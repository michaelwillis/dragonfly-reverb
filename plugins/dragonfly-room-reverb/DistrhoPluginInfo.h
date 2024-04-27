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

#define DISTRHO_PLUGIN_BRAND   "Dragonfly"
#define DISTRHO_PLUGIN_NAME    "Dragonfly Room Reverb"
#define DISTRHO_PLUGIN_URI     "urn:dragonfly:room"
#define DISTRHO_PLUGIN_CLAP_ID "michaelwillis.dragonfly.room"

#define DISTRHO_PLUGIN_AU_TYPE   aufx
#define DISTRHO_PLUGIN_BRAND_ID  Dfrv
#define DISTRHO_PLUGIN_UNIQUE_ID Dfrr

#define DISTRHO_PLUGIN_HAS_UI        1
#define DISTRHO_PLUGIN_IS_RT_SAFE    1
#define DISTRHO_PLUGIN_NUM_INPUTS    2
#define DISTRHO_PLUGIN_NUM_OUTPUTS   2
#define DISTRHO_PLUGIN_WANT_STATE    1

#define DISTRHO_PLUGIN_LV2_CATEGORY "lv2:ReverbPlugin"
#define DISTRHO_PLUGIN_VST3_CATEGORIES "Fx|Reverb|Stereo"
#define DISTRHO_PLUGIN_CLAP_FEATURES "audio-effect", "reverb", "stereo"

#include "Artwork.hpp"

#define DISTRHO_UI_DEFAULT_WIDTH  Artwork::backgroundWidth
#define DISTRHO_UI_DEFAULT_HEIGHT Artwork::backgroundHeight

enum Parameters
{
    paramDry = 0,
    paramEarly,
    paramEarlySend,
    paramLate,
    paramSize,
    paramWidth,
    paramPredelay,
    paramDecay,
    paramDiffuse,
    paramSpin,
    paramWander,
    paramInHighCut,
    paramEarlyDamp,
    paramLateDamp,
    paramBoost,
    paramBoostLPF,
    paramInLowCut,
    paramCount
};

static const Param PARAMS[paramCount] = {
  {paramDry,        "Dry Level",    "dry_level",      0.0f,   100.0f,   "%"},
  {paramEarly,      "Early Level",  "early_level",    0.0f,   100.0f,   "%"},
  {paramEarlySend,  "Early Send",   "early_send",     0.0f,   100.0f,   "%"},
  {paramLate,       "Late Level",   "late_level",     0.0f,   100.0f,   "%"},
  {paramSize,       "Size",         "size",           8.0f,    32.0f,   "m"},
  {paramWidth,      "Width",        "width",         50.0f,   150.0f,   "%"},
  {paramPredelay,   "Predelay",     "predelay",       0.0f,   100.0f,  "ms"},
  {paramDecay,      "Decay",        "decay",          0.1f,    10.0f,   "s"},
  {paramDiffuse,    "Diffuse",      "diffuse",        0.0f,   100.0f,   "%"},
  {paramSpin,       "Spin",         "spin",           0.0f,     5.0f,  "Hz"},
  {paramWander,     "Wander",       "wander",         0.0f,   100.0f,   "%"},
  {paramInHighCut,  "High Cut",     "in_high_cut", 1000.0f, 16000.0f,  "Hz"},
  {paramEarlyDamp,  "Early Damp",   "early_damp",  1000.0f, 16000.0f,  "Hz"},
  {paramLateDamp,   "Late Damp",    "late_damp",   1000.0f, 16000.0f,  "Hz"},
  {paramBoost,      "Low Boost",    "low_boost",      0.0f,   100.0f,   "%"},
  {paramBoostLPF,   "Boost Freq",   "boost_freq",    50.0f,  1050.0f,  "Hz"},
  {paramInLowCut,   "Low Cut",      "in_low_cut",     0.0f,   200.0f,  "Hz"}
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
    "Small", {               // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Small Bright Room",  { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.2,    60.0,  0.4,   40.0,    16000,      16000,     16000,       20.0,       600,    4.0}},
      {"Small Clear Room",   { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.2,    60.0,  0.8,   40.0,    16000,      11200,     10000,       40.0,       600,    4.0}},
      {"Small Dark Room",    { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   4.0,   0.3,    70.0,  1.6,   20.0,    16000,       6400,      5500,       60.0,      1000,    4.0}},
      {"Small Drum Room",    { 80.0,  10.0,    20.0, 20.0,  9.0,  90.0,   8.0,   0.2,    24.0,  2.1,   10.0,    16000,       8200,      7000,       40.0,       400,    4.0}},
      {"Small Vocal Room",   { 80.0,  10.0,    20.0, 20.0,  8.0,  90.0,   0.0,   0.3,    86.0,  2.4,   12.0,    16000,       7600,      6400,       20.0,       400,    4.0}}
    }
  },
  {
    "Medium", {              // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Medium Bright Room", { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.4,    60.0,  0.4,   40.0,    16000,      16000,     14000,       25.0,       600,    4.0}},
      {"Medium Clear Room",  { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.4,    70.0,  0.8,   40.0,    16000,      10000,      9400,       50.0,       600,    4.0}},
      {"Medium Dark Room",   { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   8.0,   0.5,    70.0,  1.6,   20.0,    16000,       5800,      4600,       70.0,      1000,    4.0}},
      {"Medium Drum Room",   { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,  12.0,   0.4,    32.0,  2.4,   10.0,    16000,       8000,      6000,       50.0,       300,    4.0}},
      {"Medium Vocal Room",  { 80.0,  10.0,    20.0, 20.0, 12.0, 100.0,   2.0,   0.6,    92.0,  2.7,   12.0,    16000,       8000,      6000,       25.0,       400,    4.0}}
    }
  },
  {
    "Large", {               // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Large Bright Room",  { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.7,    70.0,  0.4,   40.0,    16000,      16000,     14000,       30.0,       600,    4.0}},
      {"Large Clear Room",   { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.7,    80.0,  0.4,   40.0,    16000,       9400,      8500,       60.0,       600,    4.0}},
      {"Large Dark Room",    { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,  12.0,   0.8,    80.0,  1.6,   20.0,    16000,       5200,      4000,       80.0,      1000,    4.0}},
      {"Large Drum Room",    { 80.0,  10.0,    20.0, 20.0, 14.0, 100.0,  16.0,   0.7,    40.0,  2.7,   10.0,    16000,       8000,      5000,       60.0,       300,    4.0}},
      {"Large Vocal Room",   { 80.0,  10.0,    20.0, 20.0, 15.0, 100.0,   4.0,   1.0,    98.0,  3.1,   12.0,    16000,       7000,      5000,       30.0,       400,    4.0}}
    }
  },
  {
    "Halls", {               // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Bright Hall",        { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.6,    78.0,  0.4,   40.0,    16000,      16000,     14000,       30.0,       600,    4.0}},
      {"Clear Hall",         { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.6,    84.0,  0.4,   40.0,    16000,      14000,     12000,       60.0,       600,    4.0}},
      {"Dark Hall",          { 80.0,  10.0,    20.0, 20.0, 24.0, 100.0,  12.0,   1.9,    80.0,  1.6,   20.0,    16000,       8000,      6000,       80.0,      1000,    4.0}},
      {"Percussion Hall",    { 80.0,  10.0,    20.0, 20.0, 20.0, 100.0,  16.0,   1.5,    40.0,  2.7,   10.0,    16000,       7000,      5000,       60.0,       300,    4.0}},
      {"Vocal Hall",         { 80.0,  10.0,    20.0, 20.0, 25.0, 100.0,   4.0,   2.0,    98.0,  3.1,   12.0,    16000,       6000,      5000,       30.0,       400,    4.0}}
    }
  },
  {
    "Effects", {             // dry, early, e. send, late, size, width, delay, decay, diffuse, spin, wander, high cut, early damp, late damp, bass boost, boost lpf, low cut
      {"Bright Plate",       { 80.0,  10.0,    20.0, 20.0, 10.0, 100.0,   0.0,   1.8,    78.0,  1.6,   40.0,    16000,      16000,     15000,       10.0,      1000,    4.0}},
      {"Dark Plate",         { 80.0,  10.0,    20.0, 20.0, 10.0, 100.0,   0.0,   2.0,    78.0,  0.4,   20.0,    16000,       5200,      4000,       40.0,       500,    4.0}},
      {"Brick Wall",         { 80.0,  10.0,    20.0, 20.0, 20.0,  80.0,  80.0,   0.4,    12.0,  0.2,    5.0,    16000,      16000,     15000,       10.0,       200,    4.0}},
      {"Echo Chamber",       { 80.0,  10.0,    20.0, 20.0, 12.0, 120.0,  10.0,   4.0,    24.0,  2.3,   80.0,    16000,      12000,      9000,       20.0,       500,    4.0}},
      {"Long Tunnel",        { 80.0,  10.0,    20.0, 20.0, 25.0,  50.0,   0.0,   8.0,    90.0,  0.4,   10.0,    16000,       8000,      6000,       50.0,       500,    4.0}},
    }
  }
};

static const int DEFAULT_BANK   = 1; // Medium Rooms
static const int DEFAULT_PRESET = 1; // Second preset in each bank


#endif // DISTRHO_PLUGIN_INFO_H_INCLUDED
