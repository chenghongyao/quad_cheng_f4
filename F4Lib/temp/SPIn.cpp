#include "SPIn.h"



//****Type	SCK		MISO	MOSI	
//****SPI1	PA5		PA6		PA7
//****SPI2	PB13	PB14	PB15
//****SPI3	PC10	PC11	PC12

void SPIn::init(LatchEdge latch_edge,FirstBit firstbit) const
{	 
	

	
	//SPI IO����,ʱ�ӳ�ʼ��
	if(_spi == SPI1)
	{
		RCCX.SPI1_.Enable();
		GPIO(PA5).mode_af(GPIO_AF_SPI1);
		GPIO(PA6).mode_af(GPIO_AF_SPI1);
		GPIO(PA7).mode_af(GPIO_AF_SPI1);
	}
	else if(_spi == SPI2)
	{
		RCCX.SPI2_.Enable();
		GPIO(PB13).mode_af(GPIO_AF_SPI2);
		GPIO(PB14).mode_af(GPIO_AF_SPI2);
		GPIO(PB15).mode_af(GPIO_AF_SPI2);	
	}
	else if(_spi == SPI3)
	{
		RCCX.SPI3_.Enable();
		GPIO(PC10).mode_af(GPIO_AF_SPI3);
		GPIO(PC11).mode_af(GPIO_AF_SPI3);
		GPIO(PC12).mode_af(GPIO_AF_SPI3);	
	}
		
	//CS��ʼ��
	cs.mode_out();
	cs = 1;
	

	SPI_InitTypeDef  SPI_InitStructure;
	SPI_InitStructure.SPI_CPHA 		= (latch_edge == Rising_Edge) ?                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        SPI_CPHA_2Edge:SPI_CPHA_1Edge;					//����ͬ��ʱ�ӵĵڶ���������(�����أ����ݱ�����
	SPI_InitStructure.SPI_FirstBit 	= (firstbit == MSB_First ) ? SPI_FirstBit_MSB:SPI_FirstBit_LSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  	//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode		= SPI_Mode_Master;						//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize 	= SPI_DataSize_8b;						//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL 		= SPI_CPOL_High;						//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_NSS 		= SPI_NSS_Soft;							//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_CRCPolynomial = 7;								//CRCֵ����Ķ���ʽ
	SPI_Init(_spi, &SPI_InitStructure);  									//����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

	enable(true);
}



static void spi_clock_div(SPI_TypeDef* SPIx ,uint8_t div)
{
	assert_param(IS_SPI_BAUDRATE_PRESCALER(div));//�ж���Ч��
	SPIx->CR1&=0XFFC7;	//λ3-5���㣬�������ò�����
	SPIx->CR1|=div;		//����SPI1�ٶ� 
}

void SPIn::set_speed(uint8_t div)	const 
{
	spi_clock_div(_spi,div);
}
	
uint8_t SPIn::write(uint8_t c)const 
{
	while (SPI_I2S_GetFlagStatus(_spi, SPI_I2S_FLAG_TXE) == RESET){}	//�ȴ���������  
	SPI_I2S_SendData(_spi, c); 											//ͨ������SPIx����һ��byte  ����	
	while (SPI_I2S_GetFlagStatus(_spi, SPI_I2S_FLAG_RXNE) == RESET){} 	//�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(_spi); 									//����ͨ��SPIx������յ�����		
}



