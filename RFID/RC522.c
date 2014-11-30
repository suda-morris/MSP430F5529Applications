#include "RC522.h"
#define MAXRLEN 18
//M1����ĳһ��дΪ���¸�ʽ����ÿ�ΪǮ�����ɽ��տۿ�ͳ�ֵ����
//4�ֽڽ����ֽ���ǰ����4�ֽڽ��ȡ����4�ֽڽ�1�ֽڿ��ַ��1�ֽڿ��ַȡ����1�ֽڿ��ַ��1�ֽڿ��ַȡ��
unsigned char const Money[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,
								0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
unsigned char CardType[2] = {0,0};//������
unsigned char CardID[4] = {0,0,0,0}; //����ID��
unsigned char const DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
/////////////////////////////////////////////////////////////////////
//��    �ܣ�Ѱ��
//����˵��: req_code[IN]:Ѱ����ʽ
//                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�--->PICC_REQALL
//                0x26 = Ѱδ��������״̬�Ŀ�--->PICC_REQIDL
//          pTagType[OUT]����Ƭ���ʹ���
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;
   unsigned int  unLen;					//���ڱ��淵�����ݵ�λ����
   unsigned char ucComMF522Buf[MAXRLEN];
   ClearBitMask(Status2Reg,0x08);		//ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ��������Ҫ�������
   WriteRawRC(BitFramingReg,0x07);		//��������λ��֡�ķ��ͣ�0x07��ʾ���һ���ֽڵ�����λ����Ҫ����
   SetBitMask(TxControlReg,0x03);		//TX1,TX2�ܽŵ�����źŽ����ݾ��������ݵ�����13.56MHZ�������ز��ź�

   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   if ((status == MI_OK) && (unLen == 0x10))		//���ؿ�Ƭ����,2���ֽ�(16λ)
   {
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }							//RC522�뿨Ƭͨ��ʧ�ܣ�����error

   return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����ײ
//����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);			//ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ��������Ҫ�������
    WriteRawRC(BitFramingReg,0x00);			//��������λ��֡�ķ��ͣ�0x00��ʾ���һ���ֽڵ�����λ��Ҫ����
    ClearBitMask(CollReg,0x80);				//���н��յ�λ�ڳ�ײ�����

    ucComMF522Buf[0] = PICC_ANTICOLL1;		//����ײָ��
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)					//ͨ�ųɹ������ؿ�ƬID,4���ֽ�
    {
    	 for (i=0; i<4; i++)
         {
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])	//��5���ֽ�������У��
         {   status = MI_ERR;    }
    }

    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ѡ����Ƭ
//����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;			//����ײ
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;						//��7���ֽ����ڴ��У��
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	//���ص�CRCУ��������ucComMF522Buf[7]��
    												//ucComMF522Buf[8]��

    ClearBitMask(Status2Reg,0x08);			//��ִ��PCD_TRANSCEIVEָ��֮ǰ����Ҫ����仰

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))	//����3���ֽڵ�����
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���֤��Ƭ����
//����˵��: auth_mode[IN]: ������֤ģʽ
//                 0x60 = ��֤A��Կ-->PICC_AUTHENT1A
//                 0x61 = ��֤B��Կ-->PICC_AUTHENT1B
//          addr[IN]�����ַ
//          pKey[IN]������
//          pSnr[IN]����Ƭ���кţ�4�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;		//��֤ģʽ
    ucComMF522Buf[1] = addr;			//���ַ
    for (i=0; i<6; i++)					//6���ֽڵ����룬ucComMF522Buf[2]~ucComMF522Buf[7]
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<4; i++)					//6���ֽڵĿ���,ucComMF522Buf[8]~ucComMF522Buf[11]
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))		//���û�гɹ�ִ��Authentָ��
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���ȡM1��һ������
//����˵��: addr[IN]�����ַ
//          pData[OUT]�����������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;		//����
    ucComMF522Buf[1] = addr;			//���ַ
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);		//��ǰ�����ֽ�����1���ֽڵ�CRCУ��������
    													//ucComMF522Buf[2]��ucComMF522Buf[3]��

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))		//����18���ֽڣ�ǰ16���ֽ�Ϊ���������ݣ���������ֽ���У����
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�д���ݵ�M1��һ��
//����˵��: addr[IN]�����ַ
//          pData[IN]��д������ݣ�16�ֽ�
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;			//����
    ucComMF522Buf[1] = addr;				//���ַ
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);//�ȴ�����͵�ַ

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)				//��ʾ���Դ���������
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);//16�ֽڵĴ�д�����ݼ��Ϻ��������ֽڵ�У���룬�ܹ�18���ֽ�

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//��    �ܣ����Ƭ��������״̬
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdHalt(void)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}

