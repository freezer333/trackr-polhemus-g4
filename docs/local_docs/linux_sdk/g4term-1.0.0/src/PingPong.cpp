

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



#include <pthread.h>
#include "PingPong.h"
#include <string.h>


PingPong::PingPong(){
  for (int i=0;i<NUMBUFS;i++){
    m_buf[i]=NULL;
    m_len[i]=0;
  }
  m_readInd=m_writeInd=m_size=0;

  // init the mutex to synchronize reads and writes between different threads
  pthread_mutex_init(&m_mutex,NULL);
}

PingPong::~PingPong(){

  // need to clean up 
  for (int i=0;i<NUMBUFS;i++){
    if (m_buf[i])
      delete[] m_buf[i];
  }
}

// create buffers
int PingPong::InitPingPong(int bufSize){

  int memerr=0;
  for (int i=0;i<NUMBUFS;i++){
    m_buf[i]=new BYTE[bufSize];
    if (!m_buf[i]){
      memerr=1;
      break;
    }
  }
  if (memerr){
    for (int i=0;i<NUMBUFS;i++){
      if (m_buf[i]){
	delete[] m_buf[i];
	m_buf[i]=NULL;
      }
    }
    return -1;
  }


  m_size=bufSize;
  return 0;
}

int PingPong::GetBufferSize(){
  return m_size;
}

// return number of bytes available in read buffer
int PingPong::IsDataAvail(){

  return m_len[m_readInd];
}

//#include <stdio.h>

// read from the read buffer
int PingPong::ReadPP(BYTE* buf){

  pthread_mutex_lock(&m_mutex);
  int len=m_len[m_readInd];

  if (len==0){
    pthread_mutex_unlock(&m_mutex);
    return 0;
  }

  memcpy(buf,m_buf[m_readInd],len);
  m_len[m_readInd]=0;
  //   printf("Read %d bytes from buffer %d\n",len,m_readInd);
  IncrInd(m_readInd);

  pthread_mutex_unlock(&m_mutex);

  return len;
}


// write to the write buffer
int PingPong::WritePP(BYTE* buf,int len){

  int rv=0;

  pthread_mutex_lock(&m_mutex);

  if(len>m_size)
    len=m_size;

  if (m_len[m_writeInd]==0){  // don't overwrite unread data
    memcpy(m_buf[m_writeInd],buf,len);
    m_len[m_writeInd]=len;
    //   printf("Wrote %d bytes to buffer %d\n",len,m_writeInd);
    IncrInd(m_writeInd);
    rv=len;
  }

  pthread_mutex_unlock(&m_mutex);
  return rv;
}

// increment the index to the next in progression
void PingPong::IncrInd(int& index){

  index++;
  if (index==NUMBUFS)
    index=0;
}

void PingPong::ClearBuffers(){

  pthread_mutex_lock(&m_mutex);
  for (int i=0;i<NUMBUFS;i++)
    m_len[i]=0;
  
  m_readInd=m_writeInd=0;
  pthread_mutex_unlock(&m_mutex);
}
