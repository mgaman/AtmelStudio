/*
 * i2c_lsm.h
 *
 * Created: 24-Nov-14 3:15:43 PM
 *  Author: User
 */ 


#ifndef I2C_LSM_H_
#define I2C_LSM_H_

////////////////////////////
// LSM9DS0 Gyro Registers //
////////////////////////////
#define WHO_AM_I_G			0x0F
#define CTRL_REG1_G			0x20
#define CTRL_REG2_G			0x21
#define CTRL_REG3_G			0x22
#define CTRL_REG4_G			0x23
#define CTRL_REG5_G			0x24
#define REFERENCE_G			0x25
#define STATUS_REG_G		0x27
#define OUT_X_L_G			0x28
#define OUT_X_H_G			0x29
#define OUT_Y_L_G			0x2A
#define OUT_Y_H_G			0x2B
#define OUT_Z_L_G			0x2C
#define OUT_Z_H_G			0x2D
#define FIFO_CTRL_REG_G		0x2E
#define FIFO_SRC_REG_G		0x2F
#define INT1_CFG_G			0x30
#define INT1_SRC_G			0x31
#define INT1_THS_XH_G		0x32
#define INT1_THS_XL_G		0x33
#define INT1_THS_YH_G		0x34
#define INT1_THS_YL_G		0x35
#define INT1_THS_ZH_G		0x36
#define INT1_THS_ZL_G		0x37
#define INT1_DURATION_G		0x38

//////////////////////////////////////////
// LSM9DS0 Accel/Magneto (XM) Registers //
//////////////////////////////////////////
#define OUT_TEMP_L_XM		0x05
#define OUT_TEMP_H_XM		0x06
#define STATUS_REG_M		0x07
#define OUT_X_L_M			0x08
#define OUT_X_H_M			0x09
#define OUT_Y_L_M			0x0A
#define OUT_Y_H_M			0x0B
#define OUT_Z_L_M			0x0C
#define OUT_Z_H_M			0x0D
#define WHO_AM_I_XM			0x0F
#define INT_CTRL_REG_M		0x12
#define INT_SRC_REG_M		0x13
#define INT_THS_L_M			0x14
#define INT_THS_H_M			0x15
#define OFFSET_X_L_M		0x16
#define OFFSET_X_H_M		0x17
#define OFFSET_Y_L_M		0x18
#define OFFSET_Y_H_M		0x19
#define OFFSET_Z_L_M		0x1A
#define OFFSET_Z_H_M		0x1B
#define REFERENCE_X			0x1C
#define REFERENCE_Y			0x1D
#define REFERENCE_Z			0x1E
#define CTRL_REG0_XM		0x1F
#define CTRL_REG1_XM		0x20
#define CTRL_REG2_XM		0x21
#define CTRL_REG3_XM		0x22
#define CTRL_REG4_XM		0x23
#define CTRL_REG5_XM		0x24
#define CTRL_REG6_XM		0x25
#define CTRL_REG7_XM		0x26
#define STATUS_REG_A		0x27
#define OUT_X_L_A			0x28
#define OUT_X_H_A			0x29
#define OUT_Y_L_A			0x2A
#define OUT_Y_H_A			0x2B
#define OUT_Z_L_A			0x2C
#define OUT_Z_H_A			0x2D
#define FIFO_CTRL_REG		0x2E
#define FIFO_SRC_REG		0x2F
#define INT_GEN_1_REG		0x30
#define INT_GEN_1_SRC		0x31
#define INT_GEN_1_THS		0x32
#define INT_GEN_1_DURATION	0x33
#define INT_GEN_2_REG		0x34
#define INT_GEN_2_SRC		0x35
#define INT_GEN_2_THS		0x36
#define INT_GEN_2_DURATION	0x37
#define CLICK_CFG			0x38
#define CLICK_SRC			0x39
#define CLICK_THS			0x3A
#define TIME_LIMIT			0x3B
#define TIME_LATENCY		0x3C
#define TIME_WINDOW			0x3D
#define ACT_THS				0x3E
#define ACT_DUR				0x3F


//#define LSM9DS0_XM  0x1D    // Would be 0x1E if SDO_XM is LOW
//#define LSM9DS0_XM  0x1E    // Would be 0x1E if SDO_XM is LOW
// SEE DATASHEET Table 15
#define SDO_XM   1 // tied to high
#if SDO_XM == 0
#define LSM9DS0_XM   0x1E
#else
#define LSM9DS0_XM   0x1D
#endif

