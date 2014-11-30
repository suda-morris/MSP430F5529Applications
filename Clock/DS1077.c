#include <Clock/DS1077.h>
/*
 *DS1077оƬ�ڲ�����Ϊ40MHz
 */


void DS1077_Init(void)
{
	USCIB0_EnterIntoConfig();					//USCIB0��������ģʽ
	USCIB0_SetBaud(400000);						//���ó�400kHz�Ĳ�����,���������ڿ���ģʽ��
	USCIB0_IIC_Config(DS1077_DeviceAddr);		//����IIC�����ô�������ַ
	USCIB0_ExitConfig();						//USCIB0�˳�����ģʽ
	DS1077_WriteBUS(0x00);						//����������ַΪ1011000���Զ����Ĵ����ĸ���д��EEPROM
	DS1077_WriteMUX(0x1f80);
}

void DS1077_WriteReg(u8 reg, u8 data)
{
	USCIB0_IIC_SendByte(reg,data);
}

void DS1077_WriteBuffer(u8 reg,u8 length,u8* buffer)
{
	USCIB0_IIC_SendBuffer(reg,length,buffer);
}

void DS1077_WriteMUX(unsigned int mux)
{
	unsigned char byte[2];
	byte[0] = (unsigned char)(mux>>8);
	byte[1] = (unsigned char)(mux);
	DS1077_WriteBuffer(DS1077_MUX,2,byte);
}

void DS1077_WriteDIV(unsigned int div)
{
	unsigned char byte[2];
	if(div<2 || div>1025)		//div��ȡֵ��Χ��2~1025֮�䣨������
	{
		return;
	}
	byte[0] = (unsigned char)((div-2)>>2);
	byte[1] = (unsigned char)(((div-2)%4)<<6);
	DS1077_WriteBuffer(DS1077_DIV,2,byte);
}

void DS1077_WriteBUS(unsigned char bus)
{
	DS1077_WriteReg(DS1077_BUS,bus);
}

/*
 * ����Ԥ��Ƶ��div0��out0��Ԥ��Ƶ��div1��out1��Ԥ��Ƶ
 */
void DS1077_PreDiv(unsigned char div0,unsigned char div1)
{
	unsigned char buffer[2] = {0,0};
	USCIB0_IIC_ReceiveBuffer(DS1077_MUX,2,buffer);
	buffer[0] &= ~0x07;
	buffer[1] &= ~0x80;
	buffer[0] |= (div0<<1);
	buffer[0] |= (div1>>1);
	buffer[1] |= ((div1%2)<<7);
	DS1077_WriteBuffer(DS1077_MUX,2,buffer);
}
