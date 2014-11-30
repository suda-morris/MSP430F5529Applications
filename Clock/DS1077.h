#ifndef _DS1077_H_
#define _DS1077_H_
#include "MyF5529.h"

/*
 * ������ַ(7λ)
 */
#define DS1077_DeviceAddr	0x58
/*
 * �Ĵ�������
 */
#define DS1077_DIV	0x01
#define DS1077_MUX	0x02
#define DS1077_BUS	0x0D
#define DS1077_E2	0x3F

/*
 * ����Ԥ��Ƶϵ��M
 */
#define	DS1077_PreDiv_1		0
#define	DS1077_PreDiv_2		1
#define	DS1077_PreDiv_4		2
#define	DS1077_PreDiv_8		3

/*
 * ����Ԥ����
 */
extern void DS1077_Init(void);
extern void DS1077_WriteMUX(unsigned int mux);
extern void DS1077_WriteDIV(unsigned int div);
extern void DS1077_WriteBUS(unsigned char bus);
extern void DS1077_PreDiv(unsigned char div0,unsigned char div1);

#endif /* DS1077_H_ */
