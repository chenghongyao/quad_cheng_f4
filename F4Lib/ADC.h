#ifndef _ADC_H_
#define _ADC_H_

#include "sys.h"



class ADCx
{
public:
	
	ADCx(ADC_TypeDef* adcx,uint8_t channel)
		:_adcx(adcx),_channel(channel)
	{
		_time = ADCx::SampleTime_480Cycles;
	}



	enum 
	{
		SampleTime_3Cycles = ADC_SampleTime_3Cycles,
		SampleTime_15Cycles = ADC_SampleTime_15Cycles,
		SampleTime_28Cycles = ADC_SampleTime_28Cycles,
		SampleTime_56Cycles = ADC_SampleTime_56Cycles,
		SampleTime_84Cycles = ADC_SampleTime_84Cycles,
		SampleTime_112Cycles = ADC_SampleTime_112Cycles,
		SampleTime_144Cycles = ADC_SampleTime_144Cycles,
		SampleTime_480Cycles = ADC_SampleTime_480Cycles,
	};

	
	static void Setup(ADC_TypeDef* adcx);
	void init();
	
	
	void SampleTime(uint8_t sampletime)	{_time = sampletime;}
	void start_conv();
	uint16_t wait_result()	
	{
		while(!ADC_GetFlagStatus(_adcx, ADC_FLAG_EOC));
		return ADC_GetConversionValue(_adcx);
	}		
	uint16_t read_result() {return ADC_GetConversionValue(_adcx);}
	
	uint16_t to_temprature(uint16_t val)
	{
		float temperate=(float)val*(3.3f/4096); 		//��ѹֵ
		temperate=(temperate-0.76f)/0.0025f+25.0f;
		return temperate*100;
	}
		

private:
	ADC_TypeDef* _adcx;
	uint8_t _channel;
	uint8_t _time;

};












#endif





