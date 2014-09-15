/**
  ******************************************************************************
  * @file    IAP/src/ymodem.c 
  * @author  MCD Application Team
  * @version V3.1.0
  * @date    07/27/2009
  * @brief   This file provides all the software functions related to the ymodem 
  *          protocol.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/** @addtogroup IAP
  * @{
  */ 
  
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "ymodem.h"
#include "raw_api.h"

#include "bsp.h"
#include "debug_uart.h"
#include "ff.h"

/*
移植三个函数
void UARTySendByte(uint8_t SendData)
{       
}

void SerialPutString(uint8_t* SendData)
{        
}

void UARTySendBytes(uint8_t* SendData,uint16_t len)
{       
}

调用Size = Ymodem_Receive(&tab_1024[0]);
*/



/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
static void ymemcpy(uint8_t* dest,uint8_t* source,uint16_t len)
{
	while(len-- != 0)
	{
	   *dest++ = *source++;
	}
}
/**
  * @brief  Receive byte from sender
  * @param  c: Character
  * @param  timeout: Timeout
  * @retval 0: Byte received
  *         -1: Timeout
  */
static inline int Receive(void *c, int len, int timeout)
{
	return debug_read_noblock(c, len, timeout/100);
}

/**
  * @brief  Send a byte
  * @param  c: Character
  * @retval 0: Byte sent
  */
static inline int Send_Byte (unsigned char c)
{
	return debug_write(&c, sizeof(c));
}

/**
  * @brief  Receive a packet from sender
  * @param  data
  * @param  length
  * @param  timeout
  *     0: end of transmission
  *    -1: abort by sender
  *    >0: packet length
  * @retval 0: normally return
  *        -1: timeout or packet error
  *         1: abort by user
  */
static int Receive_Packet (unsigned char *data, int *length, unsigned int timeout)
{
	unsigned int i, packet_size;
	unsigned char c;
	*length = 0;
	
	if (Receive(&c, sizeof(c), timeout) == 0)
	{
		return -1;
	}
	
	switch (c)
	{
	case SOH:
		packet_size = PACKET_SIZE;
		break;
	case STX:
#if 0
		packet_size = PACKET_1K_SIZE;
	break;
#else
		return -1;
#endif
	case EOT:
		return 0;
	case CA:
		if ((Receive(&c, sizeof(c), timeout) > 0) && (c == CA))
		{
			*length = -1;
			return 0;
		}
		else
		{
			return -1;
		}
	case ABORT1:
	case ABORT2:
		return 1;
	default:
		return -1;
	}
	*data = c;
	for (i = 1; i < (packet_size + PACKET_OVERHEAD); i ++)
	{
		if (Receive(data + i, 1, timeout) == 0)
		{
			return -1;
		}
	}
	if (data[PACKET_SEQNO_INDEX] != ((data[PACKET_SEQNO_COMP_INDEX] ^ 0xff) & 0xff))
	{
		return -1;
	}
	*length = packet_size;
	return 0;
}

/**
  * @brief  Receive a file using the ymodem protocol
  * @param  buf: Address of the first byte
  * @retval The size of the file
  */
int ymodem_recv_to_buf(char *file_name, unsigned char *buf, int buf_len)
{
#if 0
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
#else
	unsigned char packet_data[PACKET_SIZE + PACKET_OVERHEAD];
#endif
	unsigned char file_size[FILE_SIZE_LENGTH];
	unsigned char *file_ptr, *buf_ptr;
	int packet_length, session_done, file_done, packets_received, errors, session_begin, size = 0;
	int i;

	for (session_done = 0, errors = 0, session_begin = 0; ;)
	{
		for (packets_received = 0, file_done = 0, buf_ptr = buf; ;)
		{
			switch( Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT) )
			{
			case 0:
				errors = 0;
				switch (packet_length)
				{
				/* Abort by sender */
				case -1:
					Send_Byte(ACK);
					return 0;
				/* End of transmission */
				case 0:
					Send_Byte(ACK);
					file_done = 1;
					break;
				/* Normal packet */
				default:
					if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))//
					{
						Send_Byte(NAK);
					}
					else
					{
						if (packets_received == 0)
						{	/* Filename packet */
							if (packet_data[PACKET_HEADER] != 0)
							{	/* Filename packet has valid data */
								for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
								{
									file_name[i++] = *file_ptr++;
								}
								file_name[i++] = '\0';
								for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
								{
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								size = atoi((char *)file_size);
								/* Test the size of the image to be sent */
								/* Image size is greater than Flash size */
								if (size > (buf_len - 1))
								{
									/* End session */
									Send_Byte(CA);
									Send_Byte(CA);
									return -1;
								}

								Send_Byte(ACK);
								Send_Byte(CRC16);
							}
							/* Filename packet is empty, end session */
							else
							{
								Send_Byte(ACK);
								file_done = 1;
								session_done = 1;
								break;
							}
						}
						/* Data packet */
						else
						{
							ymemcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
							buf_ptr += packet_length;
							Send_Byte(ACK);
						}
						packets_received ++;
						session_begin = 1;
					}
				}
				break;
			case 1:
				Send_Byte(CA);
				Send_Byte(CA);
				return -3;
			default:
				if (session_begin > 0)
				{
					errors ++;
				}
				if (errors > MAX_ERRORS)
				{
					Send_Byte(CA);
					Send_Byte(CA);
					return 0;
				}
				Send_Byte(CRC16);
				break;
			}
		  
			if (file_done != 0)
			{
				break;
			}
		}
	
		if (session_done != 0)
		{
			break;
		}
	}

	return (int)size;
}

