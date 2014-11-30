/*
 * �������ӣ�
 * 		SD_MOSI------->P4.1
 * 		SD_MISO------->P4.2
 * 		SD_SCLK------->P4.3
 * 		SD_CS--------->P3.7
 */

#include "SDCard.h"

unsigned char  SD_Type;		//SD��������

unsigned char SD_SPI_ReadWriteByte(unsigned char data)
{
	return USCIB1_SPI_SendByte(data);
}

//�ȴ���׼����
//����ֵ:0,׼������;����,�������
unsigned char SD_WaitReady(void)
{
	unsigned long t=0;
	do
	{
		if(SD_SPI_ReadWriteByte(0XFF)==0XFF)return 0;//OK
		t++;
	}while(t<0XFFFFFF);//�ȴ�
	return 1;
}

//ȡ��ѡ��,�ͷ�SPI����
void SD_DisSelect(void)
{
	SDCard_CS_High();
 	SD_SPI_ReadWriteByte(0xff);	//�ṩ�����8��ʱ��
}

//ѡ��sd��,���ҵȴ���׼��OK
//����ֵ:0,�ɹ�;1,ʧ��;
unsigned char SD_Select(void)
{
	SDCard_CS_Low();
	if(SD_WaitReady()==0)
	{
		return 0;				//�ȴ��ɹ�
	}
	SD_DisSelect();
	return 1;					//�ȴ�ʧ��
}

//�ȴ�SD����Ӧ
//Response:Ҫ�õ��Ļ�Ӧֵ
//����ֵ:0,�ɹ��õ��˸û�Ӧֵ
//    ����,�õ���Ӧֵʧ��
unsigned char SD_GetResponse(u8 Response)
{
	unsigned int Count=0xFFFF;				//�ȴ�����
	while ((SD_SPI_ReadWriteByte(0XFF)!=Response)&&Count)
	{
		Count--;							//�ȴ��õ�׼ȷ�Ļ�Ӧ
	}
	if (Count==0)
	{
		return MSD_RESPONSE_FAILURE;		//�õ���Ӧʧ��
	}
	else
	{
		return MSD_RESPONSE_NO_ERROR;		//��ȷ��Ӧ
	}
}


