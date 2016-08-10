/*
   LSM9DS0 driver
   Uses I2C mode @ 400Khz
*/

#include <asf.h>
#include <string.h>
#include "LSM9DS0.h"
#include "Telemetry.h"
extern status_code_t master_status;

// pedantic prototypes
bool LSM6DS0MReady(void);
bool LSM6DS0GReady(void);
bool LSM6DS0AReady(void);
bool LSM9DS0readAccel(void);
bool LSM9DS0readMag(void);
bool LSM9DS0readGyro(void);



twi_package_t LSMpacket;
//uint16_t LSM9DS0rawdata[9];  //0-2 accel, 3-5 mag 6-8 gyro
struct sLSM9DS0_Data LSM9DS0Data;
struct sLSM9SD0_Calib LSM9DS0Calib;

// these defaults can be updated from eeprom
uint8_t ConfigGRegs[] = {
	G_ODR_95_BW_25|PD|XEN|YEN|ZEN,       //CTRL_REG1_G
	0,                                   //CTRL_REG2_G
	I2_DRDY | I1_INT1,                   //CTRL_REG3_G
	BDU,                                 //CTRL_REG4_G
	0                                    //CTRL_REG5_G
};

uint8_t ConfigXMRegs[] = {
	0,                            //CTRL_REG0_XM
	A_ODR_100|AXEN|AYEN|AZEN,     //CTRL_REG1_XM
	A_SCALE_2G,                   //CTRL_REG2_XM
	P1_DRDYA,                     //CTRL_REG3_XM
	P2_DRDYM,                     //CTRL_REG4_XM
	M_ODR_100|M_LO_RES,           //CTRL_REG5_XM
	0,                            //CTRL_REG6_XM
	0                             //CTRL_REG7_XM
};

// sanity check, read who am i registers
bool LSM9DS0GInit(void)
{
	uint8_t buf;
	LSMpacket.addr[0] = WHO_AM_I_G;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = &buf;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_G_SLAVE;
	LSMpacket.no_wait = false;
	master_status = twi_master_read(&TWI_MASTER, &LSMpacket);
	return master_status == STATUS_OK && buf == LSM9DS0_GYRO_ID;
}

bool LSM9DS0XMInit(void)
{
	uint8_t buf;
	LSMpacket.addr[0] = WHO_AM_I_XM;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = &buf;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_XM_SLAVE;
	LSMpacket.no_wait = false;
	master_status = twi_master_read(&TWI_MASTER, &LSMpacket);
	return master_status == STATUS_OK && buf == LSM9DS0_XM_ID;
}

/*
 write all config registers
 */
bool LSM9DS0Config(void)
{
	bool rc = false;
	int i;
	uint8_t test[8];
	LSMpacket.addr[0] = CTRL_REG1_G;
	LSMpacket.addr_length = 1;
	// update config regs from eeprom
	memcpy(LSM9DS0Calib.G,ConfigGRegs,sizeof(ConfigGRegs));
	memcpy(LSM9DS0Calib.XM,ConfigXMRegs,sizeof(ConfigXMRegs));
	LSMpacket.buffer = ConfigGRegs;
	LSMpacket.length = 1; //sizeof(ConfigGRegs);
	LSMpacket.chip = LSM9DS0_G_SLAVE;
	LSMpacket.no_wait = false;
	for (i=0;i<5;i++)
	{
		rc &= STATUS_OK == twi_master_write(&TWI_MASTER, &LSMpacket);
		LSMpacket.addr[0]++;
		LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
	}
//	if (STATUS_OK == twi_master_write(&TWI_MASTER, &LSMpacket))
	if (true)
	{
		memset(test,0xff,5);
		LSMpacket.buffer = test;
		LSMpacket.length = 1;
		LSMpacket.addr[0] = CTRL_REG1_G;
		for (i=0;i<5;i++)
		{
			twi_master_read(&TWI_MASTER, &LSMpacket);
			LSMpacket.addr[0]++;
			//LSMpacket.buffer++;
			LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
		}
		if (memcmp(ConfigGRegs,test,5) == 0)
		{
			LSMpacket.addr[0] = CTRL_REG0_XM;
			LSMpacket.addr_length = 1;
			LSMpacket.buffer = ConfigXMRegs;
			LSMpacket.length = 1;
			LSMpacket.chip = LSM9DS0_XM_SLAVE;
			LSMpacket.no_wait = false;
			for (i=0;i<8;i++)
			{
				rc = (STATUS_OK == twi_master_write(&TWI_MASTER, &LSMpacket));
				LSMpacket.addr[0]++;
				//LSMpacket.buffer++;
				LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
			}
			memset(test,0xff,8);
			LSMpacket.buffer = test;
			LSMpacket.length = 1;
			LSMpacket.addr[0] = CTRL_REG0_XM;
			for (i=0;i<8;i++)
			{
				twi_master_read(&TWI_MASTER, &LSMpacket);
				LSMpacket.addr[0]++;
				//LSMpacket.buffer++;
				LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
			}
			rc = (memcmp(ConfigXMRegs,test,8) == 0);			
		}
	}
	return rc;
}

