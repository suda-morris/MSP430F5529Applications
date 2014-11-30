#ifndef TMP275_H_
#define TMP275_H_

#include "MyF5529.h"

#define TMP275_DeviceAddr	0x48				//定义器件地址(7位地址)
#define TMP275_TempReg		0x00				//温度寄存器地址
#define TMP275_ConfigReg	0x01				//配置寄存器地址
#define TMP275_TLowReg		0x02				//TLow寄存器地址
#define TMP275_THighReg		0x03				//THigh寄存器地址

extern void TMP275_WriteReg(u8 reg, u8 data);
extern void TMP275_Init();
extern double TMP275_ReadTemp();


#endif /* TMP275_H_ */
