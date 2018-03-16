#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "Vector3f.h"
#include "GPIO.h"

#include "sdio_sdcard.h"
#include "stdio.h"
#include <iostream>


#include "ff.h"
#include "wavplay.h" 
#include "audio.h"



using namespace std;



__audiodev audiodev;


//I2S2 TX DMA ����
//����Ϊ˫����ģʽ,������ DMA ��������ж�
//buf0:M0AR ��ַ.
//buf1:M1AR ��ַ.
//num:ÿ�δ���������
void I2S2_TX_DMA_Init(uint8_t* buf0,uint8_t *buf1,u16 num)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);//DMA1 ʱ��ʹ��
	DMA_DeInit(DMA1_Stream4);
	
	while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE){}//�ȴ�������
		
	/* ���� DMA Stream */
	DMA_InitStructure.DMA_Channel = DMA_Channel_0; 								//ͨ�� 0 SPI2_TX ͨ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;					//�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = (u32)buf0;							//DMA �洢�� 0 ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;						//�洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize = num;										//���ݴ�����
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//�洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;	//�������ݳ���:16 λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//�洢�����ݳ��ȣ� 16 λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								// ʹ��ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//�����ȼ�
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; 						//��ʹ�� FIFO ģʽ
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
		
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;					//����ͻ�����δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			//�洢��ͻ�����δ���
	DMA_Init(DMA1_Stream4, &DMA_InitStructure);									//��ʼ�� DMA Stream
		
	DMA_DoubleBufferModeConfig(DMA1_Stream4,(u32)buf1,DMA_Memory_0);			//˫����ģʽ����
	DMA_DoubleBufferModeCmd(DMA1_Stream4,ENABLE);								//˫����ģʽ����
	
	DMA_ITConfig(DMA1_Stream4,DMA_IT_TC,ENABLE);								//������������ж�
	
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;				//��ռ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;						//��Ӧ���ȼ� 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);//����
}





void I2S2_Init(uint16_t I2S_DataFormat,uint32_t I2S_AudioFreq)
{
	I2S_InitTypeDef I2S_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	
	
	//IO��ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);					//GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;		//PB3~5���ù������	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;							//���ù���
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;							//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;						//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;						//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);									//��ʼ��
	
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource12, GPIO_AF_SPI2);  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);

	
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);//ʹ�� SPI2 ʱ��
	
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,ENABLE); //��λ SPI2
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_SPI2,DISABLE);//������λ
	
	
	
	I2S_InitStructure.I2S_Mode=I2S_Mode_MasterTx;						//IIS ģʽ ������
	I2S_InitStructure.I2S_Standard=I2S_Standard_Phillips;				//IIS ��׼Phillips
	I2S_InitStructure.I2S_DataFormat=I2S_DataFormat;				//IIS ���ݳ���	16λ
	I2S_InitStructure.I2S_MCLKOutput=I2S_MCLKOutput_Disable;			//��ʱ�������ֹ
	I2S_InitStructure.I2S_AudioFreq=I2S_AudioFreq;						//IIS Ƶ������
	I2S_InitStructure.I2S_CPOL=I2S_CPOL_Low;							//����״̬ʱ�ӵ�ƽ
	I2S_Init(SPI2,&I2S_InitStructure);									//��ʼ�� IIS
	
	SPI_I2S_DMACmd(SPI2,SPI_I2S_DMAReq_Tx,ENABLE);//SPI2 TX DMA ����ʹ��.
	I2S_Cmd(SPI2,ENABLE);//SPI2 I2S EN ʹ��.
	
	
	RCC_I2SCLKConfig(RCC_I2S2CLKSource_PLLI2S);
    RCC_PLLI2SCmd(ENABLE);
	while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == SET);
} 



//I2S DMA �ص�����ָ��
void (*i2s_tx_callback)(void); //TX �ص�����

//DMA1_Stream4 �жϷ�����
extern "C" void DMA1_Stream4_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_Stream4,DMA_IT_TCIF4)==SET)//������ɱ�־
	{
		
		DMA_ClearITPendingBit(DMA1_Stream4,DMA_IT_TCIF4);
		i2s_tx_callback(); //ִ�лص�����,��ȡ���ݵȲ����������洦��
	}
}



__align(32) uint8_t audio_buf1[WAV_I2S_TX_DMA_BUFSIZE];
__align(32) uint8_t audio_buf2[WAV_I2S_TX_DMA_BUFSIZE];
__align(32) uint8_t audio_tbuf[WAV_I2S_TX_DMA_BUFSIZE];
FIL		audio_file;



