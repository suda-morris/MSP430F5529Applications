/*------------------------------------------------------------------------/
/  Bitbanging MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2010, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
---------------------------------------------------------------------------*/

#include <intrinsics.h>         /* Include MSP430-specific intrincs */
#include "diskio.h"             /* Common include file for FatFs and disk I/O layer */
#include "SDCard/SDCard.h"         	/* MSP-EXP430F5529 specific SD Card driver */

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv            /* Drive number (0) */
)
{
    return 0;
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE drv		/* Physical drive nmuber (0) */
)
{
	BYTE res=0;
	switch(drv)
	{
		case SD_CARD://SD��
			res = SD_Initialize();//SD_Initialize()
/*		 	if(res)//��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
			{
				SD_SPI_SpeedLow();
				SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
				SD_SPI_SpeedHigh();
			}*/
  			break;
		default:
			res=1;
	}
	if(res)
	{
		return  STA_NOINIT;
	}
	else
	{
		return 0; //��ʼ���ɹ�
	}
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
)
{
	BYTE res=0;
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������
	switch(drv)
	{
		case SD_CARD://SD��
			res=SD_ReadDisk(buff,sector,count);
/*		 	if(res)//STM32 SPI��bug,��sd������ʧ�ܵ�ʱ�������ִ����������,���ܵ���SPI��д�쳣
			{
				SD_SPI_SpeedLow();
				SD_SPI_ReadWriteByte(0xff);//�ṩ�����8��ʱ��
				SD_SPI_SpeedHigh();
			}*/
			break;
		default:
			res=1;
	}
   //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res==0x00)return RES_OK;
    else return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..128) */
)
{
	BYTE res=0;
    if (!count)return RES_PARERR;//count���ܵ���0�����򷵻ز�������
	switch(drv)
	{
		case SD_CARD://SD��
			res=SD_WriteDisk((u8*)buff,sector,count);
			break;
		default:
			res=1;
	}
    //������ֵ����SPI_SD_driver.c�ķ���ֵת��ff.c�ķ���ֵ
    if(res == 0x00)return RES_OK;
    else return RES_ERROR;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
	DRESULT res;
	if(drv==SD_CARD)//SD��
	{
	    switch(ctrl)
	    {
		    case CTRL_SYNC:
				SDCard_CS_Low();
		        if(SD_WaitReady()==0)res = RES_OK;
		        else res = RES_ERROR;
				SDCard_CS_High();
		        break;
		    case GET_SECTOR_SIZE:
		        *(WORD*)buff = 512;
		        res = RES_OK;
		        break;
		    case GET_BLOCK_SIZE:
		        *(WORD*)buff = 8;
		        res = RES_OK;
		        break;
		    case GET_SECTOR_COUNT:
		        *(DWORD*)buff = SD_GetSectorCount();
		        res = RES_OK;
		        break;
		    default:
		        res = RES_PARERR;
		        break;
	    }
	}
	return res;
}

DWORD get_fattime (void)
{
	return 0;
}
