#ifndef RC522_H_
#define RC522_H_
#include "MyF5529.h"

//备注： Mifare 1卡总共16个扇区，每个扇区4个块，每个块16个字节
/*引脚连接图：
*			P4.0--->SCK
*			P4.4--->MOSI
*			P4.5--->MISO
*			P7.0--->SDA(NSS)
*			P2.0--->IRQ
*			P3.2--->RST
*			茅胜荣的苏大卡ID：CD30A011
*			肖家文的苏大卡ID：FDA39411
*			*/
#define Reset_RC522_NSS()			GPIO_ResetBit(7,0)
#define Set_RC522_NSS()				GPIO_SetBit(7,0)
#define Reset_RC522_RST()			GPIO_ResetBit(3,2)
#define Set_RCC522_RST()			GPIO_SetBit(3,2)
/**********************************************************************************************
*MF RC522命令字
***********************************************************************************************/
#define PCD_IDLE              0x00               //取消当前命令
#define PCD_AUTHENT           0x0E               //验证密钥,执行读卡器的MIFARE标准认证
#define PCD_RECEIVE           0x08               //接收数据，激活接收器电路
#define PCD_TRANSMIT          0x04               //发送数据，发送FIFO缓冲区的命令,该命令在FIFO变空之后自动停止
#define PCD_TRANSCEIVE        0x0C               //发送并接收数据
#define PCD_RESETPHASE        0x0F               //复位MFR522
#define PCD_CALCCRC           0x03               //CRC计算,激活CRC协处理器或者执行自测试

/////////////////////////////////////////////////////////////////////
//Mifare_1卡片命令字
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //寻天线区内未进入休眠状态
#define PICC_REQALL           0x52               //寻天线区内全部卡
#define PICC_ANTICOLL1        0x93               //防冲撞
#define PICC_ANTICOLL2        0x95               //防冲撞
#define PICC_AUTHENT1A        0x60               //验证A密钥
#define PICC_AUTHENT1B        0x61               //验证B密钥
#define PICC_READ             0x30               //读块
#define PICC_WRITE            0xA0               //写块
#define PICC_DECREMENT        0xC0               //扣款
#define PICC_INCREMENT        0xC1               //充值
#define PICC_RESTORE          0xC2               //调块数据到缓冲区
#define PICC_TRANSFER         0xB0               //保存缓冲区中数据
#define PICC_HALT             0x50               //休眠

