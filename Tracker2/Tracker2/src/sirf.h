/*
   SIRF related structures and functions
*/

//#pragma pack(1)
struct sSirfHeader {
  uint8_t a0;
  uint8_t a2;
  uint8_t szmsb;  // size big-endian
  uint8_t szlsb;
  uint8_t MID;
};
struct sSirfTrailer {
  uint16_t crc;  // big-endian
  uint8_t b0;
  uint8_t b3;
};
// Messages to GPS
struct sMID4 {  // Nav message
  struct sSirfHeader header;
  int32_t XPos,YPos,ZPos; 
  int16_t Xvel,Yvel,Zvel;
  uint8_t Mode,HDOP2,Mode2;
  uint16_t GPSweek;
  uint32_t GPSTOW;
  uint8_t SVinFix;
  uint8_t CHPRN[12];
  struct sSirfTrailer trailer;
};

struct sMID132 {  // poll software version
  struct sSirfHeader header;
  uint8_t filler;
  struct sSirfTrailer trailer;
};

struct sMID136 {   // Mode control
  struct sSirfHeader header;
  uint16_t spare1;
  uint8_t DegradedMode;
  uint8_t PositionCalcMode;
  uint8_t spare2;
  int16_t Altitude;
  uint8_t AltHoldMode;
  uint8_t AltHoldSource;
  uint8_t spare3;
  uint8_t DegradedTimeOut;
  uint8_t DRTimeOut;
  uint8_t MeasurementTrackSmoothing;
  struct sSirfTrailer trailer;
};

struct sMID152 {  // poll nav parameters
  struct sSirfHeader header;
  uint8_t filler;
  struct sSirfTrailer trailer;
};

struct sMID166 {   // set message rate
  struct sSirfHeader header;
  uint8_t mode;
  uint8_t mid;
  uint8_t rate;
  uint8_t filler[4];
  struct sSirfTrailer trailer;
};

// Messages from GPS
struct sMID11 {
  struct sSirfHeader header;
  unsigned char SID;
  unsigned char ACIID;
  struct sSirfTrailer trailer;
};

struct sMID41 {
  struct sSirfHeader header;
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
  struct sSirfTrailer trailer;
};

struct sMID65 {
  struct sSirfHeader header;
  unsigned char SID;
  unsigned short GPIOstate;
  struct sSirfTrailer trailer;
};

//#pragma pack()

enum eMsgMode {ENABLEDISABLESINGLE=0,POLLONE,ENABLEDISABLEALL,ENABLEDISABLENAV,ENABLEDISABLEDEBUG,ENABLEDISABLENAVDEBUG};
enum eParseState {A0, A2, SZMSB, SZLSB, PAYLOAD, CRC1, CRC2, B0, B3};
// enable/disable messages
struct sMID166 *MID166(enum eMsgMode mode, uint8_t mid, uint8_t rate);
// calculate CRC of a buffer
uint16_t SirfCRC(uint8_t *msg,int msgLength, bool bswap);
// poll SW version
struct sMID132 *MID132(void);
// poll Nav parameters
struct sMID152 *MID152(void);
// parser init
void SirfParseInit(void);
// Mode control
struct sMID136 *MID136(void);
// true when anykind of block is ready
bool SirfBlockReady(uint8_t c);
// get completed block
struct sSirfHeader *SirfData(void);
// OSF numbers are big endian so we need to convert
uint32_t BELE4(uint32_t s);
uint16_t BELE2(uint16_t s);
