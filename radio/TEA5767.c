#include "TEA5767.h"
/*
 * 【新闻911】,91.1MHz
 * 【江苏交通广播网】,101.1MHz
 * 【动听948】,94.8MHz
 */
unsigned char const Radio_PowerUpData[5]={0xb1,0xa0,0x20,0x11,0x00}; //上电复位后设置为静音
unsigned char const Radio_ConfigData[5]={0x31,0xa0,0x20,0x11,0x00};  //写入TEA5767的配置数据

void Radio_Init()
{
	USCIB0_EnterIntoConfig();					//USCIB0进入配置模式
	USCIB0_SetBaud(400000);						//设置成400kHz的波特率
	USCIB0_IIC_Config(TEA5767_DeviceAddr);		//配置IIC，设置从器件地址
	USCIB0_ExitConfig();						//USCIB0退出配置模式
	TEA5767_WriteBuffer((u8*)Radio_PowerUpData);//上电后写入的数据,静音操作
	TEA5767_WriteBuffer((u8*)Radio_ConfigData);	//初步配置
	Radio_SetFrequency(91100);					//初始化电台频率
}

void TEA5767_WriteBuffer(u8* buffer)
{
	u8 i = 0;
	USCIB0_IIC_Start_Trans();
	for(i=0;i<5;i++)
	{
		while(!(UCB0IFG & UCTXIFG));		//等待TXBUF再次变空
		UCB0TXBUF = buffer[i];
	}
	while(!(UCB0IFG & UCTXIFG));			//此时数据并不一定发送完毕
	USCIB0_IIC_Stop();
	delayms(1);								//等待发送完成
}

void TEA5767_ReadBuffer(u8* buffer)
{
	u8 i = 0;
	buffer[0] = UCB0RXBUF;					//清空接收缓存器
	USCIB0_IIC_Start_Receive();				//启动读操作
	for(i=0;i<5;i++)
	{
		while(!(UCB0IFG & UCRXIFG));		//等待数据接收完毕
		if(i==4)
		{
			USCIB0_IIC_NoACK();				//发送NACK位和停止位
		}
		buffer[i] = UCB0RXBUF;
	}
	USCIB0_IIC_Stop();
}

unsigned long Radio_CurrentFrequency()		//获得当前频率
{
	unsigned long frequency = 0;
	unsigned int pll = 0;
	u8 receiveBuffer[5];
	TEA5767_ReadBuffer(receiveBuffer);
	pll = (receiveBuffer[0] & 0x3f)*256 + receiveBuffer[1];
	if((Radio_ConfigData[2]) & 0x10)			//高端本振注入
	{
		frequency=(unsigned long)(((float)(pll))*(float)8.192-225);    //频率单位:KHz
	}
	else									//低端本振注入
	{
		frequency=(unsigned long)(((float)(pll))*(float)8.192+225);    //频率单位:KHz
	}
	return frequency;
}

unsigned int Radio_CaculatePll(unsigned long frequency)		//根据输入的频率值计算相应的PLL数值
{
	unsigned int pll = 0;
	if(Radio_ConfigData[2]&0x10)			//高端本振注入
	{
		pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //频率单位:k
	}
	else									//低端本振注入
	{
		pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //频率单位:k
	}
	return pll;
}

void Radio_SetFrequency(unsigned long frequency)
{
	unsigned int pll = 0;
	u8 radio_write_data[5];
	if((frequency<RADIO_Fre_MIN)||(frequency>RADIO_Fre_MAX))		//频率不在欧美频段范围内
	{
		return;
	}
	pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//非静音，非搜索模式,pll高6位
    radio_write_data[1]=pll%256;				//pll低8位
    radio_write_data[2]=0x20;					//低端本振注入
    radio_write_data[3]=0x11;					//欧美频段，Fxtal=32.768KHz，高音切割关闭，立体声噪声消除关闭
    radio_write_data[4]=0x00;					//6.5MHz参考频率PLL不可用，取加重时间常数50us
    TEA5767_WriteBuffer(radio_write_data);
}

