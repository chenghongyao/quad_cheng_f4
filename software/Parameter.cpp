#include "Parameter.h"
#include "SPIFlash.h"


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





ParameterBase::ParameterBase()
{
	if(ParametersNum < PARAMETER_NUM_MAX)
	{
		ParametersAddress[ParametersNum] = this;
		ParametersNum ++;
	}
	else
	{
		
	}
}



int ParameterBase::Count()
{
	return ParametersNum;
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


void ParameterBase::Setup()
{
	uint16_t param_count = ParameterBase::Count();					//��������
	param_debug("Parameter Count:%d\r\n",ParameterBase::Count());	


	ParamHeader *ph = (ParamHeader *)ParameterBuffer;
	uint8_t *pparam = (uint8_t *)ParameterBuffer;

	spiflash.read(PARAM_FLASH_ADDR_BASE,ParameterBuffer,sizeof(ParamHeader));		//��ȡ��Ϣ
	if (ph->magic1 == PARAMETER_MAGIC1 && ph->magic2 == PARAMETER_MAGIC2 && ph->magic3 == PARAMETER_MAGIC3&&
		ph->count == param_count && ph->version == PARAMETER_VER && ph->sub_version == PARAMETER_SUB_VER)	//��֤ͷ��Ϣ
	{
		param_debug("param check pass!\r\n");
		spiflash.read(PARAM_FLASH_ADDR_BASE, pparam, PARAMETER_BUFFER_SIZE);		//��ȡ��������
		pparam += sizeof(ParamHeader);
		for (int i = 0; i < ParameterBase::Count(); i++)
		{
			ParameterBase *pb = ParameterBase::GetAddress(i);
			pparam += pb->param_get(pparam);
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
		pparam += pb->param_set(pparam);
	}	
	spiflash.write_sector(PARAM_FLASH_ADDR_BASE,ParameterBuffer,pparam - ParameterBuffer);
	
	_save_all = false;

	param_debug("param size=%d\r\n",pparam - ParameterBuffer);

}


