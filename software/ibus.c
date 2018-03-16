#include "ibus.h"



#define IBUS_MAX_CHANNEL 14
#define IBUS_TELEMETRY_PACKET_LENGTH (4)
#define IBUS_SERIAL_RX_PACKET_LENGTH (32)

enum 
{
	IBUS_MODEL_IA6B = 0,
	IBUS_MODEL_IA6
}; 




static uint8_t ibusModel;		//IBUS_MODEL_IA6B �� IBUS_MODEL_IA6
static uint8_t ibusFrameSize;	//֡��
ibus_t ibus;


/*************IBUSЭ��***********************
1.Ia6
��С31�ֽ�,֡ͷ0x55
0.0x55
1-28:14��ͨ��������,ÿ��ͨ�������ֽ�
[29 30]:��У��,ͨ�������ۼӺ�ȡ��16λ,С��ģʽ,�͵�ַ��ŵ��ֽ�

2.Ia6B
[0 1]	֡ͷ,ָʾ֡����
[2 29]	14��ͨ������
[30 31]	��У�鲹,���ϵ�0-29�ֽڵĺ�Ϊ0xFFFF

ͨ����Сֵ1000,���ֵ2000,��ָ1500
ң���źŶ�ʧ��ֹͣ�������
*************************************/


static uint8_t isValidIa6bIbusPacketLength(uint8_t length)
{
	return (length == IBUS_TELEMETRY_PACKET_LENGTH) || (length == IBUS_SERIAL_RX_PACKET_LENGTH);
}
static void updateIa6bChannelData(void)
{
#define  byte2u16(_offset)		(ibus.buffer[(_offset<<1) + 2] + (ibus.buffer[(_offset<<1) + 3] << 8))

	if (ibusModel != IBUS_MODEL_IA6B)return;

	ibus.channel.ch1 = byte2u16(0);
	ibus.channel.ch2 = byte2u16(1);
	ibus.channel.ch3 = byte2u16(2);
	ibus.channel.ch4 = byte2u16(3);
	ibus.channel.ch5 = byte2u16(4);
	ibus.channel.ch6 = byte2u16(5);
	ibus.channel.ch7 = byte2u16(6);
	ibus.channel.ch8 = byte2u16(7);
	ibus.channel.ch9 = byte2u16(8);
	ibus.channel.ch10 = byte2u16(9);
// 	ibus.channel.ch11 = byte2u16(10);
// 	ibus.channel.ch12 = byte2u16(11);
// 	ibus.channel.ch13 = byte2u16(12);
// 	ibus.channel.ch14 = byte2u16(13);
}
static uint8_t isChecksumOkIa6(void)
{
	uint8_t offset;
	uint8_t i;
	uint16_t chksum, rxsum;
	
	rxsum = ibus.buffer[ibusFrameSize - 2] + (ibus.buffer[ibusFrameSize - 1] << 8);
	chksum = 0;
	for (i = 0, offset = 1; i < IBUS_MAX_CHANNEL; i++, offset += 2) 
	{
		chksum += ibus.buffer[offset] + (ibus.buffer[offset + 1] << 8);
	}
	return chksum == rxsum;
}
static uint8_t isChecksumOkIa6b()
{
	uint8_t i;
	uint16_t chksum, rxsum;

	uint8_t dataSize = ibus.buffer[0] - 2;

	rxsum = ibus.buffer[ibusFrameSize - 2] + (ibus.buffer[ibusFrameSize - 1] << 8);
	chksum = 0;
	for (i = 0; i < dataSize; i++) 
	{
		chksum += ibus.buffer[i];
	}

	return (chksum + rxsum == 0xFFFF);
}
static uint8_t isChecksumOk(void)
{
	if (ibusModel == IBUS_MODEL_IA6B)
	{
		return isChecksumOkIa6b();
	}
	else if (ibusModel == IBUS_MODEL_IA6)
	{
		return isChecksumOkIa6();
	}
	else
		return 0;
}


void ibus_init(void)
{
	ibus.state = IBUS_EMPTY;
	ibus.rx_cnt = 0;
}
void ibus_recvbyte_event(uint8_t byte)
{
	if (ibus.rx_cnt == 0)		//��һ���ֽ�,��ʽ,����4��32,
	{
		if (isValidIa6bIbusPacketLength(byte))	//Ia6B
		{
			ibusModel = IBUS_MODEL_IA6B;
			ibusFrameSize = byte;
		}
		else if (byte == 0x55)		//Ia6
		{
			ibusModel = IBUS_MODEL_IA6;
			ibusFrameSize = 31;
		}
		else
		{
			return;
		}
	}
	if (ibus.rx_cnt < sizeof(ibus.buffer))
	{
		ibus.buffer[ibus.rx_cnt++] = byte;
	}

}
void ibus_frame_event(void)
{
	if (ibus.state != IBUS_USING && isChecksumOk())	//У��
	{
		ibus.state = IBUS_EMPTY;
		updateIa6bChannelData();	
		ibus.state = IBUS_AVAILABLE;
	}
	ibus.rx_cnt = 0;
	
}

