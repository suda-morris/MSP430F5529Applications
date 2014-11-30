#ifndef _SDCARD_H
#define _SDCARD_H
/*
 * �������ӣ�
 * 		SD_MOSI------->P4.1
 * 		SD_MISO------->P4.2
 * 		SD_SCLK------->P4.3
 * 		SD_CS--------->P3.7
 */
#include "MyF5529.h"

#define SDCard_CS_Low()			GPIO_ResetBit(3,7)
#define SDCard_CS_High()		GPIO_SetBit(3,7)

// SD�����Ͷ���
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06

// SD��ָ���
#define CMD0    0       //����λ
#define CMD1    1		//�����ʼ��
#define CMD8    8       //����8 �����ͽӿ�״̬
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������һ����д��ĳ��ȣ� Ӧ����0x00
#define CMD17   17      //����17����ȡһ���������
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00

//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF

//SD����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF


extern unsigned char SD_SPI_ReadWriteByte(unsigned char data);
extern unsigned char SD_WaitReady(void);					//�ȴ�SD��׼��
extern unsigned char SD_GetResponse(u8 Response);					//�����Ӧ
extern unsigned char SD_Initialize(void);							//��ʼ��
extern unsigned char SD_ReadDisk(u8*buf,u32 sector,u8 cnt);			//����
extern unsigned char SD_WriteDisk(u8*buf,u32 sector,u8 cnt);		//д��
extern unsigned long SD_GetSectorCount(void);   					//��������
extern unsigned char SD_GetCID(u8 *cid_data);                     	//��SD��CID
extern unsigned char SD_GetCSD(u8 *csd_data);                     	//��SD��CSD

#endif /* HAL_SDCARD_H */
