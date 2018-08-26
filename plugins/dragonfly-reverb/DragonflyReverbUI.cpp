/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


#include "DistrhoUI.hpp"
#include "DragonflyReverbUI.hpp"
#include "DragonflyReverbArtwork.hpp"
#include "DistrhoPluginInfo.h"
#include "NotoSans_Regular.ttf.hpp"
#include <array>
#include <vector>
#include <math.h>
#include <string>

START_NAMESPACE_DISTRHO

namespace Art = DragonflyReverbArtwork;
using DGL::Color;

static const int knobx[]  = {144, 710, 796, 884};
static const int knoby[]  = {12,  122, 232};

// -----------------------------------------------------------------------------------------------------------
DragonflyReverbUI::DragonflyReverbUI()
  : UI ( Art::backgroundWidth, Art::backgroundHeight ),
    fImgBackground ( Art::backgroundData, Art::backgroundWidth, Art::backgroundHeight, GL_BGRA ),
    fImgTabOff ( Art::tab_offData, Art::tab_offWidth, Art::tab_offHeight, GL_BGR ),
    fImgTabOn ( Art::tab_onData, Art::tab_onWidth,Art::tab_onHeight, GL_BGR )
{
  // text
  fNanoFont  = fNanoText.createFontFromMemory ( "notosans", font_notosans::notosans_ttf, font_notosans::notosans_ttf_size, false );
  fNanoText.fontFaceId ( fNanoFont );

  fKnobSize      = new LabelledKnob (this, this, &fNanoText, paramSize,      "%3.2f m",  knobx[0], knoby[0]);
  fKnobWidth     = new LabelledKnob (this, this, &fNanoText, paramWidth,     "%3.2f %%", knobx[3], knoby[0]);
  fKnobSpin      = new LabelledKnob (this, this, &fNanoText, paramSpin,      "%2.1f Hz", knobx[3], knoby[1]);
  fKnobWander    = new LabelledKnob (this, this, &fNanoText, paramWander,    "%2.1f ms", knobx[3], knoby[2]);
  fKnobPredelay  = new LabelledKnob (this, this, &fNanoText, paramPredelay,  "%4.2f ms", knobx[1], knoby[0]);
  fKnobDiffuse   = new LabelledKnob (this, this, &fNanoText, paramDiffuse,   "%2.1f %%", knobx[2], knoby[0]);
  fKnobLowCut    = new LabelledKnob (this, this, &fNanoText, paramLowCut,    "%4.1f Hz", knobx[0], knoby[2]);
  fKnobLowXover  = new LabelledKnob (this, this, &fNanoText, paramLowXover,  "%4.1f Hz", knobx[1], knoby[2]);
  fKnobLowMult   = new LabelledKnob (this, this, &fNanoText, paramLowMult,   "%2.2f X",  knobx[2], knoby[2]);
  fKnobHighCut   = new LabelledKnob (this, this, &fNanoText, paramHighCut,   "%5.0f Hz", knobx[0], knoby[1]);
  fKnobHighXover = new LabelledKnob (this, this, &fNanoText, paramHighXover, "%5.0f Hz", knobx[1], knoby[1]);
  fKnobHighMult  = new LabelledKnob (this, this, &fNanoText, paramHighMult,  "%2.2f X",  knobx[2], knoby[1]);

  // sliders


  fSliderDry_level = new ImageSlider ( this,
                                       Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderDry_level->setId ( paramDry_level );
  fSliderDry_level->setStartPos ( 17, 102 );
  fSliderDry_level->setEndPos ( 17, 302 );
  fSliderDry_level->setRange ( 0.0f, 100.0f );
  fSliderDry_level->setInverted ( true );
  fSliderDry_level->setCallback ( this );

  fSliderEarly_level = new ImageSlider ( this,
                                         Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderEarly_level->setId ( paramEarly_level );
  fSliderEarly_level->setStartPos ( 57, 102 );
  fSliderEarly_level->setEndPos ( 57, 302 );
  fSliderEarly_level->setRange ( 0.0f, 100.0f );
  fSliderEarly_level->setInverted ( true );
  fSliderEarly_level->setCallback ( this );

  fSliderLate_level = new ImageSlider ( this,
                                        Image ( Art::sliderData, Art::sliderWidth, Art::sliderHeight, GL_BGRA ) );
  fSliderLate_level->setId ( paramLate_level );
  fSliderLate_level->setStartPos ( 97, 102 );
  fSliderLate_level->setEndPos ( 97, 302 );
  fSliderLate_level->setRange ( 0.0f, 100.0f );
  fSliderLate_level->setInverted ( true );
  fSliderLate_level->setCallback ( this );

  // rectangles for displaytabs
  rectTabResponse.setPos ( 315, 85 );
  rectTabResponse.setSize ( Art::tab_onWidth, Art::tab_onHeight );
  rectTabPrograms.setPos ( 405, 85 );
  rectTabPrograms.setSize ( Art::tab_onWidth, Art::tab_onHeight );
  rectTabAbout.setPos ( 495, 85 );
  rectTabAbout.setSize ( Art::tab_onWidth, Art::tab_onHeight );

  // rectangles for display
  rectDisplay.setPos ( 245,110 );
  rectDisplay.setSize ( 450,210 );

  spectrogram = new Spectrogram(this, &fNanoText, &rectDisplay);
  spectrogram->setAbsolutePos (245, 110);

  // rectangles for programs
  for ( int i = 0; i < 3; ++i ) // 3 columns
    {
      int startx = 290, starty = 115;
      int height = ( rectDisplay.getHeight()-10 ) / 8;
      int width = ( rectDisplay.getWidth()-10 ) /3;

      for ( int j=0; j < 8; ++j ) // 8 rows
        {
          int programIndex = j + ( i * 8 );
          int x = startx + i * width;
          int y = starty + height * j;
          rectPrograms[programIndex].setPos ( x,y );
          rectPrograms[programIndex].setSize ( width,height );
        }
    }

  // rectangles for sliders
  rectSliders[0].setPos ( 17,102 );
  rectSliders[0].setSize ( 26,200 );
  rectSliders[1].setPos ( 57,102 );
  rectSliders[1].setSize ( 26,200 );
  rectSliders[2].setPos ( 97,102 );
  rectSliders[2].setSize ( 26,200 );
}

/**
   A parameter has changed on the plugin side
   This is called by the host to inform the UI about parameter changes.
 */
void DragonflyReverbUI::parameterChanged ( uint32_t index, float value )
{
  switch ( index )
    {
      // sliders

    case paramDry_level:
      fSliderDry_level->setValue ( value );
      break;

    case paramEarly_level:
      fSliderEarly_level->setValue ( value );
      break;

    case paramLate_level:
      fSliderLate_level->setValue ( value );
      break;

      // knobs

    case paramSize:
      fKnobSize->setValue ( value );
      break;

    case paramPredelay:
      fKnobPredelay->setValue ( value );
      break;

    case paramDiffuse:
      fKnobDiffuse->setValue ( value );
      break;

    case paramLowCut:
      fKnobLowCut->setValue ( value );
      break;

    case paramLowXover:
      fKnobLowXover->setValue ( value );
      break;

    case paramLowMult:
      fKnobLowMult->setValue ( value );
      break;

    case paramHighCut:
      fKnobHighCut->setValue ( value );
      break;

    case paramHighXover:
      fKnobHighXover->setValue ( value );
      break;

    case paramHighMult:
      fKnobHighMult->setValue ( value );
      break;

    case paramWidth:
      fKnobHighCut->setValue ( value );
      break;

    case paramSpin:
      fKnobHighXover->setValue ( value );
      break;

    case paramWander:
      fKnobHighMult->setValue ( value );
      break;
    }

    spectrogram->setParameterValue(index, value);
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

void DragonflyReverbUI::programLoaded ( uint32_t index )
{
  currentProgram = index;
  const float *preset = presets[index].params;
  fSliderDry_level->setValue ( preset[paramDry_level] );
  fSliderEarly_level->setValue ( preset[paramEarly_level] );
  fSliderLate_level->setValue ( preset[paramLate_level] );
  fKnobSize->setValue ( preset[paramSize] );
  fKnobPredelay->setValue ( preset[paramPredelay] );
  fKnobDiffuse->setValue ( preset[paramDiffuse] );
  fKnobLowCut->setValue ( preset[paramLowCut] );
  fKnobLowXover->setValue ( preset[paramLowXover] );
  fKnobLowMult->setValue ( preset[paramLowMult] );
  fKnobHighCut->setValue ( preset[paramHighCut] );
  fKnobHighXover->setValue ( preset[paramHighXover] );
  fKnobHighMult->setValue ( preset[paramHighMult] );
  fKnobWidth->setValue ( preset[paramWidth] );
  fKnobSpin->setValue ( preset[paramSpin] );
  fKnobWander->setValue ( preset[paramWander] );
  for ( uint32_t i = 0; i < paramCount; i++ ) {
    setParameterValue ( i, preset[i] );
    spectrogram->setParameterValue(i, preset[i]);
  }
}

bool DragonflyReverbUI::onMouse ( const MouseEvent& ev )
{
  // display
  if ( ev.button != 1 )
    return false;
  if ( ev.press )
    {
      if ( rectDisplay.contains ( ev.pos ) )
        {
          switch ( currentDisplayMode )
            {
            case displayResponse:
              return false;
              break;


            case displayPrograms:
              for ( int i = 0; i < 24; i++ )
                {
                  if ( rectPrograms[i].contains ( ev.pos ) )
                    {
                      currentProgram = i;

                      programLoaded ( currentProgram );
                      currentDisplayMode = displayResponse;
                      repaint();
                    } // end if
                } // end for

              break;

            case displayAbout:
              currentDisplayMode = displayResponse;
              repaint();

              break;
            } // end switch
          return false;
        } // end if rectDisplay

      // check for clicks on tabs

      if ( rectTabResponse.contains ( ev.pos ) )
        {
          currentDisplayMode = displayResponse;
          repaint();
          return true;
        }

      if ( rectTabPrograms.contains ( ev.pos ) )
        {
          currentDisplayMode = displayPrograms;
          repaint();
          return true;
        }

      if ( rectTabAbout.contains ( ev.pos ) )
        {
          currentDisplayMode = displayAbout;
          repaint();
          return true;
        }

    } // end if ev.press
  return false;
}

void DragonflyReverbUI::onDisplay()
{
  fImgBackground.draw();
  float r,g,b;
  // wipe display
  r = 80.0f / 256.0f;
  g = 80.0f / 256.0f;
  b = 80.0f / 256.0f;
  // rectangle to wipe
  glColor4f ( r, g, b, 1.0f );
  rectDisplay.draw();

  // print parameters
  fNanoText.beginFrame ( this );
  fNanoText.fontSize ( 16 );
  fNanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

  r = 230.0f / 256;
  g = 230.0f / 256;
  b = 230.0f / 256;
  fNanoText.fillColor ( Color ( r, g, b ) );

  char strBuf[32+1];
  strBuf[32] = '\0';

  float xstart = 20.0f, ystart= 92.0f;
  float xoffset = 88.0f, yoffset = 110.0f;

  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderDry_level->getValue() ) );
  fNanoText.textBox ( 17 - 2  , 314 , 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderEarly_level->getValue() ) );
  fNanoText.textBox ( 57 - 2, 314, 35.0f, strBuf, nullptr );
  std::snprintf ( strBuf, 32, "%i %%", int ( fSliderLate_level->getValue() ) );
  fNanoText.textBox ( 97 - 2, 314 , 35.0f, strBuf, nullptr );

  // print lables;
  r = 205.0f / 256;
  g = 241.0f / 256;
  b = 255.0f / 256;
  fNanoText.fillColor ( Color ( r, g, b ) );
  fNanoText.fontSize ( 18 );

  float labelWidth = 65.0f;
  fNanoText.textBox ( 17 - 2, 90, 30, "Dry",   nullptr );
  fNanoText.textBox ( 57 - 2, 90, 30, "Early", nullptr );
  fNanoText.textBox ( 97 - 2, 90, 30, "Late",  nullptr );

  fNanoText.endFrame();

  //draw faders
  r = 82.0f/255.f;
  g = 148.0f/255.f;
  b = 88.0f/255.f;
  glColor4f ( r, g, b, 1.0f );
  float dry = ( float ( fSliderDry_level->getValue() ) / 100 ) * 200;
  float early = ( float ( fSliderEarly_level->getValue() ) / 100 ) * 200;
  float late = ( float ( fSliderLate_level->getValue() ) / 100 ) * 200;

  rectSliders[0].setHeight ( dry + 0.5f );
  rectSliders[0].setY ( 103 + 200 - dry + 0.5f );

  rectSliders[1].setHeight ( early + 0.5f );
  rectSliders[1].setY ( 103 + 200 - early + 0.5f );

  rectSliders[2].setHeight ( late + 0.5f );
  rectSliders[2].setY ( 103 + 200 - late + 0.5f );

  if ( int ( dry ) > 1 )
    rectSliders[0].draw();
  if ( int ( early ) > 1 )
    rectSliders[1].draw();
  if ( int ( late ) > 1 )
    rectSliders[2].draw();


  switch ( int ( currentDisplayMode ) ) {
    case displayResponse:
    {
      spectrogram->show();

      // draw tabs
      glColor4f ( 1.0f,1.0f,1.0f,1.0f ); // reset colour

      fImgTabOn.drawAt ( 315,85 );
      fImgTabOff.drawAt ( 405,85 );
      fImgTabOff.drawAt ( 495,85 );

      break;
    }

    case displayPrograms:
    {
      spectrogram->hide();
      fNanoText.beginFrame ( this );
      fNanoText.fontSize ( 18 );
      fNanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );

      r = 230.0f / 256;
      g = 230.0f / 256;
      b = 230.0f / 256;
      fNanoText.fillColor ( Color ( r, g, b ) );

      char strBuf[32+1];
      strBuf[32] = '\0';
      for ( int i = 0; i < 24; i++ )
        {
          int x = rectPrograms[i].getX();
          int y = rectPrograms[i].getY();
          int w = rectPrograms[i].getWidth();
          int h = rectPrograms[i].getHeight();
          std::snprintf ( strBuf, 32, "%s", presets[i].name );
          fNanoText.textBox ( x, y + ( h/2 ), w, strBuf, nullptr );
        }
      fNanoText.endFrame();

      // draw box around current program
      // r = 230.0f / 256;
      // g = 230.0f / 256;
      // b = 230.0f / 256;
      glColor4f ( r,g,b ,1.0f );
//       for ( int i = 0 ; i < 24 ; i++ )
// 	rectPrograms[i].drawOutline();
//
      // rectPrograms[currentProgram].drawOutline();

      // draw tabs
      glColor4f ( 1.0f,1.0f,1.0f,1.0f );
      fImgTabOff.drawAt ( 315,85 );
      fImgTabOn.drawAt ( 405,85 );
      fImgTabOff.drawAt ( 495,85 );


      break;
    }
    case displayAbout:
    {
      spectrogram->hide();
      fNanoText.beginFrame ( this );
      fNanoText.fontSize ( 18 );
      fNanoText.textAlign ( NanoVG::ALIGN_LEFT|NanoVG::ALIGN_TOP );

      r = 230.0f / 256;
      g = 230.0f / 256;
      b = 230.0f / 256;
      fNanoText.fillColor ( Color ( r, g, b ) );

      int x = rectDisplay.getX() + 10;
      int y = rectDisplay.getY() + 5;
      int w = rectDisplay.getWidth() - 20;

      fNanoText.textBox ( x, y , w ,
        "A hall-style reverb based on algorithms from Freeverb3\n"
        "License: GPL 3+ (https://www.gnu.org/licenses/gpl-3.0.en.html)\n\n"
        "Michael Willis - Initial concept, plugin, and spectrogram\n\n"
        "Rob van dev Berg - Graphical interface\n\n"
        "Teru Kamogashira - Freeverb3\n\n"
        "Thanks to Filipe \"falkTX\" Coelho for developing the excellent Distrho Plugin Framework and providing plenty of advice."
        , nullptr );
      fNanoText.endFrame();
      // draw tabs
      glColor4f ( 1.0f,1.0f,1.0f,1.0f );
      fImgTabOff.drawAt ( 315,85 );
      fImgTabOff.drawAt ( 405,85 );
      fImgTabOn.drawAt ( 495,85 );

      break;
    }

  } // end switch

  // draw labels on tabs
  fNanoText.beginFrame ( this );
  fNanoText.fontSize ( 18 );
  fNanoText.textAlign ( NanoVG::ALIGN_CENTER|NanoVG::ALIGN_MIDDLE );
  r = 205.0f / 256;
  g = 241.0f / 256;
  b = 255.0f / 256;
  fNanoText.fillColor ( Color ( r, g, b ) );
  float y = 85 + ( 22/2 );
  fNanoText.textBox ( 315.0f, y, 80.0f , "Response", nullptr );
  fNanoText.textBox ( 405.0f ,y, 80.0f , "Presets", nullptr );
  fNanoText.textBox ( 495.0f, y, 80.0f , "About"   , nullptr );
  fNanoText.endFrame();
}

void DragonflyReverbUI::uiIdle() {
  spectrogram->uiIdle();
}

/* ------------------------------------------------------------------------------------------------------------
 * UI entry point, called by DPF to create a new UI instance. */

UI* createUI()
{
  return new DragonflyReverbUI();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
