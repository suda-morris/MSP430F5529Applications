#include "DAC/TLV5614.h"
/*
 * 引脚连接图：
 * 			P4.0---->SCLK
 * 			P4.4---->DIN
 * 			P2.1--->FS
 * 	时钟特性：
 * 			主机在时钟上升沿把数据发送到数据线，从机（TLV5614）在时钟下降沿从数据线上读取数据。时钟信号空闲时候位高电平
 */

//address consists two bits:
//A1,A0(decides the ch);control consists two bits:PWR(1=power off,0=normal),SPD(1=Fast,0=Slow)
void TLV5614_DAConverter(unsigned char address,float voltage)
{
	unsigned int temp[2] = {0,0};
	temp[0] = (unsigned int)(voltage*2048/(float)TLV5614_REF);
	temp[1]=(((unsigned int)address)<<14)|(((unsigned int)1)<<12)|temp[0];
	GPIO_SetBit(2,1);
	delayus(1);
	GPIO_ResetBit(2,1);				//FS下降沿
	USCIA1_SPI_SendByte((unsigned char)(temp[1]>>8));
	USCIA1_SPI_SendByte((unsigned char)temp[1]);
	GPIO_SetBit(2,1);				//FS拉高
}

void TLV5614_Init()
{
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(20000000);				//设置为20MHz的波特率
	USCIA1_SPI_Config();
	USCIA1_ExitConfig();
	GPIO_Set_Direction_Out(2,1,HIGH);		//FS引脚模式设置为输出
}
