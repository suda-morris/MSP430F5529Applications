#include "IrDA.h"
/*
 * 2014-7-15:�������趨��3000���ز�Ƶ��38KHz��û�б��룬�ȶ�����
 */
void IrDA_Init(void)
{
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(3000);				//������3000
	USCIA1_UART_Config();
	USCIA1_ExitConfig();
	USCIA1_Enable_RXInterrupt(YES);		//�򿪽����ж�
}

