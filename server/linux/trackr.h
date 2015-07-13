
#define G4C_PATH "/projects/polhemus/dual_source_config.g4c"

#define CONNECT_FAILURE -1
#define SETUP_FAILURE -2
#define START_POLLING_FAILURE -3

#define PRINT_RECORDS false

#define G4_SOCKET_PORT 1986

#define G4_MAX_SENSORS_PER_HUB 96 

struct po_req {
	bool sensor_0;
	bool sensor_1;
	bool sensor_2;
};

struct po_sample {
	int frame_number;
	float pos[3];
	float ori[3];
} ;