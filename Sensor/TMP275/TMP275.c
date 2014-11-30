#include "TMP275.h"

void TMP275_WriteReg(u8 reg, u8 data)
{
	USCIB0_IIC_SendByte(reg,data);
}

void TMP275_Init()
{
	USCIB0_EnterIntoConfig();					//USCIB0进入配置模式
	USCIB0_SetBaud(400000);						//设置成400kHz的波特率
	USCIB0_IIC_Config(TMP275_DeviceAddr);		//配置IIC，设置从器件地址
	USCIB0_ExitConfig();						//USCIB0退出配置模式
	USCIB0_IIC_SendByte(TMP275_ConfigReg,0x60);	//配置TMP275的工作方式:关断模式被关闭，启用比较模式，
												//ALERT引脚低电平有效，故障队列长度为1，转换器分辨率12位
}

double TMP275_ReadTemp()
{
	double value = 1;
	unsigned char buffer[2];
	signed int temp = 0;
	USCIB0_IIC_ReceiveBuffer(TMP275_TempReg,2,buffer);
	temp += buffer[1];
	temp <<= 8;
	temp += buffer[0];
	temp &= 0xfff0;
	temp >>= 4;
	if(temp & 0x800)				//是负数
	{
		temp ^= 0xfff;
		temp += 1;
		value = -1;
	}
	value = value*((double)temp)*0.0625;
	return value;
}

