#ifndef _RCC_BASE_H_
#define _RCC_BASE_H_



#include "sys.h"


template <void (*ClockCmd)(uint32_t,FunctionalState),void (*ResetCmd)(uint32_t,FunctionalState),uint32_t AHB1Perip>
class RCC_Perip
{
	public:
		void Enable(bool en=true)const
		{
			ClockCmd(AHB1Perip,en?ENABLE:DISABLE);
		}
		
		void Reset(bool en=true)const
		{
		
			ResetCmd(AHB1Perip,en?ENABLE:DISABLE);
		}
};



#define RCC_PERI_DEF(_BUS,_PERI)		RCC_Perip<RCC_##_BUS##PeriphClockCmd,RCC_##_BUS##PeriphResetCmd,RCC_##_BUS##Periph_##_PERI> _PERI##_

class RCC_Collecter
{
public:
	
	RCC_Collecter(){}
	
	//AHB1,168M
	RCC_PERI_DEF(AHB1,GPIOA);
	RCC_PERI_DEF(AHB1,GPIOB);
	RCC_PERI_DEF(AHB1,GPIOC);
	RCC_PERI_DEF(AHB1,GPIOD);
	RCC_PERI_DEF(AHB1,GPIOE);
	RCC_PERI_DEF(AHB1,GPIOF);
	RCC_PERI_DEF(AHB1,GPIOG);
	RCC_PERI_DEF(AHB1,GPIOH);
	RCC_PERI_DEF(AHB1,GPIOI);


	//APB2,168M
	RCC_PERI_DEF(APB2,TIM1);
	RCC_PERI_DEF(APB2,TIM8);
	RCC_PERI_DEF(APB2,TIM9);
	RCC_PERI_DEF(APB2,TIM10);
	RCC_PERI_DEF(APB2,TIM11);
	RCC_PERI_DEF(APB2,SDIO);
	RCC_PERI_DEF(APB2,USART1);
	RCC_PERI_DEF(APB2,USART6);
	RCC_PERI_DEF(APB2,SPI1);
	RCC_PERI_DEF(APB2,ADC1);
	RCC_PERI_DEF(APB2,ADC2);
	RCC_PERI_DEF(APB2,ADC3);

	//84M
	RCC_PERI_DEF(APB1,TIM2);
	RCC_PERI_DEF(APB1,TIM3);
	RCC_PERI_DEF(APB1,TIM4);
	RCC_PERI_DEF(APB1,TIM5);
	RCC_PERI_DEF(APB1,TIM6);
	RCC_PERI_DEF(APB1,TIM7);
	
	RCC_PERI_DEF(APB1,TIM12);
	RCC_PERI_DEF(APB1,TIM13);
	RCC_PERI_DEF(APB1,TIM14);
	RCC_PERI_DEF(APB1,USART2);
	RCC_PERI_DEF(APB1,USART3);
	RCC_PERI_DEF(APB1,UART4);
	RCC_PERI_DEF(APB1,UART5);
	RCC_PERI_DEF(APB1,SPI2);
	RCC_PERI_DEF(APB1,SPI3);
	RCC_PERI_DEF(APB1,I2C1);
	RCC_PERI_DEF(APB1,I2C2);
	RCC_PERI_DEF(APB1,I2C3);	
	RCC_PERI_DEF(APB1,DAC);
};


	
const RCC_Collecter RCCX;



#endif
