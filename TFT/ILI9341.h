/*
	²ÊÆÁ¹Ü½ÅÁ¬½ÓÍ¼£º
		TFT_CS  		--> P7.0
		TFT_RESET		-->	P2.0
		TFT_D/C			-->	P3.2
		TFT_MOSI		-->	P4.4
		TFT_MISO		-->	P4.5
		TFT_CLK			--> P4.0
*/
#ifndef __ILI9341_H_
#define __ILI9341_H_

#include "MyF5529.h"

#define TFT_CS_High()		GPIO_SetBit(7,0)
#define TFT_CS_Low()		GPIO_ResetBit(7,0)
#define TFT_Reset_High()	GPIO_SetBit(2,0)
#define TFT_Reset_Low()		GPIO_ResetBit(2,0)
#define TFT_Select_Data()	GPIO_SetBit(3,2)
#define TFT_Select_CMD()	GPIO_ResetBit(3,2)

extern void TFT_WriteBus(unsigned char dat);
extern void TFT_Write_CMD(unsigned char cmd);
extern void TFT_Write_Data(unsigned char data);
extern void TFT_SetWindow(unsigned char x1,unsigned int y1,unsigned char x2,unsigned int y2);
extern void TFT_SetXY(unsigned char x,unsigned int y);
extern void TFT_Init(void);

#endif