//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
//����ֵ:0,�ɹ�;����,ʧ��;
unsigned char SD_RecvData(u8*buf,u16 len)
{
	if(SD_GetResponse(0xFE))				//�ȴ�SD������������ʼ����0xFE
	{
		return 1;							//û���յ���ʼ����
	}
    while(len--)							//��ʼ��������
    {
        *buf=SD_SPI_ReadWriteByte(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    SD_SPI_ReadWriteByte(0xFF);
    SD_SPI_ReadWriteByte(0xFF);
    return 0;//��ȡ�ɹ�
}


//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
//����ֵ:0,�ɹ�;����,ʧ��;
unsigned char SD_SendBlock(u8*buf,u8 cmd)
{
	unsigned int t;
	if(SD_WaitReady())
	{
		return 1;//�ȴ�׼��ʧЧ
	}
	SD_SPI_ReadWriteByte(cmd);
	if(cmd!=0XFD)//���ǽ���ָ��
	{
		for(t=0;t<512;t++)
		{
			SD_SPI_ReadWriteByte(buf[t]);
		}
	    SD_SPI_ReadWriteByte(0xFF);//����crc
	    SD_SPI_ReadWriteByte(0xFF);
		t=SD_SPI_ReadWriteByte(0xFF);//������Ӧ
		if((t&0x1F)!=0x05)
		{
			return 2;//��Ӧ����
		}
	}
    return 0;//д��ɹ�
}

//��SD������һ������
//����: u8 cmd   ����
//      u32 arg  �������
//      u8 crc   crcУ��ֵ
//����ֵ:SD�����ص���Ӧ
unsigned char SD_SendCmd(u8 cmd, u32 arg, u8 crc)
{
    unsigned char r1;
	unsigned char Retry=0;
	SD_DisSelect();//ȡ���ϴ�Ƭѡ
	if(SD_Select())
	{
		return 0XFF;//ƬѡʧЧ
	}
	//����
    SD_SPI_ReadWriteByte(cmd | 0x40);//�ֱ�д������
    SD_SPI_ReadWriteByte(arg >> 24);
    SD_SPI_ReadWriteByte(arg >> 16);
    SD_SPI_ReadWriteByte(arg >> 8);
    SD_SPI_ReadWriteByte(arg);
    SD_SPI_ReadWriteByte(crc);
	if(cmd==CMD12)
	{
		SD_SPI_ReadWriteByte(0xff);//Skip a stuff byte when stop reading
	}
    //�ȴ���Ӧ����ʱ�˳�
	Retry=0X1F;
	do
	{
		r1=SD_SPI_ReadWriteByte(0xFF);
	}while((r1&0X80) && Retry--);
	//����״ֵ̬
    return r1;
}


//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//		 1������
unsigned char SD_GetCID(unsigned char *cid_data)
{
    unsigned char r1;
    //��CMD10�����CID
    r1=SD_SendCmd(CMD10,0,0x01);
    if(r1==0x00)
	{
		r1=SD_RecvData(cid_data,16);//����16���ֽڵ�����
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *csd_data(���CSD���ڴ棬����16Byte��
//����ֵ:0��NO_ERR
//		 1������
unsigned char SD_GetCSD(u8 *csd_data)
{
    unsigned char r1;
    r1=SD_SendCmd(CMD9,0,0x01);//��CMD9�����CSD
    if(r1==0)
	{
    	r1=SD_RecvData(csd_data, 16);//����16���ֽڵ�����
    }
	SD_DisSelect();//ȡ��Ƭѡ
	if(r1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


//��ȡSD����������������������
//����ֵ:0�� ȡ��������
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.
unsigned long SD_GetSectorCount(void)
{
    unsigned char csd[16];
    unsigned long Capacity;
    unsigned char n;
	unsigned int csize;
	//ȡCSD��Ϣ������ڼ����������0
    if(SD_GetCSD(csd)!=0)
    {
    	return 0;
    }
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {
		csize = csd[9] + ((u16)csd[8] << 8) + 1;
		Capacity = (u32)csize << 10;//�õ�������
    }else//V1.XX�Ŀ�
    {
		n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
		csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
		Capacity= (u32)csize << (n - 9);//�õ�������
    }
    return Capacity;
}


//��ʼ��SD��
unsigned char SD_Initialize(void)
{
    unsigned char r1;      // ���SD���ķ���ֵ
    unsigned int retry;  // �������г�ʱ����
    unsigned char buf[4];
	unsigned int i;


	GPIO_Set_Direction_Out(3,7,HIGH);
	USCIB1_EnterIntoConfig();
	USCIB1_SetBaud(300000);						//��ʼ��ʱ�ӱ���С��400KHz
	USCIB1_SPI_Config()	;
	USCIB1_ExitConfig();

 	for(i=0;i<10;i++)
 	{
 		SD_SPI_ReadWriteByte(0XFF);//��������74������
 	}
	retry=20;
	do
	{
		r1=SD_SendCmd(CMD0,0,0x95);//����IDLE״̬
	}while((r1!=0X01) && retry--);
 	SD_Type=0;//Ĭ���޿�
	if(r1==0X01)
	{
		if(SD_SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
		{
			for(i=0;i<4;i++)
			{
				buf[i]=SD_SPI_ReadWriteByte(0XFF);	//Get trailing return value of R7 resp
			}
			if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
			{
				retry=0XFFFE;
				do
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0x40000000,0X01);//����CMD41
				}while(r1&&retry--);
				if(retry&&SD_SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
				{
					for(i=0;i<4;i++)
					{
						buf[i]=SD_SPI_ReadWriteByte(0XFF);//�õ�OCRֵ
					}
					if(buf[0]&0x40)
					{
						SD_Type=SD_TYPE_V2HC;    //���CCS
					}
					else
					{
						SD_Type=SD_TYPE_V2;
					}
				}
			}
		}else//SD V1.x/ MMC	V3
		{
			SD_SendCmd(CMD55,0,0X01);		//����CMD55
			r1=SD_SendCmd(CMD41,0,0X01);	//����CMD41
			if(r1<=1)
			{
				SD_Type=SD_TYPE_V1;
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					SD_SendCmd(CMD55,0,0X01);	//����CMD55
					r1=SD_SendCmd(CMD41,0,0X01);//����CMD41
				}while(r1&&retry--);
			}else
			{
				SD_Type=SD_TYPE_MMC;//MMC V3
				retry=0XFFFE;
				do //�ȴ��˳�IDLEģʽ
				{
					r1=SD_SendCmd(CMD1,0,0X01);//����CMD1
				}while(r1&&retry--);
			}
			if(retry==0||SD_SendCmd(CMD16,512,0X01)!=0)
			{
				SD_Type=SD_TYPE_ERR;//����Ŀ�
			}
		}
	}
	SD_DisSelect();//ȡ��Ƭѡ
	USCIB1_EnterIntoConfig();
	USCIB1_SetBaud(10000000);				//�ٶ����Ϊ10MHz
	USCIB1_ExitConfig();
	if(SD_Type)
	{
		return 0;
	}
	else if(r1)
	{
		return r1;
	}
	return 0xaa;//��������
}


//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
unsigned char SD_ReadDisk(unsigned char*buf,unsigned long sector,unsigned char cnt)
{
	unsigned char r1;
	if(SD_Type!=SD_TYPE_V2HC)
	{
		sector <<= 9;//ת��Ϊ�ֽڵ�ַ
	}
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD17,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�
		}
	}else
	{
		r1=SD_SendCmd(CMD18,sector,0X01);//����������
		do
		{
			r1=SD_RecvData(buf,512);//����512���ֽ�
			buf+=512;
		}while(--cnt && r1==0);
		SD_SendCmd(CMD12,0,0X01);	//����ֹͣ����
	}
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;
}


//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
unsigned char SD_WriteDisk(unsigned char*buf,unsigned long sector,unsigned char cnt)
{
	unsigned char r1;
	if(SD_Type!=SD_TYPE_V2HC)
	{
		sector *= 512;//ת��Ϊ�ֽڵ�ַ
	}
	if(cnt==1)
	{
		r1=SD_SendCmd(CMD24,sector,0X01);//������
		if(r1==0)//ָ��ͳɹ�
		{
			r1=SD_SendBlock(buf,0xFE);//д512���ֽ�
		}
	}else
	{
		if(SD_Type!=SD_TYPE_MMC)
		{
			SD_SendCmd(CMD55,0,0X01);
			SD_SendCmd(CMD23,cnt,0X01);//����ָ��
		}
 		r1=SD_SendCmd(CMD25,sector,0X01);//����������
		if(r1==0)
		{
			do
			{
				r1=SD_SendBlock(buf,0xFC);//����512���ֽ�
				buf+=512;
			}while(--cnt && r1==0);
			r1=SD_SendBlock(0,0xFD);//����512���ֽ�
		}
	}
	SD_DisSelect();//ȡ��Ƭѡ
	return r1;//
}
