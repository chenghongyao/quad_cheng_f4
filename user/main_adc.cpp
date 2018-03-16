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


//

void My_ADC_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	//���� ADC �� GPIO ���ʱ�Ӻͳ�ʼ�� GPIO
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);//ʹ�� GPIOC ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ʹ�� ADC1 ʱ��

	//�ȳ�ʼ�� ADC1 ͨ�� 0 IO ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				//PC0 ͨ�� 10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;			//ģ������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;		//����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);					//��ʼ��
	
	
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE); //ADC1 ��λ
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE); //��λ����
	
	ADC_TempSensorVrefintCmd(ENABLE);//ʹ���ڲ��¶ȴ�����
	
	//��ʼ��ͨ������
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay =	ADC_TwoSamplingDelay_5Cycles;	//���������׶�֮����ӳ� 5 ��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode =	ADC_DMAAccessMode_Disabled; 		//DMA ʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;						//Ԥ��Ƶ 4 ��Ƶ,ADCCLK=PCLK2/4=84/4=21Mhz,ADC ʱ����ò�Ҫ���� 36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);										//��ʼ��
	
	//��ʼ�� ADC1 ��ز���
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//12 λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//1 ��ת���ڹ���������
	ADC_Init(ADC1, &ADC_InitStructure);												//ADC ��ʼ��
	
	//���� ADC ת��
	ADC_Cmd(ADC1, ENABLE);//���� AD ת����
}	
	





	
uint16_t Get_Adc(uint8_t ch)
{
	uint16_t value = 0;

	//����ָ�� ADC �Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_480Cycles );
	ADC_SoftwareStartConv(ADC1); 							//ʹ��ָ���� ADC1 �����ת����������
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));			//�ȴ�ת������
	return  ADC_GetConversionValue(ADC1); 					//�������һ�� ADC1 �������ת�����

}



int16_t Get_Temprate(void)
{
	uint32_t adcx; 
	int16_t result;
	float                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     temperate;
	
	adcx=Get_Adc(ADC_Channel_16); 			//��ȡͨ�� 16
	temperate=(float)adcx*(3.3f/4096); 		//��ѹֵ
	temperate=(temperate-0.76)/0.0025+25.0f; 	//ת��Ϊ�¶�ֵ
	
	
	return temperate*100;
}


int main(void)
{	
	uint32_t time;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	led.init();
	
	My_ADC_Init();
	
	while (1)
	{
		delay_ms(500);	
		
//		time = micros();
//		time = micros() - time;
//		cout<<"time="<<time<<endl;
		
		cout<<"adc_value:"<<Get_Adc(10)<<endl;
		cout<<"temp:"<<Get_Temprate()<<endl;
		
	}
}

