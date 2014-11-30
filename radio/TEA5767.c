#include "TEA5767.h"
/*
 * ������911��,91.1MHz
 * �����ս�ͨ�㲥����,101.1MHz
 * ������948��,94.8MHz
 */
unsigned char const Radio_PowerUpData[5]={0xb1,0xa0,0x20,0x11,0x00}; //�ϵ縴λ������Ϊ����
unsigned char const Radio_ConfigData[5]={0x31,0xa0,0x20,0x11,0x00};  //д��TEA5767����������

void Radio_Init()
{
	USCIB0_EnterIntoConfig();					//USCIB0��������ģʽ
	USCIB0_SetBaud(400000);						//���ó�400kHz�Ĳ�����
	USCIB0_IIC_Config(TEA5767_DeviceAddr);		//����IIC�����ô�������ַ
	USCIB0_ExitConfig();						//USCIB0�˳�����ģʽ
	TEA5767_WriteBuffer((u8*)Radio_PowerUpData);//�ϵ��д�������,��������
	TEA5767_WriteBuffer((u8*)Radio_ConfigData);	//��������
	Radio_SetFrequency(91100);					//��ʼ����̨Ƶ��
}

void TEA5767_WriteBuffer(u8* buffer)
{
	u8 i = 0;
	USCIB0_IIC_Start_Trans();
	for(i=0;i<5;i++)
	{
		while(!(UCB0IFG & UCTXIFG));		//�ȴ�TXBUF�ٴα��
		UCB0TXBUF = buffer[i];
	}
	while(!(UCB0IFG & UCTXIFG));			//��ʱ���ݲ���һ���������
	USCIB0_IIC_Stop();
	delayms(1);								//�ȴ��������
}

void TEA5767_ReadBuffer(u8* buffer)
{
	u8 i = 0;
	buffer[0] = UCB0RXBUF;					//��ս��ջ�����
	USCIB0_IIC_Start_Receive();				//����������
	for(i=0;i<5;i++)
	{
		while(!(UCB0IFG & UCRXIFG));		//�ȴ����ݽ������
		if(i==4)
		{
			USCIB0_IIC_NoACK();				//����NACKλ��ֹͣλ
		}
		buffer[i] = UCB0RXBUF;
	}
	USCIB0_IIC_Stop();
}

unsigned long Radio_CurrentFrequency()		//��õ�ǰƵ��
{
	unsigned long frequency = 0;
	unsigned int pll = 0;
	u8 receiveBuffer[5];
	TEA5767_ReadBuffer(receiveBuffer);
	pll = (receiveBuffer[0] & 0x3f)*256 + receiveBuffer[1];
	if((Radio_ConfigData[2]) & 0x10)			//�߶˱���ע��
	{
		frequency=(unsigned long)(((float)(pll))*(float)8.192-225);    //Ƶ�ʵ�λ:KHz
	}
	else									//�Ͷ˱���ע��
	{
		frequency=(unsigned long)(((float)(pll))*(float)8.192+225);    //Ƶ�ʵ�λ:KHz
	}
	return frequency;
}

