/*-----------------------------------------------------------------------
/  Low level disk interface modlue include file   (C)ChaN, 2009
/-----------------------------------------------------------------------*/

#ifndef _DISKIO
#define _DISKIO

#include "integer.h"

#define SD_CARD	 0  //SD卡,卷标为0

/* Status of Disk Functions */
typedef BYTE	DSTATUS;

/* Results of Disk Functions */
typedef enum {
	RES_OK = 0,		/* 0: Successful */
	RES_ERROR,		/* 1: R/W Error */
	RES_WRPRT,		/* 2: Write Protected */
	RES_NOTRDY,		/* 3: Not Ready */
	RES_PARERR		/* 4: Invalid Parameter */
} DRESULT;


/*---------------------------------------*/
/* Prototypes for disk control functions */

DSTATUS disk_initialize (BYTE);			//磁盘初始化
DSTATUS disk_status (BYTE);				//获取磁盘状态
DRESULT disk_read (BYTE, BYTE*, DWORD, BYTE);	//读扇区
DRESULT disk_write (BYTE, const BYTE*, DWORD, BYTE);	//写扇区
DRESULT disk_ioctl (BYTE, BYTE, void*);					//设备相关的控制特性



/* Disk Status Bits (DSTATUS) */

#define STA_NOINIT		0x01	/* Drive not initialized */
#define STA_NODISK		0x02	/* No medium in the drive */
#define STA_PROTECT		0x04	/* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (mandatory for FatFs) */
#define CTRL_SYNC			0	/* Flush disk cache (for write functions) */
#define GET_SECTOR_COUNT	1	/* Get media size (for only f_mkfs()) */
#define GET_SECTOR_SIZE		2	/* Get sector size (for multiple sector size (_MAX_SS >= 1024)) */
#define GET_BLOCK_SIZE		3	/* Get erase block size (for only f_mkfs()) */

#endif
