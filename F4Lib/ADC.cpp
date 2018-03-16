#include "ADC.h"
#include "RCCX.h"
#include "GPIO.h"



void ADCx::Setup(ADC_TypeDef* adcx)
{
	if(adcx == ADC1)
	{
		RCCX.ADC1_.Enable();
	}
	else if(adcx == ADC2)
	{
		RCCX.ADC2_.Enable();
	}
	else if(adcx == ADC3)
	{
		RCCX.ADC3_.Enable();
	}
	else
	{
		return ;
	}
	
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;						//����ģʽ
	ADC_CommonInitStructure.ADC_TwoSamplingDelay =	ADC_TwoSamplingDelay_5Cycles;	//���������׶�֮����ӳ� 5 ��ʱ��
	ADC_CommonInitStructure.ADC_DMAAccessMode =	ADC_DMAAccessMode_Disabled; 		//DMA ʧ��
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;						//Ԥ��Ƶ 4 ��Ƶ,ADCCLK=PCLK2/4=84/4=21Mhz,ADC ʱ����ò�Ҫ���� 36Mhz
	ADC_CommonInit(&ADC_CommonInitStructure);										//��ʼ��
	
	
	ADC_InitTypeDef ADC_InitStructure;
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;							//12 λģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;									//��ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;								//�ر�����ת��
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;		//��ֹ������⣬ʹ���������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;							//�Ҷ���
	ADC_InitStructure.ADC_NbrOfConversion = 1;										//1 ��ת���ڹ���������
	ADC_Init(adcx, &ADC_InitStructure);												//ADC ��ʼ��
	
	
	ADC_Cmd(adcx, ENABLE);//���� AD ת����

}



void ADCx::init()
{
	
	if(_channel <= 7)
	{
		GPIO::ModeADC((GPIO_PIN_T)((uint16_t)PA0+_channel));
	}
	else if(_channel == 8)
	{
		GPIO::ModeADC(PB0);
	}
	else if(_channel == 9)
	{
		GPIO::ModeADC(PB1);
	}
	else if(_channel <= 15)
	{
		GPIO::ModeADC((GPIO_PIN_T)((uint16_t)PC0+_channel-10));
	}
	else if(_channel == 16)
	{
		ADC_TempSensorVrefintCmd(ENABLE);//ʹ���ڲ��¶ȴ�����
	}
	else if(_channel == 17)
	{
		ADC_VBATCmd(ENABLE);				//�ڲ��ο���ѹ
	}

}


void ADCx::start_conv()
{
	ADC_RegularChannelConfig(_adcx, _channel, 1, _time );
	ADC_SoftwareStartConv(_adcx); 							//ʹ��ָ���� ADC1 �����ת����������

}




