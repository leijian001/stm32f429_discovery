#include <raw_api.h>


/*
*****************************************************************
*
* 4 byte user memcpy
* if src and dst are not 4 bytes aligned, this function can not save time.
*
*****************************************************************
*/
RAW_VOID *raw_memcpy(RAW_VOID *dest, const RAW_VOID *src, RAW_U32 count)
{
	RAW_U8 *dest_ptr = (RAW_U8 *)dest;
	RAW_U8 *src_ptr = (RAW_U8 *)src;
	
	RAW_U32  *aligned_dest;
	RAW_U32  *aligned_src;

	/*if copy size is bigger than 16 bytes and both src and dst are 4 bytes aligned*/
	if ((count >= 16u) && (((RAW_U32)src & 3u) == 0u)  && (((RAW_U32)dest & 3u) == 0u))
	{
		aligned_dest = (RAW_U32 *)dest_ptr;
		aligned_src = (RAW_U32 *)src_ptr;

		while (count >= 16u) 
		{
			*aligned_dest++ = *aligned_src++;
			*aligned_dest++ = *aligned_src++;
			*aligned_dest++ = *aligned_src++;
			*aligned_dest++ = *aligned_src++;
			count -= 16u;
		}

		while (count >= 4)
		{		
			*aligned_dest++ = *aligned_src++;
			count -= 4;
		}

		dest_ptr = (RAW_U8 *)aligned_dest;
		src_ptr = (RAW_U8 *)aligned_src;
	}

	while (count--) 
	{
		*dest_ptr++ = *src_ptr++;
	}

	return dest;
}



/*
*****************************************************************
*
* 4 byte user memset
* if src are not 4 bytes aligned, this function can not save time.
*  
*
*****************************************************************
*/
RAW_VOID *raw_memset(RAW_VOID *src, RAW_U8 byte, RAW_U32 count)
{

	RAW_U32 *aligned_temp_addr;
	RAW_U8 *temp_addr;
	RAW_U32 temp_word;

	temp_addr = src;

	if ((count >= 4u) && (((RAW_U32)src & 3u) == 0u)) {
	
		aligned_temp_addr = (RAW_U32 *)src;

		/*make a 4 bytes word*/
		temp_word  =  (byte << 8u) | byte;
		temp_word |= (temp_word << 16u);
	
		while (count >= 16u) {
	
			*aligned_temp_addr++ = temp_word;
			*aligned_temp_addr++ = temp_word;
			*aligned_temp_addr++ = temp_word;
			*aligned_temp_addr++ = temp_word;
			count -= 16u;
		}

		while (count >= 4u) {
		
			*aligned_temp_addr++ = temp_word;
			count -= 4u;
		}

	
		temp_addr = (RAW_U8 *)aligned_temp_addr;
	}

	while (count--) {
		
		*temp_addr++ = byte;
	}

	return src;


}



