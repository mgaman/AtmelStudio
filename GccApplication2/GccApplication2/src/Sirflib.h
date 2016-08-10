/*
   SIRF related structures and functions
*/

struct sSirfHeader {
  uint8_t a0;
  uint8_t a2;
  uint8_t szmsb;  // size big-endian
  uint8_t szlsb;
};

struct sSirfTrailer {
  uint16_t crc;  // big-endian
  uint8_t b0;
  uint8_t b3;
};

struct sMID166 {   // set message rate
  struct sSirfHeader header;
  uint8_t MID;
  uint8_t mode;
  uint8_t mid;
  uint8_t rate;
  uint8_t filler[4];
  struct sSirfTrailer trailer;
};

struct sMID4 {  // Nav message
  struct sSirfHeader header;
  uint8_t MID;
  int32_t XPos,YPos,ZPos; 
  int16_t Xvel,Yvel,Zvel;
  uint8_t Mode,HDOP2,Mode2;
  uint16_t GPSweek;
  uint32_t GPSTOW;
  uint8_t SVinFix;
  uint8_t CHPRN[12];
  struct sSirfTrailer trailer;
};

struct sMID41 {  // Nav message
	struct sSirfHeader header;
	uint8_t MID;
	uint16_t NavValid,NavType,ExtendedWeekNumber;
	uint32_t TOW;
	uint16_t UTCYear;
	uint8_t UTCMonth,UTCDay,UTCHour,UTCMinute;
	uint16_t UTCSecond;
	uint32_t SatelliteISList;
	int32_t Latitude,Longitude,AltitudeEllipsoid,AltitudeMSL;
	uint8_t MapDatum; 
	uint16_t SOG,COG;
	int16_t MagVariation,ClimbRate,HeadingRate;
	uint32_t EHPE,EVPE,ETE;
	uint16_t EHVE;
	uint32_t ClockBias,ClockBiasError;
	int32_t ClockDrift;
	uint32_t ClockDriftError,Distance;
	uint16_t DistanceError,HeadingError;
	uint8_t NumSatellites,HDOP,AdditionalModeInfo;
	struct sSirfTrailer trailer;
};

enum eMsgMode {ENABLEDISABLESINGLE=0,POLLONE,ENABLEDISABLEALL,ENABLEDISABLENAV,ENABLEDISABLEDEBUG,ENABLEDISABLENAVDEBUG};
enum eParseState {A0, A2, SZMSB, SZLSB, PAYLOAD, CRC1, CRC2, B0, B3};
// enable/disable messages
struct sMID166 *MID166(enum eMsgMode mode, uint8_t mid, uint8_t rate);
// calculate CRC of a buffer
uint16_t SirfCRC(uint8_t *msg,int msgLength, enum eEndian finalEndian);
// parser init
void SirfParseInit(void);
// true when any kind of block is ready
bool SirfBlockReady(uint8_t c);
