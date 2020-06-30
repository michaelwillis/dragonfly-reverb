/**
 *  Reverb Base Class
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

class _FV3_(revbase)
{
public:
  _FV3_(revbase)() throw(std::bad_alloc);
  virtual _FV3_(~revbase)();
  virtual void         setSampleRate(_fv3_float_t fs) throw(std::bad_alloc);
  virtual _fv3_float_t getSampleRate();
  virtual _fv3_float_t getTotalSampleRate(){ return getSampleRate(); }

  /**
   * set the factor of delay loops.
   * @param[in] the factor.
   */
  virtual void setRSFactor(_fv3_float_t value);
  virtual _fv3_float_t getRSFactor() const;
  virtual _fv3_float_t getTotalFactorFs(){ return getSampleRate()*getRSFactor(); }

  virtual void setFsFactors();

  /**
   * set the reverb mode. This depends on the implementation.
   * @param[type] .
   */
  virtual void     setReverbType(unsigned type);
  virtual unsigned getReverbType();

  /**
   * set the delay length of the reverb front sound.
   * @param[in] value reverb time in samples (not seconds).
   * @attention the reverb sound comes first if the value < 0.
   */
  virtual void setInitialDelay(long numsamples);
  virtual long getInitialDelay();
  virtual void         setPreDelay(_fv3_float_t value_ms);
  virtual _fv3_float_t getPreDelay() const;
  virtual long getLatency();
  virtual void mute();
  virtual void processreplace(_fv3_float_t *inputL, _fv3_float_t *inputR, _fv3_float_t *outputL, _fv3_float_t *outputR, long numsamples)
    throw(std::bad_alloc) = 0;

  /**
   * set the reverb front sound level.
   * @param[in] value dB level.
   */
  virtual void         setwet(_fv3_float_t value);
  virtual _fv3_float_t getwet();

  /**
   * set the reverb front sound level.
   * @param[in] value level.
   */
  virtual void         setwetr(_fv3_float_t value);
  virtual _fv3_float_t getwetr() const;

  /**
   * set the dry signal level.
   * @param[in] value dB level.
   */
  virtual void         setdry(_fv3_float_t value);
  virtual _fv3_float_t getdry();

  /**
   * set the dry signal level.
   * @param[in] value level.
   */
  virtual void         setdryr(_fv3_float_t value);
  virtual _fv3_float_t getdryr();

  /**
   * set the width signal level.
   * @param[in] value width level. must be 0~1.
   */
  virtual void         setwidth(_fv3_float_t value);
  virtual _fv3_float_t getwidth() const;

  /**
   * set the prime mode for delay lines.
   * the size of the delay lines will prime numbers by default.
   * To inhibit prime number size delay lines, set this options to false.
   * @param[in] value set true (default) to enable prime mode.
   */
  virtual void setPrimeMode(bool value);
  virtual bool getPrimeMode();

  /**
   * To enable mute on fs/factor change, turn this option to true. (default=false)
   * @param[in] value set true (default) to enable mute on fs/factor change.
   */
  virtual void setMuteOnChange(bool value);
  virtual bool getMuteOnChange();

  virtual void printconfig();

 protected:
  long initialDelay;
  _FV3_(delay) delayL, delayR, delayWL, delayWR;
  _fv3_float_t currentfs, rsfactor, preDelay, wetDB, wet, wet1, wet2, dryDB, dry, width;
  virtual void update_wet();
  virtual _fv3_float_t limFs2(_fv3_float_t fq);

  virtual long f_(long def, _fv3_float_t factor);
  virtual long f_(_fv3_float_t def, _fv3_float_t factor);
  virtual long p_(long def, _fv3_float_t factor);
  virtual long p_(_fv3_float_t def, _fv3_float_t factor);
  bool primeMode, muteOnChange;
  unsigned reverbType;

 private:
  _FV3_(revbase)(const _FV3_(revbase)& x);
  _FV3_(revbase)& operator=(const _FV3_(revbase)& x);
};
