// G4Trk.cpp: implementation of the CG4Trk class.
//
//////////////////////////////////////////////////////////////////////
#include <GL/glu.h>
#include "Quaternion.h"
#include "struct.h"
#include "G4Trk.h"
#include <unistd.h>
#include <string.h>


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const int FRAMERATE=60;



CG4Trk::CG4Trk()
{

  m_bConnected=false;
  m_numHubs=0;
  m_hubList=NULL;
  m_pframeData=NULL;
  m_srcLoc=NULL;

}

CG4Trk::~CG4Trk()
{

  if (m_bConnected)
    g4_close_tracker();
  if (m_hubList)
    delete[] m_hubList;
  if (m_pframeData)
    delete[] m_pframeData;
  if (m_srcLoc)
    delete[] m_srcLoc;

}

bool CG4Trk::Connect(char* cfgFile)
{
  int res=g4_init_sys(&m_sysId,cfgFile,NULL);
  if (res==G4_ERROR_NONE){
    m_bConnected=true;

    // get a list of hubs
    G4_CMD_STRUCT cs;
    cs.cmd=G4_CMD_GET_ACTIVE_HUBS;
    cs.cds.id=G4_CREATE_ID(m_sysId,0,0);
    cs.cds.pParam=NULL;

    int count=0;
    do {                      // may need to spin awhile until system recognizes attached hubs
      usleep(100000);
      g4_set_query(&cs);
    } while ((cs.cds.iParam==0) && (++count<150));

    if (cs.cds.iParam==0){
      m_bConnected=false;
      g4_close_tracker();
      return false;
    }

    usleep(500000);    // one more time to catch any stragglers
    g4_set_query(&cs);

    m_numHubs=cs.cds.iParam;

    m_hubList=new int[m_numHubs];
    cs.cds.pParam=m_hubList;
    g4_set_query(&cs);


    usleep(100000);

    // set units
    // int u=G4_TYPE_INCH;
    // cs.cmd=G4_CMD_UNITS;
    // cs.cds.action=G4_ACTION_SET;
    // cs.cds.iParam=G4_DATA_POS;
    // cs.cds.pParam=&u;
    // int xx=g4_set_query(&cs);
		
    // set up struct for data acquisition
    m_pframeData=new G4_FRAMEDATA[m_numHubs];
    g4_get_frame_data(m_pframeData,m_sysId,m_hubList,m_numHubs);

    // get source qty and locations
    cs.cmd=G4_CMD_GET_SOURCE_MAP;
    cs.cds.id=G4_CREATE_ID(m_sysId,0,0);
    cs.cds.pParam=NULL;
    g4_set_query(&cs);
    m_numSrc=cs.cds.iParam;

    m_srcLoc=new float[m_numSrc][6];

    // build a structure
    LPG4_SRC_MAP map=new G4_SRC_MAP[m_numSrc];
    cs.cds.pParam=map;
    cs.cds.iParam=(G4_TYPE_INCH<<16)|G4_TYPE_EULER_DEGREE;
    g4_set_query(&cs);
    for (int i=0;i<m_numSrc;i++)
      memcpy(m_srcLoc[i],map[i].pos,sizeof(float)*6);

    delete[] map;
    return true;
  }

  return false;

}
//#include <stdio.h>
int CG4Trk::GetHubsPno(REND_STRUCT* prs)
{
  // get all pno
  int rv=g4_get_frame_data(m_pframeData,m_sysId,m_hubList,m_numHubs);
  int num_read=rv&0xffff;

  for (int i=0;i<num_read;i++){
    for (int a=0;a<m_numHubs;a++){
      if ((unsigned int)prs->hubList[a].GetId()==m_pframeData[i].hub){
	prs->hubreadmap|=(0x01<<a);
	for (int j=0;j<G4_SENSORS_PER_HUB;j++){
	  if (m_pframeData[i].stationMap&(0x01<<j))
	    prs->hubList[a].SetPnoData(m_pframeData[i].sfd[j].pos,j);	// set it
	}	// end for j
	break;
      }	// end if
    }	// end for a
  }	// end for i




  // /////////////////////////
  // for (int i=0;i<numHubs;i++){		// for ea hub asked for
  //   for (int a=0;a<m_numHubs;a++){		// check against all hubs we're tracking
  //     if (hubarr[i].GetId()==(int)m_pframeData[a].hub){	// a match
  // 	if (m_pframeData[a].frame!=0xffffffff){			// make sure new data
  // 	  for (int j=0;j<G4_SENSORS_PER_HUB;j++){	
  // 	    if (m_pframeData[a].stationMap&(0x01<<j))	// for ea active sensor
  // 	      hubarr[i].SetPnoData(m_pframeData[a].sfd[j].pos,j);	// set it

  // 	  }	// end for j
  // 	}// end if
  //     }// end if
  //   }// end for a
  // }// end for i


  return rv>>16;	// return total hubs on system
}