int main(void)
{
	FATFS fs_sd;
	uint8_t result=0;
//	FIL file;
//	TCHAR buffer[150];
//	uint8_t buf[1024];
//	UINT	sz;
//	UINT	i;
//	uint32_t time;
	__wavctrl wavOldMoney;
	u32 fillnum; 
	//DWORD vsn;
	
	
	int res;
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);
	delay_init();
	usart1_init(500000);
	delay_ms(500);
		
	GPIO led(PG15);
	led.init(GPIO::Mode_In);

	printf("program start\r\n");
	
	while(SD_Init())//��ⲻ��SD��
	{
		led = !led;
		delay_ms(500);
	}
		
	if(!f_mount(&fs_sd,"0:",1))
	{
		cout <<"f_mount result:"<< res<<endl;
	}
	else	
	{
		cout <<"f_mount fail"<<endl;
		while(1);
	}
	
	
	audiodev.file=&audio_file;
	audiodev.i2sbuf1=audio_buf1;
	audiodev.i2sbuf2=audio_buf2;
	audiodev.tbuf=audio_tbuf;
	
	result = wav_open(audiodev.file,"old money.wav",&wavOldMoney);
	if(!result)
	{
		cout <<"open file finish:"<<endl;
	}
	else
	{
		cout <<"open fail fail:"<<(uint32_t)result<<endl;
		while(1);
	}
	
	
	I2S2_Init(I2S_DataFormat_16b,I2S_AudioFreq_44k);								//��ʼ��I2S,������16λ
	I2S2_TX_DMA_Init(audiodev.i2sbuf1,audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE/2); 	//����TX DMA
	i2s_tx_callback = wav_i2s_dma_tx_callback;
	
	audio_stop();
	res = f_open(audiodev.file,"old money.wav",FA_READ);
	
	if(res == 0)
	{
		f_lseek(audiodev.file, wavOldMoney.datastart);//�����ļ�ͷ
		//printf("start:%x\r\n", f_tell(audiodev.file));
		fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavOldMoney.bps);
		fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavOldMoney.bps);

		cout<<"start"<<endl;
		audio_start();  
		

		while(res == 0)
		{
			while(wavtransferend==0);//�ȴ�wav�������; 
			wavtransferend=0;		//�����ɱ�־
			
			
			if(fillnum!=WAV_I2S_TX_DMA_BUFSIZE)//���Ž���?
			{
				cout<<"stop"<<endl;
				audio_stop(); 
				f_close(audiodev.file);
				break;
			} 
						
						
			if(wavwitchbuf)
				fillnum=wav_buffill(audiodev.i2sbuf2,WAV_I2S_TX_DMA_BUFSIZE,wavOldMoney.bps);//���buf2
			else 
				fillnum=wav_buffill(audiodev.i2sbuf1,WAV_I2S_TX_DMA_BUFSIZE,wavOldMoney.bps);//���buf1
			
			//cout<<"load"<<endl;
		}
	
	}
	else
	{
		cout<<"open file fail"<<endl;
	}
	
	
	
	
	
	

	
	
	
	
	
	

	
//	if(FR_OK == f_getlabel("0:",label,&vsn))
//	{
//		cout<<"label:"<<label<<endl;
//		cout<<"vsn:"<<vsn<<endl;
//	}	
//	else
//	{
//		cout<<"get label error"<<endl;
//	}
	
	
//	if(!f_open(&file,"�ı��ĵ�.txt",FA_OPEN_EXISTING|FA_READ))
//	{	
//		cout << f_gets(buffer,150,&file)<<endl;
//		
//	}
//	else
//	{
//		cout<<"open file error"<<endl;
//	}
	
	
//	if(!f_open(&file,"testfile.dat",FA_CREATE_ALWAYS|FA_WRITE))
//	{	
//		time = micros();
//		f_write(&file,buf,1024,&sz);
//		
//		cout<<"time write="<<micros()-time<<endl;
//		
//		f_close(&file);
//		
//		cout << "writing finish:"<<sz<<endl;
//		
//		
//	}
//	else
//	{
//		cout<<"open file error"<<endl;
//	}
	
	
//	if(!f_mkdir("�½��ļ���"))
//	{
//		cout<<"mkdir finish"<<endl;
//	}
//	else
//	{
//		cout<<"mkdir fail"<<endl;
//	}
//	
//	
//	if(!f_open(&file,"//�½��ļ���//�ı��ĵ�.txt",FA_OPEN_ALWAYS|FA_WRITE))
//	{	
//		f_puts("�ı��ĵ�\r\n",&file);
//		f_close(&file);
//		cout << "writing in dirction finish"<<endl;
//		
//	}
//	else
//	{
//		cout<<"open file error"<<endl;
//	}
	
	
	
	
	
	
	
	
//	f_open(&file,"mytxt.txt",FA_OPEN_ALWAYS|FA_WRITE);
//	f_puts("test\r\n",&file);
//	f_close(&file);
//	cout <<"writing finish\r\n"<<endl;

	
	
	
	
	while (1)
	{}
}





