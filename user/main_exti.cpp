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




void EXTIX_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	//KEY_Init(); //������Ӧ�� IO �ڳ�ʼ��
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);//ʹ�� SYSCFG ʱ��
	GPIO_InitTypeDef  GPIO_InitStructure;	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//PE0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;			//
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN ;			//����������
	GPIO_Init(GPIOE, &GPIO_InitStructure);					//��ʼ��
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ�� SYSCFG ʱ��
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);//PE0 ������0
	/* ���� EXTI_Line0 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; //�����ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ�� LINE0
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ�ж� 0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;//��Ӧ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);//���� NVIC
}

GPIO led(PG15);
int main(void)
{	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
	
	led.init();
	led = 1;
	EXTIX_Init();
	
	cout<<"start"<<endl;
	

	while(1)
	{
		//led = !led;
		//delay_ms(1000);
	}
}

//�ⲿ�ж� 0 �������
extern "C" void EXTI0_IRQHandler(void)
{

	cout<<"exti0"<<endl;
	led = !led;
	EXTI_ClearITPendingBit(EXTI_Line0); //��� LINE0 �ϵ��жϱ�־λ
}



