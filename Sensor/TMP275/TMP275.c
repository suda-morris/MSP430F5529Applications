#include "TMP275.h"

void TMP275_WriteReg(u8 reg, u8 data)
{
	USCIB0_IIC_SendByte(reg,data);
}

void TMP275_Init()
{
	USCIB0_EnterIntoConfig();					//USCIB0��������ģʽ
	USCIB0_SetBaud(400000);						//���ó�400kHz�Ĳ�����
	USCIB0_IIC_Config(TMP275_DeviceAddr);		//����IIC�����ô�������ַ
	USCIB0_ExitConfig();						//USCIB0�˳�����ģʽ
	USCIB0_IIC_SendByte(TMP275_ConfigReg,0x60);	//����TMP275�Ĺ�����ʽ:�ض�ģʽ���رգ����ñȽ�ģʽ��
												//ALERT���ŵ͵�ƽ��Ч�����϶��г���Ϊ1��ת�����ֱ���12λ
}

double TMP275_ReadTemp()
{
	double value = 1;
	unsigned char buffer[2];
	signed int temp = 0;
	USCIB0_IIC_ReceiveBuffer(TMP275_TempReg,2,buffer);
	temp += buffer[1];
	temp <<= 8;
	temp += buffer[0];
	temp &= 0xfff0;
	temp >>= 4;
	if(temp & 0x800)				//�Ǹ���
	{
		temp ^= 0xfff;
		temp += 1;
		value = -1;
	}
	value = value*((double)temp)*0.0625;
	return value;
}