/////////////////////////////////////////////////////////////////////
//��MF522����CRC16����
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);			//��CRC������Ч�����ݱ�����ʱ����λ
    WriteRawRC(CommandReg,PCD_IDLE);		//ʹRC522ת�����״̬
    SetBitMask(FIFOLevelReg,0x80);			//�ڲ�FIFO�Ļ������Ķ���дָ���Լ��Ĵ���ErrReg��BufferOvfi��־���̱����
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);		// CRC����
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);			//�����ж������־
        i--;
    }
    while ((i!=0) && !(n&0x04));			//��CRC������Ϻ�
    pOutData[0] = ReadRawRC(CRCResultRegL);//����CRC����ʵ�ʵ�LSBֵ
    pOutData[1] = ReadRawRC(CRCResultRegM);//����CRC����ʵ�ʵ�MSBֵ
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���λRC522
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdReset(void)
{
    Set_RCC522_RST();
    delayus(1);
    Reset_RC522_RST();
    delayus(1);
    Set_RCC522_RST();
    delayus(1);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delayus(1);

    WriteRawRC(ModeReg,0x3D);            	//��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);				//�ڲ���ʱ��Ԥװֵ30
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);				//����Prescaler��ֵ
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);				//����Ҫ
    return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//��    �أ�������ֵ
/////////////////////////////////////////////////////////////////////
unsigned char ReadRawRC(unsigned char Address)
{
     unsigned char ucAddr[1];
     unsigned char ucResult;

     Reset_RC522_NSS();
     ucAddr[0] = ((Address<<1)&0x7E)|0x80;

     USCIA1_SPI_SendByte(ucAddr[0]);
     ucResult = USCIA1_SPI_SendByte(0x00);

     Set_RC522_NSS();
     return ucResult;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�дRC632�Ĵ���
//����˵����Address[IN]:�Ĵ�����ַ
//          value[IN]:д���ֵ
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
    unsigned char ucAddr[1];

    Reset_RC522_NSS();				//ʹ��Ƭѡ
    ucAddr[0] = ((Address<<1)&0x7E);

    USCIA1_SPI_SendByte(ucAddr[0]);
    USCIA1_SPI_SendByte(value);

    Set_RC522_NSS();
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pInData[IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOutData[OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
/////////////////////////////////////////////////////////////////////
char PcdComMF522(unsigned char Command,
                 unsigned char *pInData,
                 unsigned char InLenByte,
                 unsigned char *pOutData,
                 unsigned int  *pOutLenBit)
{
    char status = MI_ERR;
    unsigned char irqEn   = 0x00;
    unsigned char waitFor = 0x00;
    unsigned char lastBits;
    unsigned char n;
    unsigned int i;
    switch (Command)
    {
       case PCD_AUTHENT:				//��֤��Կ
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:				//���Ͳ���������
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);/*�����֤��Կ,�ܽ�IRQ�ϵ��ź���IRqλ�෴,��������ж�����,��������ж�����
                                                                                ������Ͳ���������,�ܽ�IRQ�ϵ��ź���IRqλ�෴,���������ж�����,����������ж�����,
                                                                                ��������ж�����,����;����ж�����,��������ж�����,����ʱ���ж�����*/
    ClearBitMask(ComIrqReg,0x80);			//�������ж�λ
    WriteRawRC(CommandReg,PCD_IDLE);		//��RC522�������״̬
    SetBitMask(FIFOLevelReg,0x80);			//�ڲ�FIFO�Ļ������Ķ���дָ���Լ��Ĵ���ErrReg��BufferOvfi��־���̱������FIFO����

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }//�����ݷ���д��FIFO������
    WriteRawRC(CommandReg, Command);			//ִ��RC522������


    if (Command == PCD_TRANSCEIVE)				//���������Ϊ���Ͳ���������
    {    SetBitMask(BitFramingReg,0x80);  }		//�������ݴ���(��FIFO�е����ݴ��뿨��)

    i = 600;								//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
    do
    {
         n = ReadRawRC(ComIrqReg);		 	//��ȡ�ж������־�Ĵ�����ֵ
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));/*���ȴ�ʱ�䵽ʱ,��ʱ����TimerValue�Ĵ�����ֵ�ݼ�����ʱ,
    											��CommandREG��ֵ�����������Ϊ��������ʱ*/
    ClearBitMask(BitFramingReg,0x80);			//�������ݴ���

    if (i!=0)
    {
         if(!(ReadRawRC(ErrorReg)&0x1B))			//û�з���FIFO�����,λ��ͻ,��żУ�����,SOF����,
        	 	 	 	 	 	 	 	 	 	 	//���������յ��ֽ�������ʱ
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)			//���������Ϊ���Ͳ���������
             {
               	n = ReadRawRC(FIFOLevelReg);		//��ȡFIFO������ֽ���
              	lastBits = ReadRawRC(ControlReg) & 0x07;//��ʾ�����յ����ֽڵ���Чλ����Ŀ
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)				//���FIFO������ֽ���Ϊ0
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg); }//������Ƭ���ص�����
            }
         }
         else
         {   status = MI_ERR;   }

   }


   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE);		 //ȡ����ǰ����
   return status;
}