/////////////////////////////////////////////////////////////////////
//MF522 FIFO长度定义
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522寄存器定义
/////////////////////////////////////////////////////////////////////
// PAGE 0,命令和状态
#define     RFU00                 0x00		//保留为将来之用
#define     CommandReg            0x01		//启动停止命令的执行
#define     ComIEnReg             0x02		//中断请求传递的使能和停止控制位
#define     DivlEnReg             0x03		//中断请求传递的使能和停止控制位
#define     ComIrqReg             0x04		//包含中断请求标志
#define     DivIrqReg             0x05		//包含中断请求标志
#define     ErrorReg              0x06		//错误标志，指示执行的上一个命令的错误状态
#define     Status1Reg            0x07		//包含通信的状态标志
#define     Status2Reg            0x08		//包含接收器和发送器的状态标志
#define     FIFODataReg           0x09		//64字节FIFO缓冲区的输入和输出
#define     FIFOLevelReg          0x0A		//指示FIFO中存储的字节数
#define     WaterLevelReg         0x0B		//定义FIFO下溢和上溢报警的FIFO深度
#define     ControlReg            0x0C		//不同的控制寄存器
#define     BitFramingReg         0x0D		//面向位的帧的调节
#define     CollReg               0x0E		//RF接口上检测到的第一个位冲突的位的位置
#define     RFU0F                 0x0F		//保留为将来所用
// PAGE 1,命令
#define     RFU10                 0x10		//保留为将来之用
#define     ModeReg               0x11		//定义发送和接收的常用模式
#define     TxModeReg             0x12		//定义发送过程的数据传输速率
#define     RxModeReg             0x13		//定义接收过程的数据传输速率
#define     TxControlReg          0x14		//控制天线驱动器管脚的TX1和TX2的逻辑特性
#define     TxAutoReg             0x15		//控制天线驱动器的设置
#define     TxSelReg              0x16		//选择天线驱动器的内部源
#define     RxSelReg              0x17		//选择内部的接收器设置
#define     RxThresholdReg        0x18		//选择位译码器的阈值
#define     DemodReg              0x19		//定义解调器的设置
#define     RFU1A                 0x1A		//保留为将来所用
#define     RFU1B                 0x1B		//保留为将来所用
#define     MifareReg             0x1C		//控制ISO 14443/MIFARE模式中106kb/s通信
#define     RFU1D                 0x1D		//保留为将来所用
#define     RFU1E                 0x1E		//保留为将来所用
#define     SerialSpeedReg        0x1F		//选择串行UART接口速率
// PAGE 2,CFG
#define     RFU20                 0x20		//保留为将来所用
#define     CRCResultRegM         0x21		//显示CRC计算的实际MSB
#define     CRCResultRegL         0x22		//显示CRC计算的实际LSB
#define     RFU23                 0x23		//保留为将来所用
#define     ModWidthReg           0x24		//控制ModWidth的设置
#define     RFU25                 0x25		//保留为将来所用
#define     RFCfgReg              0x26		//配置接收器增益
#define     GsNReg                0x27		//选择天线驱动器管脚TX1和TX2的调制电导
#define     CWGsCfgReg            0x28		//选择天线驱动器管脚TX1和TX2的调制电导
#define     ModGsCfgReg           0x29		//选择天线驱动器管脚TX1和TX2的调制电导
#define     TModeReg              0x2A		//定义内部定时器的设置
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C		//描述16位长的定时器重装值
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E		//描述16位长的实际定时器值
#define     TCounterValueRegL     0x2F
// PAGE 3,测试
#define     RFU30                 0x30		//保留为将来所用
#define     TestSel1Reg           0x31		//常用测试信号的配置
#define     TestSel2Reg           0x32		//常用测试信号的配置和PRBS控制
#define     TestPinEnReg          0x33		//D1-D7输出驱动器的使能管脚
#define     TestPinValueReg       0x34		//定义D1-D7用作IO总线时的值
#define     TestBusReg            0x35		//显示内部测试总线的状态
#define     AutoTestReg           0x36		//控制数字自测试
#define     VersionReg            0x37		//显示版本
#define     AnalogTestReg         0x38		//控制管脚AUX1和AUX2
#define     TestDAC1Reg           0x39		//定义TestDAC1的测试值
#define     TestDAC2Reg           0x3A		//定义TestDAC2的测试值
#define     TestADCReg            0x3B		//显示ADC和Q通道实际值
#define     RFU3C                 0x3C		//保留用于产品测试
#define     RFU3D                 0x3D
#define     RFU3E                 0x3E
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//和MF522通讯时返回的错误代码
/////////////////////////////////////////////////////////////////////
#define MI_ERR                      0xFE
#define Money_WIPE_OUT					0x03			//钱包钱不够
#define No_Packet					0x04				//不是钱包错误

#define MI_OK                          0
#define MI_CHK_OK                      0
#define MI_CRC_ZERO                    0

#define MI_CRC_NOTZERO                 1

#define MI_NOTAGERR                 0xFF
#define MI_CHK_FAILED               0xFF
#define MI_CRCERR                   0xFE
#define MI_CHK_COMPERR              0xFE
#define MI_EMPTY                    0xFD
#define MI_AUTHERR                  0xFC
#define MI_PARITYERR                0xFB
#define MI_CODEERR                  0xFA

