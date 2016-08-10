/*
   Common Telemetry definitions and types for all sensors
*/

enum eTelemTypes { LSM9DS0_ALL = 24 ,LSM9DS0_CALIB,MS561101BA03,MS561101BA03_CALIB, NMEA_TYPE}; // use same as Miznach
#pragma pack(1)
struct sTelemHeader {
	uint8_t type;
	uint8_t ID;
	uint32_t timestamp;
	};
	
struct sNMEA_Data {
	struct sTelemHeader header;
	uint8_t length;
	uint8_t sentence[100];	// more than enough, will only write actual amount to host or flash
	};
	
struct sLSM9DS0_Data {
	struct sTelemHeader header;
	int16_t Accelerometer[3];
	int16_t Magnetometer[3];	
	int16_t Gyro[3];	// X,Y,Z
	};
	
struct sMS5611_Calibration {
	struct sTelemHeader header;
	uint16_t C[6];
};

struct sMS5611_Data {
	struct sTelemHeader header;
	uint32_t D1;
	uint32_t D2;	
	};
	
#pragma pack(0)
	
// prototypes
bool TelemetryWriteToFlash(enum eTelemTypes type,void *rawData); // return true for success else false
void TelemetryReset();
