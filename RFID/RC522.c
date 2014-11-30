#include "RC522.h"
#define MAXRLEN 18
//M1卡的某一块写为如下格式，则该块为钱包，可接收扣款和充值命令
//4字节金额（低字节在前）＋4字节金额取反＋4字节金额＋1字节块地址＋1字节块地址取反＋1字节块地址＋1字节块地址取反
unsigned char const Money[16] = {0x12,0x34,0x56,0x78,0xED,0xCB,0xA9,0x87,
								0x12,0x34,0x56,0x78,0x01,0xFE,0x01,0xFE};
unsigned char CardType[2] = {0,0};//卡类型
unsigned char CardID[4] = {0,0,0,0}; //卡的ID号
unsigned char const DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
/////////////////////////////////////////////////////////////////////
//功    能：寻卡
//参数说明: req_code[IN]:寻卡方式
//                0x52 = 寻感应区内所有符合14443A标准的卡--->PICC_REQALL
//                0x26 = 寻未进入休眠状态的卡--->PICC_REQIDL
//          pTagType[OUT]：卡片类型代码
//                0x4400 = Mifare_UltraLight
//                0x0400 = Mifare_One(S50)
//                0x0200 = Mifare_One(S70)
//                0x0800 = Mifare_Pro(X)
//                0x4403 = Mifare_DESFire
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRequest(unsigned char req_code,unsigned char *pTagType)
{
   char status;
   unsigned int  unLen;					//用于保存返回数据的位长度
   unsigned char ucComMF522Buf[MAXRLEN];
   ClearBitMask(Status2Reg,0x08);		//只有成功执行MFAuthent命令后，该位才能置位，这里需要软件清零
   WriteRawRC(BitFramingReg,0x07);		//用于面向位的帧的发送，0x07表示最后一个字节的所有位都不要发送
   SetBitMask(TxControlReg,0x03);		//TX1,TX2管脚的输出信号将传递经发送数据调整的13.56MHZ的能量载波信号

   ucComMF522Buf[0] = req_code;

   status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);
   if ((status == MI_OK) && (unLen == 0x10))		//返回卡片类型,2个字节(16位)
   {
       *pTagType     = ucComMF522Buf[0];
       *(pTagType+1) = ucComMF522Buf[1];
   }
   else
   {   status = MI_ERR;   }							//RC522与卡片通信失败，返回error

   return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：防冲撞
//参数说明: pSnr[OUT]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAnticoll(unsigned char *pSnr)
{
    char status;
    unsigned char i,snr_check=0;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);			//只有成功执行MFAuthent命令后，该位才能置位，这里需要软件清零
    WriteRawRC(BitFramingReg,0x00);			//用于面向位的帧的发送，0x00表示最后一个字节的所有位都要发送
    ClearBitMask(CollReg,0x80);				//所有接收的位在冲撞后被清除

    ucComMF522Buf[0] = PICC_ANTICOLL1;		//防冲撞指令
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)					//通信成功，返回卡片ID,4个字节
    {
    	 for (i=0; i<4; i++)
         {
             *(pSnr+i)  = ucComMF522Buf[i];
             snr_check ^= ucComMF522Buf[i];
         }
         if (snr_check != ucComMF522Buf[i])	//第5个字节是用于校验
         {   status = MI_ERR;    }
    }

    SetBitMask(CollReg,0x80);
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：选定卡片
//参数说明: pSnr[IN]:卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdSelect(unsigned char *pSnr)
{
    char status;
    unsigned char i;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;			//防冲撞
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;						//第7个字节用于存放校验
    for (i=0; i<4; i++)
    {
    	ucComMF522Buf[i+2] = *(pSnr+i);
    	ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);	//返回的CRC校验码存放在ucComMF522Buf[7]和
    												//ucComMF522Buf[8]中

    ClearBitMask(Status2Reg,0x08);			//在执行PCD_TRANSCEIVE指令之前都需要加这句话

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))	//返回3个字节的数据
    {   status = MI_OK;  }
    else
    {   status = MI_ERR;    }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：验证卡片密码
