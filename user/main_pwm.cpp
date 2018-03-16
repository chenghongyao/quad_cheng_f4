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

	

void TIM4_PWM_Init(uint32_t psc,uint32_t arr)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);		//TIM4 ʱ��ʹ��	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ�� PORTB ʱ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_TIM4);	//PB6 ����Ϊ TIM4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 				//GPIOB6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 		//�ٶ� 50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			//����
	GPIO_Init(GPIOB,&GPIO_InitStructure); 					//��ʼ�� PB6
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc-1; 					//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr-1; 						//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);				//��ʼ����ʱ��4
		
	
	//��ʼ�� TIM4 Channel1 PWM ģʽ
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//PWM ����ģʽ 1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//������Ե�
	TIM_OC1Init(TIM4, &TIM_OCInitStructure); 						//��ʼ������ TIM1 4OC1
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable); 				//ʹ��CCR1Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM4,ENABLE);								//ARPE ʹ��

	
	
	TIM_Cmd(TIM4, ENABLE); //ʹ�� TIM14
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
	TIM4_PWM_Init(84,1000);
	TIM_SetCompare1(TIM4,200);

	while(1)
	{

	}
}






	


