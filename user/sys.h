#ifndef _SYS_H_
#define _SYS_H_

#include "stm32f4xx.h"
#include "stdio.h"
#include "delay.h"



#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */




#define _DEBUG
#define USB_DEBUG
#define PARAM_DEBUG


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define debug(fmt,...) 	printf(fmt,##__VA_ARGS__)
#else
#define debug(fmt,...)		do{}while(0)
#endif

#ifdef USB_DEBUG
#define usb_debug(fmt,...) 	printf(fmt,##__VA_ARGS__)
#else
#define usb_debug(...)	;
#endif

#ifdef PARAM_DEBUG
#define param_debug(fmt,...) 	printf(fmt,##__VA_ARGS__)
#else
#define param_debug(...)	;
#endif






//����Ϊ��ຯ��
void WFI_SET(void);		//ִ��WFIָ��
void INTX_DISABLE(void);//�ر������ж�
void INTX_ENABLE(void);	//���������ж�
void MSR_MSP(u32 addr);	//���ö�ջ��ַ 
	 
#ifdef __cplusplus
}
#endif /* __cplusplus */



#endif

