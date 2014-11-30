#ifndef TMP275_H_
#define TMP275_H_

#include "MyF5529.h"

#define TMP275_DeviceAddr	0x48				//����������ַ(7λ��ַ)
#define TMP275_TempReg		0x00				//�¶ȼĴ�����ַ
#define TMP275_ConfigReg	0x01				//���üĴ�����ַ
#define TMP275_TLowReg		0x02				//TLow�Ĵ�����ַ
#define TMP275_THighReg		0x03				//THigh�Ĵ�����ַ

extern void TMP275_WriteReg(u8 reg, u8 data);
extern void TMP275_Init();
extern double TMP275_ReadTemp();


#endif /* TMP275_H_ */
