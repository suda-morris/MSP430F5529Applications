#ifndef TEA5767_H_
#define TEA5767_H_

#include "MyF5529.h"

/*
 * 当天线上面的信号电平ADC输出大于停止电平的时候，自动搜索便会停止，当前频率被锁定，这是自动搜台的原理
 */

#define TEA5767_DeviceAddr				0x60						//1100000

#define RADIO_Fre_MAX	 				108000
#define RADIO_Fre_MIN 					87500

#define Radio_Search_High				1
#define Radio_Search_Low				0


extern void Radio_Init();
extern void TEA5767_WriteBuffer(u8* buffer);
extern void TEA5767_ReadBuffer(u8* buffer);
extern unsigned long Radio_CurrentFrequency();
extern unsigned int Radio_CaculatePll(unsigned long frequency);
extern void Radio_SetFrequency(unsigned long frequency);
extern unsigned long Radio_ManualSearchHigh();
extern unsigned long Radio_ManualSearchLow();
extern unsigned long Radio_AutoSearch(u8 direction);


#endif /* TEA5767_H_ */
