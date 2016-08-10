/*
 * I2CGps.h
 *
 * Created: 7/1/2014 2:46:14 PM
 *  Author: User
 */ 


#ifndef I2CGPS_H_
#define I2CGPS_H_

#define ORG1140_ADDR 0x60

#define ADDR_GPS_W (ORG1140_ADDR<<1)
#define ADDR_GPS_R (ORG1140_ADDR<<1) | 0x01

//extern char fromGps[100];

extern char measuredUtcTime[10];
extern char measuredLatitude[9];
extern char measuredLongitude[10];
extern char measuredAltitude[8];
extern char measuredSatelites[2];


uint8_t gps_activation(void);
void enable_gprmc(void);
void disable_gprmc(void);


void get_gps_data(void);


#endif /* I2CGPS_H_ */