/*
   When polling just test Gyro & assume others tie into this
*/
bool LSM6DS0GReady(void)
{
	uint8_t temp = 0;
	LSMpacket.addr[0] = STATUS_REG_G;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = &temp;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_G_SLAVE;
	LSMpacket.no_wait = false;
	master_status = twi_master_read(&TWI_MASTER, &LSMpacket);
	return (master_status == STATUS_OK && (temp & XYZDA) != 0);
}

bool LSM6DS0MReady(void)
{
	uint8_t temp = 0;
	LSMpacket.addr[0] = STATUS_REG_M;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = &temp;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_XM_SLAVE;
	LSMpacket.no_wait = false;
	master_status = twi_master_read(&TWI_MASTER, &LSMpacket);
	return (master_status == STATUS_OK && (temp & XYZDA) != 0);
}

bool LSM6DS0AReady()
{
	uint8_t temp = 0;
	LSMpacket.addr[0] = STATUS_REG_A;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = &temp;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_XM_SLAVE;
	LSMpacket.no_wait = false;
	master_status = twi_master_read(&TWI_MASTER, &LSMpacket);
	return (master_status == STATUS_OK && (temp & XYZDA) != 0);
}

bool LSM9DS0readAccel()
{
	int i;
	bool rc = true;
	LSMpacket.addr[0] = OUT_X_L_A;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = LSM9DS0Data.Accelerometer; //LSM9DS0rawdata;
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_XM_SLAVE;
	LSMpacket.no_wait = false;
	for (i=0;i<6;i++)
	{
		rc &= (STATUS_OK == twi_master_read(&TWI_MASTER, &LSMpacket));
		LSMpacket.addr[0]++;
		//LSMpacket.buffer++;
		LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
	}
	return rc;
}

bool LSM9DS0readMag()
{
	bool rc = true;
	int i;
	LSMpacket.addr[0] = OUT_X_L_M;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = LSM9DS0Data.Magnetometer; //&LSM9DS0rawdata[3];
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_XM_SLAVE;
	LSMpacket.no_wait = false;
	for (i=0;i<6;i++)
	{
		rc &= (STATUS_OK == twi_master_read(&TWI_MASTER, &LSMpacket));
		LSMpacket.addr[0]++;
		//LSMpacket.buffer++;
		LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
	}
	return rc;
}
	

bool LSM9DS0readGyro()
{
	bool rc = true;
	int i;
	LSMpacket.addr[0] = OUT_X_L_G;
	LSMpacket.addr_length = 1;
	LSMpacket.buffer = LSM9DS0Data.Gyro; //&LSM9DS0rawdata[6];
	LSMpacket.length = 1;
	LSMpacket.chip = LSM9DS0_G_SLAVE;
	LSMpacket.no_wait = false;
	for (i=0;i<6;i++)
	{
		rc &= (STATUS_OK == twi_master_read(&TWI_MASTER, &LSMpacket));
		LSMpacket.addr[0]++;
		//LSMpacket.buffer++;
		LSMpacket.buffer = (uint8_t *)LSMpacket.buffer+1;
	}
	return rc;	
}

bool LSM9DS0readAll(void)
{
	bool rc = true;
	while (!LSM6DS0AReady() && !LSM6DS0MReady()){}
	rc &= LSM9DS0readAccel();
	rc &= LSM9DS0readMag();
	rc &= LSM9DS0readGyro();
	return rc;
}
