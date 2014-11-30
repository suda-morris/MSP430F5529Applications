#include "ADS8344.h"

unsigned char ADS8344_SendByte(unsigned char value)
{
	unsigned char ret = 0;
	ADS8344_CS_Low();
	ret = USCIA1_SPI_SendByte(value);
	ADS8344_CS_High();
	return ret;
}


void ADS8344_Init(void)
{
	GPIO_Set_Direction_Out(7,0,HIGH);
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(100000);				//设置波特100KHz
	USCIA1_SPI_Config();
	USCIA1_ExitConfig();
}

float ADS8344_ADConverter(unsigned char channel)
{
	unsigned int i = 0;
	unsigned int controlbyte = 0;
	unsigned long value = 0;
	unsigned int max = 0;
	unsigned int min = 0;
	unsigned long temp = 0;
	float ret = 0;
#ifdef  ADS8344_Dif
	controlbyte = 0x80 | (channel<<4);
#else
	controlbyte = 0x84 | (channel<<4);
#endif
	ADS8344_SendByte(controlbyte);
	value = ADS8344_SendByte(0x00);
	value = value<<8;
	value += ADS8344_SendByte(0x00);
	max=min=value;
	for(i=0;i<258;i++)									//均值滤波
	{
		ADS8344_SendByte(controlbyte);
		value = ADS8344_SendByte(0x00);
		value = value<<8;
		value += ADS8344_SendByte(0x00);
		if(value > max)
		{
			max = value;
		}
		if(value < min)
		{
			min = value;
		}
		temp += value;
	}
	temp = (temp-max-min) >> 8;
	ret = (float)temp/65536*ADS8344_REF;
	return ret;
}
