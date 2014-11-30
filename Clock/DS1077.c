#include <Clock/DS1077.h>
/*
 *DS1077芯片内部晶振为40MHz
 */


void DS1077_Init(void)
{
	USCIB0_EnterIntoConfig();					//USCIB0进入配置模式
	USCIB0_SetBaud(400000);						//设置成400kHz的波特率,器件工作在快速模式下
	USCIB0_IIC_Config(DS1077_DeviceAddr);		//配置IIC，设置从器件地址
	USCIB0_ExitConfig();						//USCIB0退出配置模式
	DS1077_WriteBUS(0x00);						//配置器件地址为1011000，自动将寄存器的更改写入EEPROM
	DS1077_WriteMUX(0x1f80);
}

void DS1077_WriteReg(u8 reg, u8 data)
{
	USCIB0_IIC_SendByte(reg,data);
}

void DS1077_WriteBuffer(u8 reg,u8 length,u8* buffer)
{
	USCIB0_IIC_SendBuffer(reg,length,buffer);
}

void DS1077_WriteMUX(unsigned int mux)
{
	unsigned char byte[2];
	byte[0] = (unsigned char)(mux>>8);
	byte[1] = (unsigned char)(mux);
	DS1077_WriteBuffer(DS1077_MUX,2,byte);
}

void DS1077_WriteDIV(unsigned int div)
{
	unsigned char byte[2];
	if(div<2 || div>1025)		//div的取值范围在2~1025之间（包括）
	{
		return;
	}
	byte[0] = (unsigned char)((div-2)>>2);
	byte[1] = (unsigned char)(((div-2)%4)<<6);
	DS1077_WriteBuffer(DS1077_DIV,2,byte);
}

void DS1077_WriteBUS(unsigned char bus)
{
	DS1077_WriteReg(DS1077_BUS,bus);
}

/*
 * 设置预分频，div0是out0的预分频，div1是out1的预分频
 */
void DS1077_PreDiv(unsigned char div0,unsigned char div1)
{
	unsigned char buffer[2] = {0,0};
	USCIB0_IIC_ReceiveBuffer(DS1077_MUX,2,buffer);
	buffer[0] &= ~0x07;
	buffer[1] &= ~0x80;
	buffer[0] |= (div0<<1);
	buffer[0] |= (div1>>1);
	buffer[1] |= ((div1%2)<<7);
	DS1077_WriteBuffer(DS1077_MUX,2,buffer);
}
