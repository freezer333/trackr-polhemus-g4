/* G4SrcCfgIncl.h*/

#ifndef G4SRCCFGINCL_H_
#define G4SRCCFGINCL_H_

#define MAX_SOURCES  8
#define ATTR_LEN     64

#define G4_SRC_CFG_EULER_DEGREE 0
#define G4_SRC_CFG_EULER_RADIAN 1
#define G4_SRC_CFG_QUATERNION   2
#define G4_SRC_CFG_INCH         3
#define G4_SRC_CFG_FOOT         4
#define G4_SRC_CFG_CM           5
#define G4_SRC_CFG_METER        6

#define G4_SRC_CFG_FILE_OPEN_ERR  -200
#define G4_SRC_CFG_FILE_ERR       -199



typedef struct _G4_SRC_DATA_STRUCT {

  int sourceMap;				// 4
  char id[MAX_SOURCES][16];			// 128
  float pos_ori[MAX_SOURCES][7];		// 224
  unsigned char freq[MAX_SOURCES];		// 8
  unsigned char start_hem[MAX_SOURCES];		// 8
  unsigned char attr[MAX_SOURCES][ATTR_LEN];	// 512

}*LPG4_SRC_DATA_STRUCT,G4_SRC_DATA_STRUCT;	// 884 bytes


#ifdef __cplusplus
extern "C" {
#endif

  /* Function Prototypes */

  // Function name	: g4_read_source_cfg_file
  // Description        : Read a source cfg file and place data into a G4_SRC_DATA_STRUCT
  // Return type	: int - 0 for success, non-zero for failure
  // Argument           : int pos_units - the desired units for source position data.  Use
  //                      G4_SRC_CFG_INCH,G4_SRC_CFG_FOOT,G4_SRC_CFG_CM, or G4_SRC_CFG_METER
  // Argument           : int orientation units - the desired units for orienation.  Use
  //                      G4_SRC_CFG_EULER_DEGREE,G4_SRC_CFG_EULER_RADIAN, or G4_SRC_CFG_QUATERNION
  // Argument           : LPG4_SRC_DATA_STRUCT - Pointer to struct to receive file data. 
  // Argument           : filename - File to read. 
  int g4_read_source_cfg_file(int pos_units,int orientation_units,LPG4_SRC_DATA_STRUCT pds,const char* filename);

  // Function name	: g4_create_source_cfg_file
  // Description        : Create a source cfg file from data in a G4_SRC_DATA_STRUCT
  // Return type	: int - 0 for success, non-zero for failure
  // Argument           : int pos_units - the units for source position data.  Use
  //                      G4_SRC_CFG_INCH,G4_SRC_CFG_FOOT,G4_SRC_CFG_CM, or G4_SRC_CFG_METER
  // Argument           : int orientation units - the units for orienation.  Use
  //                      G4_SRC_CFG_EULER_DEGREE,G4_SRC_CFG_EULER_RADIAN, or G4_SRC_CFG_QUATERNION
  // Argument           : LPG4_SRC_DATA_STRUCT - Pointer to struct that contains data. 
  // Argument           : filename - File to create.
  int g4_create_source_cfg_file(int pos_unis,int orientation_units,LPG4_SRC_DATA_STRUCT pds,const char* filename);

  // Function name	: g4_get_freq_sn_from_attr
  // Description        : Extracts the source frequency and id from the attribute data
  // Return type	: int - the frequency of the source
  // Argument           : unsigned char* - The attribute string as read from the *.gs4 file.
  // Argument           : char* - a string to receive the serial number of the source.  May be NULL
  int g4_get_freq_sn_from_attr(unsigned char* attr,char* id);

#ifdef __cplusplus
}
#endif

#endif