static void buf2file(const char *filename, void *file_buf, int file_size)
{
	FIL fp;
	FRESULT fret;
	unsigned int len;
	char *str = file_buf;
	fret = f_open(&fp, filename, FA_OPEN_ALWAYS|FA_WRITE);
	if(FR_OK != fret)
	{
//		raw_printf("open file error\n");
//		raw_printf("%s\n", fatfs_err2str(fret));
		return;
	}
	fret = f_write(&fp, str, file_size, &len);
	if(FR_OK != fret)
	{
//		raw_printf("write file error\n");
//		raw_printf("%s\n", fatfs_err2str(fret));
		f_close(&fp);
		return;	
	}
//	raw_printf("write file OK\n");

	f_close(&fp);
}

#define FREE(buf) 	do{ if(buf){ port_free(buf); buf=0; } }while(0)
int ymodem_recv_to_fatfs(void)
{
#if 0
	unsigned char packet_data[PACKET_1K_SIZE + PACKET_OVERHEAD];
#else
	unsigned char packet_data[PACKET_SIZE + PACKET_OVERHEAD];
#endif
	
	unsigned char file_size[FILE_SIZE_LENGTH];
	char file_name[FILE_NAME_LENGTH];
	unsigned char *buf = 0;
	
	unsigned char *file_ptr, *buf_ptr;
	int session_begin, session_done, errors, file_done;
	int packets_received, packet_length, size = 0;
	
	int i;
	for (session_done = 0, session_begin = 0, errors = 0; ! session_done; /* NULL */)
	{		
		for (packets_received = 0, file_done = 0; ! file_done; /* NULL */)
		{
			switch( Receive_Packet(packet_data, &packet_length, NAK_TIMEOUT) )
			{
			case 0:
				errors = 0;
				switch (packet_length)
				{
				/* Abort by sender */
				case -1:
					Send_Byte(ACK);
					FREE(buf);
					return 0;
				/* End of transmission */
				case 0:
					Send_Byte(ACK);
					buf2file(file_name, buf, size);
					FREE(buf);
					file_done = 1;
					break;
				/* Normal packet */
				default:
					if ((packet_data[PACKET_SEQNO_INDEX] & 0xff) != (packets_received & 0xff))//
					{
						Send_Byte(NAK);
					}
					else
					{
						if (packets_received == 0)
						{	/* Filename packet */
							if (packet_data[PACKET_HEADER] != 0)
							{	/* Filename packet has valid data */
								for (i = 0, file_ptr = packet_data + PACKET_HEADER; (*file_ptr != 0) && (i < FILE_NAME_LENGTH);)
								{
									file_name[i++] = *file_ptr++;
								}
								file_name[i++] = '\0';
								for (i = 0, file_ptr ++; (*file_ptr != ' ') && (i < FILE_SIZE_LENGTH);)
								{
									file_size[i++] = *file_ptr++;
								}
								file_size[i++] = '\0';
								size = atoi((char *)file_size);
								buf = port_malloc( (size+127) & ~127);
								/* Test the size of the image to be sent */
								/* Image size is greater than Flash size */
								//if (size > (buf_len - 1))
								if( !buf )
								{
									/* End session */
									Send_Byte(CA);
									Send_Byte(CA);
									return -1;
								}
								buf_ptr = buf;
								Send_Byte(ACK);
								Send_Byte(CRC16);
							}
							/* Filename packet is empty, end session */
							else
							{
								Send_Byte(ACK);
								file_done = 1;
								session_done = 1;
								break;
							}
						}
						/* Data packet */
						else
						{
							ymemcpy(buf_ptr, packet_data + PACKET_HEADER, packet_length);
							buf_ptr += packet_length;
							Send_Byte(ACK);
						}
						packets_received ++;
						session_begin = 1;
					}
				}
				break;
			case 1:
				Send_Byte(CA);
				Send_Byte(CA);
				FREE(buf);
				return -3;
			default:
				if (session_begin > 0)
				{
					errors ++;
				}
				if (errors > MAX_ERRORS)
				{
					Send_Byte(CA);
					Send_Byte(CA);
					FREE(buf);
					return 0;
				}
				Send_Byte(CRC16);
				break;
			}
		}
	}

	FREE(buf);
	return (int)size;
}