/////////////////////////////////////////////////////////////////////
//��������
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn()
{
    unsigned char i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}


/////////////////////////////////////////////////////////////////////
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}


/////////////////////////////////////////////////////////////////////
//��    �ܣ��ۿ�ͳ�ֵ
//����˵��: dd_mode[IN]��������
//               0xC0 = �ۿ�--->PICC_DECREMENT
//               0xC1 = ��ֵ--->PICC_INCREMENT
//          addr[IN]��Ǯ����ַ
//          pValue[IN]��4�ֽ���(��)ֵ����λ��ǰ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdValue(unsigned char dd_mode,unsigned char addr,unsigned char *pValue)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];
    unsigned char i;

    ucComMF522Buf[0] = dd_mode;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {    ucComMF522Buf[i] = *(pValue+i);   }
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);
        unLen = 0;
        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = PICC_TRANSFER;	//���滺����������
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�����Ǯ��(����Դ��ַ�����ݸ��Ʊ��浽Ŀ�ĵ�ַȥ)
//����˵��: sourceaddr[IN]��Դ��ַ
//          goaladdr[IN]��Ŀ���ַ
//��    ��: �ɹ�����MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;	//�������ݵ�������
    ucComMF522Buf[1] = sourceaddr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)
    {
        ucComMF522Buf[0] = 0;
        ucComMF522Buf[1] = 0;
        ucComMF522Buf[2] = 0;
        ucComMF522Buf[3] = 0;
        CalulateCRC(ucComMF522Buf,4,&ucComMF522Buf[4]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,6,ucComMF522Buf,&unLen);
        if (status != MI_ERR)
        {    status = MI_OK;    }
    }

    if (status != MI_OK)
    {    return MI_ERR;   }

    ucComMF522Buf[0] = PICC_TRANSFER;		//���滺����������
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//��  �ܣ�����RC522�Ĺ�����ʽ
//����˵��:
//��  ��:
/////////////////////////////////////////////////////////////////////
char M500PcdConfigISOType(unsigned char type)
{
	if (type == 'A')                     //ISO14443_A
	{
		ClearBitMask(Status2Reg,0x08);

 /*     WriteRawRC(CommandReg,0x20);    //as default
		WriteRawRC(ComIEnReg,0x80);     //as default
       	WriteRawRC(DivlEnReg,0x0);      //as default
		WriteRawRC(ComIrqReg,0x04);     //as default
     	WriteRawRC(DivIrqReg,0x0);      //as default
     	WriteRawRC(Status2Reg,0x0);//80    //trun off temperature sensor
     	WriteRawRC(WaterLevelReg,0x08); //as default
       	WriteRawRC(ControlReg,0x20);    //as default
     	WriteRawRC(CollReg,0x80);    //as default
*/
		WriteRawRC(ModeReg,0x3D);//3F
/*     	WriteRawRC(TxModeReg,0x0);      //as default
     	WriteRawRC(RxModeReg,0x0);      //as default
     	WriteRawRC(TxControlReg,0x80);  //as default

     	WriteRawRC(TxSelReg,0x10);      //as default???
   */
		WriteRawRC(RxSelReg,0x86);//84
 //     WriteRawRC(RxThresholdReg,0x84);//as default
 //     WriteRawRC(DemodReg,0x4D);      //as default

 //     WriteRawRC(ModWidthReg,0x13);//26
		WriteRawRC(RFCfgReg,0x7F);   //4F
  /*    WriteRawRC(GsNReg,0x88);        //as default???
        WriteRawRC(CWGsCfgReg,0x20);    //as default???
        WriteRawRC(ModGsCfgReg,0x20);   //as default???
*/
        WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        WriteRawRC(TReloadRegH,0);
        WriteRawRC(TModeReg,0x8D);
        WriteRawRC(TPrescalerReg,0x3E);

        delayus(1);
        PcdAntennaOn();
	}
	else{ return MI_ERR; }

	return MI_OK;
}

