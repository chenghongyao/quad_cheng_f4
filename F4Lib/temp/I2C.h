#ifndef _I2C_H_
#define _I2C_H_


#include "sys.h"




#define I2C_WAIT_ACK_OK			0
#define I2C_WAIT_ACK_TIMEOUT	1





class I2C
{
public:
	I2C(uint8_t slave_addr):_slave_addr(slave_addr),_fast_mode(true){}
		
	virtual ~I2C(){};
	virtual void init()const  = 0;
		
	/**************************/
	//�ֶ�����ӻ���ַ
	uint8_t write(uint8_t slave_addr,uint8_t c)const;											//дһ���ֽ�
	uint8_t write(uint8_t slave_addr,const void *buf,uint16_t len)const;						//дһ������
	
	uint8_t write_reg(uint8_t slave_addr,uint8_t reg_addr,uint8_t reg_val)const;					//��Ĵ���дһ������
	uint8_t write_reg(uint8_t slave_addr,uint8_t reg_addr,const void *buf,uint16_t len)const;	//��Ĵ���дһ������


	//��һ���ֽڵĺ��������ش��󣬴���ʱ����Ĭ��ֵ0
	uint8_t read(uint8_t slave_addr)const;
	uint8_t read(uint8_t slave_addr,void *buf,uint16_t len)const;
	
	uint8_t read_reg(uint8_t slave_addr,uint8_t reg_addr)const;								//�ӼĴ�����һ������
	uint8_t read_reg(uint8_t slave_addr,uint8_t reg_addr,void *buf,uint16_t len)const;	//�ӼĴ�����һ������
	/**************************/
	//�Զ����ôӻ���ַ
	//void slave_address(uint8_t addr){_slave_addr = addr;}
	
	uint8_t write(uint8_t c)const{	return write(slave_address(),c);	}										//дһ���ֽ�
	uint8_t write(const void *buf,uint16_t len)const{	return write(slave_address(),buf,len);		}			//дһ������
	uint8_t write_reg(uint8_t reg_addr,uint8_t c)const{	return write_reg(slave_address(),reg_addr,c);	}		//�Ĵ���дһ������
	uint8_t write_reg(uint8_t reg_addr,const void *buf,uint16_t len)const{	return write_reg(slave_address(),reg_addr,buf,len);	}	//��Ĵ���дһ������


	uint8_t read()const{	return read(slave_address());	}
	uint8_t read(void *buf,uint16_t len)const{	return read(slave_address(),buf,len);	}
	uint8_t read_reg(uint8_t reg_addr)const{	return read_reg(slave_address(),reg_addr);}				//�ӼĴ�����һ������
	uint8_t read_reg(uint8_t reg_addr,void *buf,uint16_t len)const{ return read_reg(slave_address(),reg_addr,buf,len);};	//�ӼĴ�����һ������

	/*****************************/
	//uint8_t get_error(){uint8_t t =_error_code;_error_code = 0;return t;}
	uint8_t slave_address()const{return _slave_addr;};
	virtual	void fast_mode(bool fast)const{_fast_mode = fast;};
	bool fast_mode()const{	return _fast_mode;}

private:
	const uint8_t _slave_addr;
	mutable bool 	_fast_mode;
	//uint8_t _error_code;
protected:
	//�ӿ�
	virtual void start() const = 0;
	virtual void stop()const   = 0;
	virtual void ack() const  = 0;
	virtual void nack() const  = 0;
	virtual uint8_t wait_ack() const  = 0;			//Ӧ��ɹ�����0�����򷵻�1

	virtual void write_byte(uint8_t c)const   = 0;	
	virtual uint8_t read_byte()const   = 0;

};




#endif

