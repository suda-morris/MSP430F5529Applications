#ifndef JOYPAD_H_
#define JOYPAD_H_

#include "MyF5529.h"
/*
 * P6.4 ----> Data
 * P6.6 ----> Latch
 * P6.7 ----> Clock
 */
#define JoyPad_Latch_Low()		GPIO_ResetBit(6,6)
#define JoyPad_Latch_High()		GPIO_SetBit(6,6)
#define JoyPad_Clock_Low()		GPIO_ResetBit(6,7)
#define JoyPad_Clock_High()		GPIO_SetBit(6,7)
#define JoyPad_Data_High()		(GPIO_Read(6,4) == 0x01)
#define JoyPad_Up				0xF7
#define JoyPad_Down				0xFB
#define JoyPad_Right			0xFE
#define JoyPad_Left				0xFD
#define JoyPad_Start			0xDF
#define JoyPad_Mode				0xEF
#define JoyPad_A				0x7F
#define JoyPad_B				0xBF
#define JoyPad_C				0x3F

extern void JoyPad_Init(void);
extern unsigned char JoyPad_Read();

#endif /* JOYPAD_H_ */