void RC522_Init(void)
{
	USCIA1_EnterIntoConfig();			//USCIA1��������ģʽ
	USCIA1_SetBaud(6000000);			//6MHz�Ĳ�����
	USCIA1_SPI_Config();				//3��SPI������ģʽ�����е͵�ƽ�������ط������ݣ��½��ض�ȡ����,8λ���ݣ���λ����
	USCIA1_ExitConfig();				//�˳�����ģʽ
	GPIO_Set_Direction_Out(7,0,HIGH);	//NSS
	GPIO_Set_Direction_Out(3,2,HIGH);	//RST
	GPIO_Set_Direction_In(2,0,GPIO_IN_UP);//IRQ
//	GPIO_ExInt_Enable(2,0,GPIO_INT_EDGE_DOWN);//�½��ش����ⲿ�ж�
//	GPIO_Clr_ExInt_Flag(2,0);			//����ⲿ�жϱ�־
	PcdReset();
	PcdAntennaOff();
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
}

/*
 * RC522��ȡMifare���ڲ��������
 * addr:���ַ
 * pKey���������Կ����
 * pSnr��ѡ���Ŀ�Ƭ��ID������
 * dataout�����صĵõ�������
 * ��ȡ�ɹ�����MI_OK��ʧ�ܷ���MI_ERR
 */
char RC522_ReadData(u8 addr,u8 *pKey,u8 *pSnr,u8 *dataout)
{
	char status,k;
	status = MI_ERR;
	k=5;	//���²�����������
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
    }while(status!=MI_OK && k>0);

	status=MI_ERR;				//����֤�ɹ������У���Ҫ��ȡ�ɹ�������ķ���MI_OK
	k=5;
	do
    {
		status=PcdRead(addr,dataout);
		k--;
    }while(status!=MI_OK && k>0);

	return status;
}

/*
 * RC522��Mifare���ڲ���д����
 * addr:���ַ
 * pKey���������Կ����
 * pSnr��ѡ���Ŀ�Ƭ��ID������
 * datain����д�������
 * ��ȡ�ɹ�����MI_OK��ʧ�ܷ���MI_ERR
 */
char RC522_WriteData(u8 addr,u8 *pKey,u8 *pSnr,u8 *datain)
{
	char status,k;
	status=MI_ERR;
	k=5;
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
    }while(status!=MI_OK && k>0);

	status=MI_ERR;
	k=5;
	do
    {
		status=PcdWrite(addr,datain);
		k--;
    }while(status!=MI_OK && k>0);

	return status;
}

/*
 * ����������Ԥ��0Ԫ
 * addr:Ǯ����ַ
 * pKey:Ǯ������������Կ
 * pSnr:��Ƭ��ID��
 */
