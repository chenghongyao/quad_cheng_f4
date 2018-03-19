#include "Parameter.h"
#include "SPIFlash.h"
#include "QMath.h"

/********************************************
�ӿ�
get_size,read_param,write_param
��Ա
bool _save,
uint32_t offset;

����ʱ��������offset
���棺
��ȡȫ��������Ҫ������޸ģ�����������д��



magic1,magic2,magic3,
ver,sub_ver
�ܳ���
�ַ�������,�ַ���,���ݳ���,����

1.��ȡ�����ݳ���,�����ں��油������
2.ƥ������,�鿴�ڴ����Ƿ���ڸñ���,��������,������󲹳�..
******************************************/

ParameterBase* ParameterBase::ParametersAddress[PARAMETER_NUM_MAX];
uint16_t ParameterBase::ParametersNum = 0;						
bool ParameterBase::_save_all = false;						


static uint8_t ParameterBuffer[PARAMETER_BUFFER_SIZE];





typedef struct 
{
	uint8_t magic1;
	uint8_t magic2;
	uint8_t magic3;

	uint16_t count;

	uint8_t version;
	uint8_t sub_version;


}ParamHeader;

//���캯��
ParameterBase::ParameterBase()
{
	if(ParametersNum < PARAMETER_NUM_MAX)
	{
		ParametersAddress[ParametersNum] = this;
	}
	else
	{

	}

	ParametersNum++;
}


ParameterBase* ParameterBase::GetAddress(uint16_t index)
{
	if (index >= ParametersNum)
	{
		return NULL;
	}
	else
	{
		return ParametersAddress[index];
	}
}

uint16_t ParameterBase::Count()
{
	return ParametersNum;
}
//ռ�����ڴ��С
uint32_t ParameterBase::Size()
{
	uint32_t total_size = sizeof(ParamHeader);

	uint16_t num = min(PARAMETER_NUM_MAX, ParametersNum);
	for (int i = 0; i < num; i++)
	{
		ParameterBase *pb = ParameterBase::GetAddress(i);
		total_size += pb->param_size();
	}
	return total_size;
}



void param_memory_error(uint8_t type)
{

	
	if (type == 1)
	{
		param_debug("param num error\r\n");
		hal.LEDR = 1;
	}
	else if (type == 2)
	{
		param_debug("param size error\r\n");
		hal.LEDB = 1;
		hal.LEDR = 1;

	}
	else
		return;
	
	while (1);
}



void ParameterBase::Setup()
{

	uint16_t param_count = ParameterBase::Count();					//��������
	uint32_t total_size = ParameterBase::Size();

	param_debug("Parameter Count:%d\r\n", param_count);
	param_debug("Parameter Size:%d byte (%d%%)\r\n", total_size,(uint16_t)(100.0f*(float)total_size/ PARAMETER_BUFFER_SIZE));
	//����������
	if (param_count > PARAMETER_NUM_MAX){
		param_memory_error(1);
	}

	//���ռ�ÿؼ�
	if(total_size > PARAMETER_BUFFER_SIZE){
		param_memory_error(2);
	}

	ParamHeader *ph = (ParamHeader *)ParameterBuffer;
	uint8_t *pparam = (uint8_t *)ParameterBuffer;

	spiflash.read(PARAM_FLASH_ADDR_BASE,ParameterBuffer,sizeof(ParamHeader));		//��ȡ��Ϣ
	if (ph->magic1 == PARAMETER_MAGIC1 && ph->magic2 == PARAMETER_MAGIC2 && ph->magic3 == PARAMETER_MAGIC3&&
		ph->count == param_count && ph->version == PARAMETER_VER && ph->sub_version == PARAMETER_SUB_VER)	//��֤ͷ��Ϣ
	{
		param_debug("param check pass!\r\n");
		spiflash.read(PARAM_FLASH_ADDR_BASE, pparam, total_size);		//��ȡ��������
		pparam += sizeof(ParamHeader);
		for (int i = 0; i < ParameterBase::Count(); i++)
		{
			ParameterBase *pb = ParameterBase::GetAddress(i);
			pb->param_get(pparam);
			pparam += pb->param_size();
		}
		return;
	}

	param_debug("param check fail!\r\n");
	param_debug("param init....!\r\n");
	ph->magic1 = PARAMETER_MAGIC1;
	ph->magic2 = PARAMETER_MAGIC2;
	ph->magic3 = PARAMETER_MAGIC3;
	ph->count = param_count;
	ph->version = PARAMETER_VER;
	ph->sub_version = PARAMETER_SUB_VER;
	


	ParameterBase::SaveAll();
	param_debug("param finish....!\r\n");
}


void ParameterBase::SaveAll()
{
	spiflash.erase_sector(PARAM_FLASH_ADDR_BASE/spiflash.sector_size());

	uint8_t *pparam = ParameterBuffer + sizeof(ParamHeader);
	for (int i = 0; i < ParameterBase::Count(); i++)
	{
		ParameterBase *pb = ParameterBase::GetAddress(i);
		 pb->param_set(pparam);
		 pparam += pb->param_size();
	}	

	spiflash.write_sector(PARAM_FLASH_ADDR_BASE,ParameterBuffer,pparam - ParameterBuffer);
	_save_all = false;

	param_debug("param size=%d\r\n",pparam - ParameterBuffer);

}


