/*********************************************************************
*          Portions COPYRIGHT 2013 STMicroelectronics                *
*          Portions SEGGER Microcontroller GmbH & Co. KG             *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_stm32f429i_discovery.c
Purpose     : Driver for STM32429I-Discovery board RevB

The part between 'DISPLAY CONFIGURATION START' and 'DISPLAY CONFIGURA-
TION END' can be used to configure the following for each layer:

- Color mode
- Layer size
- Layer orientation

Further the background color used on positions without a valid layer
can be set here.

---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"

#include "stm32f429i_discovery_lcd.h"

/*********************************************************************
*
*       Display configuration (to be modified)
*
**********************************************************************
*/


/*********************************************************************
*
*       Common
*/
#undef  LCD_SWAP_XY
//#undef  LCD_MIRROR_Y

#define LCD_SWAP_XY  		1
//#define LCD_MIRROR_Y 		1

#define NUM_BUFFERS 		1
#define NUM_VSCREENS 		1 	// Number of virtual screens to be used

#define XSIZE_PHYS 			LCD_PIXEL_WIDTH
#define YSIZE_PHYS 			LCD_PIXEL_HEIGHT

/*******************************************
*
*       Color mode definitions
*/
#define _CM_ARGB8888 1
#define _CM_RGB888   2
#define _CM_RGB565   3
#define _CM_ARGB1555 4
#define _CM_ARGB4444 5
#define _CM_L8       6
#define _CM_AL44     7
#define _CM_AL88     8

/*******************************************
*
*       Layer 0
*/
//
// Color mode layer 0
//
#define COLOR_MODE_0 _CM_RGB565
//
// Layer size
//
#define XSIZE_0 240
#define YSIZE_0 320

/*******************************************
*/

/*******************************************
*
*       Automatic selection of driver and color conversion
*/
#if   (COLOR_MODE_0 == _CM_ARGB8888)
#	define COLOR_CONVERSION_0 GUICC_M8888I
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_32
#elif (COLOR_MODE_0 == _CM_RGB888)
#define COLOR_CONVERSION_0 GUICC_M888
#define DISPLAY_DRIVER_0   GUIDRV_LIN_24
#elif (COLOR_MODE_0 == _CM_RGB565)
#	define COLOR_CONVERSION_0 GUICC_M565
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_ARGB1555)
#	define COLOR_CONVERSION_0 GUICC_M1555I
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_ARGB4444)
#	define COLOR_CONVERSION_0 GUICC_M4444I
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#elif (COLOR_MODE_0 == _CM_L8)
#	define COLOR_CONVERSION_0 GUICC_8666
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL44)
#	define COLOR_CONVERSION_0 GUICC_1616I
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_8
#elif (COLOR_MODE_0 == _CM_AL88)
#	define COLOR_CONVERSION_0 GUICC_88666I
#	define DISPLAY_DRIVER_0   GUIDRV_LIN_16
#else
#	error Illegal color mode 0!
#endif

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static unsigned int VRAM_ADDR[] = {LCD_FRAME_BUFFER, LCD_FRAME_BUFFER + BUFFER_OFFSET};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/


/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       DMA2D_ISR_Handler
*
* Purpose:
*   Transfer-complete-interrupt of DMA2D
*/
void DMA2D_ISR_Handler(void)
{

}

/*********************************************************************
*
*       LTDC_ISR_Handler
*
* Purpose:
*   End-Of-Frame-Interrupt for managing multiple buffering
*/
void LTDC_ISR_Handler(void)
{

}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
	int r = -1;

	switch (Cmd)
	{
	case LCD_X_INITCONTROLLER:
		//
		// Called during the initialization process in order to set up the display controller and put it into operation.
		//
		LCD_Init();
		LCD_LayerInit();
		LCD_Clear(0xFFFF);
		LCD_SetLayer(LCD_FOREGROUND_LAYER);
		
		r = 0;
		break;

	case LCD_X_SETORG:
	{
		// 该函数用于虚拟屏幕。如果需要设置显示原点，则需调用该函数。典型反应是修改帧缓冲器起始地址。
		// Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
		//
//		LCD_X_SETORG_INFO *org = pData;
		
		r = -1;
		break;
	}

	case LCD_X_ON:
		//
		// Required if the display controller should support switching on and off
		//
		LCD_DisplayOn();
		LTDC_Cmd(ENABLE);
		r = 0;
		break;

	case LCD_X_OFF:
		//
		// Required if the display controller should support switching on and off
		//
		LCD_DisplayOff();
		r = 0;
		break;

	case LCD_X_SETVIS:
		//
		// Required for setting the layer visibility which is passed in the 'OnOff' element of pData
		//
		break;

	case LCD_X_SETPOS:
		//
		// Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
		//
		break;

	case LCD_X_SETSIZE:
		//
		// Required for setting the layer position which is passed in the 'xPos' and 'yPos' element of pData
		//
		break;

	default:
		r = -1;
	}
	
	return r;
}

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{
	int i;

	//
	// At first initialize use of multiple buffers on demand
	//
#if (NUM_BUFFERS > 1)
	for (i = 0; i < GUI_NUM_LAYERS; i++)
	{
		GUI_MULTIBUF_ConfigEx(i, NUM_BUFFERS);
	}
#endif

	// 选择显示驱动和颜色转换程序
	// Set display driver and color conversion for 1st layer
	//
	GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER_0, COLOR_CONVERSION_0, 0, 0);

	// 设置大小
	// Set size of 1st layer
	//
	LCD_SetSizeEx (0, XSIZE_0, YSIZE_0);
	LCD_SetVSizeEx(0, XSIZE_0, YSIZE_0 * NUM_VSCREENS);

	//
	// Setting up VRam address and custom functions for CopyBuffer-, CopyRect- and FillRect operations
	//
	for (i = 0; i < GUI_NUM_LAYERS; i++) 
	{
		LCD_SetVRAMAddrEx(i, (void *)(VRAM_ADDR[1]));	
	}
}

/*************************** End of file ****************************/
