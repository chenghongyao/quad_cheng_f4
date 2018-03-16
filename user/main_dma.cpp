#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "sdio_sdcard.h"
#include "stdio.h"
#include <iostream>




using namespace std;

GPIO led(PG15);


void My_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,uint32_t chx,uint32_t par,uint32_t mar,u16 ndtr)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	if((uint32_t)DMA_Streamx>(uint32_t)DMA2)//�õ���ǰ stream ������ DMA2 ���� DMA1
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2,ENABLE);//DMA2 ʱ��ʹ��
	else
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1 ʱ��ʹ��

	DMA_DeInit(DMA_Streamx);
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){}//�ȴ� DMA ������
		
		
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_Channel = chx; 										//ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = par;			
		//DMA �����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = mar;								//DMA �洢�� 0 ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;						//�洢��������ģʽ
		
	DMA_InitStructure.DMA_BufferSize = ndtr;									//���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//�洢������ģʽ
		
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;		//�������ݳ���:8 λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;				//�洢�����ݳ���:8 λ
		
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;								// ʹ����ͨģʽ (����һ��ѭ��ģʽ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;						//�е����ȼ�
		
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;						//FIFO ģʽ��ֹ	����
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;				//FIFO ��ֵ
		
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;					//�洢��ͻ�����δ���	������
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			//����ͻ�����δ���
	
	DMA_Init(DMA_Streamx, &DMA_InitStructure);									//��ʼ�� DMA Stream
}



//����һ�� DMA ����
//DMA_Streamx:DMA ������,DMA1_Stream0~7/DMA2_Stream0~7
//ndtr:���ݴ�����
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	DMA_Cmd(DMA_Streamx, DISABLE); 							//�ر� DMA ����
	while (DMA_GetCmdStatus(DMA_Streamx) != DISABLE){} 		//ȷ�� DMA ���Ա�����
	DMA_SetCurrDataCounter(DMA_Streamx,ndtr); 				//���ݴ�����
	DMA_Cmd(DMA_Streamx, ENABLE);							//���� DMA ����
}



#define SEND_BUF_SIZE 8200
u8 SendBuff[SEND_BUF_SIZE]; //�������ݻ�����
const u8 TEXT_TO_SEND[]="ALIENTEK Explorer STM32F4 DMA ����ʵ��";



int main(void)
{	
	uint16_t i;
	uint8_t t=0,j,mask=0;
	uint32_t pro,time;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();delay_ms(500);
	usart1_init(500000);
	led.mode_out();
	
	
	
	j=sizeof(TEXT_TO_SEND);
	for(i=0;i<SEND_BUF_SIZE;i++)//��� ASCII �ַ�������
	{
		if(t>=j)//���뻻�з�
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}else
			{
				SendBuff[i]=0x0d;
				mask++;
			}
		}
		else//���� TEXT_TO_SEND ���
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		}
	}
	
	
	My_DMA_Config(DMA2_Stream7,DMA_Channel_4,(u32)&USART1->DR,(u32)SendBuff,SEND_BUF_SIZE);
	
	
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); //ʹ�ܴ��� 1 �� DMA ����
	MYDMA_Enable(DMA2_Stream7,SEND_BUF_SIZE); //��ʼһ�� DMA ���䣡
	time = micros();
	
	while (1)
	{
		
		if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ� DMA2_Steam7 �������
		{
			DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//�崫����ɱ�־
			cout<<"time:"<<micros() - time<<endl;
			break;
		}
		else
		{
			pro=DMA_GetCurrDataCounter(DMA2_Stream7);//�õ���ǰʣ��������
			pro=1-pro/SEND_BUF_SIZE;//�õ��ٷֱ�
			pro*=100; //���� 100 ��
		}
		
		
	}
	
	cout<<"DMA Finish"<<endl;
	while(1);
}

