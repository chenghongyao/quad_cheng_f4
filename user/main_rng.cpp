#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "sdio_sdcard.h"
#include "stdio.h"
#include <iostream>


#include "rtc.h"

using namespace std;

GPIO led(PG15);





uint8_t My_RNG_Init()
{
	uint16_t retry=0;
	
	RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE); 		//���� RNG ʱ��
	RNG_Cmd(ENABLE);										 	//ʹ�� RNG
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET&&retry<10000)	//�ȴ�����
	{ 
		retry++; delay_us(100);
	}
	
	//���ó����ж�
	
	if(retry>=10000)return 1;//���������������������
	return 0;
}




uint32_t RNG_Get_RandomNum(void)
{
	while(RNG_GetFlagStatus(RNG_FLAG_DRDY)==RESET); //�ȴ����������
	return RNG_GetRandomNumber(); 
}


//����[min,max]��Χ�������
int RNG_Get_RandomRange(int min,int max)
{
	return RNG_Get_RandomNum()%(max-min+1) +min;
}




int main(void)
{	

	uint16_t t;
	
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	led.init();
	
	
	My_RNG_Init();
	

	
	while (1)
	{

		cout<<"random:"<<RNG_Get_RandomRange(100,200)<<endl;
		delay_ms(500);	
	
	}
}

