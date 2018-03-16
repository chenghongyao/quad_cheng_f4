#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "sdio_sdcard.h"
#include "stdio.h"
#include <iostream>


#include "rtc.h"
#include "w25qxx.h"

using namespace std;




//ͨ�ö�ʱ�� 3 �жϳ�ʼ��
//arr���Զ���װֵ�� psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ�� 3!
void TIM3_Int_Init(u16 psc,u16 arr)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); 			//ʹ�� TIM3 ʱ��
	
	
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc-1; 				//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_Period = arr-1; 					//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);				//��ʼ����ʱ�� TIM3
	
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); 						//����ʱ�� 3 �����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; 					//��ʱ�� 3 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 		//��ռ���ȼ� 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 			//��Ӧ���ȼ� 3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);									// ��ʼ�� NVIC
	
	TIM_Cmd(TIM3,ENABLE); 											//ʹ�ܶ�ʱ�� 3
}



	


GPIO led(PG15);
uint16_t cnt = 0;

int main(void)
{	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	led.init();
	led = 1;
	
	TIM3_Int_Init(84,1000);

	while(1)
	{

	}
}



//��ʱ�� 3 �жϷ�����
extern "C" void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		if(++cnt == 1000)
		{
			cnt = 0;
			led = !led;
		}
		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
	}
	
}	






	


