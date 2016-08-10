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
  unsigned short NavValid;		// 0
  unsigned short NavType;			// 2
  unsigned short ExtendedWeekNumber;	// 4
  unsigned long TOW;				// 6
  unsigned short UTCYear;			// 10
  unsigned char  UTCMonth;		// 12
  unsigned char  UTCDay;			// 13
  unsigned char  UTCHour;			// 14
  unsigned char  UTCMinute;		// 15
  unsigned short  UTCSecond;		// 16
  unsigned long  SatelliteList;	// 18
  long  Latitude;					// 22
  long  Longitude;				// 26
  long  AltitudeFromEllipsoid;	// 30
  long  AltitudeFromMSL;			// 34
  unsigned char Datum;			// 38
  unsigned short  SOG;			// 39
  unsigned short  COG;			// 41
  short MagVar;					// 43
  short ClimbRate;				// 45
  short HeadingRate;				// 47
  unsigned long EHPE;				// 49
  unsigned long EVPE;				// 53
  unsigned long EstimatedTimeError; // 57
  unsigned short EHVE;			// 61
  long ClockBias;					// 65
  unsigned long ClockBiasError;	// 67
  long ClockDrift;				// 71
  unsigned long ClockDriftError;	// 75
  long Distance;					// 79
  unsigned short DistanceError;	// 83
  unsigned short HeadingError;	// 85
  unsigned char NumSVinFix;		// 87
  unsigned char HDOP;				// 88
  unsigned char AdditionalModeInfo; // 89
};

struct sLSM9SD0_Calib {
	struct sTelemHeader header;
	uint8_t G[5];   // control registers
	uint8_t XM[8];
};

//#pragma pack()
	
// prototypes
bool TelemetryWriteToFlash(enum eTelemTypes type,void *rawData); // return true for success else false
void TelemetryReset(void);
