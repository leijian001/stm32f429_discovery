/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */


/* Definitions of physical drive number for each media */
#define RAM		0
#define USB		1


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

		res = RES_OK;
		break;

	case USB :

		res = RES_OK;
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

		res = RES_OK;
		break;

	case USB :

		res = RES_OK;
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

		res = RES_OK;
		break;

	case USB :

		res = RES_OK;
		break;
	
	default:
		res = RES_PARERR;
		break;
	}
	
	return res;
}
#endif

DWORD get_fattime (void)
{

    /* Pack date and time into a DWORD variable */
    return 0;
}

#include "ff.h"
#include "bsp.h"

int ff_cre_syncobj (BYTE vol, _SYNC_t* sobj)	/* Create a sync object */
{
	RAW_ASSERT(0 != sobj);
	
	*sobj = port_malloc(sizeof(RAW_MUTEX));
	if( ! *sobj )
	{
		return -1;
	}
	
	RAW_U16 ret;
	
	ret = raw_mutex_create(*sobj, (RAW_U8 *)"fatfs_mutex", RAW_MUTEX_INHERIT_POLICY, 0);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	return 0;
}
int ff_del_syncobj (_SYNC_t sobj)				/* Delete a sync object */
{
	RAW_ASSERT(0 != sobj);
	
	RAW_U16  ret;
	ret = raw_mutex_delete(sobj);
	RAW_ASSERT(RAW_SUCCESS == ret);
	
	port_free(sobj);
	
	return 0;
}
int ff_req_grant (_SYNC_t sobj)				/* Lock sync object */
{
	RAW_U16 ret;
	ret = raw_mutex_get(sobj, RAW_WAIT_FOREVER);
	if(RAW_SUCCESS != ret && RAW_MUTEX_OWNER_NESTED != ret)
	{
		return -1;
	}
	
	return 0;
}
void ff_rel_grant (_SYNC_t sobj)				/* Unlock sync object */
{
	RAW_U16 ret;
	ret = raw_mutex_put(sobj);
	RAW_ASSERT(RAW_SUCCESS == ret);
}
