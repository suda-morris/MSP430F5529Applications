#ifndef _ADS8344_H_
#define _ADS8344_H_

/*
 *16位ADC底层驱动程序
 *引脚连接图：
 *			DCLK 	---> P4.0
 *			CS  	---> P7.0
 *			DIN		---> P4.4
 *			DOUT	---> P4.5
 *差分信号输入方式：
 *			A2A1A0		+IN		-IN
 *			000			CH0		CH1
 *			001			CH2		CH3
 *			010			CH4		CH5
 *			011			CH6		CH7
 *			100			CH1		CH0
 *			101			CH3		CH2
 *			110			CH5		CH4
 *			111			CH7		CH6
 */

#include "MyF5529.h"

#define ADS8344_Dif						//差分信号输入

#define ADS8344_REF		4.98

#define ADS8344_CS_High()		GPIO_SetBit(7,0)
#define ADS8344_CS_Low()		GPIO_ResetBit(7,0)

extern void ADS8344_Init(void);
extern float ADS8344_ADConverter(unsigned char channel);

#endif /* ADS8344_H_ */
