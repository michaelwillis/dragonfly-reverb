/**
 *  Gardner Room Reverberator Class
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

#include "freeverb/dl_gardner.hpp"
#include "freeverb/fv3_type_float.h"
#include "freeverb/fv3_ns_start.h"

FV3_(dl_gd_largeroom)::FV3_(dl_gd_largeroom)()
		      throw(std::bad_alloc)
{
  setDCC(4);
  setLPF(2600);
  setDecay(0.2);
}

void FV3_(dl_gd_largeroom)::mute()
{
  FV3_(delayline)::mute();
  lpf_loop.mute();
  dccut.mute();
}

void FV3_(dl_gd_largeroom)::setSampleRate(fv3_float_t fs)
			   throw(std::bad_alloc)
{
  FV3_(delayline)::setSampleRate(fs);
  pbidx[0][1] = p_(8);   pbidx[0][0] = p_(1);
  pbidx[1][1] = p_(12);  pbidx[1][0] = p_(1)+p_(8)+p_(1);
  
  pbidx[2][1] = p_(87);  pbidx[2][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17);
  pbidx[3][1] = p_(62);  pbidx[3][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(5);
  
  pbidx[4][1] = p_(120); pbidx[4][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31)+p_(3);
  pbidx[5][1] = p_(76);  pbidx[5][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31)+p_(3)+p_(5);
  pbidx[6][1] = p_(30);  pbidx[6][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31)+p_(3)+p_(5)+p_(76)+p_(5);

  pbidx[7][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2);
  pbidx[8][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31);
  pbidx[9][0] = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31)+p_(3)+p_(120);

  long totalLength = p_(1)+p_(8)+p_(1)+p_(12)+p_(2)+p_(17)+p_(87)+p_(31)+p_(3)+p_(120)+p_(10); // 10ms padding
  setsize(totalLength);
  mute();
}

fv3_float_t FV3_(dl_gd_largeroom)::process(fv3_float_t input)
{
  (*this)[0] += dccut.process(input);

  allpass(pbidx[0][0], pbidx[0][1], 0.3);
  allpass(pbidx[1][0], pbidx[1][1], 0.3);

  allpass(pbidx[2][0], pbidx[2][1], 0.5);
  allpass(pbidx[3][0], pbidx[3][1], 0.25);

  allpass(pbidx[4][0], pbidx[4][1], 0.5);
  allpass(pbidx[5][0], pbidx[5][1], 0.25);
  allpass(pbidx[6][0], pbidx[6][1], 0.25);

  fv3_float_t out = 0.34*(*this)[pbidx[7][0]] + 0.14*(*this)[pbidx[8][0]] + 0.14*(*this)[pbidx[9][0]];
  fv3_float_t loop = decay * lpf_loop.process((*this)[pbidx[9][0]]);

  // decrement base index
  baseidx --; if(baseidx < 0) baseidx += bufsize;
  (*this)[0] = loop;
  return out;
}

//

FV3_(gd_largeroom)::FV3_(gd_largeroom)() throw(std::bad_alloc)
{
  setroomsize(0.2);
  setdccutfreq(4);
  setdamp(2600);
  setLRDiffFactor(1.01);
}

void FV3_(gd_largeroom)::processreplace(fv3_float_t *inputL, fv3_float_t *inputR, fv3_float_t *outputL, fv3_float_t *outputR, long numsamples)
			throw(std::bad_alloc)
{
  if(numsamples <= 0) return;
  long count = numsamples;

  fv3_float_t outL, outR;

  while(count-- > 0)
    {
      UNDENORMAL(*inputL); UNDENORMAL(*inputR);
      outL = outR = 0.0;
 
      outL = DL_Left.process(*inputL);
      outR = DL_Right.process(*inputR);

      fv3_float_t fpL = delayWL.process(outL);
      fv3_float_t fpR = delayWR.process(outR);
      *outputL = fpL*wet1 + fpR*wet2 + delayL.process(*inputL)*dry;
      *outputR = fpR*wet1 + fpL*wet2 + delayR.process(*inputR)*dry;
      UNDENORMAL(*outputL); UNDENORMAL(*outputR);
      inputL ++; inputR ++; outputL ++; outputR ++;
    }
}

void FV3_(gd_largeroom)::setFsFactors()
{
  FV3_(revbase)::setFsFactors();
  fv3_float_t totalFactor = getTotalFactorFs();
  DL_Left.setSampleRate(totalFactor);
  DL_Right.setSampleRate(totalFactor*lrfactor);
}

#include "freeverb/fv3_ns_end.h"
