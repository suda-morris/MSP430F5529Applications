#ifndef RC522_H_
#define RC522_H_
#include "MyF5529.h"

//��ע�� Mifare 1���ܹ�16��������ÿ������4���飬ÿ����16���ֽ�
/*��������ͼ��
*			P4.0--->SCK
*			P4.4--->MOSI
*			P4.5--->MISO
*			P7.0--->SDA(NSS)
*			P2.0--->IRQ
*			P3.2--->RST
*			éʤ�ٵ��մ�ID��CD30A011
*			Ф���ĵ��մ�ID��FDA39411
*			*/
#define Reset_RC522_NSS()			GPIO_ResetBit(7,0)
#define Set_RC522_NSS()				GPIO_SetBit(7,0)
#define Reset_RC522_RST()			GPIO_ResetBit(3,2)
#define Set_RCC522_RST()			GPIO_SetBit(3,2)
/**********************************************************************************************
*MF RC522������
***********************************************************************************************/
#define PCD_IDLE              0x00               //ȡ����ǰ����
#define PCD_AUTHENT           0x0E               //��֤��Կ,ִ�ж�������MIFARE��׼��֤
#define PCD_RECEIVE           0x08               //�������ݣ������������·
#define PCD_TRANSMIT          0x04               //�������ݣ�����FIFO������������,��������FIFO���֮���Զ�ֹͣ
#define PCD_TRANSCEIVE        0x0C               //���Ͳ���������
#define PCD_RESETPHASE        0x0F               //��λMFR522
#define PCD_CALCCRC           0x03               //CRC����,����CRCЭ����������ִ���Բ���

/////////////////////////////////////////////////////////////////////
//Mifare_1��Ƭ������
/////////////////////////////////////////////////////////////////////
#define PICC_REQIDL           0x26               //Ѱ��������δ��������״̬
#define PICC_REQALL           0x52               //Ѱ��������ȫ����
#define PICC_ANTICOLL1        0x93               //����ײ
#define PICC_ANTICOLL2        0x95               //����ײ
#define PICC_AUTHENT1A        0x60               //��֤A��Կ
#define PICC_AUTHENT1B        0x61               //��֤B��Կ
#define PICC_READ             0x30               //����
#define PICC_WRITE            0xA0               //д��
#define PICC_DECREMENT        0xC0               //�ۿ�
#define PICC_INCREMENT        0xC1               //��ֵ
#define PICC_RESTORE          0xC2               //�������ݵ�������
#define PICC_TRANSFER         0xB0               //���滺����������
#define PICC_HALT             0x50               //����

/////////////////////////////////////////////////////////////////////
//MF522 FIFO���ȶ���
/////////////////////////////////////////////////////////////////////
#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

