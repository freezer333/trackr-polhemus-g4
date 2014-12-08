/************************************************************************

		POLHEMUS PROPRIETARY

		Polhemus 
		P.O. Box 560
		Colchester, Vermont 05446
		(802) 655-3159



        		
	    Copyright © 2009 by Polhemus
		All Rights Reserved.


*************************************************************************/

// G4TrackIncl.h


#ifndef _G4TRACKINCL_H_
#define _G4TRACKINCL_H_

#include "stdint.h"		// for uint32_t -- unfortunately compiler specific

// defines


#define G4_SENSORS_PER_HUB	 3
#define G4_CREATE_ID(sys,hub,sensor)	(((sys<<24)&0xff000000)|((hub<<8)&0x0fff00)|(sensor&0x7f))
#define G4_CREATE_ID_SENS_MAP(sys,hub,sensorMap)	G4_CREATE_ID(sys,hub,sensorMap)|0x80


// enum definitions

// commands
enum {
  G4_CMD_WHOAMI,
  G4_CMD_GETMAXSRC,
  G4_CMD_BORESIGHT,
  G4_CMD_FILTER,
  G4_CMD_INCREMENT,
  G4_CMD_FOR_ROTATE,
  G4_CMD_FOR_TRANSLATE,
  G4_CMD_TIP_OFFSET,
  G4_CMD_UNITS,
  G4_CMD_GET_ACTIVE_HUBS,
  G4_CMD_GET_STATION_MAP,
  G4_CMD_GET_SOURCE_MAP,
  G4_CMD_FRAMERATE,
  G4_CMD_RESTORE_DEF_CFG,
  G4_CMD_BLOCK_CFG,
  G4_TOTAL_COMMANDS
};

// different actions
enum {G4_ACTION_SET,G4_ACTION_GET,G4_ACTION_RESET};

// unit types
enum dataType {G4_DATA_POS,G4_DATA_ORI};


// types
enum {
  G4_TYPE_EULER_DEGREE,
  G4_TYPE_EULER_RADIAN,
  G4_TYPE_QUATERNION,
  G4_TYPE_INCH,
  G4_TYPE_FOOT,
  G4_TYPE_CM,
  G4_TYPE_METER,
};


// errors -- just the beginning
enum {
  G4_ERROR_NONE,
  G4_ERROR_NO_FRAME_DATA_AVAIL=-100,
  G4_ERROR_UNSUPPORTED_ACTION,
  G4_ERROR_UNSUPPORTED_TYPE,
  G4_ERROR_UNSUPPORTED_COMMAND,
  G4_ERROR_INVALID_STATION,
  G4_ERROR_NO_CONNECTION,				//-95
  G4_ERROR_NO_HUBS,
  G4_ERROR_FRAMERATE_SET,
  G4_ERROR_MEMORY_ALLOCATION,
  G4_ERROR_INVALID_SYSTEM_ID,
  G4_ERROR_SRC_CFG_FILE_OPEN,			//-90
  G4_ERROR_INVALID_SRC_CFG_FILE,
  G4_ERROR_UNABLE_TO_START_TIMER,
  G4_ERROR_HUB_NOT_ACTIVE,
  G4_ERROR_SYS_RESET_FAIL,
  G4_ERROR_DONGLE_CONNECTION,			//-85
  G4_ERROR_DONGLE_USB_CONFIGURATION,
  G4_ERROR_DONGLE_USB_INTERFACE_0,
  G4_ERROR_DUPLICATE_SYS_IDS,
  G4_ERROR_INVALID_WILDCARD_USE,
  G4_ERROR_TOTAL
};


// structure definitions

// for giving pno data to host app
typedef struct _G4_SEN_FRAMEDATA {		// per sensor data
  uint32_t id;
  float pos[3];
  float ori[4];
}*LPG4_SENFRAMEDATA,G4_SENFRAMEDATA;		//32 bytes

typedef struct _G4_FRAMEDATA {		// all sensors in hub
  uint32_t hub;
  uint32_t frame;
  uint32_t stationMap;	// indicates active sensors on hub
  uint32_t dig_io;
  G4_SENFRAMEDATA sfd[G4_SENSORS_PER_HUB];
}*LPG4_FRAMEDATA,G4_FRAMEDATA;				




// for receiving config commands from host app or SDK
typedef struct _G4_CMD_DATA_STRUCT{
  uint32_t id;			// -1 for all sensors
  uint32_t action;		// set,get,reset
  uint32_t iParam;		// command specific
  void* pParam;			// ditto
}*LPG4_CMD_DATA_STRUCT,G4_CMD_DATA_STRUCT;


typedef struct _G4_CMD_STRUCT {
  int cmd;
  G4_CMD_DATA_STRUCT cds;
}*LPG4_CMD_STRUCT,G4_CMD_STRUCT;

typedef struct _G4_SRC_MAP {
  int id;
  int freq;
  float pos[3];
  float att[4];
}*LPG4_SRC_MAP,G4_SRC_MAP;

typedef struct _G4_CMD_BLOCK_STRUCT {
  int units[2];			// pos, att
  uint8_t version_info[50];
  float filter_params[2][4];	// pos,att
  float increment[G4_SENSORS_PER_HUB][2];		// pos, att
  float rot_angles[3];	// az, el, rl
  float translate_xyz[3];
  float tip_offset[G4_SENSORS_PER_HUB][3];
}*LPG4_CMD_BLOCK_STRUCT,G4_CMD_BLOCK_STRUCT;


#ifdef __cplusplus
extern "C" {
#endif

  // function prototypes /////////////////////////////////////////////////////


  // Function name	: g4_init_sys
  // Description        : Initializes the G4 Tracker
  // Return type	: int - ERROR_NONE if successful, otherwise an error enum
  // Argument		: int* pDongleId - System id of the dongle will be returned here
  // Argument	        : const char* src_cfg_file - the source configuration file.
  // Argument		: void* reserved - Should always be NULL
  uint32_t g4_init_sys(int* pDongleId,const char* src_cfg_file,void* reserved);



  // Function name	: g4_get_frame_data
  // Description	: Gets the G4 frame data from a list of desired hubs
  // Return type	: int -- Upper 16-bit word is total hubs on system
  //				Lower 16-bit word is hubs successfully read
  // Argument		: FRAMEDATA* fd_array -- an array of FRAMEDATA structures to hold the data
  // Argument		: int sysId -- The system id
  // Argument		: const int* hub_id_list -- An array of hub id's that the frame data is requested.
  // Argument		: int num_hubs -- the number of ids in the hub_id_list
  uint32_t g4_get_frame_data(G4_FRAMEDATA* fd_array,int sysId,const int* hub_id_list,int num_hubs);	




  // Function name	: g4_set_query
  // Description	    : The function to get or set the G4 Tracker parameters
  // Return type		: int - ERROR_NONE if successful, otherwise an error enum
  // Argument         : LPCMD_STRUCT pcs - The command structure.  The values of this structure
  //											are dependent upon the actual command.
  uint32_t g4_set_query(LPG4_CMD_STRUCT pcs);




  // Function name	: close_tracker
  // Description	    : This function will close the tracker connection.
  // Return type		: void 
  // Argument         : void
  void g4_close_tracker(void);

#ifdef __cplusplus
}
#endif
#endif
