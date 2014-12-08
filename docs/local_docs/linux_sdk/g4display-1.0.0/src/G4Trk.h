// G4Trk.h: interface for the CG4Trk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_G4TRK_H__6794A431_226F_4BCD_880E_F4184AEC5EBE__INCLUDED_)
#define AFX_G4TRK_H__6794A431_226F_4BCD_880E_F4184AEC5EBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "G4TrackIncl.h"
#include "G4Hub.h"


class CG4Trk  
{
 public:
  void Boresight(bool enable=true);
  void GetSrcLoc(float (*loc)[6]);
  int GetNumSrc();
  void GetHubList(int* list,int numEl);
  int GetActHubs();
  bool Connect(char*);
  int GetHubsPno(REND_STRUCT*);
  int UpdateHubs();
  void GetSetIncr(float& posIncr,float& oriIncr,bool bSet);
  void SetFilterValues(float* pfilt,float* ofilt);
  void GetFilterValues(float* pfilt,float* ofilt);

  CG4Trk();
  virtual ~CG4Trk();

 private:
  int m_sysId;
  bool m_bConnected;
  int m_numHubs;
  int* m_hubList;
  LPG4_FRAMEDATA m_pframeData;
  int m_numSrc;
  float (*m_srcLoc)[6];
};

#endif // !defined(AFX_G4TRK_H__6794A431_226F_4BCD_880E_F4184AEC5EBE__INCLUDED_)