int CG4Trk::GetActHubs()
{
  return m_numHubs;
}

void CG4Trk::GetHubList(int *list, int numEl)
{
  for (int i=0;i<numEl;i++){
    if (i<m_numHubs)
      list[i]=m_hubList[i];
  }

}

void CG4Trk::GetSrcLoc(float (*loc)[6])
{
  memcpy(loc,m_srcLoc,sizeof(float)*m_numSrc*6);
}	

int CG4Trk::GetNumSrc()
{
  return m_numSrc;

}

void CG4Trk::Boresight(bool enable)
{
  float angle[3]={0.0f,0.0f,0.0f}	;
  G4_CMD_STRUCT cs;
  cs.cmd=G4_CMD_BORESIGHT;
  cs.cds.id=G4_CREATE_ID(m_sysId,-1,0);	// all hubs,all sensors
  cs.cds.action=enable?G4_ACTION_SET:G4_ACTION_RESET;	// boresight or unboresight?
  cs.cds.iParam=G4_TYPE_EULER_DEGREE;
  cs.cds.pParam=angle;
  g4_set_query(&cs);
}

void CG4Trk::GetFilterValues(float *pfilt, float *ofilt)
{
  G4_CMD_STRUCT cs;
  cs.cmd=G4_CMD_FILTER;
  cs.cds.id=G4_CREATE_ID(m_sysId,m_hubList[0],0);
  cs.cds.action=G4_ACTION_GET;
  cs.cds.iParam=G4_DATA_POS;
  cs.cds.pParam=pfilt;
  g4_set_query(&cs);

  cs.cds.iParam=G4_DATA_ORI;
  cs.cds.pParam=ofilt;
  g4_set_query(&cs);

}

void CG4Trk::SetFilterValues(float *pfilt, float *ofilt)
{
  G4_CMD_STRUCT cs;
  cs.cmd=G4_CMD_FILTER;
  cs.cds.id=G4_CREATE_ID(m_sysId,-1,0);
  cs.cds.action=G4_ACTION_SET;
  cs.cds.iParam=G4_DATA_POS;
  cs.cds.pParam=pfilt;
  g4_set_query(&cs);

  cs.cds.iParam=G4_DATA_ORI;
  cs.cds.pParam=ofilt;
  g4_set_query(&cs);

}

void CG4Trk::GetSetIncr(float &posIncr, float &oriIncr,bool bSet)
{
  G4_CMD_STRUCT cs;
  cs.cmd=G4_CMD_INCREMENT;
  float fIncr[2];

  if (bSet){
    cs.cds.id=G4_CREATE_ID(m_sysId,-1,0);
    cs.cds.action=G4_ACTION_SET;
    fIncr[0]=posIncr;
    fIncr[1]=oriIncr;
  }
  else {
    cs.cds.id=G4_CREATE_ID(m_sysId,m_hubList[0],0);
    cs.cds.action=G4_ACTION_GET;
  }
  cs.cds.iParam=(G4_TYPE_INCH<<16)|G4_TYPE_EULER_DEGREE;
  cs.cds.pParam=fIncr;
  g4_set_query(&cs);

  if (!bSet){
    posIncr=fIncr[0];
    oriIncr=fIncr[1];
  }



}

int CG4Trk::UpdateHubs()
{
  // get a list of hubs
  G4_CMD_STRUCT cs;
  cs.cmd=G4_CMD_GET_ACTIVE_HUBS;
  cs.cds.id=G4_CREATE_ID(m_sysId,0,0);
  cs.cds.pParam=NULL;
  g4_set_query(&cs);

  if ((int)cs.cds.iParam!=m_numHubs){
    m_numHubs=cs.cds.iParam;
    delete[] m_hubList;
    delete[] m_pframeData;
    m_hubList=new int[m_numHubs];
    m_pframeData=new G4_FRAMEDATA[m_numHubs];
    cs.cds.pParam=m_hubList;
    g4_set_query(&cs);
  }

  return m_numHubs;


}
