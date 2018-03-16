#ifndef _TEA5767_H_
#define _TEA5767_H_

#include "sys.h"



typedef __packed struct
{

	//��һ�ֽ�
	uint8_t MUTE:1;				//1:����
	uint8_t SM:1;				//1:����ģʽ	0:������ģʽ
	uint8_t PLL13_8:6;
	
	//�ڶ��ֽ�
	uint8_t PLL7_0;				
	
	//�����ֽ�
	uint8_t	SUD:1;				//1:��������	0:��������
	uint8_t	SSL:2;				//����ֹͣ����
								//0:����������ģʽ,
								//1:ADC���=5
								//2:ADC���=7
								//3:ADC���=10

	uint8_t	HLSI:1;				//1:�߱�LOע��,0:�ͱ�LOע��
	uint8_t	MS:1;				//1:ǿ�е�������0:��������
	uint8_t	MR:1;				//1:Rͨ��������ǿ�е�������0:R������
	uint8_t	ML:1;				//1:Lͨ��������ǿ�е�������0:L������
	uint8_t	SWP1:1;				//����ɱ�̶˿�
	
	//�����ֽ�
	uint8_t	SWP2:1;				//����ɱ�̶˿�
	uint8_t	STBY:1;				//1:��׼ģʽ��0:�Ǳ�׼ģʽ
	uint8_t	BL:1;				//Ƶ�����ƣ�1���ձ�FMƵ����0:����/ŷ��Ƶ��
	uint8_t	XTAL:1;				//�������ã�1: 32768Hz,0:13MHz
	uint8_t	SMUTI:1;			//����:	1:������ 0:������
	uint8_t	HCC:1;				//1:���п��ƿ�,0:���п��ƹ�
	uint8_t	SNC:1;				//1:����������������0:��������������
	uint8_t	SI:1;				//����ָʾ��1:SWPORT1�������������־��0:SWPORT1����ʱ����ɱ�̶˿�1
	
	//��5�ֽ�
	uint8_t	PLLREF:1;			//1:PLL��6.5MHz�ο�Ƶ��ʹ�ܣ�0:PLL��6.5MHz�ο�Ƶ�ʹر�
	uint8_t	DTC:1;				//1:ȥ����ʱ�䳣��75us,0:ȥ����ʱ�䳣��50us,
	uint8_t	resv:6;				//����
	
	
	


}TEA5767_PARM_WRITE_T;


typedef __packed struct
{

	//��һ�ֽ�
	uint8_t RF:1;				//1:���ҵ�һ����̨����Ƶ�ʼ���	0:û�з��ֵ�̨
	uint8_t BLF:1;				//1:����Ƶ�ʼ���	0:δ��Ƶ�ʼ���
	uint8_t PLL13_8:6;
	
	//�ڶ��ֽ�
	uint8_t PLL7_0;				
	
	//�����ֽ�
	uint8_t	STEREO:1;			//1:����������	0:��ͨ������
	uint8_t	IF6_0:7;			//IF�������
	
	//�����ֽ�
	uint8_t	LEV3_0:4;			//ADC�������
	uint8_t	CI3_1:3;			//оƬʶ��
	uint8_t resv:1;
	
	
	//��5�ֽ�
	uint8_t	resv5;			//ȫ������
	

}TEA5767_PARM_READ_T;
#endif


