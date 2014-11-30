#include "TPL0102.h"

void TPL0102_Init(void)
{
	USCIB0_EnterIntoConfig();
	USCIB0_SetBaud(400000);			//²¨ÌØÂÊ400KHz
	USCIB0_IIC_Config(TPL0102_DeviceAddr);
	USCIB0_ExitConfig();
}

void TPL0102_WritePointerA(unsigned char value)
{
	USCIB0_IIC_SendByte(TPL0102_PointerA,value);
}

unsigned char TPL0102_ReadPointerA(void)
{
	return USCIB0_IIC_ReceiveByte(TPL0102_PointerA);
}

void TPL0102_WritePointerB(unsigned char value)
{
	USCIB0_IIC_SendByte(TPL0102_PointerB,value);
}

unsigned char TPL0102_ReadPointerB(void)
{
	return USCIB0_IIC_ReceiveByte(TPL0102_PointerB);
}
