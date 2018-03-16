#include "AHRS_Q.h"


#define SPIN_RATE_LIMIT 20



void AHRS_Q::update(float dt)
{
	_ins.update();	//���¼��ٶ�����

	//��Ԫ������
	_omega = _ins.get_gyro();
	_omega += _omega_I;									//������
	_q.roate((_omega + _omega_P + _omega_yaw_P)*dt);	//���������

	_q.normalize();									//�淶��
	_dcm_matrix = _q.to_dcm();						//��ȡ��ת����
	_accel_ef = _dcm_matrix*_ins.get_accel();		//��ȡ��������ϵ���ٶ�

	drift_correction(dt);				//���½��� _omega_P,_omega_I
	euler_angles();						//תΪŷ����
	update_trig();						//��ŷ�������Ǻ���
		
}

float AHRS_Q::_P_gain(float spin_rate) const
{
	if (spin_rate < radians(50)) {
		return 1.0f;
	}
	if (spin_rate > radians(500)) {	//�޷�
		return 10.0f;
	}

	return spin_rate / radians(50);//��50�ȹ�һ��
}
void AHRS_Q::drift_correction_yaw(float dt)
{

}
void AHRS_Q::drift_correction(float dt)
{
	drift_correction_yaw(dt);

	Vector3f GA_e(0, 0, -1);					//�������ٶ� 

	Vector3f GA_b = _accel_ef / GRAVITY_MSS;	//ת1g��λ
	GA_b.normalize();							

	//���
	Vector3f error = GA_b%GA_e;						//������
	error = _dcm_matrix.mul_transpose(error);		//���ת������

	float spin_rate = _omega.length();				//��ת���ʴ�С

	//������
	_omega_P = error*_kp*_P_gain(spin_rate);		//1-10,(50-500��)
	if(_flags.fast_ground_gains){					//�����������
		_omega_P *= 8;
	}

	//������
	if (spin_rate < radians(SPIN_RATE_LIMIT)) {		//С��20��ÿ��
		_omega_I_sum += error*_ki*dt;
		_omega_I_sum_time += dt;
	}

	if (_omega_I_sum_time >= 2)	//����ֵ2s����һ��
	{
		float change_limit = _gyro_drift_limit * _omega_I_sum_time;		//0.5��/60s
		_omega_I_sum.x = constrain_float(_omega_I_sum.x, -change_limit, change_limit);
		_omega_I_sum.y = constrain_float(_omega_I_sum.y, -change_limit, change_limit);
		_omega_I_sum.z = constrain_float(_omega_I_sum.z, -change_limit, change_limit);

		_omega_I += _omega_I_sum;

		_omega_I_sum.zero();
		_omega_I_sum_time = 0;
	}


}



void AHRS_Q::reset(bool recover_euler)
{
	reset_gyro_drift();
	_omega_P.zero();
	_omega_yaw_P.zero();
	_omega.zero();

	if(recover_euler && !isnan(roll) && isnan(pitch) && isnan(yaw))
	{
		_q.from_euler(roll, pitch, yaw);
		_dcm_matrix = _q.to_dcm();
	}
	else
	{
		_q.from_euler(0, 0, 0);
		_dcm_matrix.identity();
	}
	
}
void AHRS_Q::euler_angles(void)
{
	_body_dcm_matrix = _dcm_matrix;
	_body_dcm_matrix.rotateXYinv(_trim);
	_body_dcm_matrix.to_euler(&roll, &pitch, &yaw);
	update_cd_values();
}