unsigned int Radio_CaculatePll(unsigned long frequency)		//���������Ƶ��ֵ������Ӧ��PLL��ֵ
{
	unsigned int pll = 0;
	if(Radio_ConfigData[2]&0x10)			//�߶˱���ע��
	{
		pll=(unsigned int)((float)((frequency+225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
	}
	else									//�Ͷ˱���ע��
	{
		pll=(unsigned int)((float)((frequency-225)*4)/(float)32.768);    //Ƶ�ʵ�λ:k
	}
	return pll;
}

void Radio_SetFrequency(unsigned long frequency)
{
	unsigned int pll = 0;
	u8 radio_write_data[5];
	if((frequency<RADIO_Fre_MIN)||(frequency>RADIO_Fre_MAX))		//Ƶ�ʲ���ŷ��Ƶ�η�Χ��
	{
		return;
	}
	pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//�Ǿ�����������ģʽ,pll��6λ
    radio_write_data[1]=pll%256;				//pll��8λ
    radio_write_data[2]=0x20;					//�Ͷ˱���ע��
    radio_write_data[3]=0x11;					//ŷ��Ƶ�Σ�Fxtal=32.768KHz�������и�رգ����������������ر�
    radio_write_data[4]=0x00;					//6.5MHz�ο�Ƶ��PLL�����ã�ȡ����ʱ�䳣��50us
    TEA5767_WriteBuffer(radio_write_data);
}

unsigned long Radio_ManualSearchHigh()			//Ƶ��+10KHz,���ÿ���TEA5767������̨�����λ:SM,SUD
{
	unsigned long frequency = 0;
	unsigned int pll = 0;
	u8 radio_write_data[5];
    frequency = Radio_CurrentFrequency();
    frequency += 10;
    if(frequency > RADIO_Fre_MAX)
    {
    	frequency = RADIO_Fre_MIN;
    }
    pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//�Ǿ�����������ģʽ,pll��6λ
    radio_write_data[1]=pll%256;				//pll��8λ
    radio_write_data[2]=0x20;					//�Ͷ˱���ע��
    radio_write_data[3]=0x11;					//ŷ��Ƶ�Σ�Fxtal=32.768KHz�������и�رգ����������������ر�
    radio_write_data[4]=0x00;					//6.5MHz�ο�Ƶ��PLL�����ã�ȡ����ʱ�䳣��50us
    TEA5767_WriteBuffer(radio_write_data);
    return frequency;
}

unsigned long Radio_ManualSearchLow()			//Ƶ��-10KHz,���ÿ���TEA5767������̨�����λ:SM,SUD
{
	unsigned long frequency = 0;
	unsigned int pll;
	u8 radio_write_data[5];
    frequency = Radio_CurrentFrequency();
    frequency -= 10;
    if(frequency < RADIO_Fre_MIN)
    {
    	frequency = RADIO_Fre_MAX;
    }
    pll = Radio_CaculatePll(frequency);
    radio_write_data[0]=pll/256;				//�Ǿ�����������ģʽ,pll��6λ
    radio_write_data[1]=pll%256;				//pll��8λ
    radio_write_data[2]=0x20;					//�Ͷ˱���ע��
    radio_write_data[3]=0x11;					//ŷ��Ƶ�Σ�Fxtal=32.768KHz�������и�رգ����������������ر�
    radio_write_data[4]=0x00;					//6.5MHz�ο�Ƶ��PLL�����ã�ȡ����ʱ�䳣��50us
    TEA5767_WriteBuffer(radio_write_data);
    return frequency;
}

unsigned long Radio_AutoSearch(u8 direction)		//�����Զ������õ���Ƶ��
{
	unsigned long frequency[1] = {0};
	unsigned int pll = 0;
	u8 radio_write_data[5];
	u8 radio_read_data[5];
	frequency[0] = Radio_CurrentFrequency();
	if(direction)							//direction==1.��ʾ��������
	{
		while(frequency[0] < RADIO_Fre_MAX)
		{
			frequency[0] += 100;
			pll = Radio_CaculatePll(frequency[0]);
			radio_write_data[0]=pll/256;
			radio_write_data[1]=pll%256;
			radio_write_data[2]=0xA0;					//��������
			radio_write_data[3]=0x11;
			radio_write_data[4]=0x00;
			radio_write_data[0]|=0x40;					//��������ģʽ
			TEA5767_WriteBuffer(radio_write_data);
			delayms(500);								//�ȴ��������
			TEA5767_ReadBuffer(radio_read_data);
			delayms(10);
			frequency[0] = Radio_CurrentFrequency();
			if((radio_read_data[0]&0x80) != 0x80)		//û���ѵ�̨Ҳû���ѵ�������������
			{
				continue;
			}
			if((radio_read_data[0] & 0x40))				//�ѵ��˶�
			{
				Radio_SetFrequency(RADIO_Fre_MIN);
				return RADIO_Fre_MIN;					//�ѵ��˶���������Ƶ����СƵ��
			}
			return frequency[0];
		}
		return RADIO_Fre_MIN;							//�ѵ��˶���������Сֵ
	}
	else												//�����Զ�����
	{
		while(frequency[0] > RADIO_Fre_MIN)
		{
			frequency[0] -= 100;
			pll = Radio_CaculatePll(frequency[0]);
			radio_write_data[0]=pll/256;
			radio_write_data[1]=pll%256;
			radio_write_data[2]=0x20;					//��������
			radio_write_data[3]=0x11;
			radio_write_data[4]=0x00;
			radio_write_data[0]|=0x40;					//��������ģʽ
			TEA5767_WriteBuffer(radio_write_data);
			delayms(500);								//�ȴ��������
			TEA5767_ReadBuffer(radio_read_data);
			delayms(10);
			frequency[0] = Radio_CurrentFrequency();
			if((radio_read_data[0]&0x80) != 0x80)		//û���ѵ�̨Ҳû���ѵ�������������
			{
				continue;
			}
			if((radio_read_data[0] & 0x40))				//�ѵ��˵�
			{
				Radio_SetFrequency(RADIO_Fre_MAX);
				return RADIO_Fre_MAX;					//�ѵ��˵ײ�������Ƶ�����Ƶ��
			}
			return frequency[0];
		}
		return RADIO_Fre_MAX;							//�ѵ��˵ף��������ֵ
	}
}



