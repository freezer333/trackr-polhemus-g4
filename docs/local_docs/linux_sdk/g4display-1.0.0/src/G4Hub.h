// G4Hub.h: interface for the CG4Hub class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_G4HUB_H__41DD193E_B016_4FF7_93E6_CF8AEC652FF1__INCLUDED_)
#define AFX_G4HUB_H__41DD193E_B016_4FF7_93E6_CF8AEC652FF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "G4TrackIncl.h"

#define BYTE unsigned char

class CG4Hub  
{
public:
	void SetColor(const float* col);
	void Render(GLUquadric* quadric,float scale);
	void SetId(int id);
	int GetId();
	void SetPnoData(float* pno,int sen);
	CG4Hub();
	virtual ~CG4Hub();

private:
	bool m_senActive[G4_SENSORS_PER_HUB];
	float m_invCol[3][3];
	float m_col[3][3];
	int m_id;
	float m_pno[G4_SENSORS_PER_HUB][6];
};

#endif // !defined(AFX_G4HUB_H__41DD193E_B016_4FF7_93E6_CF8AEC652FF1__INCLUDED_)
