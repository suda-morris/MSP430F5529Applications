#include "Clock/DS1085.h"

/*
 * 引脚连接图：
 *
 */
const offset_t Offset_Table[13] = {
		{-6,30.7,35.8},{-5,33.3,38.4},{-4,35.8,41.0},{-3,38.4,43.5},{-2,41.0,46.1},
		{-1,43.5,48.6},{0,46.1,51.2},{1,48.6,53.8},{2,51.2,56.3},{3,53.8,58.9},{4,56.3,61.4},
		{5,58.9,64.0},{6,61.4,66.6}
};

void DS1085_Init(void)
{
	USCIB0_EnterIntoConfig();					//USCIB0进入配置模式
	USCIB0_SetBaud(400000);						//设置成400kHz的波特率,器件工作在快速模式下
	USCIB0_IIC_Config(DS1085_DeviceAddr);		//配置IIC，设置从器件地址
	USCIB0_ExitConfig();						//USCIB0退出配置模式
	DS1085_WriteAddr(0x00);						//配置器件地址为1011000，自动将寄存器的更改写入EEPROM
	DS1085_SetMainClk(66);						//DS1085内部主时钟66MHz
	DS1085_WriteMUX(0x1f80);					//预分频初始化为8
}

void DS1085_WriteReg(u8 reg, u8 data)
{
	USCIB0_IIC_SendByte(reg,data);
}

void DS1085_WriteBuffer(u8 reg,u8 length,u8* buffer)
{
	USCIB0_IIC_SendBuffer(reg,length,buffer);
}

void DS1085_ReadBuffer(u8 reg,u8 length,u8*buffer)
{
	USCIB0_IIC_ReceiveBuffer(reg,length,buffer);
}

void DS1085_WriteMUX(unsigned int mux)
{
	unsigned char byte[2];
	byte[0] = (unsigned char)(mux>>8);
	byte[1] = (unsigned char)(mux);
	DS1085_WriteBuffer(DS1085_MUX,2,byte);
}

void DS1085_WriteDIV(unsigned int div)
{
	unsigned char byte[2];
	if(div<2 || div>1025)		//div的取值范围在2~1025之间（包括）
	{
		return;
	}
	byte[0] = (unsigned char)((div-2)>>2);
	byte[1] = (unsigned char)(((div-2)%4)<<6);
	DS1085_WriteBuffer(DS1085_DIV,2,byte);
}

void DS1085_WriteAddr(unsigned char bus)
{
	DS1085_WriteReg(DS1085_ADDR,bus);
}

/*
 * 设置预分频，div0是out0的预分频，div1是out1的预分频
 */
void DS1085_PreDiv(unsigned char div0,unsigned char div1)
{
	unsigned char buffer[2] = {0,0};
	USCIB0_IIC_ReceiveBuffer(DS1085_MUX,2,buffer);
	buffer[0] &= ~0x07;
	buffer[1] &= ~0x80;
	buffer[0] |= (div0<<1);
	buffer[0] |= (div1>>1);
	buffer[1] |= ((div1%2)<<7);
	DS1085_WriteBuffer(DS1085_MUX,2,buffer);
}

void DS1085_SetMainClk(float mhz)
{
	unsigned int dac = 0;
	unsigned char i = 0;
	unsigned char buffer[2] = {0,0};
	char offset = 0;
	for(i=0;i<13;i++)
	{
		if((mhz < Offset_Table[i].range_low))
		{
			offset = Offset_Table[i-1].offset;
			dac = (mhz - Offset_Table[i-1].range_low)*1000/5;
			break;
		}
		if(i == 12)
		{
			offset = Offset_Table[i].offset;
			dac = (mhz - Offset_Table[i].range_low)*1000/5;
		}
	}
	DS1085_ReadBuffer(DS1085_RANGE,2,buffer);
	offset = (buffer[0]>>3)+offset;
	DS1085_WriteReg(DS1085_OFFSET,offset);
	buffer[0] = (unsigned char)(dac>>2);
	buffer[1] = (unsigned char)((dac%4)<<6);
	DS1085_WriteBuffer(DS1085_DAC,2,buffer);
}
