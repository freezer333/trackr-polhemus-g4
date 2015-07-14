// struct.h

#ifndef STRUCT_H_
#define STRUCT_H_

class CG4Trk;
class CG4Hub;


typedef struct _REND_STRUCT {
  int numSrc;
  int numHub;
  int hubreadmap;
  int tot_hubs_on_system;
  float (*srcList)[6];
  CG4Hub* hubList;
  float viewTrans[3];
  CQuaternion viewRot;
  CG4Trk* pTrk;
  float srcScale;
  float senScale;
  int counter;
}REND_STRUCT;


#endif
