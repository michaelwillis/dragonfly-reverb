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

#include "DistrhoUI.hpp"
#include "DragonflyVersion.h"
#include "DSP.hpp"
#include "UI.hpp"
#include "Artwork.hpp"
#include "DistrhoPluginInfo.h"
#include <array>
#include <vector>
#include <math.h>
#include <string>

START_NAMESPACE_DISTRHO

namespace Art = Artwork;
using DGL::Color;

static const int knobx[]  = {435, 510, 585};
static const int knoby[]  = {130, 245};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : DragonflyReverbAbstractUI ( Art::backgroundWidth, Art::backgroundHeight, PARAMS, Art::knobData, Art::knobWidth, Art::knobHeight, Art::questionData, Art::questionWidth, Art::questionHeight),
    imgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, kImageFormatBGRA ),
    imgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, kImageFormatBGR ),
    imgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, kImageFormatBGR )
{
  currentPreset = DEFAULT_PRESET;
  currentAlg = presets[currentPreset].params[paramAlgorithm];

  knobWidth       = createLabelledKnob(&params[paramWidth],      "%3.0f%%", knobx[0], knoby[0]);
  knobPredelay    = createLabelledKnob(&params[paramPredelay],  "%2.0f ms", knobx[1], knoby[0]);
  knobDecay       = createLabelledKnob(&params[paramDecay],      "%2.1f s", knobx[2], knoby[0]);

  knobLowCut      = createLabelledKnob(&params[paramLowCut],    "%4.0f Hz", knobx[0], knoby[1]);
  knobHighCut     = createLabelledKnob(&params[paramHighCut],   "%5.0f Hz", knobx[1], knoby[1]);
  knobDamp        = createLabelledKnob(&params[paramDamp],      "%5.0f Hz", knobx[2], knoby[1]);

  sliderDry = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderDry->setId ( paramDry );
  sliderDry->setStartPos ( 17, 157 );
  sliderDry->setEndPos ( 17, 317 );
  sliderDry->setRange ( 0.0f, 100.0f );
  sliderDry->setInverted ( true );
  sliderDry->setCallback ( this );

  sliderWet = new ImageSlider ( this, Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, kImageFormatBGRA ) );
  sliderWet->setId ( paramWet );
  sliderWet->setStartPos ( 57, 157 );
  sliderWet->setEndPos ( 57, 317 );
  sliderWet->setRange ( 0.0f, 100.0f );
  sliderWet->setInverted ( true );
  sliderWet->setCallback ( this );

  rectSliders[0].setPos  ( 17,  157 );
  rectSliders[0].setSize ( 26,  160 );
  rectSliders[1].setPos  ( 57,  157 );
  rectSliders[1].setSize ( 26,  160 );

  rectDisplay.setPos  ( 110, 126 );
  rectDisplay.setSize ( 305, 207 );

  for ( int i = 0; i < ALGORITHM_COUNT; ++i)
  {
    rectAlgorithms[i].setPos( 575, 35 + (i * 21) );
    rectAlgorithms[i].setSize( 100, 21 );
  }

  for ( int i = 0; i < NUM_PRESETS; ++i)
  {
    int x = i < 4 ? 340 : 455;
    rectPresets[i].setPos( x, 25 + ((i % 4) * 21) );
    rectPresets[i].setSize( 125, 21 );
  }

  aboutButton->setAbsolutePos ( 390, 130 );

  AbstractDSP *dsp = new DragonflyReverbDSP(SPECTROGRAM_SAMPLE_RATE);
  spectrogram = new Spectrogram(this, &nanoText, &rectDisplay, dsp);
  spectrogram->setAbsolutePos (110, 126);
}

/**
   A parameter has changed on the plugin side
   This is called by the host to inform the UI about parameter changes.
 */
void DragonflyReverbUI::parameterChanged ( uint32_t index, float value )
{
  displayAbout = false;

  switch ( index )
  {
    case paramDry:            sliderDry->setValue ( value ); break;
    case paramWet:            sliderWet->setValue ( value ); break;

    case paramWidth:          knobWidth->setValue ( value ); break;
    case paramPredelay:    knobPredelay->setValue ( value ); break;
    case paramDecay:          knobDecay->setValue ( value ); break;

    case paramLowCut:        knobLowCut->setValue ( value ); break;
    case paramHighCut:      knobHighCut->setValue ( value ); break;
    case paramDamp:            knobDamp->setValue ( value ); break;
  }

  if (index != paramDry) {
    spectrogram->setParameterValue(index, value);
  }
}

