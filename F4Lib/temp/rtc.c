#include "rtc.h"
#include "stdio.h"
#include "delay.h"




//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions  :RTC_H12_AM/RTC_H12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec,uint8_t ampm)
{
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;
	RTC_TimeTypeInitStructure.RTC_Minutes=min;
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;
	RTC_TimeTypeInitStructure.RTC_H12=ampm;
	
	return RTC_SetTime(RTC_Format_BIN,&RTC_TimeTypeInitStructure);
	
}


//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
ErrorStatus RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date,uint8_t week)
{
	
	RTC_DateTypeDef RTC_DateTypeInitStructure;
	RTC_DateTypeInitStructure.RTC_Date=date;
	RTC_DateTypeInitStructure.RTC_Month=month;
	RTC_DateTypeInitStructure.RTC_WeekDay=week;
	RTC_DateTypeInitStructure.RTC_Year=year;
	return RTC_SetDate(RTC_Format_BIN,&RTC_DateTypeInitStructure);
}




uint8_t My_RTC_Init()
{
	RTC_InitTypeDef RTC_InitStructure;
	uint16_t retry = 0x1FFF;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//使能 PWR 时钟
	PWR_BackupAccessCmd(ENABLE);					 		//使能后备寄存器访问
	
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) !=0x5050)		//未初始化
	{
		//打开时钟
		RCC_LSEConfig(RCC_LSE_ON);							//LSE 开启
		while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	//等待时钟就绪
		{
			retry ++;
			delay_ms(10);
		}
		if(retry==0)return 1;								//LSE开启失败
	
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 			//设选择 LSE 作为 RTC 时钟
		RCC_RTCCLKCmd(ENABLE); 								//使能 RTC 时钟
		
		
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;				//RTC 异步分频系数(1~0X7F)
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;				//RTC 同步分频系数(0~7FFF)
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;	//24 小时格式
		RTC_Init(&RTC_InitStructure);							//初始化 RTC 参数
		
		
		RTC_Set_Time(23,59,56,RTC_H12_AM); 				//设置时间
		RTC_Set_Date(14,5,5,1);			 				//设置日期
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x5050);	//标记已经初始化过了
	}
	return 0;
}


//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_Alarm_Definitions
//hour,min,sec:小时,分钟,秒钟
//RTC_Alarm_IRQn -> Line17
void RTC_Set_AlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmTypeInitStructure;
	RTC_TimeTypeDef RTC_TimeTypeInitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	
	RTC_AlarmCmd(RTC_Alarm_A,DISABLE);//关闭闹钟A 
	
	RTC_TimeTypeInitStructure.RTC_Hours=hour;//小时
	RTC_TimeTypeInitStructure.RTC_Minutes=min;//分钟
	RTC_TimeTypeInitStructure.RTC_Seconds=sec;//秒
	RTC_TimeTypeInitStructure.RTC_H12=RTC_H12_AM;
  
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDay=week;								//星期
	RTC_AlarmTypeInitStructure.RTC_AlarmDateWeekDaySel=RTC_AlarmDateWeekDaySel_WeekDay;	//按星期闹
	RTC_AlarmTypeInitStructure.RTC_AlarmMask=RTC_AlarmMask_None;						//精确匹配星期，时分秒
	RTC_AlarmTypeInitStructure.RTC_AlarmTime=RTC_TimeTypeInitStructure;
	RTC_SetAlarm(RTC_Format_BIN,RTC_Alarm_A,&RTC_AlarmTypeInitStructure);
 
	
	RTC_ClearITPendingBit(RTC_IT_ALRA);	//清除RTC闹钟A的标志
	RTC_ITConfig(RTC_IT_ALRA,ENABLE);	//开启闹钟A中断
	RTC_AlarmCmd(RTC_Alarm_A,ENABLE);	//开启闹钟A 


	EXTI_ClearITPendingBit(EXTI_Line17);					//清除LINE17上的中断标志位 
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;				//LINE17
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 	//上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能LINE17
	EXTI_Init(&EXTI_InitStructure);							//配置

	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);								//配置
}




//周期性唤醒定时器设置  
/*wksel:  @ref RTC_Wakeup_Timer_Definitions
#define RTC_WakeUpClock_RTCCLK_Div16        ((uint32_t)0x00000000)
#define RTC_WakeUpClock_RTCCLK_Div8         ((uint32_t)0x00000001)
#define RTC_WakeUpClock_RTCCLK_Div4         ((uint32_t)0x00000002)
#define RTC_WakeUpClock_RTCCLK_Div2         ((uint32_t)0x00000003)
#define RTC_WakeUpClock_CK_SPRE_16bits      ((uint32_t)0x00000004)
#define RTC_WakeUpClock_CK_SPRE_17bits      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
//RTC_WKUP_IRQn	->	Line22
void RTC_Set_WakeUp(uint32_t wksel,uint16_t cnt)
{ 
	EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	
	RTC_WakeUpCmd(DISABLE);//关闭WAKE UP
	
	RTC_WakeUpClockConfig(wksel);	//唤醒时钟选择
	RTC_SetWakeUpCounter(cnt);		//设置WAKE UP自动重装载寄存器
	
	
	RTC_ClearITPendingBit(RTC_IT_WUT); 	//清除RTC WAKE UP的标志
	RTC_ITConfig(RTC_IT_WUT,ENABLE);	//开启WAKE UP 定时器中断
	RTC_WakeUpCmd( ENABLE);				//开启WAKE UP 定时器　
	
	EXTI_ClearITPendingBit(EXTI_Line22);					//清除LINE22上的中断标志位 
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;				//LINE22
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 	//上升沿触发 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//使能LINE22
	EXTI_Init(&EXTI_InitStructure);							//配置
 
 
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;			//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);									//配置
}


//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_ALRAF)==SET)//ALARM A中断?
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);//清除中断标志
		printf("ALARM A!\r\n");
	}   
	EXTI_ClearITPendingBit(EXTI_Line17);	//清除中断线17的中断标志 											 
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{    
	if(RTC_GetFlagStatus(RTC_FLAG_WUTF)==SET)//WK_UP中断?
	{ 
		RTC_ClearFlag(RTC_FLAG_WUTF);	//清除中断标志
		//printf("wake up");
	}   
	EXTI_ClearITPendingBit(EXTI_Line22);//清除中断线22的中断标志 								
}
 



