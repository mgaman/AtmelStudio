/*
   Common Telemetry definitions and types for all sensors
*/

//                         24        25            26           27  
enum eTelemTypes { LSM9DS0_ALL = 24 ,LSM9DS0_CALIB,MS561101BA03,MS561101BA03_CALIB, // use same as Miznach
					//   28         29        30              31          32           33          34
					BATTERY_CURRENT,LSM9DS0_M,MMC_INU_CONTROL,MMC_INU_NAV,MMC_INU_1PPS,MMC_INU_MLL,MID41,NUM_TYPES};
//#pragma pack(1)
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
	
struct sMID41_Data {
	struct sTelemHeader header;
	int32_t Longitude;
	int32_t Latitude;
	int32_t Altitude;
	uint16_t UTCYear;
	uint8_t  UTCMonth;	
	uint8_t  UTCDay;	
	uint8_t  UTCHour;		
	unsigned char  UTCMinute;
	uint16_t  UTCSecond;	
	uint8_t HDOP;
	};
//#pragma pack(0)
	
// prototypes
bool TelemetryWriteToFlash(enum eTelemTypes type,void *rawData); // return true for success else false
void TelemetryReset(void);
