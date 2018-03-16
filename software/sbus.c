#include "sbus.h"
#include "string.h"

sbus_t sbus;


void sbus_init(void)
{
	sbus.state = SBUS_EMPTY;
	sbus.rx_cnt = 0;

}

void sbus_recvbyte_event(uint8_t byte)
{
	if (sbus.rx_cnt < sizeof(sbus.buffer))
	{
		sbus.buffer[sbus.rx_cnt++] = byte;
	}
}


void sbus_frame_event()
{
	if ((sbus.state != SBUS_USING)		//�����������ڴ���
		&& (sbus.rx_cnt == 25)			//�յ�����Ч������
		&& (sbus.buffer[0] == 0x0F)		//����֡ͷУ��
		&& (sbus.buffer[24] == 0x00))	//����֡βУ��
	{
		sbus.state = SBUS_EMPTY;
		memcpy(&sbus.channel, sbus.buffer, 25);
		sbus.state = SBUS_AVAILABLE;
	}

	sbus.rx_cnt = 0;
}


