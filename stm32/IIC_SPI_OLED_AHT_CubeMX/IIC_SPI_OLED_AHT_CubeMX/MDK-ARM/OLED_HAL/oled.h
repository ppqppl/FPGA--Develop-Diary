/***************************************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
****************************************************************************************************/	
#ifndef __OLED_H
#define __OLED_H			  	 
#include "stm32f1xx_hal.h"
 
//--------------OLED��������---------------------
#define PAGE_SIZE    8
#define XLevelL		0x00
#define XLevelH		0x10
#define YLevel       0xB0
#define	Brightness	 0xFF 
#define WIDTH 	     128
#define HEIGHT 	     64	

//-------------д��������ݶ���-------------------
#define OLED_CMD     0	//д����
#define OLED_DATA    1	//д���� 
   						  
//-----------------OLED�˿ڶ���----------------
/*
#define OLED_CS   GPIO_PIN_11   //Ƭѡ�ź�           PB11
#define OLED_DC   GPIO_PIN_10   //����/��������ź�  PB10
#define OLED_RST  GPIO_PIN_12   //��λ�ź�           PB12
*/

// ���ã����壩RES��DC��CS ��ʹ�õ�����

// RES
#define OLED_RES_GPIO_Port GPIOA
#define OLED_RES_Pin	GPIO_PIN_6

// DC
#define	OLED_DC_GPIO_Port GPIOA
#define OLED_DC_Pin	GPIO_PIN_4

// CS
#define OLED_CS_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_3


//-----------------OLED�˿ڲ�������---------------- 
/*
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,OLED_CS)
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,OLED_CS)

#define OLED_DC_Clr()  GPIO_ResetBits(GPIOB,OLED_DC)
#define OLED_DC_Set()  GPIO_SetBits(GPIOB,OLED_DC)
 					   
#define OLED_RST_Clr()  GPIO_ResetBits(GPIOB,OLED_RST)
#define OLED_RST_Set()  GPIO_SetBits(GPIOB,OLED_RST)
*/

#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_RESET)//RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_GPIO_Port,OLED_RES_Pin,GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET)//DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET)

#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_RESET)//CS
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS_GPIO_Port,OLED_CS_Pin,GPIO_PIN_SET)

//OLED�����ú���
void OLED_WR_Byte(unsigned dat,unsigned cmd);     							   		    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Reset(void);
void OLED_Init_GPIO(void);	   							   		    
void OLED_Init(void);
void OLED_Set_Pixel(unsigned char x, unsigned char y,unsigned char color);
void OLED_Display(void);
void OLED_Clear(unsigned dat);  
#endif
