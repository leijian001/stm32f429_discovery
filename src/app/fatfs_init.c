#include "raw_api.h"
#include "ff.h"
#include "diskio.h"

static FATFS _FS;

void fatfs_init(unsigned char prio)
{
	FRESULT fret;
	
	raw_printf("FatFs init...\r\t\t\t\t");
	disk_initialize(0);
	fret = f_mount(&_FS, _T("0"), 0);
	if(FR_OK != fret)
	{
		RAW_ASSERT(0);
	}
	raw_printf("[OK]\n");
	
//	if( need_format(0) )
	if(1)
	{
		raw_printf("mkfs ...\r\t\t\t\t");
		fret = f_mkfs(_T("0:"), 0, 512);
		if(FR_OK != fret)
		{
			RAW_ASSERT(0);
		}
		raw_printf("[OK]\n");
	}
}
