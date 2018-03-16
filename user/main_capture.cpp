#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "sdio_sdcard.h"
#include "stdio.h"
#include <iostream>


#include "rtc.h"
#include "w25qxx.h"

using namespace std;




GPIO led(PG15);
uint16_t cnt = 0;


void TIM4_Capcture_Init()
{


}



//��ʱ�� 5 ͨ�� 1 ���벶������
//arr���Զ���װֵ(TIM5,TIM5 �� 32 λ��!!) psc��ʱ��Ԥ��Ƶ��
void TIM5_CH1_Cap_Init(u16 psc,u32 arr)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_ICInitTypeDef TIM5_ICInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE); 	//TIM5 ʱ��ʹ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ�� PORTA ʱ��
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 				//GPIOA0
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;			//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 		//�ٶ� 100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			//���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; 			//����
	GPIO_Init(GPIOA,&GPIO_InitStructure); 					//��ʼ�� PA0
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); 	//PA0 ����λ��ʱ�� 5
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc-1; 					//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr-1; 					//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; 					//ѡ������� IC1 ӳ�䵽 TI1 ��
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; 		//�����ز���
	TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 	//ӳ�䵽 TI1 ��
	TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; 				//���������Ƶ,����Ƶ
	TIM5_ICInitStructure.TIM_ICFilter = 0x0;							//IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM5, &TIM5_ICInitStructure); 							//��ʼ�� TIM5 ���벶�����
	
	TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);					//������ºͲ����ж�
	TIM_Cmd(TIM5,ENABLE );												//ʹ�ܶ�ʱ�� 5
	
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;				//��ռ���ȼ� 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;					//��Ӧ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 					//IRQ ͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure); 									//����ָ���Ĳ�����ʼ�� VIC �Ĵ�����
}


//����״̬
//[7]:0,û�гɹ��Ĳ���;1,�ɹ�����һ��.
//[6]:0,��û���񵽵͵�ƽ;1,�Ѿ����񵽵͵�ƽ��.
//[5:0]:����͵�ƽ������Ĵ���(���� 32 λ��ʱ����˵,1us �������� 1,���ʱ��:4294 ��)
u8 TIM5CH1_CAPTURE_STA=0; //���벶��״̬
u32 TIM5CH1_CAPTURE_VAL;//���벶��ֵ(TIM5/TIM5 �� 32 λ)
//��ʱ�� 5 �жϷ������
extern "C" void TIM5_IRQHandler(void)
{	
	if((TIM5CH1_CAPTURE_STA&0X80)==0)//��δ�ɹ�����
	{
		if(TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)//���
		{
			if(TIM5CH1_CAPTURE_STA&0X40)//�Ѿ����񵽸ߵ�ƽ��
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)	//�ߵ�ƽ̫����
				{
					TIM5CH1_CAPTURE_STA|=0X80; 			//��ǳɹ�������һ��
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}
				else 
					TIM5CH1_CAPTURE_STA++;
			}
		}
		if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET)//���� 1 ���������¼�
		{
			if(TIM5CH1_CAPTURE_STA&0X40) //����һ���½���
			{
				TIM5CH1_CAPTURE_STA|=0X80; 					//��ǳɹ�����һ�θߵ�ƽ����
				TIM5CH1_CAPTURE_VAL=TIM_GetCapture1(TIM5);	//��ȡ��ǰ�Ĳ���ֵ.
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Rising); //���������ز���
			}
			else //��δ��ʼ,��һ�β���������
			{
				TIM5CH1_CAPTURE_STA=0;//���
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;//��ǲ�����������
				
				TIM_Cmd(TIM5,DISABLE ); //ʹ�ܶ�ʱ�� 5
				TIM_SetCounter(TIM5,0); //���������
				TIM_OC1PolarityConfig(TIM5,TIM_ICPolarity_Falling);//�����½��ز���
				TIM_Cmd(TIM5,ENABLE ); //ʹ�ܶ�ʱ�� 5
				
			}
		}
	}
	
	TIM_ClearITPendingBit(TIM5, TIM_IT_CC1|TIM_IT_Update); //����жϱ�־λ
}




int main(void)
{	
	long long temp=0;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();
	delay_ms(500);
	usart1_init(500000);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG,ENABLE);
	led.init();
	led = 1;
	
	
	TIM5_CH1_Cap_Init(84,0xffffffff);
	cout<<"start"<<endl;
	while(1)
	{
		if(TIM5CH1_CAPTURE_STA&0X80)//�ɹ�������һ�θߵ�ƽ
		{
			temp=TIM5CH1_CAPTURE_STA&0X3F;
			temp*=0XFFFFFFFF; 				//���ʱ���ܺ�
			temp+=TIM5CH1_CAPTURE_VAL; 		//�õ��ܵĸߵ�ƽʱ��
			printf("HIGH:%lld us\r\n",temp);//��ӡ�ܵĸߵ�ƽʱ��
			cout<<"high:"<<temp<<endl;
			TIM5CH1_CAPTURE_STA=0; 			//������һ�β���
		}

	}
}






	