//参数说明: auth_mode[IN]: 密码验证模式
//                 0x60 = 验证A密钥-->PICC_AUTHENT1A
//                 0x61 = 验证B密钥-->PICC_AUTHENT1B
//          addr[IN]：块地址
//          pKey[IN]：密码
//          pSnr[IN]：卡片序列号，4字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;		//认证模式
    ucComMF522Buf[1] = addr;			//块地址
    for (i=0; i<6; i++)					//6个字节的密码，ucComMF522Buf[2]~ucComMF522Buf[7]
    {    ucComMF522Buf[i+2] = *(pKey+i);   }
    for (i=0; i<4; i++)					//6个字节的卡号,ucComMF522Buf[8]~ucComMF522Buf[11]
    {    ucComMF522Buf[i+8] = *(pSnr+i);   }

    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))		//如果没有成功执行Authent指令
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：读取M1卡一块数据
//参数说明: addr[IN]：块地址
//          pData[OUT]：读出的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdRead(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;		//命令
    ucComMF522Buf[1] = addr;			//块地址
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);		//对前两个字节生成1个字节的CRC校验码存放在
    													//ucComMF522Buf[2]和ucComMF522Buf[3]中

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))		//返回18个字节，前16个字节为读出的数据，最后两个字节是校验码
    {
        for (i=0; i<16; i++)
        {    *(pData+i) = ucComMF522Buf[i];   }
    }
    else
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：写数据到M1卡一块
//参数说明: addr[IN]：块地址
//          pData[IN]：写入的数据，16字节
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdWrite(unsigned char addr,unsigned char *pData)
{
    char status;
    unsigned int  unLen;
    unsigned char i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;			//命令
    ucComMF522Buf[1] = addr;				//块地址
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);//先传命令和地址

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    if (status == MI_OK)				//表示可以传送数据了
    {
        for (i=0; i<16; i++)
        {    ucComMF522Buf[i] = *(pData+i);   }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);//16字节的代写入数据加上后面两个字节的校验码，总共18个字节

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }

    return status;
}



/////////////////////////////////////////////////////////////////////
//功    能：命令卡片进入休眠状态
//返    回: 成功返回MI_OK
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
//用MF522计算CRC16函数
/////////////////////////////////////////////////////////////////////
void CalulateCRC(unsigned char *pIndata,unsigned char len,unsigned char *pOutData)
{
    unsigned char i,n;
    ClearBitMask(DivIrqReg,0x04);			//当CRC命令有效且数据被处理时才置位
    WriteRawRC(CommandReg,PCD_IDLE);		//使RC522转入空闲状态
    SetBitMask(FIFOLevelReg,0x80);			//内部FIFO的缓冲区的读与写指针以及寄存器ErrReg的BufferOvfi标志立刻被清除
    for (i=0; i<len; i++)
    {   WriteRawRC(FIFODataReg, *(pIndata+i));   }
    WriteRawRC(CommandReg, PCD_CALCCRC);		// CRC计算
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);			//读出中断请求标志
        i--;
    }
    while ((i!=0) && !(n&0x04));			//当CRC计算完毕后
    pOutData[0] = ReadRawRC(CRCResultRegL);//读出CRC计算实际的LSB值
    pOutData[1] = ReadRawRC(CRCResultRegM);//读出CRC计算实际的MSB值
}

/////////////////////////////////////////////////////////////////////
//功    能：复位RC522
//返    回: 成功返回MI_OK
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

    WriteRawRC(ModeReg,0x3D);            	//和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);				//内部定时器预装值30
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);				//设置Prescaler的值
    WriteRawRC(TPrescalerReg,0x3E);
    WriteRawRC(TxAutoReg,0x40);				//必须要
    return MI_OK;
}
/////////////////////////////////////////////////////////////////////
//功    能：读RC632寄存器
//参数说明：Address[IN]:寄存器地址
//返    回：读出的值
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
//功    能：写RC632寄存器
//参数说明：Address[IN]:寄存器地址
//          value[IN]:写入的值
/////////////////////////////////////////////////////////////////////
void WriteRawRC(unsigned char Address, unsigned char value)
{
    unsigned char ucAddr[1];

    Reset_RC522_NSS();				//使能片选
    ucAddr[0] = ((Address<<1)&0x7E);

    USCIA1_SPI_SendByte(ucAddr[0]);
    USCIA1_SPI_SendByte(value);

    Set_RC522_NSS();
}

