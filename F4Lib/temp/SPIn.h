#ifndef _SPIN_H_
#define _SPIN_H_


#include "sys.h"
#include "SPI.h"







class SPIn:public SPI
{
public:	
	
	SPIn(SPI_TypeDef* spin,GPIO pin_cs):SPI(pin_cs),_spi(spin)
	{}
	
	virtual ~SPIn(){}	
	virtual void init(LatchEdge = Rising_Edge,FirstBit firstbit = MSB_First)	const ;
	virtual void set_speed(uint8_t div)	const ;
	virtual uint8_t write(uint8_t c)	const ;

	/*************��������********************/
	void enable(bool en = true)const
	{

		SPI_Cmd(SPI1,en?ENABLE:DISABLE); //ʹ��SPI����
	}


	
	
	
private:
	SPI_TypeDef* _spi;
	
};










#endif