void DragonflyReverbUI::stateChanged(const char* key, const char* value)
{
  if (std::strcmp(key, "preset") == 0) {
    for (int p = 0; p < NUM_PRESETS; p++) {
      if (std::strcmp(value, presets[p].name) == 0) {
	currentPreset = p;
      }
    }

    updatePresetDefaults();
  }

  repaint();
}

/* ----------------------------------------------------------------------------------------------------------
 * Widget Callbacks
 *----------------------------------------------------------------------------------------------------------*/

void DragonflyReverbUI::imageKnobDragStarted ( ImageKnob* knob )
{
  editParameter ( knob->getId(), true );
}

void DragonflyReverbUI::imageKnobDragFinished ( ImageKnob* knob )
{
  editParameter ( knob->getId(), false );
}

void DragonflyReverbUI::imageKnobValueChanged ( ImageKnob* knob, float value )
{
  setParameterValue ( knob->getId(),value );
  spectrogram->setParameterValue ( knob->getId(), value );
}

void  DragonflyReverbUI::imageSliderDragStarted ( ImageSlider* slider )
{
  editParameter ( slider->getId(), true );
}

void  DragonflyReverbUI::imageSliderDragFinished ( ImageSlider* slider )
{
  editParameter ( slider->getId(), false );
}

void  DragonflyReverbUI::imageSliderValueChanged ( ImageSlider* slider, float value )
{
  int SliderID = slider->getId();
  setParameterValue ( SliderID,value );
  spectrogram->setParameterValue ( SliderID, value );
}

bool DragonflyReverbUI::onMouse ( const MouseEvent& ev )
{
  if ( ev.button == 1 && ev.press )
  {
    for (int row = 0; row < ALGORITHM_COUNT; row++)
    {
      if (rectAlgorithms[row].contains ( ev.pos ))
      {
        currentAlg = row;
        setParameterValue ( paramAlgorithm, row );
        spectrogram->setParameterValue( paramAlgorithm, row);
      }
    }

    bool presetClicked = false;

    for (int row = 0; row < NUM_PRESETS; row++)
    {
      if (rectPresets[row].contains ( ev.pos ))
      {
        currentPreset = row;
        presetClicked = true;
      }
    }

    if (presetClicked)
    {
      setState("preset", presets[currentPreset].name);
      updatePresetDefaults();

      const float *preset = presets[currentPreset].params;

      knobWidth->setValue ( preset[paramWidth] );
      knobPredelay->setValue ( preset[paramPredelay] );
      knobDecay->setValue ( preset[paramDecay] );

      knobLowCut->setValue ( preset[paramLowCut] );
      knobHighCut->setValue ( preset[paramHighCut] );
      knobDamp->setValue ( preset[paramDamp] );

      for ( uint32_t i = 0; i < paramCount; i++ ) {
        // Don't set sliders
        if (i != paramDry && i != paramWet) {
                setParameterValue ( i, preset[i] );
                spectrogram->setParameterValue(i, preset[i]);
        }
      }

      repaint();
      return true;
    }
  }

  return DragonflyReverbAbstractUI::onMouse(ev);
}

