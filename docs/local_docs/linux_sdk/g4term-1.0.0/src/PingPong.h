// PingPong.h


/*
Polhemus Tracker Terminal version 1.0.0 -- Terminal Interface to Polhemus Trackers: Fastrak, Patriot, and Liberty
Copyright  ©  2009  Polhemus, Inc.

This file is part of Tracker Terminal.

Tracker Terminal is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Tracker Terminal is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Tracker Terminal.  If not, see <http://www.gnu.org/licenses/>.

*************************************************************************

Tracker Terminal version 1.0.0 uses the libusb library version 1.0
libusb Copyright © 2007-2008 Daniel Drake <dsd@gentoo.org>
libusb Copyright © 2001 Johannes Erdfelt <johannes@erdfelt.com>
Licensed under the GNU Lesser General Public License version 2.1 or later.
*/



#ifndef PINGPONG_H_
#define PINGPONG_H_

#define NUMBUFS   100
#define BYTE unsigned char

class PingPong {

 private:

  BYTE* m_buf[NUMBUFS];
  int m_readInd;
  int m_writeInd;
  int m_size;
  int m_len[NUMBUFS];
  pthread_mutex_t m_mutex;

  void IncrInd(int& ind);

 public:

  PingPong();
  ~PingPong();

  int InitPingPong(int);
  int GetBufferSize();
  int IsDataAvail();
  int ReadPP(BYTE*);
  int  WritePP(BYTE*,int);
  void ClearBuffers();
};





#endif
