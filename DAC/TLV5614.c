#include "DAC/TLV5614.h"
/*
 * ��������ͼ��
 * 			P4.0---->SCLK
 * 			P4.4---->DIN
 * 			P2.1--->FS
 * 	ʱ�����ԣ�
 * 			������ʱ�������ذ����ݷ��͵������ߣ��ӻ���TLV5614����ʱ���½��ش��������϶�ȡ���ݡ�ʱ���źſ���ʱ��λ�ߵ�ƽ
 */

//address consists two bits:
//A1,A0(decides the ch);control consists two bits:PWR(1=power off,0=normal),SPD(1=Fast,0=Slow)
void TLV5614_DAConverter(unsigned char address,float voltage)
{
	unsigned int temp[2] = {0,0};
	temp[0] = (unsigned int)(voltage*2048/(float)TLV5614_REF);
	temp[1]=(((unsigned int)address)<<14)|(((unsigned int)1)<<12)|temp[0];
	GPIO_SetBit(2,1);
	delayus(1);
	GPIO_ResetBit(2,1);				//FS�½���
	USCIA1_SPI_SendByte((unsigned char)(temp[1]>>8));
	USCIA1_SPI_SendByte((unsigned char)temp[1]);
	GPIO_SetBit(2,1);				//FS����
}

void TLV5614_Init()
{
	USCIA1_EnterIntoConfig();
	USCIA1_SetBaud(20000000);				//����Ϊ20MHz�Ĳ�����
	USCIA1_SPI_Config();
	USCIA1_ExitConfig();
	GPIO_Set_Direction_Out(2,1,HIGH);		//FS����ģʽ����Ϊ���
}
