/**
 *  Stereo Enhancer
 *
 *  Copyright (C) 2006-2018 Teru Kamogashira
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "freeverb/stenh.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(stenh)::FV3_(stenh)()
	    throw(std::bad_alloc)
{
  currentfs = FV3_REVBASE_DEFAULT_FS;
  RMS = Attack = Release = 0; Ratio = 3; SoftKnee = 10;
  DepthBPF = 7; DepthBRF = 4; DepthOA = 7;
  setRatio(3);
  setSoftKnee(10);
  setThreshold(0);
  setChValL(1); setChValR(1);  setDiffusion(0.9);
  setWidth(1); setDry(1);
  setBPF_LPF(0.5); setBPF_HPF(0.5);
  setBRF_LPF(0.5); setBRF_HPF(0.5);
  updateNRT();
  update();
}

fv3_float_t FV3_(stenh)::getSampleRate()
{
  return currentfs;
}

void FV3_(stenh)::setSampleRate(fv3_float_t fs)
{
  currentfs = fs;
  updateNRT();
  update();
}

void FV3_(stenh)::mute()
{
  bpf.mute(); lpf.mute(); hpf.mute();
  delayBPF.mute(); delayBRF.mute(); delayOA.mute();
  compS.mute(); compD.mute();
}

void FV3_(stenh)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
{
  for(long i = 0;i < numsamples;i ++)
    {
      fv3_float_t iL = inputL[i], iR = inputR[i], diff, directS, delayS, sumS, gainS, gainD, vcaFactor;
      
      diff = chvL*iL - chvR*iR;
      directS = delayBPF.process(bpf.processL(diff));
      delayS = diffusion * delayBRF.process(lpf.processL(diff) + hpf.processL(diff)); 
     
      sumS = directS + delayS;
      gainS = compS.process(iL+iR);
      gainD = compD.process(sumS);
      if(gainS < gainD)	vcaFactor = gainS*width;
      else vcaFactor = gainD*width;
      sumS = delayOA.process(sumS);
      
      outputL[i] = dry*iL - vcaFactor*sumS;
      outputR[i] = dry*iR + vcaFactor*sumS;
    }
}

void FV3_(stenh)::setChValL(fv3_float_t value){ chvL = value; }

void FV3_(stenh)::setChValR(fv3_float_t value){ chvR = value; }

fv3_float_t FV3_(stenh)::getChValL(){ return chvL; }

fv3_float_t FV3_(stenh)::getChValR(){ return chvR; }

void FV3_(stenh)::setBPF_LPF(fv3_float_t value){ bpf.setLPF(value); }

void FV3_(stenh)::setBPF_HPF(fv3_float_t value){ bpf.setHPF(value); }

void FV3_(stenh)::setBRF_LPF(fv3_float_t value){ lpf.setLPF(value); lpf.setHPF(0);}

void FV3_(stenh)::setBRF_HPF(fv3_float_t value){ hpf.setLPF(0); hpf.setHPF(value);}

void FV3_(stenh)::setBPFDepth(fv3_float_t msec){ DepthBPF = msec; updateNRT(); }

void FV3_(stenh)::setBRFDepth(fv3_float_t msec){ DepthBRF = msec; updateNRT(); }

fv3_float_t FV3_(stenh)::getBPF_LPF(){ return bpf.getLPF(); }

fv3_float_t FV3_(stenh)::getBPF_HPF(){ return bpf.getHPF(); }

fv3_float_t FV3_(stenh)::getBRF_LPF(){ return lpf.getLPF(); }

fv3_float_t FV3_(stenh)::getBRF_HPF(){ return hpf.getHPF(); }

fv3_float_t FV3_(stenh)::getBPFDepth(){ return DepthBPF; }

fv3_float_t FV3_(stenh)::getBRFDepth(){ return DepthBRF; }

void FV3_(stenh)::setOverallDepth(fv3_float_t msec){ DepthOA = msec; updateNRT(); }

void FV3_(stenh)::setDiffusion(fv3_float_t value){ diffusion = value; }

void FV3_(stenh)::setWidth(fv3_float_t value){ width = value; }

void FV3_(stenh)::setDry(fv3_float_t value){ dry = value; }

fv3_float_t FV3_(stenh)::getOverallDepth(){ return DepthOA; }

fv3_float_t FV3_(stenh)::getDiffusion(){ return diffusion; }

fv3_float_t FV3_(stenh)::getWidth(){ return width; }

fv3_float_t FV3_(stenh)::getDry(){ return dry; }

void FV3_(stenh)::setThreshold(fv3_float_t dB)
{ thres_dB = dB; compS.setThreshold(FV3_(utils)::dB2R(thres_dB)); compD.setThreshold(FV3_(utils)::dB2R(thres_dB)); }

fv3_float_t FV3_(stenh)::getThreshold(){ return thres_dB; }

void FV3_(stenh)::setRMS(fv3_float_t msec){ RMS = msec; updateNRT(); }

void FV3_(stenh)::setAttack(fv3_float_t msec){ Attack = msec; update(); }

void FV3_(stenh)::setRelease(fv3_float_t msec){ Release = msec; update(); }

fv3_float_t FV3_(stenh)::getRMS(){ return RMS; }

fv3_float_t FV3_(stenh)::getAttack(){ return Attack; }

fv3_float_t FV3_(stenh)::getRelease(){ return Release; }

void FV3_(stenh)::setRatio(fv3_float_t value){ Ratio = value; compS.setRatio(Ratio); compD.setRatio(Ratio); }

void FV3_(stenh)::setSoftKnee(fv3_float_t dB){ SoftKnee = dB; compS.setSoftKnee(SoftKnee); compD.setSoftKnee(SoftKnee); }

fv3_float_t FV3_(stenh)::getSoftKnee(){ return SoftKnee; }

fv3_float_t FV3_(stenh)::getRatio(){ return Ratio; }

void FV3_(stenh)::updateNRT()
{
  compS.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
  compD.setRMS(FV3_(utils)::ms2sample(RMS,currentfs));
  delayBPF.setsize(FV3_(utils)::ms2sample(DepthBPF,currentfs));
  delayBRF.setsize(FV3_(utils)::ms2sample(DepthBRF,currentfs));
  delayOA.setsize(FV3_(utils)::ms2sample(DepthOA,currentfs));
}

void FV3_(stenh)::update()
{
  compS.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
  compD.setAttack(FV3_(utils)::ms2sample(Attack,currentfs));
  compS.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
  compD.setRelease(FV3_(utils)::ms2sample(Release,currentfs));
}


long FV3_(stenh)::getLatency()
{
  return 0;
}

#include "freeverb/fv3_ns_end.h"
