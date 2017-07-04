/*
******************************************************************************
* Copyright (c) 2005,��������ͨ�ż������������ڣ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�pdu.h
* ��Ŀ��ʶ��MPIP-618
* ժ    Ҫ��PDU����룬�����շ�����
* 
* ��ǰ�汾��1.0.0
* ��    �ߣ�Ĳ��Ȫ
* ������ڣ�2005��12��30��
******************************************************************************
*/


/*
*********************************�޸���־*************************************

******************************************************************************
*/

#ifndef _COMMON_PDU_H
#define _COMMON_PDU_H

#include "CBasicTools.h"
#include "Flash.h"

/*
******************************************************************************
						��������
******************************************************************************
*/
#ifndef GB2UNICODEADDR
#define GB2UNICODEADDR	FLASH_ADDR_WDLIB_GB2UNICODE //0x32000		//SIZE:7614*2;4BLOCKS.
#endif

#ifndef UNICODE2GBADDR
#define UNICODE2GBADDR	FLASH_ADDR_WDLIB_UNICODE2GB//0x36000		//SIZE:64K*2=128k,64blocks;
#endif

#define GSM_8BIT		0
#define GSM_7BIT		1
#define GSM_UCS2		2


/*
******************************************************************************
								�ӿں���ԭ��
******************************************************************************
*/
s16 Ucs2Encode(u8 *pDst, u8 *pSrc, s16 iSrcLen);
s16 Ucs2Decode( u8  *pDst, u8  *pSrc, s16 iSrcLen );
s16 PDUEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS );
s16 PDUDecode( u8  *pSCA, u8  *pDA, u8 *pTime, u8  *pUD, s16 *pDCS, u8  *pPdu, s16 iPduLen );

#if 1//def PERIPHERAL_NAVIGATE
s16 USC2PDUEncode(u8 *pPdu, u8 *pSCA, u8 *pDA, u8 *pUD, s16 iUDL, s16 iDCS);
#endif

s16 ASCIIUSCDecode(u8 *pDA, u8 *pTime, u8 *pUD, u8 *pPdu, s16 iPduLen);
s16 PDU2USCDecode(u8 *pSCA, u8 *pDA, u8 *pTime, u8 *pUD, s16 *pDCS, u8 *pPdu, s16 iPduLen);
s16 USCEncode3(u8 *dest,u8 *src,s16 len);
s16 UcsDecode3(u8 *pDst, u8 *pSrc, s16 iSrcLen); 

#endif	/* _COMMON_PDU_H */