/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(unsigned char reg,unsigned char mask)
{
    char tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pInData[IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOutData[OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
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
       case PCD_AUTHENT:				//验证密钥
          irqEn   = 0x12;
          waitFor = 0x10;
          break;
       case PCD_TRANSCEIVE:				//发送并接收数据
          irqEn   = 0x77;
          waitFor = 0x30;
          break;
       default:
         break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);/*如果验证密钥,管脚IRQ上的信号与IRq位相反,允许空闲中断请求,允许错误中断请求
                                                                                如果发送并接受数据,管脚IRQ上的信号与IRq位相反,允许发送器中断请求,允许接受器中断请求,
                                                                                允许空闲中断请求,允许低警报中断请求,允许错误中断请求,允许定时器中断请求*/
    ClearBitMask(ComIrqReg,0x80);			//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE);		//是RC522进入空闲状态
    SetBitMask(FIFOLevelReg,0x80);			//内部FIFO的缓冲区的读与写指针以及寄存器ErrReg的BufferOvfi标志立刻被清除清FIFO缓存

    for (i=0; i<InLenByte; i++)
    {   WriteRawRC(FIFODataReg, pInData[i]);    }//将数据发送写入FIFO缓冲区
    WriteRawRC(CommandReg, Command);			//执行RC522命令字


    if (Command == PCD_TRANSCEIVE)				//如果命令字为发送并接受数据
    {    SetBitMask(BitFramingReg,0x80);  }		//启动数据传输(将FIFO中的数据传入卡中)

    i = 600;								//根据时钟频率调整，操作M1卡最大等待时间25ms
    do
    {
         n = ReadRawRC(ComIrqReg);		 	//读取中断请求标志寄存器的值
         i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));/*当等待时间到时,定时器的TimerValue寄存器的值递减到零时,
    											当CommandREG的值从其它命令变为空闲命令时*/
    ClearBitMask(BitFramingReg,0x80);			//结束数据传输

    if (i!=0)
    {
         if(!(ReadRawRC(ErrorReg)&0x1B))			//没有发生FIFO区溢出,位冲突,奇偶校验出错,SOF出错,
        	 	 	 	 	 	 	 	 	 	 	//数据流接收到字节数出错时
         {
             status = MI_OK;
             if (n & irqEn & 0x01)
             {   status = MI_NOTAGERR;   }
             if (Command == PCD_TRANSCEIVE)			//如果命令字为发送并接受数据
             {
               	n = ReadRawRC(FIFOLevelReg);		//读取FIFO保存的字节数
              	lastBits = ReadRawRC(ControlReg) & 0x07;//显示最后接收到的字节的有效位的数目
                if (lastBits)
                {   *pOutLenBit = (n-1)*8 + lastBits;   }
                else
                {   *pOutLenBit = n*8;   }
                if (n == 0)				//如果FIFO保存的字节数为0
                {   n = 1;    }
                if (n > MAXRLEN)
                {   n = MAXRLEN;   }
                for (i=0; i<n; i++)
                {   pOutData[i] = ReadRawRC(FIFODataReg); }//读出卡片返回的数据
            }
         }
         else
         {   status = MI_ERR;   }

   }


   SetBitMask(ControlReg,0x80);           // stop timer now
   WriteRawRC(CommandReg,PCD_IDLE);		 //取消当前命令
   return status;
}


/////////////////////////////////////////////////////////////////////
//开启天线
//每次启动或关闭天险发射之间应至少有1ms的间隔
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
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff()
{
    ClearBitMask(TxControlReg, 0x03);
}


/////////////////////////////////////////////////////////////////////
//功    能：扣款和充值
//参数说明: dd_mode[IN]：命令字
//               0xC0 = 扣款--->PICC_DECREMENT
//               0xC1 = 充值--->PICC_INCREMENT
//          addr[IN]：钱包地址
//          pValue[IN]：4字节增(减)值，低位在前
//返    回: 成功返回MI_OK
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
        ucComMF522Buf[0] = PICC_TRANSFER;	//保存缓冲区中数据
        ucComMF522Buf[1] = addr;
        CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {   status = MI_ERR;   }
    }
    return status;
}