char RC522_CreatPacket(u8 addr,u8 *pKey,u8 *pSnr)
{
	char status;
	u8 temp[16];
	char k;
	for(k=0;k<4;k++)
	{
		temp[k] = 0;
		temp[k+8] = 0;
		temp[k+4]= ~temp[k];
	}
	temp[12] = addr;
	temp[14] = temp[12];
	temp[13] = ~temp[12];
	temp[15] = temp[13];

	status=MI_ERR;
	k=5;
	do
    {
		status = RC522_WriteData(addr,pKey,pSnr,temp);
		k--;
    }while(status!=MI_OK && k>0);
	return status;
}

/*
 * ʵ�ֿ۷ѵĺ���
 * addr��Ǯ����ַ
 * pKey��Ǯ��������������Կ
 * pKey����Ƭ��ID��
 * pValue:��Ҫ�۳��ķ���
 */
char RC522_DecreaseMoney(u8 addr,u8 *pKey,u8 *pSnr,u32 pValue)
{
	char status,k;
	u8 temp[16];
	u32 money = 0;

	status=MI_ERR;
	k=5;
	do
    {
		status = RC522_ReadData(addr,pKey,pSnr,temp);
		k--;
    }while(status!=MI_OK && k>0);
	if(status != MI_OK)
	{
		return MI_ERR;
	}

	for(k=0;k<4;k++)
	{
		if((temp[k] != temp[k+8]) || (temp[k]+temp[k+4] != 0xff))
		{
			return No_Packet;								//��ʾ��ǰ�����ܵ���Ǯ��ʹ��,���ڰ�ȫ�Կ���
		}
	}

	if((temp[12] != temp[14]) || (temp[13] != temp[15]) || (temp[12]+temp[13] != 0xff))
	{
		return No_Packet;								//��ʾ��ǰ�����ܵ���Ǯ��ʹ��,���ڰ�ȫ�Կ���
	}

	for(k=0;k<4;k++)
	{
		money <<= 8;
		money += temp[3-k];								//Ǯ���ֽ���ǰ�����ֽ��ں�
	}
	if(money < pValue)
	{
		return Money_WIPE_OUT;								//Ǯ����
	}
	money -= pValue;									//��Ǯ


	for(k=0;k<4;k++)
	{
		temp[k] = (u8)money;
		temp[k+8] = temp[k];
		temp[k+4] = ~temp[k];
		money >>= 8;
	}


	status=MI_ERR;
	k=5;
	do
	{
		status = RC522_WriteData(addr,pKey,pSnr,temp);
		k--;
	}while(status!=MI_OK && k>0);

	return status;
}

char RC522_IncreaseMoney(u8 addr,u8 *pKey,u8 *pSnr,u32 pValue)
{
	char status,k;
	u8 temp[16];
	u32 money = 0;

	status=MI_ERR;
	k=5;
	do
    {
		status = RC522_ReadData(addr,pKey,pSnr,temp);
		k--;
    }while(status!=MI_OK && k>0);
	if(status != MI_OK)
	{
		return MI_ERR;
	}

	for(k=0;k<4;k++)
	{
		if((temp[k] != temp[k+8]) || (temp[k]+temp[k+4] != 0xff))
		{
			return No_Packet;								//��ʾ��ǰ�����ܵ���Ǯ��ʹ��,���ڰ�ȫ�Կ���
		}
	}
	if((temp[12] != temp[14]) || (temp[13] != temp[15]) || (temp[12]+temp[13] != 0xff))
	{
		return No_Packet;									//��ʾ��ǰ�����ܵ���Ǯ��ʹ��,���ڰ�ȫ�Կ���
	}

	for(k=0;k<4;k++)
	{
		money <<= 8;
		money += temp[3-k];								//Ǯ���ֽ���ǰ�����ֽ��ں�
	}
	money += pValue;									//��ֵ

	for(k=0;k<4;k++)
	{
		temp[k] = (u8)money;
		temp[k+8] = temp[k];
		temp[k+4] = ~temp[k];
		money >>= 8;
	}

	status=MI_ERR;
	k=5;
	do
    {
		status = RC522_WriteData(addr,pKey,pSnr,temp);
		k--;
    }while(status!=MI_OK && k>0);

	return status;
}

