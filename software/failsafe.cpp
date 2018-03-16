#include "failsafe.h"
#include "Parameters.h"
#include "Motors.h"

failsafe_t failsafe;
extern Parameters g;
extern Motors motors;



void failsafe_radio_on_event(void);		//�ر�ʧ�ر���
void failsafe_radio_off_event(void);	//����ʧ�ر���
void failsafe_set_radio(uint8_t b)		//ʧ�ر���
{
	if (failsafe.radio != b)
	{
		failsafe.radio = b;
		if (failsafe.radio == 0)
		{
			failsafe_radio_off_event();//�ر�ʧ�ر���
		}
		else
		{
			failsafe_radio_on_event();//����ʧ�ر���ʧ�ر���
		}
	}
}



void failsafe_radio_on_event()
{
	
}

void failsafe_radio_off_event()
{
}


#define FS_COUNTER 3  
void set_throttle_failsafe(uint16_t throttle_pwm)
{
	if (g.failsafe_throttle == FS_THR_DISABLED)		////��ֹ���ű���,ֱ������
	{
		return;
	}

	//check for low throttle value
	if (throttle_pwm < g.failsafe_throttle_value)			//ʧ������
	{
		if (failsafe.radio || !motors.armed())			//δ�������Ѿ���ʧ�ر���ģʽ�������
		{
			return;
		}

		failsafe.radio_counter++;
		if (failsafe.radio_counter >= FS_COUNTER)	//ȷ��Ϊʧ������
		{
			failsafe.radio_counter = FS_COUNTER;  // check to ensure we don't overflow the counter
			failsafe_set_radio(true);
		}
	}
	else if (failsafe.radio_counter)
	{		//��������,��Сʧ�ر�����¼
			// we have a good throttle so reduce failsafe counter
		failsafe.radio_counter--;
		if (failsafe.radio_counter <= 0)
		{
			failsafe.radio_counter = 0;   // check to ensure we don't underflow the counter
			if (failsafe.radio)
			{
				failsafe_set_radio(false);
			}
		}
	}
}



void failsafe_enable()
{

}

void failsafe_disable()
{

}