#define MI_SERNRERR                 0xF8
#define MI_KEYERR                   0xF7
#define MI_NOTAUTHERR               0xF6
#define MI_BITCOUNTERR              0xF5
#define MI_BYTECOUNTERR             0xF4
#define MI_IDLE                     0xF3
#define MI_TRANSERR                 0xF2
#define MI_WRITEERR                 0xF1
#define MI_INCRERR                  0xF0
#define MI_DECRERR                  0xEF
#define MI_READERR                  0xEE
#define MI_OVFLERR                  0xED
#define MI_POLLING                  0xEC
#define MI_FRAMINGERR               0xEB
#define MI_ACCESSERR                0xEA
#define MI_UNKNOWN_COMMAND          0xE9
#define MI_COLLERR                  0xE8
#define MI_RESETERR                 0xE7
#define MI_INITERR                  0xE7
#define MI_INTERFACEERR             0xE7
#define MI_ACCESSTIMEOUT            0xE5
#define MI_NOBITWISEANTICOLL        0xE4
#define MI_QUIT                     0xE2

#define MI_RECBUF_OVERFLOW          0xCF
#define MI_SENDBYTENR               0xCE

#define MI_SENDBUF_OVERFLOW         0xCC
#define MI_BAUDRATE_NOT_SUPPORTED   0xCB
#define MI_SAME_BAUDRATE_REQUIRED   0xCA

#define MI_WRONG_PARAMETER_VALUE    0xC5

#define MI_BREAK                    0x9E
#define MI_NY_IMPLEMENTED           0x9D
#define MI_NO_MFRC                  0x9C
#define MI_MFRC_NOTAUTH             0x9B
#define MI_WRONG_DES_MODE           0x9A
#define MI_HOST_AUTH_FAILED         0x99

#define MI_WRONG_LOAD_MODE          0x97
#define MI_WRONG_DESKEY             0x96
#define MI_MKLOAD_FAILED            0x95
#define MI_FIFOERR                  0x94
#define MI_WRONG_ADDR               0x93
#define MI_DESKEYLOAD_FAILED        0x92

#define MI_WRONG_SEL_CNT            0x8F
#define MI_RC531_WRONG_READVALUE    0x8E //LI ADDED 09-4-24
#define MI_WRONG_TEST_MODE          0x8C
#define MI_TEST_FAILED              0x8B
#define MI_TOC_ERROR                0x8A
#define MI_COMM_ABORT               0x89
#define MI_INVALID_BASE             0x88
#define MI_MFRC_RESET               0x87
#define MI_WRONG_VALUE              0x86
#define MI_VALERR                   0x85

/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
char PcdHalt(void);
char PcdReset(void);
void PcdAntennaOn(void);
void PcdAntennaOff(void);
char PcdRequest(u8 req_code,u8 *pTagType);
char PcdAnticoll(u8 *pSnr);
char PcdSelect(u8 *pSnr);
char PcdAuthState(u8 auth_mode,u8 addr,u8 *pKey,u8 *pSnr);
char PcdRead(u8 addr,u8 *pData);
char PcdWrite(u8 addr,u8 *pData);
char PcdValue(u8 dd_mode,u8 addr,u8 *pValue);
char PcdBakValue(u8 sourceaddr, u8 goaladdr);
char PcdComMF522(u8 Command, u8 *pInData, u8 InLenByte,u8 *pOutData, u16  *pOutLenBit);
void CalulateCRC(u8 *pIndata,u8 len,u8 *pOutData);
void WriteRawRC(u8 Address,u8 value);
u8 ReadRawRC(u8 Address);
void SetBitMask(u8 reg,u8 mask);
void ClearBitMask(u8 reg,u8 mask);
char M500PcdConfigISOType(unsigned char type);
void RC522_Init(void);
char RC522_ReadData(u8 addr,u8 *pKey,u8 *pSnr,u8 *dataout);
char RC522_WriteData(u8 addr,u8 *pKey,u8 *pSnr,u8 *datain);
char RC522_DecreaseMoney(u8 addr,u8 *pKey,u8 *pSnr,u32 pValue);
char RC522_IncreaseMoney(u8 addr,u8 *pKey,u8 *pSnr,u32 pValue);
char RC522_CreatPacket(u8 addr,u8 *pKey,u8 *pSnr);
#endif
