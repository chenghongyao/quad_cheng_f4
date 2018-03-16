#include "SPIDriver.h"
#include "GPIO.h"
#include "RCCX.h"



void SPIDriver::Setup(SPI_TypeDef* spix)
{
	//SPI IO����,ʱ�ӳ�ʼ��
	if(spix == SPI1)
	{
		RCCX.SPI1_.Enable();
		GPIO::ModeAF(PA5,GPIO_AF_SPI1);
		GPIO::ModeAF(PA6,GPIO_AF_SPI1);
		GPIO::ModeAF(PA7,GPIO_AF_SPI1);
	}
	else if(spix == SPI2)
	{
		RCCX.SPI2_.Enable();
		GPIO::ModeAF(PB13,GPIO_AF_SPI2);
		GPIO::ModeAF(PB14,GPIO_AF_SPI2);
		GPIO::ModeAF(PB15,GPIO_AF_SPI2);	
	}
	else if(spix == SPI3)
	{
		RCCX.SPI3_.Enable();
// 		GPIO::ModeAF(PC10,GPIO_AF_SPI3);
// 		GPIO::ModeAF(PC11,GPIO_AF_SPI3);
// 		GPIO::ModeAF(PC12,GPIO_AF_SPI3);

		GPIO::ModeAF(PB3,GPIO_AF_SPI3);
		GPIO::ModeAF(PB4,GPIO_AF_SPI3);
		GPIO::ModeAF(PB5,GPIO_AF_SPI3);
	}	

}

void SPIDriver::open()
{
	SPI_Init(_spix, &_SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	SPI_Cmd(_spix, ENABLE); //ʹ��SPI����
}

void SPIDriver::close()
{
	SPI_Cmd(_spix, ENABLE); //ʹ��SPI����
}

uint16_t SPIDriver::write(uint16_t c)
{
	while (SPI_I2S_GetFlagStatus(_spix, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������  
	SPI_I2S_SendData(_spix, c); //ͨ������SPIx����һ��byte  ����	
	while (SPI_I2S_GetFlagStatus(_spix, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte  
	return SPI_I2S_ReceiveData(_spix); //����ͨ��SPIx������յ�����		

}
void SPIDriver::write(uint8_t *pbuf,uint32_t len)
{
	for(uint32_t i=0;i<len;i++)
	{
		write(*pbuf++);
	}
}


void SPIDriver::read(uint8_t *pbuf,uint32_t len)
{
	for(uint32_t i=0;i<len;i++)
	{
		*pbuf++ = read();
	}
}

uint8_t SPIDriver::read_reg(uint8_t reg_addr)
{
	write((reg_addr | 0x80));
	return read();
}

void SPIDriver::read_reg(uint8_t reg_addr, uint8_t *pbuf, uint8_t len)
{
	write((reg_addr | 0x80));
	read(pbuf, len);
}


void SPIDriver::write_reg(uint8_t reg_addr,uint8_t val)
{
	write(reg_addr & 0x7F);
	write(val);
}
void SPIDriver::write_reg(uint8_t reg_addr, uint8_t *pbuf,uint8_t len)
{
	write(reg_addr & 0x7F);
	write(pbuf,len);

}
