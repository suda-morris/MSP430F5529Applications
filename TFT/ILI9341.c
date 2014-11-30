/*
 *彩屏驱动芯片ILI9341的底层驱动程序
 *Author：茅胜荣
 */
#include "ILI9341.h"

void TFT_WriteBus(unsigned char dat)
{
	USCIA1_SPI_SendByte(dat);
}

void TFT_Write_Data(unsigned char data)
{
	TFT_CS_Low();
	TFT_Select_Data();
	TFT_WriteBus(data);
	TFT_CS_High();
}

void TFT_Write_CMD(unsigned char cmd)
{
	TFT_CS_Low();
	TFT_Select_CMD();
	TFT_WriteBus(cmd);
	TFT_CS_High();
}

void TFT_SetWindow(unsigned char x1,unsigned int y1,unsigned char x2,unsigned int y2)
{
	TFT_Write_CMD(0x2A); //Set Column Address
	TFT_Write_Data(x1>>8);
	TFT_Write_Data(x1);
	TFT_Write_Data(x2>>8);
	TFT_Write_Data(x2);
	TFT_Write_CMD(0x2B); //Set Page Address
	TFT_Write_Data(y1>>8);
	TFT_Write_Data(y1);
	TFT_Write_Data(y2>>8);
	TFT_Write_Data(y2);
   	TFT_Write_CMD(0x2C);
}

void TFT_SetXY(unsigned char x,unsigned int y)
{
	TFT_SetWindow(x,y,x,y);	

}

void TFT_Init(void)
{
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(15000000);			//波特率10MHz
	USCIA1_SPI_Config();
	USCIA1_ExitConfig();
	GPIO_Set_Direction_Out(7,0,HIGH);
	GPIO_Set_Direction_Out(2,0,HIGH);
	GPIO_Set_Direction_Out(3,2,HIGH);

	TFT_Reset_Low();
	delayms(20);
	TFT_Reset_High();
	delayms(20);

	TFT_Write_CMD(0xCB);
    TFT_Write_Data(0x39);
    TFT_Write_Data(0x2C);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x34);
    TFT_Write_Data(0x02);

    TFT_Write_CMD(0xCF);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0XC1);
    TFT_Write_Data(0X30);

    TFT_Write_CMD(0xE8);
    TFT_Write_Data(0x85);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x78);

    TFT_Write_CMD(0xEA);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x00);

    TFT_Write_CMD(0xED);
    TFT_Write_Data(0x64);
    TFT_Write_Data(0x03);
    TFT_Write_Data(0X12);
    TFT_Write_Data(0X81);

    TFT_Write_CMD(0xF7);
    TFT_Write_Data(0x20);

    TFT_Write_CMD(0xC0);    //Power control
    TFT_Write_Data(0x23);   //VRH[5:0]

    TFT_Write_CMD(0xC1);    //Power control
    TFT_Write_Data(0x10);   //SAP[2:0];BT[3:0]

    TFT_Write_CMD(0xC5);    //VCM control
    TFT_Write_Data(0x3e); //对比度调节
    TFT_Write_Data(0x28);

    TFT_Write_CMD(0xC7);    //VCM control2
    TFT_Write_Data(0x86);  //--

    TFT_Write_CMD(0x36);    // Memory Access Control
    TFT_Write_Data(0x48); //C8	   //48 68竖屏//28 E8 横屏

    TFT_Write_CMD(0x3A);
    TFT_Write_Data(0x55);

    TFT_Write_CMD(0xB1);
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x18);

    TFT_Write_CMD(0xB6);    // Display Function Control
    TFT_Write_Data(0x08);
    TFT_Write_Data(0x82);
    TFT_Write_Data(0x27);

    TFT_Write_CMD(0xF2);    // 3Gamma Function Disable
    TFT_Write_Data(0x00);

    TFT_Write_CMD(0x26);    //Gamma curve selected
    TFT_Write_Data(0x01);

    TFT_Write_CMD(0xE0);    //Set Gamma
    TFT_Write_Data(0x0F);
    TFT_Write_Data(0x31);
    TFT_Write_Data(0x2B);
    TFT_Write_Data(0x0C);
    TFT_Write_Data(0x0E);
    TFT_Write_Data(0x08);
    TFT_Write_Data(0x4E);
    TFT_Write_Data(0xF1);
    TFT_Write_Data(0x37);
    TFT_Write_Data(0x07);
    TFT_Write_Data(0x10);
    TFT_Write_Data(0x03);
    TFT_Write_Data(0x0E);
    TFT_Write_Data(0x09);
    TFT_Write_Data(0x00);

    TFT_Write_CMD(0XE1);    //Set Gamma
    TFT_Write_Data(0x00);
    TFT_Write_Data(0x0E);
    TFT_Write_Data(0x14);
    TFT_Write_Data(0x03);
    TFT_Write_Data(0x11);
    TFT_Write_Data(0x07);
    TFT_Write_Data(0x31);
    TFT_Write_Data(0xC1);
    TFT_Write_Data(0x48);
    TFT_Write_Data(0x08);
    TFT_Write_Data(0x0F);
    TFT_Write_Data(0x0C);
    TFT_Write_Data(0x31);
    TFT_Write_Data(0x36);
    TFT_Write_Data(0x0F);

    TFT_Write_CMD(0x11);    //Exit Sleep
    delayms(120);

    TFT_Write_CMD(0x29);    //Display on
    TFT_Write_CMD(0x2c);
}