/////////////////////////////////////////////////////////////////////
//MF522�Ĵ�������
/////////////////////////////////////////////////////////////////////
// PAGE 0,�����״̬
#define     RFU00                 0x00		//����Ϊ����֮��
#define     CommandReg            0x01		//����ֹͣ�����ִ��
#define     ComIEnReg             0x02		//�ж����󴫵ݵ�ʹ�ܺ�ֹͣ����λ
#define     DivlEnReg             0x03		//�ж����󴫵ݵ�ʹ�ܺ�ֹͣ����λ
#define     ComIrqReg             0x04		//�����ж������־
#define     DivIrqReg             0x05		//�����ж������־
#define     ErrorReg              0x06		//�����־��ָʾִ�е���һ������Ĵ���״̬
#define     Status1Reg            0x07		//����ͨ�ŵ�״̬��־
#define     Status2Reg            0x08		//�����������ͷ�������״̬��־
#define     FIFODataReg           0x09		//64�ֽ�FIFO����������������
#define     FIFOLevelReg          0x0A		//ָʾFIFO�д洢���ֽ���
#define     WaterLevelReg         0x0B		//����FIFO��������籨����FIFO���
#define     ControlReg            0x0C		//��ͬ�Ŀ��ƼĴ���
#define     BitFramingReg         0x0D		//����λ��֡�ĵ���
#define     CollReg               0x0E		//RF�ӿ��ϼ�⵽�ĵ�һ��λ��ͻ��λ��λ��
#define     RFU0F                 0x0F		//����Ϊ��������
// PAGE 1,����
#define     RFU10                 0x10		//����Ϊ����֮��
#define     ModeReg               0x11		//���巢�ͺͽ��յĳ���ģʽ
#define     TxModeReg             0x12		//���巢�͹��̵����ݴ�������
#define     RxModeReg             0x13		//������չ��̵����ݴ�������
#define     TxControlReg          0x14		//���������������ܽŵ�TX1��TX2���߼�����
#define     TxAutoReg             0x15		//��������������������
#define     TxSelReg              0x16		//ѡ���������������ڲ�Դ
#define     RxSelReg              0x17		//ѡ���ڲ��Ľ���������
#define     RxThresholdReg        0x18		//ѡ��λ����������ֵ
#define     DemodReg              0x19		//��������������
#define     RFU1A                 0x1A		//����Ϊ��������
#define     RFU1B                 0x1B		//����Ϊ��������
#define     MifareReg             0x1C		//����ISO 14443/MIFAREģʽ��106kb/sͨ��
#define     RFU1D                 0x1D		//����Ϊ��������
#define     RFU1E                 0x1E		//����Ϊ��������
#define     SerialSpeedReg        0x1F		//ѡ����UART�ӿ�����
// PAGE 2,CFG
#define     RFU20                 0x20		//����Ϊ��������
#define     CRCResultRegM         0x21		//��ʾCRC�����ʵ��MSB
#define     CRCResultRegL         0x22		//��ʾCRC�����ʵ��LSB
#define     RFU23                 0x23		//����Ϊ��������
#define     ModWidthReg           0x24		//����ModWidth������
#define     RFU25                 0x25		//����Ϊ��������
#define     RFCfgReg              0x26		//���ý���������
#define     GsNReg                0x27		//ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     CWGsCfgReg            0x28		//ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     ModGsCfgReg           0x29		//ѡ�������������ܽ�TX1��TX2�ĵ��Ƶ絼
#define     TModeReg              0x2A		//�����ڲ���ʱ��������
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C		//����16λ���Ķ�ʱ����װֵ
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E		//����16λ����ʵ�ʶ�ʱ��ֵ
#define     TCounterValueRegL     0x2F
// PAGE 3,����
#define     RFU30                 0x30		//����Ϊ��������
#define     TestSel1Reg           0x31		//���ò����źŵ�����
#define     TestSel2Reg           0x32		//���ò����źŵ����ú�PRBS����
#define     TestPinEnReg          0x33		//D1-D7�����������ʹ�ܹܽ�
#define     TestPinValueReg       0x34		//����D1-D7����IO����ʱ��ֵ
#define     TestBusReg            0x35		//��ʾ�ڲ��������ߵ�״̬
#define     AutoTestReg           0x36		//���������Բ���
#define     VersionReg            0x37		//��ʾ�汾
#define     AnalogTestReg         0x38		//���ƹܽ�AUX1��AUX2
#define     TestDAC1Reg           0x39		//����TestDAC1�Ĳ���ֵ
#define     TestDAC2Reg           0x3A		//����TestDAC2�Ĳ���ֵ
#define     TestADCReg            0x3B		//��ʾADC��Qͨ��ʵ��ֵ
#define     RFU3C                 0x3C		//�������ڲ�Ʒ����
#define     RFU3D                 0x3D
#define     RFU3E                 0x3E
#define     RFU3F		  		  0x3F

/////////////////////////////////////////////////////////////////////
//��MF522ͨѶʱ���صĴ������
/////////////////////////////////////////////////////////////////////
#define MI_ERR                      0xFE
#define Money_WIPE_OUT					0x03			//Ǯ��Ǯ����
#define No_Packet					0x04				//����Ǯ������

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
//����ԭ��
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
