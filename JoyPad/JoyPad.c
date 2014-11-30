#include "JoyPad.h"

void JoyPad_Init(void)
{
	GPIO_Set_Direction_In(6,4,GPIO_IN_UP);
	GPIO_Set_Direction_Out(6,6,HIGH);
	GPIO_Set_Direction_Out(6,7,HIGH);
}

unsigned char JoyPad_Read()
{
	u8 temp=0;
	u8 t = 0;
	JoyPad_Latch_High();					//���浱ǰ״̬
	JoyPad_Latch_Low();
	for(t=0;t<8;t++)
	{
		temp<<=1;
		if(JoyPad_Data_High()) temp|=0x01;	//Latch=0֮�󣬾͵õ���һ������
		JoyPad_Clock_High();				//ÿ��һ�����壬�յ�һ������
 		JoyPad_Clock_Low();
	}
	return temp;
}
