/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

#include <stdio.h>
#include <string.h>
#include "bsp.h"

/* Definitions of physical drive number for each media */
#define RAM		0
#define USB		1

#define RAM_DISK_SIZE 	(1*1024*1024)
#define ram_disk 		((unsigned char *)__ram_disk)
static unsigned int __ram_disk[RAM_DISK_SIZE / sizeof(unsigned int)] AT_SDRAM;

static inline int ramdisk_read(unsigned char *buf, int sector, int count)
{
	memcpy(buf, ram_disk + sector*512, count*512);
	return 0;
}

static inline int ramdisk_write(const unsigned char *buf, int sector, int count)
{
	memcpy(ram_disk + sector*512, buf, count*512);
	return 0;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS state;

	switch (pdrv)
	{
	case RAM :
		state = 0;
		break;

	case USB :

		state = 0;
		break;
	
	default:
		state = STA_NOINIT;
		break;
	}
	
	return state;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	DSTATUS state;

	switch (pdrv)
	{
	case RAM :

		state = 0;
		break;

	case USB :

		state = 0;
		break;
	
	default:
		state = STA_NOINIT;
		break;
	}
	
	return state;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	DRESULT res;

	switch (pdrv)
	{
	case RAM :
		ramdisk_read(buff, sector, count);
		res = RES_OK;
		break;

	case USB :

		res = RES_PARERR;
		break;
	
	default:
		res = RES_PARERR;
		break;
	}
	
	return res;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	DRESULT res;

	switch (pdrv)
	{
	case RAM :
		ramdisk_write(buff, sector, count);
		res = RES_OK;
		break;

	case USB :

		res = RES_PARERR;
		break;
	
	default:
		res = RES_PARERR;
		break;
	}
	
	return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	switch (pdrv)
	{
	case RAM :
		switch (cmd) 
			{
			case CTRL_SYNC:
				res = RES_OK;
				break;
			case GET_SECTOR_COUNT:
				*(DWORD*)buff = (RAM_DISK_SIZE/512); /*8M space*/
				res = RES_OK;
				break;
			case GET_SECTOR_SIZE:
				*(WORD*)buff = 512;
				res = RES_OK;
				break;
			default:
				res = RES_PARERR;
				break;
			}
		break;

	case USB :

		res = RES_PARERR;
		break;
	
	default:
		res = RES_PARERR;
		break;
	}
	
	return res;
}
#endif

/*
bit31:25
    Year origin from the 1980 (0..127)
bit24:21
    Month (1..12)
bit20:16
    Day of the month(1..31)
bit15:11
    Hour (0..23)
bit10:5
    Minute (0..59)
bit4:0
    Second / 2 (0..29) 
*/
#define FATFS_TIME(Y,M,D,h,m,s) 	( (((Y)-1980)<<25) | ((M)<<21) | ((D)<<16) | ((h)<<11) | ((m)<<5) | ((s)>>1) )
DWORD get_fattime (void)
{
    /* Pack date and time into a DWORD variable */
    return FATFS_TIME(2000, 1, 1, 0, 0, 0);
}

#include "ff.h"
#include "bsp.h"

int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj)	/* Create a sync object */
{
	RAW_ASSERT(0 != sobj);
	
	*sobj = port_malloc(sizeof(RAW_MUTEX));
	if( ! *sobj )
	{
		return 0;
	}
	
	RAW_U16 ret;
	
	ret = raw_mutex_create(*sobj, (RAW_U8 *)"fatfs_mutex", RAW_MUTEX_INHERIT_POLICY, 0);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	return 1;
}
int ff_del_syncobj (_SYNC_t sobj)				/* Delete a sync object */
{
	RAW_ASSERT(0 != sobj);
	
	RAW_U16  ret;
	ret = raw_mutex_delete(sobj);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	port_free(sobj);
	
	return 1;
}
int ff_req_grant (_SYNC_t sobj)				/* Lock sync object */
{
	RAW_U16 ret;
	ret = raw_mutex_get(sobj, RAW_WAIT_FOREVER);
	if(RAW_SUCCESS != ret && RAW_MUTEX_OWNER_NESTED != ret)
	{
		return 0;
	}
	
	return 1;
}
void ff_rel_grant (_SYNC_t sobj)				/* Unlock sync object */
{
	RAW_U16 ret;
	ret = raw_mutex_put(sobj);
	RAW_ASSERT(RAW_SUCCESS == ret);
}

FRESULT scan_files (
    char* path        /* Start node to be scanned (also used as work area) */
)
{
    FRESULT res;
    FILINFO fno;
    DIR dir;
    int i;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
    static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
    fno.lfname = lfn;
    fno.lfsize = sizeof lfn;
#endif


    res = f_opendir(&dir, path);                       /* Open the directory */
    if (res == FR_OK)
	{
        i = strlen(path);
        for (;;)
		{
            res = f_readdir(&dir, &fno);                   /* Read a directory item */
            if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
#if _USE_LFN
            fn = *fno.lfname ? fno.lfname : fno.fname;
#else
            fn = fno.fname;
#endif
            if (fno.fattrib & AM_DIR)
			{                    /* It is a directory */
                sprintf(&path[i], "/%s", fn);
                res = scan_files(path);
                if (res != FR_OK) break;
                path[i] = 0;
            }
			else
			{                                       /* It is a file. */
                raw_printf("%s/%s\n", path, fn);
            }
        }
        f_closedir(&dir);
    }

    return res;
}

const char *fatfs_err2str(FRESULT fret)
{
	const char *serr;

	switch( fret )
	{
	case FR_OK:
		serr = "(0) Succeeded";
		break;
	case FR_DISK_ERR:
		serr = "(1) A hard error occurred in the low level disk I/O layer";
		break;
	case FR_INT_ERR:
		serr = "(2) Assertion failed";
		break;
	case FR_NOT_READY:
		serr = "(3) The physical drive cannot work";
		break;
	case FR_NO_FILE:
		serr = "(4) Could not find the file";
		break;
	case FR_NO_PATH:
		serr = "(5) Could not find the path";
		break;
	case FR_INVALID_NAME:
		serr = "(6) The path name format is invalid";
		break;
	case FR_DENIED:
		serr = "(7) Access denied due to prohibited access or directory full";
		break;
	case FR_EXIST:
		serr = "(8) Access denied due to prohibited access";
		break;
	case FR_INVALID_OBJECT:
		serr = "(9) The file/directory object is invalid";
		break;
	case FR_WRITE_PROTECTED:
		serr = "(10) The physical drive is write protected";
		break;
	case FR_INVALID_DRIVE:
		serr = "(11) The logical drive number is invalid";
		break;
	case FR_NOT_ENABLED:
		serr = "(12) The volume has no work area";
		break;
	case FR_NO_FILESYSTEM:
		serr = "(13) There is no valid FAT volume";
		break;
	case FR_MKFS_ABORTED:
		serr = "(14) The f_mkfs() aborted due to any parameter error";
		break;
	case FR_TIMEOUT:
		serr = "(15) Could not get a grant to access the volume within defined period";
		break;
	case FR_LOCKED:
		serr = "(16) The operation is rejected according to the file sharing policy";
		break;
	case FR_NOT_ENOUGH_CORE:
		serr = "(17) LFN working buffer could not be allocated";
		break;
	case FR_TOO_MANY_OPEN_FILES:
		serr = "(18) Number of open files > _FS_SHARE";
		break;
	case FR_INVALID_PARAMETER:
		serr = "(19) Given parameter is invalid";
		break;
	default:
		serr = "No error number";
		break;
	}

	return serr;
}
