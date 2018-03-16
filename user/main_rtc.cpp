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












uint8_t My_RTC_Init()
{
	RTC_InitTypeDef RTC_InitStructure;
	uint16_t retry = 0x1FFF;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//ʹ�� PWR ʱ��
	PWR_BackupAccessCmd(ENABLE);					 		//ʹ�ܺ󱸼Ĵ�������
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) !=0x5050)		//δ��ʼ��
	{
		//��ʱ��
		RCC_LSEConfig(RCC_LSE_ON);							//LSE ����
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//�ȴ�ʱ�Ӿ���
		{
			retry ++;
			delay_ms(10);
		}
		if(retry==0)return 1;								//LSE����ʧ��
	
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 			//��ѡ�� LSE ��Ϊ RTC ʱ��
		RCC_RTCCLKCmd(ENABLE); 								//ʹ�� RTC ʱ��
		
		
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//RTC �첽��Ƶϵ��(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;				//RTC ͬ����Ƶϵ��(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24 Сʱ��ʽ
		RTC_Init(&RTC_InitStructure);							//��ʼ�� RTC ����
		
		
		RTC_Set_Time(23,59,56,RTC_H12_AM); 				//����ʱ��
		RTC_Set_Date(14,5,5,1);			 				//��������
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//����Ѿ���ʼ������
	}
	return 0;
}



GPIO led(PG15);

int main(void)
{	
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	uint16_t t;
	
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	
	led.init();
	
	
	
	My_RTC_Init();
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);//WAKE UP ÿ��һ���ж�
	cout<<"RTC Init Finish"<<endl;
	
	
	
	
	
	
	
	while (1)
	{
		++t;
		if((t%10) == 0)
		{
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			
			
			//printf("sec=%d\r\n",RTC_TimeStruct.RTC_Seconds);
			cout<<(uint32_t)RTC_DateStruct.RTC_Year<<"/"
				<<(uint32_t)RTC_DateStruct.RTC_Month<<"/"
				<<(uint32_t)RTC_DateStruct.RTC_Date<<"--"
				<<(uint32_t)RTC_DateStruct.RTC_WeekDay<<"  "
				<<(uint32_t)RTC_TimeStruct.RTC_Hours<<"/"
				<<(uint32_t)RTC_TimeStruct.RTC_Minutes<<"/"
				<<(uint32_t)RTC_TimeStruct.RTC_Seconds<<endl;
			
		}
		
		delay_ms(10);	
	
	}
}

