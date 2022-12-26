/*
 * FreeModbus Libary: BARE Demo Application
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "AHT20.h"

/* ----------------------- Defines ------------------------------------------*/
#define REG_INPUT_START 0	
#define REG_INPUT_NREGS 5

/* ----------------------- Static variables ---------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
//static USHORT   usRegInputBuf[REG_INPUT_NREGS];

uint16_t   usRegInputBuf[REG_INPUT_NREGS];
uint16_t InputBuff[5];

//����Ķ�����������ڻ�ȡ��ʪ��

uint32_t CT_data[2]={0,0};
volatile int  c1,t1,c2,t2,c3,t3;


/* ----------------------- Start implementation -----------------------------*/
/*int main( void )
{
    eMBErrorCode    eStatus;

    eStatus = eMBInit( MB_RTU, 0x0A, 0, 38400, MB_PAR_EVEN );

    eStatus = eMBEnable(  );

    for( ;; )
    {
        ( void )eMBPoll(  );

        usRegInputBuf[0]++;
    }
}*/

eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
	eMBErrorCode eStatus = MB_ENOERR;
	int iRegIndex;
	int i;
	
	AHT20_Read_CTdata_crc(CT_data);       //����CRCУ�飬��ȡAHT20���¶Ⱥ�ʪ������    �Ƽ�ÿ������1S��һ��
	c1 = CT_data[0]*1000/1024/1024;  //����õ�ʪ��ֵc1���Ŵ���10����
	t1 = CT_data[1]*2000/1024/1024-500;//����õ��¶�ֵt1���Ŵ���10����
	t2 = t1/10 + (t1/10)%10;	//�¶ȵ���������
	t3 = t1%10;	//�¶ȵ�С������
	c2 = c1/10 + (c1/10)%10;	// ʪ�ȵ���������
	c3 = c1%10;	//ʪ�ȵ�С������
	
	InputBuff[0] = t2;
	InputBuff[1] = t3;
	InputBuff[2] = c2;
	InputBuff[3] = c3;
	
	if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
	{
		iRegIndex = ( int )( usAddress - usRegInputStart );
		for(i=0;i<usNRegs;i++)
		{
			*pucRegBuffer=InputBuff[i+usAddress-1]>>8;
			pucRegBuffer++;
			*pucRegBuffer=InputBuff[i+usAddress-1]&0xff;
			pucRegBuffer++;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs,
                 eMBRegisterMode eMode )
{
    return MB_ENOREG;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    return MB_ENOREG;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    return MB_ENOREG;
}
