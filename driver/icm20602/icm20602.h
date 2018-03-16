#ifndef _ICM20602_H_
#define _ICM20602_H_

#include "sys.h"
#include "HAL_F4.h"


//加速度量程
#define ICM20_ACCEL_FS_2G			(0<<3)
#define ICM20_ACCEL_FS_4G			(1<<3)
#define ICM20_ACCEL_FS_8G			(2<<3)
#define ICM20_ACCEL_FS_16G			(3<<3)
//角速度量程
#define ICM20_GYRO_FS_250			(0<<3)
#define ICM20_GYRO_FS_500			(1<<3)
#define ICM20_GYRO_FS_1000			(2<<3)
#define ICM20_GYRO_FS_2000			(3<<3)
//CONFIG DPF
#define DLPF_BW_250         	0x00	//Rate=8k
#define DLPF_BW_176         	0x01
#define DLPF_BW_92          	0x02
#define DLPF_BW_41          	0x03
#define DLPF_BW_20          	0x04
#define DLPF_BW_10          	0x05
#define DLPF_BW_5           	0x06
#define DLPF_BW_328           	0x06	//Rate=8k
//ACCEL_CONFIG2
#define ACCEL_AVER_4         	(0x00<<4)	//Rate=8k
#define ACCEL_AVER_8			(0x01<<4)
#define ACCEL_AVER_16			(0x02<<4)
#define ACCEL_AVER_32			(0x03<<4)
//ACCEL_DLPF
#define ACCEL_DLPF_BW_218         	0x00	
//#define ACCEL_DLPF_BW_218         	0x01
#define ACCEL_DLPF_BW_99          	0x02
#define ACCEL_DLPF_BW_44          	0x03
#define ACCEL_DLPF_BW_21          	0x04
#define ACCEL_DLPF_BW_10          	0x05
#define ACCEL_DLPF_BW_5           	0x06
#define ACCEL_DLPF_BW_420           0x06	



class ICM20602
{
public:
	ICM20602(const I2C& iic):
		_iic(iic),
		_accel_scale(0.0f),
		_gyro_scale(0.0f)
		{}
		
	uint8_t init();
			
	uint8_t set_accel_fullscale(uint8_t fs);
	uint8_t set_gyro_fullscale(uint8_t fs);
			
		
	uint8_t get_accel_adc(int16_t *accel);
	uint8_t get_gyro_adc(int16_t *gyro);
		
	uint8_t get_accel(float *accel);
	uint8_t get_gyro(float *gyro);
	float get_temp();
			
	uint8_t gyro_scale(){return _gyro_scale;}
	uint8_t accel_scale(){return _accel_scale;}
	
	
		
private:
	const I2C& _iic;
	float _accel_scale;
	float _gyro_scale;

};

extern ICM20602 icm20602;




#endif