void DragonflyReverbUI::onDisplay()
{
  const GraphicsContext& context(getGraphicsContext());

  imgBackground.draw(context);

  float r,g,b;
  r = 230.0f / 256;
  g = 230.0f / 256;
  b = 230.0f / 256;

  // print parameters
  nanoText.beginFrame ( this );
  nanoText.fontSize ( 15 );
  nanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  nanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  std::snprintf ( strBuf, 32, "%i%%", int ( sliderDry->getValue() ) );
  nanoText.textBox ( 17 - 2, 330, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i%%", int ( sliderWet->getValue() ) );
  nanoText.textBox ( 57 - 2, 330, 35.0f, strBuf, nullptr );

  // print labels;
  nanoText.fillColor ( Color ( 0.90f, 0.95f, 1.00f ) );
  nanoText.fontSize ( 14 );
  nanoText.textBox (  10, 130, 40, "Dry\nLevel",   nullptr );
  nanoText.textBox (  50, 130, 40, "Wet\nLevel",   nullptr );
  nanoText.endFrame();

  //draw faders
  r = 178.0f/255.f;
  g = 148.0f/255.f;
  b =  59.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  uint dry = ( float ( sliderDry->getValue() ) / 100.0 ) * 160.0 + 1.0f;
  uint wet = ( float ( sliderWet->getValue() ) / 100.0 ) * 160.0 + 1.0f;

  rectSliders[0].setHeight ( dry );
  rectSliders[0].setY ( 118 + 200 - dry );

  rectSliders[1].setHeight ( wet );
  rectSliders[1].setY ( 118 + 200 - wet );

  if ( dry > 1 )
    rectSliders[0].draw(context);
  if ( wet > 1 )
    rectSliders[1].draw(context);

  glColor4f ( 1.0f,1.0f,1.0f,1.0f );

  nanoText.beginFrame ( this );
  nanoText.fontSize ( 15 );
  nanoText.textAlign ( NanoVG::ALIGN_RIGHT | NanoVG::ALIGN_TOP );

  Color bright = Color ( 0.90f, 0.95f, 1.00f );
  Color dim    = Color ( 0.65f, 0.65f, 0.65f );


  nanoText.textAlign ( NanoVG::ALIGN_CENTER | NanoVG::ALIGN_TOP );
  nanoText.fillColor(bright);
  nanoText.textBox ( 340, 10, 200, "Presets", nullptr );
  
  nanoText.textAlign ( NanoVG::ALIGN_LEFT | NanoVG::ALIGN_TOP );
  
  for (int row = 0; row < NUM_PRESETS; row ++)
  {
    DGL::Rectangle<int> presetRect = rectPresets[row];
    nanoText.fillColor( row == currentPreset ? bright : dim );
    nanoText.textBox ( presetRect.getX() + 3, presetRect.getY() + 2, presetRect.getWidth(), presets[row].name, nullptr );
  }

  nanoText.textAlign ( NanoVG::ALIGN_LEFT | NanoVG::ALIGN_TOP );
  
  nanoText.fillColor(bright);
  nanoText.textBox ( 575, 10, 100, "Reverb Type", nullptr );

  for (int row = 0; row < ALGORITHM_COUNT; row ++)
  {
    DGL::Rectangle<int> rect = rectAlgorithms[row];
    nanoText.fillColor( row == ((int)currentAlg) ? bright : dim );
    nanoText.textBox ( rect.getX(), rect.getY() + 2, rect.getWidth(), algorithmNames[row], nullptr );
  }

  nanoText.endFrame();

  if (displayAbout) {
    spectrogram->hide();
    nanoText.beginFrame ( this );
    nanoText.fontSize ( 15 );
    nanoText.textAlign ( NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP );

    r = 230.0f / 256;
    g = 230.0f / 256;
    b = 230.0f / 256;
    nanoText.fillColor ( Color ( r, g, b ) );

    int x = rectDisplay.getX() + 5;
    int y = rectDisplay.getY() + 5;
    int w = rectDisplay.getWidth() - 10;

    char textBuffer[400];

    std::snprintf(textBuffer, 400,
      "Dragonfly Plate is a free reverb effect\n"
      "based on Freeverb3 NReverb and STRev.\n\n"
      "Version: %d.%d.%d%s  License: GPL 3+\n\n"
      "• Michael Willis - Plugin Development\n"
      "• James Peters - Quality Assurance\n"
      "• Teru Kamogashira - Freeverb3\n"
      "• \"falkTX\" Coelho - Distrho Plugin Framework",
      MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION, VERSION_SUFFIX
    );

    nanoText.textBox ( x, y, w, textBuffer, nullptr );
    nanoText.endFrame();
  }
  else
  {
      spectrogram->show();
  }

}

void DragonflyReverbUI::uiIdle() {
  spectrogram->uiIdle();
}

void DragonflyReverbUI::updatePresetDefaults() {
  const float *preset = presets[currentPreset].params;

  currentAlg = preset[paramAlgorithm];

  knobWidth->setDefault ( preset[paramWidth] );
  knobPredelay->setDefault ( preset[paramPredelay] );
  knobDecay->setDefault ( preset[paramDecay] );

  knobLowCut->setDefault ( preset[paramLowCut] );
  knobHighCut->setDefault ( preset[paramHighCut] );
  knobDamp->setDefault ( preset[paramDamp] );
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