/////////////////////////////////////////////////////////////////////
//功    能：备份钱包(即将源地址的数据复制保存到目的地址去)
//参数说明: sourceaddr[IN]：源地址
//          goaladdr[IN]：目标地址
//返    回: 成功返回MI_OK
/////////////////////////////////////////////////////////////////////
char PcdBakValue(unsigned char sourceaddr, unsigned char goaladdr)
{
    char status;
    unsigned int  unLen;
    unsigned char ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_RESTORE;	//调块数据到缓冲区
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

    ucComMF522Buf[0] = PICC_TRANSFER;		//保存缓冲区中数据
    ucComMF522Buf[1] = goaladdr;

    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {   status = MI_ERR;   }

    return status;
}

/////////////////////////////////////////////////////////////////////
//功  能：设置RC522的工作方式
//参数说明:
//返  回:
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
	USCIA1_EnterIntoConfig();			//USCIA1进入配置模式
	USCIA1_SetBaud(6000000);			//6MHz的波特率
	USCIA1_SPI_Config();				//3线SPI，主机模式，空闲低电平，上升沿发送数据，下降沿读取数据,8位数据，高位在先
	USCIA1_ExitConfig();				//退出配置模式
	GPIO_Set_Direction_Out(7,0,HIGH);	//NSS
	GPIO_Set_Direction_Out(3,2,HIGH);	//RST
	GPIO_Set_Direction_In(2,0,GPIO_IN_UP);//IRQ
//	GPIO_ExInt_Enable(2,0,GPIO_INT_EDGE_DOWN);//下降沿触发外部中断
//	GPIO_Clr_ExInt_Flag(2,0);			//清除外部中断标志
	PcdReset();
	PcdAntennaOff();
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
}

/*
 * RC522读取Mifare卡内部块的数据
 * addr:块地址
 * pKey：输入的密钥数组
 * pSnr：选定的卡片的ID号数组
 * dataout：返回的得到的数据
 * 读取成功返回MI_OK，失败返回MI_ERR
 */
char RC522_ReadData(u8 addr,u8 *pKey,u8 *pSnr,u8 *dataout)
{
	char status,k;
	status = MI_ERR;
	k=5;	//重新操作的最大次数
	do
    {
		status=PcdAuthState(PICC_AUTHENT1A,addr,pKey,pSnr);
		k--;
    }while(status!=MI_OK && k>0);

	status=MI_ERR;				//光验证成功还不行，还要读取成功才能真的返回MI_OK
	k=5;
	do
    {
		status=PcdRead(addr,dataout);
		k--;
    }while(status!=MI_OK && k>0);

	return status;
}

/*
 * RC522向Mifare卡内部块写数据
 * addr:块地址
 * pKey：输入的密钥数组
 * pSnr：选定的卡片的ID号数组
 * datain：待写入的数据
 * 读取成功返回MI_OK，失败返回MI_ERR
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
 * 开卡函数，预冲0元
 * addr:钱包地址
 * pKey:钱包所在扇区密钥
 * pSnr:卡片的ID号
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
 * 实现扣费的函数
 * addr：钱包地址
 * pKey：钱包所在扇区的密钥
 * pKey：卡片的ID号
 * pValue:需要扣除的费用
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
			return No_Packet;								//表示当前卡不能当做钱包使用,出于安全性考虑
		}
	}

	if((temp[12] != temp[14]) || (temp[13] != temp[15]) || (temp[12]+temp[13] != 0xff))
	{
		return No_Packet;								//表示当前卡不能当做钱包使用,出于安全性考虑
	}

	for(k=0;k<4;k++)
	{
		money <<= 8;
		money += temp[3-k];								//钱低字节在前，高字节在后
	}
	if(money < pValue)
	{
		return Money_WIPE_OUT;								//钱不够
	}
	money -= pValue;									//扣钱


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
			return No_Packet;								//表示当前卡不能当做钱包使用,出于安全性考虑
		}
	}
	if((temp[12] != temp[14]) || (temp[13] != temp[15]) || (temp[12]+temp[13] != 0xff))
	{
		return No_Packet;									//表示当前卡不能当做钱包使用,出于安全性考虑
	}

	for(k=0;k<4;k++)
	{
		money <<= 8;
		money += temp[3-k];								//钱低字节在前，高字节在后
	}
	money += pValue;									//充值

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

