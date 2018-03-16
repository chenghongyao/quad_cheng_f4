#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "stdio.h"
#include <iostream>





using namespace std;




GPIO led(PG15);
uint16_t cnt = 0;

	
void I2S2_Init()
{
	I2S_InitTypeDef I2S_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	
	//IO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);					//GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;		//PB3~5���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;							//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);									//��ʼ��
	
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ�� SPI2 ʱ��
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE); //��λ SPI2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//������λ
	
	I2S_InitStructure.I2S_Mode=I2S_Mode_MasterTx;						//IIS ģʽ ������
	I2S_InitStructure.I2S_Standard=I2S_Standard_Phillips;				//IIS ��׼Phillips
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat_16b;				//IIS ���ݳ���	16λ
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;			//��ʱ�������ֹ
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq_192k;					//IIS Ƶ������
	I2S_InitStructure.I2S_CPOL=I2S_CPOL_Low;							//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI2,&I2S_InitStructure);									//��ʼ�� IIS
	
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2S EN ʹ��.
} 



#define PI 3.1415926
void get_coff(float fs,float f,double *A,double *y_1,uint16_t *N)
{
	double omega = 2*PI*f/fs;
	
	*A = 2*cos(omega);
	*y_1 = sin(omega);
	*N = fs/f + 0.5f;
	
}

	
int16_t signal_sin[]= {0,9209,17676,24717,29764,32411,32444,29861,24869,17872,9433,233,-8985,-17479,-24563,-29666,-32377,-32476,-29956,-25020,-18067,-9656,-467,};

	
int main(void)
{	
	int16_t	dval;
	uint16_t i;
	uint16_t	N;
	double y_0,y_1,y_1n,A,y;

	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();
	delay_ms(500);
	usart1_init(500000);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	led.init();
	led = 1;
	
	I2S2_Init();
	
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == SET);
	
	
	
	
	cout<<"start"<<endl;
	
	dval = 0;
	y_0 = 0;
	get_coff(192000,2000,&A,&y_1,&N);
	y_1n = y_1;
	cout<<"A="<<A<<endl;
	cout<<"y(n-1)="<<y_1<<endl;

	i = 0;
//	N = sizeof(signal_sin)/sizeof(signal_sin[0]);
	
	cout<<"N="<<N<<endl;
		
	while(1)
	{
		y = A*y_1 - y_0;
		y_0 = y_1;
		y_1 = y;
		dval = y*32760;
		
//		dval  = signal_sin[i];
		
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
		SPI_I2S_SendData(SPI2,dval); 
			
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
		SPI_I2S_SendData(SPI2,dval); 
			
		
		if(++i == N)
		{
			y_0 = 0;
			y_1 = y_1n;
			
			i = 0;

			
		}
		
//		if(++n == 10) 
//		{
//			n = 0;
//			if(dval%1000 == 0)cout<<dval<<endl;
//			if(dval>-32760)dval-=1;

//		}
		//delay_ms(1000);
			
		
	}
}






	


