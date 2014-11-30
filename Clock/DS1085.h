#ifndef _DS1085_H_
#define _DS1085_H_

#include "MyF5529.h"

/*
 * 器件地址(7位)
 */
#define DS1085_DeviceAddr	0x58
/*
 * 寄存器定义
 */
#define DS1085_DIV		0x01
#define DS1085_MUX		0x02
#define DS1085_DAC		0x08
#define DS1085_ADDR		0x0D
#define DS1085_OFFSET	0x0E
#define DS1085_RANGE	0x37
#define DS1085_E2		0x3F

/*
 * Offset结构体定义
 */
typedef struct DS1058_OFFSET
{
	signed char offset;
	float range_low;
	float range_high;
}offset_t;


/*
 * 定义预分频系数M
 */
#define	DS1085_PreDiv_1		0
#define	DS1085_PreDiv_2		1
#define	DS1085_PreDiv_4		2
#define	DS1085_PreDiv_8		3

/*
 * 函数预定义
 */
extern void DS1085_Init(void);
extern void DS1085_WriteMUX(unsigned int mux);
extern void DS1085_WriteDIV(unsigned int div);
extern void DS1085_WriteAddr(unsigned char bus);
extern void DS1085_PreDiv(unsigned char div0,unsigned char div1);
extern void DS1085_SetMainClk(float mhz);

#endif /* DS1085_H_ */
