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


void Dac1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitType;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//ʹ�� PA ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ�� DAC ʱ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;		//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;		//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ�� GPIO
	
	
	DAC_InitType.DAC_Trigger=DAC_Trigger_None;							//��ʹ�ô������� TEN1=0
	DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;			//��ʹ�ò��η���
	DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;	//���Ρ���ֵ����
	DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;			//�������ر�
	DAC_Init(DAC_Channel_1,&DAC_InitType); 								//��ʼ�� DAC ͨ�� 1
	
	DAC_Cmd(DAC_Channel_1, ENABLE); 			//ʹ�� DAC ͨ�� 1
	DAC_SetChannel1Data(DAC_Align_12b_R, 0); 	//12 λ�Ҷ������ݸ�ʽ
}
//����ͨ�� 1 �����ѹ
//vol:0~3300,���� 0~3.3V
void Dac1_Set_Vol(u16 vol)
{
	float temp = vol;
	temp /= 1000;
	temp  =temp*4096/3.3f;
	
	DAC_SetChannel1Data(DAC_Align_12b_R,temp);//12 λ�Ҷ������ݸ�ʽ
}

















int main(void)
{	
	uint32_t time;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	led.init();
	Dac1_Init();
	
	Dac1_Set_Vol(2000);
	while (1)
	{
		
		
	}
}

