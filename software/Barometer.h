#ifndef _BAROMETER_H_
#define _BAROMETER_H_

#include "sys.h"
#include "DerivativeFilter.h"

class Barometer
{
public:
	Barometer():
		_last_update(0),
		_pressure_samples(0),
		_ground_temperature(0),
		_ground_pressure(0),
		_last_altitude_t(0),

		_alt_offset(0),
		_altitude(0)
	{
		_flags.healthy = false;
		_flags.alt_ok = false;

	}

	virtual ~Barometer() {};


	bool healthy()const{
		return _flags.healthy && _flags.alt_ok;
	}

	//
    virtual bool init() = 0;
	virtual bool read() = 0;
	virtual float get_pressure() = 0;
	virtual float get_temperature() = 0;
	virtual void update() = 0;

	//���µ�����ѹ�͸߶�
	void calibrate();
	void update_calibration(void);


	float get_altitude_difference(float base_pressure, float pressure)const;
	float get_altitude(void);
	float get_climb_rate(void);

	//������
	uint8_t get_pressure_samples(void) {
		return _pressure_samples;
	}
	float get_ground_temperature(void)const{
		return _ground_temperature;
	}
	float get_ground_pressure(void)const{
		return _ground_pressure;
	}
	uint32_t get_last_update()const{	//�ϴθ���ʱ��,in ms
		return _last_update;
	}

protected:
	struct Baro_flags{
		uint8_t healthy		: 1;
		uint8_t alt_ok		: 1;
	}_flags;


	bool _healthy;					
	uint32_t _last_update;			//�ϴ���������ʱ��,in ms
	uint8_t _pressure_samples;		//������
private:
	float _ground_temperature;		//�����¶�
	float _ground_pressure;			//������ѹ
	uint32_t _last_altitude_t;		//��һ�θ���ʱ��

	int8_t _alt_offset;				//����߶�
	float _altitude;				//��ǰ�߶�

	DerivativeFilterFloat_Size7         _climb_rate_filter;
};



#endif

