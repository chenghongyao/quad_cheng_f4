#include "AHRS_DCM.h"




#define SPIN_RATE_LIMIT 20

void AHRS_DCM::update(float dt)
{
	_ins.update();							//����accel,gyro����

	matrix_update(dt);		//�����ǻ���,����dcm����
	normalize();				//ȷ������λ����

	drift_correction(dt);		//������Ư�ƽ���
	check_matrix();				//
	euler_angles();				//����->ŷ����
	update_trig();				//ŷ�������Ǻ���	
}


void AHRS_DCM::reset_gyro_drift(void)
{
	_omega_I.zero();
	_omega_I_sum.zero();
	_omega_I_sum_time = 0;
}

void AHRS_DCM::reset(bool recover_eulers)
{
	reset_gyro_drift();
	_omega_P.zero();
	_omega_yaw_P.zero();
	_omega.zero();
	if(recover_eulers && !isnan(pitch) && !isnan(roll) && !isnan(yaw)){
		_dcm_matrix.from_euler(roll, pitch, yaw);
	}
	else{
		_dcm_matrix.from_euler(0, 0, 0);
	}
}


void AHRS_DCM::normalize(void)
{
	float error;
	Vector3f t0, t1, t2;
	error = _dcm_matrix.a * _dcm_matrix.b;	//��һ����ڶ�������,���ӦΪ0        // eq.18
	t0 = _dcm_matrix.a - (_dcm_matrix.b * (0.5f * error));              // eq.19,
	t1 = _dcm_matrix.b - (_dcm_matrix.a * (0.5f * error));              // eq.19
	t2 = t0 % t1;                                                       // c= a x b // eq.20
	if (!renorm(t0, _dcm_matrix.a) ||				//��t0��λ���󸳸�a
		!renorm(t1, _dcm_matrix.b) ||
		!renorm(t2, _dcm_matrix.c)) {
		// Our solution is blowing up and we will force back
		// to last euler angles
		_last_failure_ms = millis();
		reset(true);
	}
}
void AHRS_DCM::drift_correction_yaw()
{
	bool new_value = false;
	bool yaw_error = 0.0f;
	float yaw_deltat = 0.0f;



	if(!new_value)
	{
		_omega_yaw_P *= 0.97;		//�������õݼ�
		return;
	}
	float error_z = _dcm_matrix.c.z * yaw_error;	//�����ת��bod����ϵ
	float spin_rate = _omega.length();					//��ת�ٶ�

	//����
	if (_kp_yaw < AHRS_YAW_P_MIN)	//����_kp_yawֵ
		_kp_yaw = AHRS_YAW_P_MIN;
	_omega_yaw_P.z = error_z*_kp_yaw * _P_gain(spin_rate)*_yaw_gain();
	if (_flags.fast_ground_gains){
		_omega_yaw_P.z *= 8;
	}
	//����
	if(yaw_deltat<2.0f && spin_rate < radians(SPIN_RATE_LIMIT)){
		_omega_I_sum.z += error_z*_ki_yaw*yaw_deltat;
	}

	_error_yaw_sum += fabsf(yaw_error);		//??
	_error_yaw_count++;
}
void AHRS_DCM::drift_correction(float deltat)
{
//	uint32_t last_correction_time;
	//drift_correction_yaw();

	_accel_ef = _dcm_matrix*_ins.get_accel();	//���ٶ�ת����������ϵ
	_ra_sum += _accel_ef * deltat;				//���ٶ�ֵ����(��ֵ�˲�)	
	_ra_deltat += deltat;
	if(_ra_deltat < 0.05f){						//100ms����һ��
		return;
	}

	Vector3f GA_e;
	GA_e = Vector3f(0, 0, -1.0f);		//1g����
	Vector3f error;
	Vector3f GA_b;
	//��ȡ���
	_ra_sum *= 1.0f / (_ra_deltat*GRAVITY_MSS);		//��ƽ��,��λתΪg
	GA_b = _ra_sum;	
	GA_b.normalize();

	error = GA_b%GA_e;
	float error_length = error.length();

	error = _dcm_matrix.mul_transpose(error);		//���ת������
	if(error.is_nan() || error.is_inf())
	{
		check_matrix();
		_last_failure_ms = millis();
	}


	//����
	if (_kp < AHRS_RP_P_MIN){
		_kp = AHRS_RP_P_MIN;
	}

	float spin_rate = _omega.length();
	_omega_P = error * _kp * _P_gain(spin_rate);
	if(_flags.fast_ground_gains){
		_omega_P *= 8;
	}

	_error_rp_sum += error_length;		//for debug??
	_error_rp_count++;

	//����
	if(spin_rate < radians(SPIN_RATE_LIMIT)){
		_omega_I_sum += error*_ki*_ra_deltat;
		_omega_I_sum_time += _ra_deltat;
	}

	if(_omega_I_sum_time >= 5)	//����ֵ5s����һ��
	{
		float change_limit = _gyro_drift_limit * _omega_I_sum_time;
		_omega_I_sum.x = constrain_float(_omega_I_sum.x, -change_limit, change_limit);
		_omega_I_sum.y = constrain_float(_omega_I_sum.y, -change_limit, change_limit);
		_omega_I_sum.z = constrain_float(_omega_I_sum.z, -change_limit, change_limit);
		
		_omega_I += _omega_I_sum;
		_omega_I_sum.zero();
		_omega_I_sum_time = 0;
	}
	_ra_sum.zero();
	_ra_deltat = 0;
	//_ra_sum_start = last_correction_time;
}
bool AHRS_DCM::renorm(Vector3f const &a, Vector3f &result)
{
	float renorm_val;
	// numerical errors will slowly build up over time in DCM,
	// causing inaccuracies. We can keep ahead of those errors
	// using the renormalization technique from the DCM IMU paper
	// (see equations 18 to 21).

	// For APM we don't bother with the taylor expansion
	// optimisation from the paper as on our 2560 CPU the cost of
	// the sqrt() is 44 microseconds, and the small time saving of
	// the taylor expansion is not worth the potential of
	// additional error buildup.

	// Note that we can get significant renormalisation values
	// when we have a larger delta_t due to a glitch eleswhere in
	// APM, such as a I2c timeout or a set of EEPROM writes. While
	// we would like to avoid these if possible, if it does happen
	// we don't want to compound the error by making DCM less
	// accurate.
	renorm_val = 1.0f / a.length();	//Ӧ��Ϊ1����

	// keep the average for reporting
	_renorm_val_sum += renorm_val;
	_renorm_val_count++;

	if (!(renorm_val < 2.0f && renorm_val > 0.5f)) {	//
		// this is larger than it should get - log it as a warning
		if (!(renorm_val < 1.0e6f && renorm_val > 1.0e-6f)) {//��С
			// we are getting values which are way out of
			// range, we will reset the matrix and hope we
			// can recover our attitude using drift
			// correction before we hit the ground!
			//Serial.printf("ERROR: DCM renormalisation error. renorm_val=%f\n",
			//	   renorm_val);
			return false;
		}
	}
	result = a * renorm_val;	//a��λ��
	return true;
}
void AHRS_DCM::reset_attitude(float roll, float pitch, float yaw)
{
	_dcm_matrix.from_euler(roll, pitch, yaw);
}
void AHRS_DCM::check_matrix(void)
{
	if(_dcm_matrix.is_nan())
	{
		reset(true);
		return;
	}

	if (fabsf(_dcm_matrix.c.x)>1.0f)//sin_pitch out of range
	{		
		normalize();// We have an invalid matrix. Force a normalisation.
		if (_dcm_matrix.is_nan() ||fabsf(_dcm_matrix.c.x) > 1.0f) 
		{
			// normalisation didn't fix the problem! We're
			// in real trouble. All we can do is reset
			//Serial.printf("ERROR: DCM matrix error. _dcm_matrix.c.x=%f\n",
			//	   _dcm_matrix.c.x);
			reset(true);
		}
	}
}
void AHRS_DCM::euler_angles(void)
{
	_body_dcm_matrix = _dcm_matrix;
	_body_dcm_matrix.rotateXYinv(_trim);
	_body_dcm_matrix.to_euler(&roll, &pitch, &yaw);
	update_cd_values();
}
float AHRS_DCM::_P_gain(float spin_rate) const
{
	if (spin_rate < radians(50)){	
		return 1.0f;
	}
	if(spin_rate > radians(500)){	//�޷�
		return 10.0f;
	}
	return spin_rate / radians(50);//��50�ȹ�һ��
}
//?????
float AHRS_DCM::_yaw_gain(void) const
{
	float VdotEFmag = pythagorous2(_accel_ef.x, _accel_ef.y);	//
	if (VdotEFmag <= 4.0f){
		return 0.2f*(4.5f - VdotEFmag);
	}
	return 0.1f;
}
void AHRS_DCM::matrix_update(float dt)
{
	_omega = _ins.get_gyro();		//���ٶ�
 	_omega += _omega_I;				//����Ư��
 	_dcm_matrix.rotate((_omega + _omega_P + _omega_yaw_P)*dt);
}

