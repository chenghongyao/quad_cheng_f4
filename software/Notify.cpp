#include "Notify.h"
#include "HAL_F4.h"


struct Notify::notify_flags Notify::flags;

void Notify::init(void)
{
	hal.led1 = 0;
	hal.led2 = 0;
	hal.LEDR = 0;
	hal.LEDG = 0;
	hal.LEDB = 0;
}

void Notify::extern_led_update(void)
{
	static int8_t arm_counter = 0;
	//IMUУ׼�����־�ֹ
	if (Notify::flags.initialising)
	{
		if (_count & 1)//5Hz
		{
			hal.LEDB = 1;
			hal.LEDR = 0;
		}
		else
		{
			hal.LEDB = 0;
			hal.LEDR = 1;
		}
		return;

	}

	hal.LEDB = 0;
	hal.LEDR = 0;

	if (Notify::flags.armed)	//�ѽ���
	{
		hal.LEDG = 1;
	}
	else//δ����,��ʾ�Ƿ���Խ���
	{
		if (_count & 0x2)arm_counter++;//4��Ƶ,2Hz

		if (Notify::flags.pre_arm_check)//�ɽ���������
		{
			switch (arm_counter)
			{
			case 0:
			case 1:
			case 2:
				hal.LEDG = 1;
				break;
			case 3:
			case 4:
			case 5:
				hal.LEDG = 0;
				break;
			default:
				arm_counter = -1;
				break;


			}
		}
		else//���ɽ�����˫��
		{
			hal.LEDG = 0;
			switch (arm_counter)
			{
			case 0:
				hal.LEDR = 1;
				break;
			case  1:
				hal.LEDR = 0;
				break;
			case 2:
				hal.LEDR = 1;
				break;
			case 3:
			case 4:
				hal.LEDR = 0;
				break;
			default:
				arm_counter = -1;
				break;
			}
		}
	}
}


//10Hz
void Notify::update(void)
{

	static int8_t arm_counter = 0;

	_count++;

	extern_led_update();

	//IMUУ׼�����־�ֹ
	if (Notify::flags.initialising)
	{
		if(_count&1)//5Hz
		{
			hal.led1 = 1;
			hal.led2 = 0;
		}
		else
		{
			hal.led1 = 0;
			hal.led2 = 1;
		}
		return;
		
	}

	hal.led1 = 0;
	if(Notify::flags.armed)	//�ѽ���
	{
		hal.led2 = 1;
	}
	else//δ����,��ʾ�Ƿ���Խ���
	{
		if (_count & 0x2)arm_counter++;//4��Ƶ,2Hz

		if (Notify::flags.pre_arm_check)//�ɽ���������
		{
			switch(arm_counter)
			{
			case 0:
			case 1:
			case 2:
				hal.led2 = 1;
				break;
			case 3:
			case 4:
			case 5:
				hal.led2 = 0;
				break;
			default:
				arm_counter = -1;
				break;


			}
		}
		else//���ɽ�����˫��
		{
			switch (arm_counter)
			{
			case 0:
				hal.led2 = 1;
				break;
			case  1:
				hal.led2 = 0;
				break;
			case 2:
				hal.led2 = 1;
				break;
			case 3:
			case 4:
				hal.led2 = 0;
				break;
			default:
				arm_counter = -1;
				break;
			}
		}
	}
	
	
}
