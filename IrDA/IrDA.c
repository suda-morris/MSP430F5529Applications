#include "IrDA.h"
/*
 * 2014-7-15:波特率设定在3000，载波频率38KHz，没有编码，稳定传输
 */
void IrDA_Init(void)
{
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(3000);				//波特率3000
	USCIA1_UART_Config();
	USCIA1_ExitConfig();
	USCIA1_Enable_RXInterrupt(YES);		//打开接收中断
}

