/**
 *  Pthread Locker Class
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

#ifndef _fv3_pthread_tool_hpp
#define _fv3_pthread_tool_hpp

#include <pthread.h>

class PthreadEvent
{
public:
  PthreadEvent()
  { pthread_mutex_init(&mutex, 0); pthread_cond_init(&cond, 0); triggered = autoReset = false; }
  virtual ~PthreadEvent()
  { pthread_mutex_destroy(&mutex); pthread_cond_destroy(&cond); }
  void autoreset(bool set)
  { autoReset = set; }
  void trigger()
  { pthread_mutex_lock(&mutex); triggered = true; pthread_cond_signal(&cond); pthread_mutex_unlock(&mutex); }
  void reset()
  { pthread_mutex_lock(&mutex); triggered = false; pthread_mutex_unlock(&mutex); }
  void set()
  { pthread_mutex_lock(&mutex); triggered = true; pthread_mutex_unlock(&mutex); }
  void wait()
  { pthread_mutex_lock(&mutex); while(!triggered) pthread_cond_wait(&cond, &mutex); if(autoReset) triggered = false; pthread_mutex_unlock(&mutex); }
private:
  pthread_mutex_t mutex; pthread_cond_t cond; bool triggered, autoReset;
};

class PthreadLocker
{
public:
  PthreadLocker()
  { pthread_mutex_init(&mutex, NULL); }
  ~PthreadLocker()
  { pthread_mutex_destroy(&mutex); }
  void lock()
  { pthread_mutex_lock(&mutex); }
  void unlock()
  { pthread_mutex_unlock(&mutex); }
  bool trylock()
  { if(pthread_mutex_trylock(&mutex) == 0) return true; else return false; }
private:
  pthread_mutex_t mutex;
};

#endif