unsigned long Radio_ManualSearchHigh()			//频率+10KHz,不用考虑TEA5767用于搜台的相关位:SM,SUD
{
	unsigned long frequency = 0;
	unsigned int pll = 0;
	u8 radio_write_data[5];
    frequency = Radio_CurrentFrequency();
    frequency += 10;
    if(frequency > RADIO_Fre_MAX)
    {
    	frequency = RADIO_Fre_MIN;
    }
    pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//非静音，非搜索模式,pll高6位
    radio_write_data[1]=pll%256;				//pll低8位
    radio_write_data[2]=0x20;					//低端本振注入
    radio_write_data[3]=0x11;					//欧美频段，Fxtal=32.768KHz，高音切割关闭，立体声噪声消除关闭
    radio_write_data[4]=0x00;					//6.5MHz参考频率PLL不可用，取加重时间常数50us
    TEA5767_WriteBuffer(radio_write_data);
    return frequency;
}

unsigned long Radio_ManualSearchLow()			//频率-10KHz,不用考虑TEA5767用于搜台的相关位:SM,SUD
{
	unsigned long frequency = 0;
	unsigned int pll;
	u8 radio_write_data[5];
    frequency = Radio_CurrentFrequency();
    frequency -= 10;
    if(frequency < RADIO_Fre_MIN)
    {
    	frequency = RADIO_Fre_MAX;
    }
    pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//非静音，非搜索模式,pll高6位
    radio_write_data[1]=pll%256;				//pll低8位
    radio_write_data[2]=0x20;					//低端本振注入
    radio_write_data[3]=0x11;					//欧美频段，Fxtal=32.768KHz，高音切割关闭，立体声噪声消除关闭
    radio_write_data[4]=0x00;					//6.5MHz参考频率PLL不可用，取加重时间常数50us
    TEA5767_WriteBuffer(radio_write_data);
    return frequency;
}

unsigned long Radio_AutoSearch(u8 direction)		//返回自动搜索得到的频率
{
	unsigned long frequency[1] = {0};
	unsigned int pll = 0;
	u8 radio_write_data[5];
	u8 radio_read_data[5];
	frequency[0] = Radio_CurrentFrequency();
	if(direction)							//direction==1.表示向上搜索
	{
		while(frequency[0] < RADIO_Fre_MAX)
		{
			frequency[0] += 100;
			pll = Radio_CaculatePll(frequency[0]);
			radio_write_data[0]=pll/256;
			radio_write_data[1]=pll%256;
			radio_write_data[2]=0xA0;					//向上搜索
			radio_write_data[3]=0x11;
			radio_write_data[4]=0x00;
			radio_write_data[0]|=0x40;					//进入搜索模式
			TEA5767_WriteBuffer(radio_write_data);
			delayms(500);								//等待搜索结果
			TEA5767_ReadBuffer(radio_read_data);
			delayms(10);
			frequency[0] = Radio_CurrentFrequency();
			if((radio_read_data[0]&0x80) != 0x80)		//没有搜到台也没有搜到顶，重新来过
			{
				continue;
			}
			if((radio_read_data[0] & 0x40))				//搜到了顶
			{
				Radio_SetFrequency(RADIO_Fre_MIN);
				return RADIO_Fre_MIN;					//搜到了顶部，返回频段最小频率
			}
			return frequency[0];
		}
		return RADIO_Fre_MIN;							//搜到了顶，返回最小值
	}
	else												//向下自动搜索
	{
		while(frequency[0] > RADIO_Fre_MIN)
		{
			frequency[0] -= 100;
			pll = Radio_CaculatePll(frequency[0]);
			radio_write_data[0]=pll/256;
			radio_write_data[1]=pll%256;
			radio_write_data[2]=0x20;					//向下搜索
			radio_write_data[3]=0x11;
			radio_write_data[4]=0x00;
			radio_write_data[0]|=0x40;					//进入搜索模式
			TEA5767_WriteBuffer(radio_write_data);
			delayms(500);								//等待搜索结果
			TEA5767_ReadBuffer(radio_read_data);
			delayms(10);
			frequency[0] = Radio_CurrentFrequency();
			if((radio_read_data[0]&0x80) != 0x80)		//没有搜到台也没有搜到顶，重新来过
			{
				continue;
			}
			if((radio_read_data[0] & 0x40))				//搜到了底
			{
				Radio_SetFrequency(RADIO_Fre_MAX);
				return RADIO_Fre_MAX;					//搜到了底部，返回频段最大频率
			}
			return frequency[0];
		}
		return RADIO_Fre_MAX;							//搜到了底，返回最大值
	}
}



