/*
   Initialize and run the ORG1411 GPS
*/

void ORG1411Reset(void);
void ORG1411Setup(void);
//void ORG1411Handler(void);
bool ORG1410_Init(void);
void ORG1411_Test(void);
struct sGPGGA {
	unsigned long time;
	unsigned long latitude;
	unsigned long latitudep;
	int NS;
	unsigned long longitude;
	unsigned long longitudep;
	int EW;
	int fix;
	int NOS;
	int HDOP;
	long altitude;
	unsigned long altitudep;
};