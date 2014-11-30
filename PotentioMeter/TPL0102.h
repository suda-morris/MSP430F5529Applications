#ifndef _TPL0102_H_
#define _TPL0102_H_

/*数字电位器TPL0102
 * 引脚连接
 * 		P3.0---->SDA
 * 		P3.1---->SCL
 */
#include "MyF5529.h"

#define TPL0102_DeviceAddr		0x50		//TPL0102的7位器件地址
#define TPL0102_PointerA		0x00		//通道A的滑头
#define TPL0102_PointerB		0x01		//通道B的滑头

extern void TPL0102_Init(void);
extern void TPL0102_WritePointerA(unsigned char value);
extern unsigned char TPL0102_ReadPointerA(void);
extern void TPL0102_WritePointerB(unsigned char value);
extern unsigned char TPL0102_ReadPointerB(void);
#endif /* TPL0102_H_ */
