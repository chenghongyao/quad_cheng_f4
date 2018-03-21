#include "InertialNav.h"
#include "Ano_OF.h"


void InertialNav::update_gains()
{
	if (_time_constant_xy == 0.0f)
	{
		_k1_xy = _k2_xy = _k3_xy = 0.0f;
	}
	else 
	{
		_k1_xy = 3.0f / _time_constant_xy;
		_k2_xy = 3.0f / (_time_constant_xy*_time_constant_xy);
		_k3_xy = 1.0f / (_time_constant_xy*_time_constant_xy*_time_constant_xy);
	}

	if (_time_constant_z == 0.0f) 
	{
		_k1_z = _k2_z = _k3_z = 0.0f;
	}
	else 
	{
		_k1_z = 3.0f / _time_constant_z;
		_k2_z = 3.0f / (_time_constant_z*_time_constant_z);
		_k3_z = 1.0f / (_time_constant_z*_time_constant_z*_time_constant_z);
	}
}
void InertialNav::init()
{
	update_gains();
}

void InertialNav::update(float dt)
{
	if (dt > 0.1f)return;	//����100ms


	if (_flags.ignore_error > 0){
		_flags.ignore_error--;
	}

	//check_baro();	//update position_error.z
	check_sonar();
	Vector3f accel_ef = _ahrs.get_accel_ef();
	accel_ef.z += GRAVITY_MSS;	//��ȥ�������ٶ�
	accel_ef *= 100.0f;			//in cm/s2

	//��ʹ��x,y����,���ٶ�����
	if (!_xy_enabled){
		accel_ef.x = 0;
		accel_ef.y = 0;
	}

	//TODO:ȷ�����ٶȷ���
	accel_ef.z = -accel_ef.z;	//����Ϊ��

//////////////////////////////////////////////////////////////////////////
	Vector2f position_error_hbf;
	Vector3f accel_correction_ef;

	//ef���ת��ˮƽ��������ϵ
	position_error_hbf.x = _postion_error.x * _ahrs.cos_yaw() + _postion_error.y * _ahrs.sin_yaw();
	position_error_hbf.y = -_postion_error.x * _ahrs.sin_yaw() + _postion_error.y * _ahrs.cos_yaw();

	//���ٶ�����
	float tmp = _k3_xy * dt;
	accel_correction_hbf.x += position_error_hbf.x * tmp;
	accel_correction_hbf.y += position_error_hbf.y * tmp;
	accel_correction_hbf.z += _postion_error.z * _k3_z * dt;

	//���ٶȽ�����ת���ٶ�����ϵ
	accel_correction_ef.x = accel_correction_hbf.x * _ahrs.cos_yaw() - accel_correction_hbf.y * _ahrs.sin_yaw();
	accel_correction_ef.y = accel_correction_hbf.x * _ahrs.sin_yaw() + accel_correction_hbf.y * _ahrs.cos_yaw();
	accel_correction_ef.z = accel_correction_hbf.z;


	//�ٶ�����
	tmp = _k2_xy*dt;
	_velocity.x += _postion_error.x * tmp;
	_velocity.y += _postion_error.y * tmp;
	_velocity.z += _postion_error.z * _k2_z * dt;

	//λ������
	tmp = _k1_xy * dt;
	_postion_correction.x += _postion_error.x * tmp;
	_postion_correction.y += _postion_error.y * tmp;
	_postion_correction.z += _postion_error.z * _k1_z * dt;

	//���ٶȻ���
	Vector3f velocity_increase = (accel_ef + accel_correction_ef)*dt;
	//����λ��
	_postion_base += (_velocity + velocity_increase*0.5f)*dt;
	_postion = _postion_base + _postion_correction;
	//�����ٶ�
	_velocity += velocity_increase;

	//����˴ι���
	_hist_position_estimate_z.push_back(_postion_base.z);

	_historic_xy_counter++;
	if(_historic_xy_counter >= INTERTIALNAV_SAVE_POS_AFTER_ITERATIONS)
	{
		_historic_xy_counter = 0;
		_hist_position_estimate_x.push_back(_postion_base.x);
		_hist_position_estimate_y.push_back(_postion_base.y);
	}
}

void InertialNav::set_velocity_xy(float x, float y)
{
	_velocity.x = x;
	_velocity.y = y;
}
void InertialNav::set_altitude(float new_altitude)
{
	_postion_base.z = new_altitude;
	_postion_correction.z = 0;
	_postion.z = new_altitude; // _position = _position_base + _position_correction
	_hist_position_estimate_z.clear();
}
// void InertialNav::check_baro()
// {
// 	uint32_t baro_update_time;
// 	baro_update_time = _baro.get_last_update();
// 	if(baro_update_time != _baro_last_update)
// 	{
// 		float dt = (float)(baro_update_time - _baro_last_update)*0.001f;	//in seconds
// 		//
// 		correct_with_baro(_baro.get_altitude()*100.0f, dt);
// 		_baro_last_update = baro_update_time;
// 
// 	}
// }

void InertialNav::check_sonar()
{
	uint32_t sonar_update_time;
	sonar_update_time = ANO_OF.update_time;
	if (sonar_update_time != _sonar_last_update)
	{
		float dt = (float)(sonar_update_time - _sonar_last_update)*0.001f;	//in seconds
																			//
		correct_with_sonar(ANO_OF.ALT2, dt);
		_sonar_last_update = sonar_update_time;

	}
}
// void InertialNav::correct_with_baro(float baro_alt, float dt)
// {
// 	static uint8_t first_read = 0;
// 	if (dt > 0.5f)return;		//500msʱ��̫��
// 
// 	if(first_read <= 10){
// 		set_altitude(baro_alt);		//����ѹ�Ƹ߶ȳ�ʼ����ǰ�߶�
// 		first_read++;
// 	}
// 
// 	//������??	
// 	if (_flags.baro_glitching)
// 	{
// 		set_altitude(baro_alt);
// 		_postion_error = 0.0f;
// 	}
// 	else
// 	{
// 		float hist_position_base_z;
// 		if(_hist_position_estimate_z.is_full())
// 		{
// 			hist_position_base_z = _hist_position_estimate_z.front();		//�����һ��,������ѹ�Ʋ����ͺ�
// 		}
// 		else
// 		{
// 			hist_position_base_z = _postion_base.z;	//ʹ�õ�ǰֵ
// 		}
// 
// 		_postion_error = baro_alt - (hist_position_base_z + _postion_correction.z);
// 	}
// 
// 
// }

void InertialNav::correct_with_sonar(float sonar_alt, float dt)
{
	static uint8_t first_read = 0;
	if (dt > 0.5f)return;		//500msʱ��̫��

	if (first_read <= 10)
	{
		set_altitude(sonar_alt);		//�ó������߶ȳ�ʼ����ǰ�߶�
		first_read++;
	}

	//������??	
	if (_flags.sonar_glitchiing)
	{
		set_altitude(sonar_alt);
		_postion_error = 0.0f;
	}
	else
	{
		float hist_position_base_z;
		if (_hist_position_estimate_z.is_full())
		{
			hist_position_base_z = _hist_position_estimate_z.front();		//�����һ��,���ڲ����ͺ�
		}
		else
		{
			hist_position_base_z = _postion_base.z;	//ʹ�õ�ǰֵ
		}

		_postion_error = sonar_alt - (hist_position_base_z + _postion_correction.z);
	}
}
