#include "SPIFLASH_W25X.h"



//ָ���
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


uint8_t SPIFLASH_W25X::write_buffer[4096];	//static uint8_t *write_buffer = new uint8_t[sector_size()];




uint16_t SPIFLASH::write_in_sector(uint32_t address,void *buf,uint16_t len)const
{
	uint16_t secremain;	      
	secremain = sector_size() - address%sector_size();	//����ʣ��ռ��С
	
   	if(len<=secremain)secremain=len;		//����������ʣ��ռ��С
	write_nocheck(address,buf,secremain);	

	return secremain;	//����ʵ��д���ֽ�

}



void SPIFLASH_W25X::init()
{
	
	_spi.cs = 1;
	_id = read_id();

	
	_sector_size = 4*1024;
	switch(_id)
	{
		case W25X40:
			_flash_size = 512*1024;	//512K Byte
			break;
		case W25Q80:
			_flash_size = 1*1024*1024;	//1M Byte
		
			break;
		case W25Q16:
			_flash_size = 2*1024*1024;	//2M Byte
			break;
		case W25Q32:
			_flash_size = 4*1024*1024;	//4M Byte
			break;
		case W25Q64:
			_flash_size = 8*1024*1024;	//8MByte
			break;		
		case W25Q128:
			_flash_size = 16*1024*1024;	//16MByte
			break;	
		
	}
}



uint16_t SPIFLASH_W25X::read_id()
{
	uint16_t temp = 0;	 
	
	_spi.cs = 0;	
	
	_spi.write(0x90);//���Ͷ�ȡID����	    
	_spi.write(0x00); 	    
	_spi.write(0x00); 	    
	_spi.write(0x00); 	 			
	
	temp|=_spi.read()<<8;  
	temp|=_spi.read();	 
	
	_spi.cs = 1;	
	
	return temp;
}

uint8_t SPIFLASH_W25X::status()const
{
	_spi.cs = 0;
	_spi.write(W25X_ReadStatusReg);
	uint8_t sta = _spi.read();
	_spi.cs = 1;
	
	return sta;
}

void SPIFLASH_W25X::status(uint8_t sta)const
{
	_spi.cs = 0;                       //ʹ������   
	_spi.write(W25X_WriteStatusReg);   //����дȡ״̬�Ĵ�������    
	_spi.write(sta);               		//д��һ���ֽ�  
	_spi.cs = 1;                       //ȡ��Ƭѡ 
}


void SPIFLASH_W25X::write_enable()const
{
	_spi.cs = 0;                          //ʹ������   
    _spi.write(W25X_WriteEnable);      	//����дʹ��  
	_spi.cs = 1;                          //ȡ��Ƭѡ  
} 



void SPIFLASH_W25X::write_disable()const
{
	_spi.cs = 0;                          //ʹ������   
    _spi.write(W25X_WriteDisable);      	//����дʹ��  
	_spi.cs = 1;                          //ȡ��Ƭѡ  
} 



//Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void SPIFLASH_W25X::write_page(uint32_t address,void *buf,uint16_t len)const
{
	while(!is_ready()){};	
	_spi.cs = 0; 
    _spi.write(W25X_ReadData);         //���Ͷ�ȡ����   
    _spi.write((uint8_t)((address)>>16));  //����24bit��ַ    
    _spi.write((uint8_t)((address)>>8));   
    _spi.write((uint8_t)address);  	

	_spi.write(buf,len);
	_spi.cs = 1;
}


void SPIFLASH_W25X::write_nocheck(uint32_t address,void *buf,uint32_t len)const
{ 			 		 
	uint16_t pageremain;	
	
	pageremain=256-address%256; 		//��ҳʣ����ֽ���		 	    
	if(len<=pageremain)pageremain=len;	//������256���ֽ�
	
	uint8_t *p = (uint8_t*)buf;
	while(1)
	{	   
		write_page(address,p,pageremain);
		
		if(len==pageremain)break;//д�������
	 	else 
		{
			p+=pageremain;
			address+=pageremain;	
			len-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			
			if(len>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=len; 	  //����256���ֽ���
		}
	};	    
}




void SPIFLASH_W25X::write(uint32_t address,const void *buf,uint32_t len)const	 
{ 
	uint32_t secpos;
	uint16_t secoff;
	uint16_t secremain;	   
 	uint16_t i;    
	
	uint8_t *W25QXX_BUF = SPIFLASH_W25X::write_buffer;	  
	uint8_t *p = (uint8_t*)buf;
 	secpos=address/sector_size();	//������ַ  
	secoff=address%sector_size();	//�������ڵ�ƫ��
	secremain=sector_size()-secoff;	//����ʣ��ռ��С   
	
	
 	if(len<=secremain)secremain=len;//����������ʣ��ռ��С
	while(1) 
	{	
		read(secpos*sector_size(),W25QXX_BUF,sector_size());//������������������
		for(i=0;i<secremain;i++)//У��ʣ��ռ�Ϊ0xFF
		{
			if(W25QXX_BUF[secoff+i]!=0xFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			erase(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=p[i];	  
			}
			write_nocheck(secpos*sector_size(),W25QXX_BUF,sector_size());//д����������  
		}
		else 
		{
			write_nocheck(address,p,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		}
		
		if(len==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	p+=secremain;  //ָ��ƫ��
			address+=secremain;//д��ַƫ��	   
		   	len-=secremain;				//�ֽ����ݼ�
			if(len>sector_size())secremain=sector_size();	//��һ����������д����
			else secremain=len;			//��һ����������д����
		}	 
	}
}


void SPIFLASH_W25X::erase_chip()const
{
	while(!is_ready());   
	_spi.cs = 0;                           //ʹ������   
	_spi.write(W25X_ChipErase);        //����Ƭ��������  
	_spi.cs = 1;    
}
//����һ��ɽ��������ʱ��:150ms
void SPIFLASH_W25X::erase(uint32_t sec_index)const
{
	sec_index *= sector_size();	//�ֽڵ�ַ
	
	while(!is_ready());   
	_spi.cs = 0;                           //ʹ������   
	_spi.write(W25X_SectorErase);      //������������ָ�� 
	_spi.write((uint8_t)((sec_index)>>16));  //����24bit��ַ    
	_spi.write((uint8_t)((sec_index)>>8));   
	_spi.write((uint8_t)sec_index);  
	_spi.cs = 1;  	
	_spi.cs = 1; 	

}


void SPIFLASH_W25X::read(uint32_t address,void *buf,uint32_t len)const
{ 
 	uint16_t i;   										    
	_spi.cs = 0;                           //ʹ������   
    _spi.write(W25X_ReadData);         //���Ͷ�ȡ����   
    _spi.write((uint8_t)((address)>>16));  //����24bit��ַ    
    _spi.write((uint8_t)((address)>>8));   
    _spi.write((uint8_t)address);   
	
	uint8_t *p = (uint8_t*)buf;
    for(i=0;i<len;i++)
	{ 
        *p++=_spi.write(0XFF);   //ѭ������  
    }
	_spi.cs = 1;
}




