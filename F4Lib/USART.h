#ifndef _USART_H_
#define _USART_H_

#include "sys.h"


//#define USART_TxBuffer_Size	256
//#define INSTANCE_USART1	console
//#define INSTANCE_USART2	usart2
//#define INSTANCE_USART3	usart3




class USART
{
public:
	USART(USART_TypeDef* USARTx):
		_usartx(USARTx)
	{
		_USART_InitStructure.USART_BaudRate = 115200;									//
		_USART_InitStructure.USART_WordLength = USART_WordLength_8b; 					//�ֳ�Ϊ 8 λ
		_USART_InitStructure.USART_StopBits = USART_StopBits_1;							//һ��ֹͣλ
		_USART_InitStructure.USART_Parity = USART_Parity_No;								//����żУ��λ
		_USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //��Ӳ������������
		_USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					//�շ�

	};
			
	~USART(){};
		
	enum
	{
		WordLength_8b = USART_WordLength_8b,
		WordLength_9b = USART_WordLength_9b,
	};
	enum 
	{
		StopBits_1 = USART_StopBits_1,
		StopBits_0_5 = USART_StopBits_0_5,
		StopBits_2 = USART_StopBits_2,
		StopBits_1_5 = USART_StopBits_1_5
	};
	enum
	{
		Parity_No = USART_Parity_No,
		Parity_Even = USART_Parity_Even,
		Parity_Odd = USART_Parity_Odd
	};
	enum 
	{
		Mode_Rx = USART_Mode_Rx,
		Mode_Tx = USART_Mode_Tx
	};
	enum 
	{
		HardwareFlowControl_None = USART_HardwareFlowControl_None,
		HardwareFlowControl_RTS = USART_HardwareFlowControl_RTS,
		HardwareFlowControl_CTS = USART_HardwareFlowControl_CTS,
		HardwareFlowControl_RTS_CTS = USART_HardwareFlowControl_RTS_CTS
	};
	enum
	{
		IT_PE = USART_IT_PE,		//У�����
		IT_TXE = USART_IT_TXE,		//������Ϊ��
		IT_TC = USART_IT_TC,		//�������
		IT_RXNE = USART_IT_RXNE,		//���յ�����
		IT_ORE_RX = USART_IT_ORE_RX, /* In case interrupt is generated if the RXNEIE bit is set */
		IT_IDLE = USART_IT_IDLE,		//����
		IT_LBD = USART_IT_LBD,
		IT_CTS = USART_IT_CTS,
		IT_ERR = USART_IT_ERR,
		IT_ORE_ER = USART_IT_ORE_ER, /* In case interrupt is generated if the EIE bit is set */
		IT_NE = USART_IT_NE,
		IT_FE = USART_IT_FE,
	};
	enum
	{
		FLAG_CTS = USART_FLAG_CTS,
		FLAG_LBD = USART_FLAG_LBD,
		FLAG_TXE = USART_FLAG_TXE,		//������Ϊ��
		FLAG_TC = USART_FLAG_TC,		//�������
		FLAG_RXNE = USART_FLAG_RXNE,		//���յ�����
		FLAG_IDLE = USART_FLAG_IDLE,		//����
		FLAG_ORE = USART_FLAG_ORE, /* In case interrupt is generated if the RXNEIE bit is set */
		FLAG_NE = USART_FLAG_NE,
		FLAG_PE = USART_FLAG_PE,		//У�����
		FLAG_FE = USART_FLAG_FE,
	};


	static void Setup(USART_TypeDef* usartx);
			
	void set_baudrate(uint32_t baudrate) { _USART_InitStructure.USART_BaudRate = baudrate; }
	void set_wordlength(uint16_t wl){ _USART_InitStructure.USART_WordLength = wl; }
	void set_stopbits(uint16_t stopbits){ _USART_InitStructure.USART_StopBits = stopbits; }
	void set_parity(uint16_t parity)			{_USART_InitStructure.USART_Parity = parity;		}
	void set_mode(uint16_t mode)				{_USART_InitStructure.USART_Mode = mode;			}
	void set_hardwareflow_control(uint16_t hf)	{_USART_InitStructure.USART_HardwareFlowControl = hf;}
	
	void add_interrupt(uint16_t it){
		USART_ITConfig(_usartx,it,ENABLE);
	}
	void remove_interrupt(uint16_t it){
		USART_ITConfig(_usartx,it,DISABLE);
	}
	bool check_interrupt(uint16_t it){
		return USART_GetITStatus(_usartx, it) == SET?true:false;
	}
	bool get_flag(uint16_t flag){
		return USART_GetFlagStatus(_usartx, flag) == SET ? true : false;
	}
	void clear_flag(uint16_t flag) {
		 USART_ClearFlag(_usartx, flag);
	}


	void begin(uint32_t baudrate);		//����ͨ����
	void open();
	void close();

	void putc(uint8_t c);
	void putbuf(uint8_t *buffer, uint16_t len);	
	void puts(char *s);

	//void Send(void *buf,uint16_t len);	
	//�жϻص�
	//void _TXE_CallBack();
	//void _RXNE_CallBack(uint16_t c);		
private:
	USART_TypeDef* _usartx;
	USART_InitTypeDef _USART_InitStructure;	

// 	uint8_t 	_TxBuffer[USART_TxBuffer_Size];
// 	uint16_t 	_Tx_InIndex;
// 	uint16_t 	_Tx_OutIndex;
// 	uint8_t 	_Tx_State;
};









#endif
