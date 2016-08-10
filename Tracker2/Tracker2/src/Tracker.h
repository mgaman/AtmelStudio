/*
	One Byte serial protocol between host and tracker
*/

// Host to Tracker
#define GetFlashOffset  0x7E
#define VarioCalibReceived 0x82
#define LSM96D0CalibReceived 0x84
#define GetFlashData  0xE0
#define StopSendingFlashData  0xE2
#define VarRadioButton  0xE6
#define LSMRadioButton  0xE8
#define GPSRadioButton  0xEA
#define CleanFlash  0xEC
#define RecordToFlash  0xEE
#define StopRecordingToFlash  0xF0
#define WriteOneRowToFlash  0xF2
#define AllSensorRadioButton  0xF7
#define ConnectToTracker  0xFE

// Tracker to Host
#define StartFlashOffset 0x7b
#define EndFlashOffset 0x7D
#define MemoryFullAck 0x80
#define StartVarioDataToHost 0xF6
#define EndVarioDataToHost 0x40
#define FlashPageCountToHost 0xc1
#define StartVarioCalibToHost 0xD1
#define EndVarioCalibToHost 0xD3
#define StartLSMDataToHost 0xd5
#define EndLSMDataToHost 0xd7
#define StartLSMCalibToHost 0xd9
#define EndLSMCalibToHost 0xdb
#define AckGetFlashData 0xE1
#define AckStopFlashData 0xE3
#define NoMoreFlashData 0xE4
#define StartLocationMessage 0xE5
#define EndLocationMessage 0xE6
#define VarButtonAck 0xE7
#define LSMButtonAck 0xE9
#define GPSButtonAck 0xEB
#define CleanFlashAck 0xED
#define RecordToFlashAck 0xEF
#define StopRecordingFlashAck 0xF1
#define WriteOneRowAck 0xF3
#define EndFlashDataToHost 0xF4
#define AllSensorsAck 0xF8
#define TelemBlockToFLash 0xFA
#define StartFlashDataToHost 0xFC
#define AckFromTracker 0xFD

//#define TIMER_RATE 1000 // Hz
//#define USART_SERIAL_PC_BAUD   115200