//#define SDO_G  0     // Tied to low
#define SDO_G  1     // Tied to high
#define LSM9DS0_G   ((0x35<<1) | SDO_G)  //6B
// CTRL_REG1_G
#define PD 0x08
#define ZEN 0x04
#define XEN 0x02
#define YEN 0x01
// apply directly to register without mask
#define G_ODR_95_BW_125   0x00 //   95         12.5
#define G_ODR_95_BW_25    0x10  //   95          25
//   0x2 and 0x3 define the same data rate and bandwidth
#define G_ODR_190_BW_125  0x40  //   190        12.5
#define G_ODR_190_BW_25   0x50  //   190         25
#define G_ODR_190_BW_50   0x60  //   190         50
#define G_ODR_190_BW_70   0x70  //   190         70
#define G_ODR_380_BW_20   0x80  //   380         20
#define G_ODR_380_BW_25   0x90  //   380         25
#define G_ODR_380_BW_50   0xA0  //   380         50
#define G_ODR_380_BW_100  0xB0  //   380         100
#define G_ODR_760_BW_30   0xC0  //   760         30
#define G_ODR_760_BW_35   0xD0  //   760         35
#define G_ODR_760_BW_50   0xE0  //   760         50
#define G_ODR_760_BW_100  0xF0  //   760         100
// CTRL_REG3_G
#define I2_DRDY 0x08
#define I1_INT1 0x80
// CTRL_REG4_G
//------------------------------------------------------------
#define G_SCALE_245DPS	0x00	//  +/- 245 degrees per second
#define G_SCALE_500DPS	(0x01<<4)	//  +/- 500 dps
#define G_SCALE_2000DPS	(0x10<<4)    //  +/- 2000 dps
#define BDU 0x80
// STATUS_REG_G
#define XYZDA 0x08  // XYZ data ready


// CTRL_REG1_XM
#define AXEN 0x01
#define AYEN 0x02
#define AZEN 0X04
#define A_POWER_DOWN (0x00<<4) 	// Power-down mode (0x0)
#define A_ODR_3125   (0x01<<4)	// 3.125 Hz	(0x1)
#define A_ODR_625    (0x02<<4)	// 6.25 Hz (0x2)
#define A_ODR_125    (0x03<<4)	// 12.5 Hz (0x3)
#define A_ODR_25     (0x04<<4)	// 25 Hz (0x4)
#define A_ODR_50     (0x05<<4)	// 50 Hz (0x5)
#define A_ODR_100    (0x06<<4)	// 100 Hz (0x6)
#define A_ODR_200    (0x07<<4)	// 200 Hz (0x7)
#define A_ODR_400    (0x08<<4)	// 400 Hz (0x8)
#define A_ODR_800    (0x09<<4)	// 800 Hz (9)
#define A_ODR_1600   (0x0A<<4)	// 1600 Hz (0xA)
// CTRL_REG2_XM
#define A_SCALE_2G  (0x00<<3)	// 000: +/- 2g
#define A_SCALE_4G  (0x01<<3)	// 001: +/- 4g
#define A_SCALE_6G  (0x02<<3)    // 010: +/- 6g
#define A_SCALE_8G  (0x03<<3)    // 011: +/- 8g
#define A_SCALE_16G (0x04<<3)    // 100: +/- 16g
// CTRL_REG3_XM
#define P1_DRDYM 0X02
#define P1_DRDYA 0X04
// CTRL_REG4_XM
#define P2_DRDYM 0X04
#define P2_DRDYA 0X08
// CTRL_REG5_XM
#define M_ODR_3125 	(0x00<<2)   // 3.125 Hz
#define M_ODR_625	(0x01<<2)   // 6.25 Hz
#define M_ODR_125	(0x02<<2)   // 12.5 Hz
#define M_ODR_25	(0x03<<2)   // 25 Hz
#define M_ODR_50	(0x04<<2)   // 50
#define M_ODR_100	(0x05<<2)   // 100 Hz
#define M_HI_RES        (0x03<<5)
#define M_LO_RES        (0x00<<5)
// CTRL_REG6_XM
#define M_SCALE_2GS  (0x00<<5)	//  +/- 2Gs
#define M_SCALE_4GS  (0x01<<5)	//  +/- 4Gs
#define M_SCALE_8GS  (0x02<<5)	//  +/- 8Gs
#define M_SCALE_12GS (0x03<<5)	//  +/- 12Gs
// status_reg_a
#define ZYXADA 0x08
// status_reg_m
#define ZYXMDA 0x08
// INT_CTRL_REG_M
#define MIEN 1   // enable
#define IEA 0x08 // active high
#define XMIEN 0x80
#define YMIEN 0x40
#define ZMIEN 0x20

#define LSM9DS0_GYRO_ID 0xD4  // from WHO_AM_I_G
#define LSM9DS0_XM_ID 	0x49  // from WHO_AM_I_XM


bool new_lsm_init_check();
//void new_get_lsm_data();
bool LSM9DS0GReady();
void LSM9DS0Handler();
void LSM9DS0Reset();
#endif /* I2C_LSM_H_ */

#ifdef LSM9DS0_MAIN
bool LSMCalibRcvd = false;
bool LSMCalibWrittenToTelem = true;
#else
extern bool LSMCalibRcvd;
extern bool LSMCalibWrittenToTelem;
#endif
