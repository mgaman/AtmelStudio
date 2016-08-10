/*
 * i2c_bar.h
 *
 * Created: 28-Dec-14 9:07:05 AM
 *  Author: User
 */ 


#ifndef I2C_BAR_H_
#define I2C_BAR_H_


bool init_bar();
void read_bar_data();
void n_send_vario_raw_data_to_host();
void add_vario_data_to_flash_buff();
void MS561101BA03Handler();
void MS561101BA03Reset();
#endif /* I2C_BAR_H_ */
#ifdef MS5611_MAIN
bool VarioCalibRcvd = false;
bool VarioCalibWrittenToTelem = false;
uint32_t varRowsRecorded = 0;
#else
extern bool VarioCalibRcvd;
extern bool VarioCalibWrittenToTelem;
extern uint32_t varRowsRecorded;
#endif
