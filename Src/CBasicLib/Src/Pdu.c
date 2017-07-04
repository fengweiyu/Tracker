/*
******************************************************************************
* Copyright (c) 2005,��������ͨ�ż������������ڣ����޹�˾
* All rights reserved.
* 
* �ļ����ƣ�pdu.c
* ��Ŀ��ʶ��MPIP-618
* ժ    Ҫ��PDU����룬�����շ�����
* 
* ��ǰ�汾��3.7.2
* ��    �ߣ�Ĳ��Ȫ
* ������ڣ�2005��12��30��
******************************************************************************
*/

/*
******************************************************************************
******************************************************************************
*/

#define USE_PDU_MAP		0
#include "Pdu.h"
#include "Config.h"
#if USE_PDU_MAP
#include "PduMap.h"
#endif


/*
******************************************************************************
						�ֲ�����ʵ��
******************************************************************************
*/
/*
******************************************************************************
* �����������绰�����ʽת����
* ���������pPhone		�绰���롣
*           iPhoneLen	�绰���볤�ȡ�
* ���������pEncode	ת����İ�λ���ʽ��
* ���ز�����ת����İ�λ����ֽڳ��ȡ�
* ����˵�������룺pPhone="13851724908", iPhoneLen=11
*           �����pEncode="3158714209F8"
*           ���أ�14
* ����˵�������룺pPhone="13851724908F", iPhoneLen=12
*           �����pEncode="3158714209F8"
*           ���أ�14
* ע�����iPhoneLen ����С��20�ֽڡ�
******************************************************************************
*/
static s16 PhoneEncode( u8  *pEncode, u8  *pPhone, s16 iPhoneLen ) 
{
	s16 iLen;
	
	if ( iPhoneLen > 20 )
	{
		return -1;
	}

	for ( iLen = 0; iLen < iPhoneLen; iLen++ )
	{
		if ( ( iLen & 1 ) == 1 ) 
		{
			pEncode[ iLen - 1 ] = pPhone[ iLen ];
		} 
		else
		{
			pEncode[ iLen + 1 ] = pPhone[ iLen ];
		}
	}

	if (( iLen & 1 ) == 1)
	{
		pEncode[ iLen - 1 ] = 'F';
		pEncode[ iLen + 1 ] = '\0';
	}
	else
	{
		pEncode[ iLen ] = '\0';
	}

	return iLen;
}

/*
******************************************************************************
* �����������绰�����ʽת����
* ���������pDecode	ת��ǰ�İ�λ�顣
*           iDecodeLen	��λ����ֽڳ��ȡ�
* ���������pPhone		ת����ĵ绰���롣
* ���ز�����ת����绰������ֽڳ��ȡ�
* ����˵�������룺pDecode="3158714209F8"  iDecodeLen= 12
*           �����pPhone="13851724908"
*           ���أ�11
******************************************************************************
*/
static s16 PhoneDecode( u8  *pPhone, u8  *pDecode, s16 iDecodeLen )
{
	s16  iLen;
	
	if ( iDecodeLen > 22 )
	{
		return -1;
	}

	for ( iLen = 0; iLen < iDecodeLen; iLen++ ) 
	{
		if ( ( iLen & 1 ) == 1 ) 
		{
			pPhone[ iLen - 1 ] = pDecode[ iLen ];
		} 
		else 
		{
			pPhone[ iLen + 1 ] = pDecode[ iLen ];
		}
	}

	if ( pPhone[ iLen - 1 ] == 'F' ) 
	{
		iLen--;
	}

	pPhone[ iLen ] = '\0';

	return iLen;
}

/*
******************************************************************************
* ����������ASCII�������Ƶ�ת����
* ���������pStr	  ASCII�����ݡ�
* ���������pByte	  ���������ݡ�
* ���ز������ɹ�����0��ʧ�ܷ���-1��
* ����˵�������룺pAscii="38"
*           �����pByte=0x38
*           ���أ�0��
******************************************************************************
*/
static s16 Str2Byte( u8  *pByte, u8  *pStr )
{
	if (( *pStr >= '0' ) && ( *pStr <= '9')) 
	{
		*pByte = ( *pStr - '0' ) << 4;
	}
	else if (( *pStr >= 'A' ) && ( *pStr <= 'F')) 
	{
		*pByte = ( *pStr - 'A' + 10 ) << 4 ;
	} 
	else if (( *pStr >= 'a' ) && ( *pStr <= 'f')) 
	{
		*pByte = ( *pStr - 'a' + 10 ) << 4;
	}
	else 
	{
		return -1;
	}

	pStr++;

	if (( *pStr >= '0' ) && ( *pStr <= '9')) 
	{
		*pByte |= *pStr - '0';
	} 
	else if (( *pStr >= 'A' ) && ( *pStr <= 'F'))
	{
		*pByte |= *pStr - 'A' + 10;
	} 
	else if (( *pStr >= 'a' ) && ( *pStr <= 'f')) 
	{
		*pByte |= *pStr - 'a' + 10;
	} 
	else 
	{
		return -1;
	}

	return 0;
}

/*
 ******************************************************************************
 * ���������������Ƶ�ASCII��ת����
 * ���������bByte	  ���������ݡ�
 * ���������pStr	  ASCII�����ݡ�
 * ���ز������ɹ�����0��ʧ�ܷ���-1��
 * ����˵�������룺bByte=0x38
 *           �����pStr ="38"
 *           ���أ�0��
 ******************************************************************************
 */
static s16 Byte2Str( u8  *pStr, u8 bByte )
{
	const u8 Byte2Hex[] = "0123456789ABCDEF";

	*(pStr++) = Byte2Hex[ bByte >> 4  ];
	*(pStr++) = Byte2Hex[ bByte & 0xF ];

	return 0;
}

/*
******************************************************************************
* ����������7bit�ı���ת����
* ���������pSrc	  ÿ���ֽ�7bit�����ݡ�
*           iSrcLen  pSrc���ֽڳ��ȡ�
* ���������pDst	  8bitASCII�����ݡ�
* ���ز������ɹ�����pDst���ֽڳ��ȣ� ʧ�ܷ���-1��
* ����˵�������룺pSrc="Hello World!", iSrcLen=11
*           �����pDst="C8329BFD065DDF72363904"
*           ���أ�22��
******************************************************************************
*/
static s16 Bit7Encode( u8  *pDst, u8  *pSrc, s16 iSrcLen )
{
	s16 iShift;
	s16 iLen;
	s16 iDstLen;
	u8 bTemp;

	const u8 Byte2Hex[] = "0123456789ABCDEF";
	
	for ( iLen = 0, iDstLen = 0; iLen < iSrcLen; iLen++ ) 
	{
		if ( ((*pSrc) & 0x80) != 0 ) 
		{
			return -1;
		}
		iShift = iLen & 7;
		if ( iShift != 7 ) 
		{
			/* 7bit to 8bit  */
			bTemp  = ( *pSrc      ) >> iShift;
			bTemp |= ( *(pSrc+1)  ) << ( 7 - iShift );
			/* 8bit to ASCII */
			*(pDst++) = Byte2Hex[ bTemp >> 4  ];
			*(pDst++) = Byte2Hex[ bTemp & 0xF ];
			iDstLen += 2;
		}
		pSrc++;
	}

	*pDst = '\0';

	return iDstLen;
}

/*
******************************************************************************
* ����������7bit�ı���ת����
* ���������pSrc	  ÿ���ֽ�8bit�����ݡ�
*           iSrcLen  pSrc���ֽڳ��ȡ�
* ���������pDst	  7bitASCII�����ݡ�
* ���ز������ɹ�����pDst���ֽڳ��ȣ�ʧ�ܷ���-1��
* ����˵�������룺pSrc="C8329BFD065DDF72363904", iSrcLen=22
*           �����pDst="Hello World!"
*           ���أ�11��
******************************************************************************
*/
static s16 Bit7Decode( u8  *pDst, u8  *pSrc, s16 iSrcLen )
{
	s16 iShift;
	s16 iLen;
	s16 iDstLen;
	u8 bTemp;
	
	/* MyPrintf( cpBuf, "\r\nlen: %d\r\n", iSrcLen ); */
	/* Trace( cpBuf ); */
	iSrcLen = iSrcLen >> 1;
	for ( iLen = 0, iDstLen = 0; iLen < iSrcLen;  )
	{
		*pDst = 0;
		for ( iShift = 0; ( iShift <= 6 ) && (iLen < iSrcLen ); iShift++ ) 
		{
			/* ASCII to byte */
			if ( Str2Byte( &bTemp, pSrc ) != 0 ) 
			{
				return -1;
			}
			/* 8bit to 7bit  */
			*pDst++ |= ( bTemp << iShift ) & 0x7F;
			*pDst    = bTemp >> ( 7 - iShift );
			pSrc += 2;
			if ( iShift == 6 )
			{
				pDst++;
				iDstLen++;
			}
			iLen++;
		}
		iDstLen += iShift;
	}

	*pDst = '\0';

	return iDstLen;
}

/*
******************************************************************************
* ����������8bit�ı���ת����
* ���������pSrc	  ���������ݡ�
*           iSrcLen  pSrc���ֽڳ��ȡ�
* ���������pDst	  ʮ������ASCII�����ݡ�
* ���ز������ɹ�����pDst���ֽڳ��ȣ�ʧ�ܷ���-1��
* ����˵�������룺pSrc={ 0,1,2,3,4,5,6,7,8,9,0xF0,0xFF ), iSrcLen=12
*           �����pDst="00010203040506070809F0FF"
*           ���أ�24��
******************************************************************************
*/
static s16 Bit8Encode( u8  *pDst, u8  *pSrc, s16 iSrcLen )
{
	s16 iDstLen;
	const u8 Byte2Hex[] = "0123456789ABCDEF";
	
	for ( iDstLen = 0; iDstLen < iSrcLen; iDstLen++ ) 
	{
		*pDst++ = Byte2Hex[ (*pSrc) >> 4  ];
		*pDst++ = Byte2Hex[ (*pSrc) & 0xF ];
		pSrc++;
	}

	*pDst = '\0';
	
	return iDstLen * 2;
}

/*
******************************************************************************
* ����������8bit�Ľ�����ת����
* ���������pSrc	  ʮ������ASCII�����ݡ�
*           iSrcLen  pSrc���ֽڳ��ȡ�
* ���������pDst	  ���������ݡ�
* ���ز������ɹ�����pDst���ֽڳ��ȣ�ʧ�ܷ���-1��
* ����˵�������룺pSrc="00010203040506070809F0FF", iSrcLen=24
*           �����pDst={ 0,1,2,3,4,5,6,7,8,9,0xF0,0xFF )
*           ���أ�12��
******************************************************************************
*/
static s16 Bit8Decode( u8  *pDst, u8  *pSrc, s16 iSrcLen )
{
	s16 iDstLen;
	
	iSrcLen = iSrcLen >> 1;

	for ( iDstLen = 0; iDstLen < iSrcLen; iDstLen++ ) 
	{
		if ( Str2Byte( pDst, pSrc ) != 0 )
		{
			return -1;
		}
		pDst++;
		pSrc += 2;
	}

	*pDst = '\0';

	return iDstLen;
}

/*
******************************************************************************
* ����������GB2312 �ı���ת��.
* ���������pSrc: GB2312 ���������.
* ���������iSrcLen:pSrc ���ֽڳ���.
* ���������pDst: Unicode �����ASCII ����.
* ���ز������ɹ�����pDst ���ֽڳ���, ʧ�ܷ���-1
* ����˵��������pSrc=, iSrcLen=
* ���������pDst=
* ���ز�����
******************************************************************************
*/
s16 Ucs2Encode(u8 *pDst, u8 *pSrc, s16 iSrcLen)
{
	const u8 Byte2Hex[] = "0123456789ABCDEF";
	u32 dwIndex;
	u8 *pUcs2;
	s16 iDstLen;
	u8 bHigh;
	u8 bLow;
	u8 gb2uTable[2];
	
	for(iDstLen = 0; iSrcLen > 0; ) 
	{
		/* GB2312 to Unicode */
		bHigh = *pSrc;
		bLow  = *(pSrc + 1);
		pUcs2 = NULL;
		if((bLow >= 0xa1) && (bLow <= 0xfe)) 
		{
			if((bHigh >= 0xa1) && (bHigh <= 0xa9))
			{
				dwIndex = ((bHigh - 0xa1) * 94 + (bLow - 0xa1)) * 2;
				FLASH_ReadDatas(GB2UNICODEADDR + dwIndex, gb2uTable, 2);
				pUcs2   = gb2uTable;
			} 
			else if((bHigh >= 0xb0) && (bHigh <= 0xf7))
			{
				dwIndex = ((bHigh - 0xb0 + 9) * 94 + (bLow - 0xa1)) * 2;
				FLASH_ReadDatas(GB2UNICODEADDR + dwIndex, gb2uTable, 2);
				pUcs2   = gb2uTable;
			}
		}
		if(pUcs2 == NULL)
		{
			bHigh = '\0';
			bLow  = *pSrc;
			pSrc += 1;
			iSrcLen -= 1;
		} 
		else 
		{
			bHigh = *pUcs2++;
			bLow  = *pUcs2;
			pSrc += 2;
			iSrcLen -= 2;
			if(iSrcLen < 0) 
			{
				return -1;
			}
		}
		
		/* Unicode to ASCII	 */
		*pDst++ = Byte2Hex[ bHigh >> 4  ];
		*pDst++ = Byte2Hex[ bHigh & 0xF ];
		*pDst++ = Byte2Hex[ bLow  >> 4  ];
		*pDst++ = Byte2Hex[ bLow  & 0xF ];
		iDstLen += 4;
	}

	*pDst = '\0';

	return iDstLen;
}

/*
******************************************************************************
* ����������GB2312 �Ľ���ת��.
* ���������pSrc: Unicode �����ASCII ����.
* ���������iSrcLen: pSrc ���ֽڳ���.
* ���������pDst: GB2312 ���������
* ���ز������ɹ�����pDst ���ֽڳ���, ʧ�ܷ���-1
* ����˵��������pSrc=, iSrcLen=
* ���������pDst=
* ���ز�����
******************************************************************************
*/
s16 Ucs2Decode(u8 *pDst, u8 *pSrc, s16 iSrcLen)
{
	u32 dwIndex;
	s16 iDstLen;
	u8 bHigh;
	u8 bLow;
	u8 u2gbTable[2];
	
	for(iDstLen = 0; iSrcLen > 0;)
	{
		/* ASCII to Unicode */
		if(Str2Byte(&bHigh, pSrc) != 0)
		{
			return -1;
		}
		
		pSrc += 2;
		
		if(Str2Byte(&bLow,  pSrc) != 0)
		{
			return -1;
		}

		pSrc += 2;

		if((iSrcLen -= 4) < 0)
		{
			return -1;
		}

		/* Unicode to GB2312 */
		if(bHigh == 0) 
		{
			*pDst++ = bLow;
			iDstLen++;
		} 
		else
		{
			dwIndex = ((((u32)bHigh) << 8) + bLow) * 2;
			FLASH_ReadDatas(UNICODE2GBADDR + dwIndex, u2gbTable, 2);
			*pDst++ = u2gbTable[0];
			*pDst++ = u2gbTable[1];
			iDstLen += 2;
		}
	}

	*pDst = '\0';

	return iDstLen;
}

#if 0
/***************************************************************
						�ӿں���ʵ��
****************************************************************/
/*
 ******************************************************************************
 * ����������PDU�Բ����
 * �����������
 * �����������
 * ���ز�������
 ******************************************************************************
 */
s16 PduTest( void )
{
	u8 cBuf[] = "����";
	u8  cBuf2[20];
	u8  cBuf3[50];
	s16  iTemp;
	s16 iTemp1;
	u16 uTemp;
	u8  *gb2uTable;
	u32	dwAddr;
	
	Trace( "PduTest\r\n" );
	gb2uTable = (u8  *)GB2UNICODEADDR;

	for ( iTemp = 0; iTemp < strlen(cBuf); iTemp++ ) 
	{
		Byte2Str( &cBuf3[ iTemp * 2 ], cBuf[ iTemp ] );
	}
	cBuf3[ iTemp * 2 ] = '\0';
	Trace( (u8  *)cBuf3 );
	Trace( "\r\n" );
	if ( Ucs2Encode( cBuf2, cBuf, strlen(cBuf) ) > 0 ) 
	{
		Trace( cBuf2 );
		Trace( "\r\n" );
	}
	memset( cBuf, 0, 20 );
	if ( Ucs2Decode( cBuf, cBuf2, strlen(cBuf2) ) > 0 )
	{
		for ( iTemp = 0; iTemp < strlen(cBuf); iTemp++ )
		{
			Byte2Str( &cBuf3[ iTemp * 2 ], cBuf[ iTemp ] );
		}
		cBuf3[ iTemp * 2 ] = '\0';
		Trace( (u8  *)cBuf3 );
		Trace( "\r\n" );
	}
	Trace( "Finish\r\n" );

	return 0;
}
#endif
/*
���ͷ�PDU��ʽ
----------------------------------------------------------------
| SCA  | PDUType | MR | DA   | PID | DCS | VP    | UDL | UD    |
|---------------------------------------------------------------
| 1-12 | 1       | 1  | 2-12 | 1   | 1   | 0,1,7 | 1   | 0-140 |
----------------------------------------------------------------
*/
s16 PDUEncode( u8  *pPdu, u8  *pSCA, u8  *pDA, u8  *pUD, s16 iUDL, s16 iDCS )
{
	s16 iPduLen = 0;
	s16 iTemp;
	
	/* SCA, �������ĵĵ绰���� 	*/
	if(pSCA == NULL)
	{
		strcpy((s8*)pPdu, "00");
		iPduLen = strlen((s8*)pPdu);
	} 
	else 
	{
		if(*pSCA == '+') 	/* ����ͨ�õĹ��ʺ���ǰ׺��*/
		{
			pSCA++;
			Byte2Str( pPdu + 2, 0x91 );
		} 
		else if(memcmp(pSCA, "00", 2) == 0) 	/* ����ͨ�õĹ��ʺ���ǰ׺��*/
		{
			pSCA +=2;
			Byte2Str(pPdu + 2, 0x91);
		} 
		else
		{
			Byte2Str(pPdu + 2, 0x81);
		}

		iPduLen = PhoneEncode(pPdu + 4, pSCA, strlen((s8*)pSCA));
		Byte2Str(pPdu, (iPduLen + 1)/2);

		if(iPduLen & 1)
		{
			iPduLen++;
		}
		iPduLen += 4;
	}

	/*  PDUType,	Э�����ݵ�Ԫ����  */
	Byte2Str(pPdu + iPduLen, 0x11);
	iPduLen += 2;

	/* MR ���гɹ���SMS-SUBMIT�ο���Ŀ */
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* DA ���շ�SME�ĵ�ַ*/
	if(*pDA == '+')	/* ����ͨ�õĹ��ʺ���ǰ׺��*/
	{
		pDA++;
		Byte2Str(pPdu + iPduLen + 2, 0x91);
	}
	else if(memcmp(pDA, "00", 2) == 0)  /* ����ͨ�õĹ��ʺ���ǰ׺��*/
	{
		pDA += 2;
		Byte2Str(pPdu + iPduLen + 2, 0x91);
	}
	else
	{
		Byte2Str(pPdu + iPduLen + 2, 0x81);
	}

	iTemp = PhoneEncode(pPdu + iPduLen + 4, pDA, strlen((s8*)pDA));
	Byte2Str(pPdu + iPduLen, iTemp);
	iPduLen += iTemp + 4;

	if(iTemp & 1)
	{
		iPduLen++;
	}

	/* PID Protocol Identifier, SMSC�Ժ��ַ�ʽ����SM 			*/
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* DCS ��ʾ�û�����(UD)����ʲô���뷽��						*/
	Byte2Str(pPdu + iPduLen, iDCS);
	iPduLen += 2;

	/* VP Validity Period ������ʾ��Ϣ��SMSC�в�����Ч��ʱ��	*/
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* UDL, UD �û����ݶγ���, �û����� 						*/
	if(iDCS == 0) 
	{
		iTemp = Bit7Encode(pPdu + iPduLen + 2, (u8*)pUD, iUDL);
		if(iTemp < 0) 
		{
			return -1;
		}
		Byte2Str(pPdu + iPduLen, iUDL);
	} 
	else if(iDCS == 4) 
	{
		iTemp = Bit8Encode(pPdu + iPduLen + 2, pUD, iUDL);
		if(iTemp < 0)
		{
			return -1;
		}
		Byte2Str(pPdu + iPduLen, iTemp/2);
	} 
	else if(iDCS == 0x0a) 
	{
		//SystemInfo("Pdu data:", (s8*)pUD, iUDL, 1);
		iTemp = iUDL;
		memcpy(pPdu + iPduLen + 2, pUD, iUDL);
		iTemp = Hex_2_Ascii((s8 *)pUD, (s8 *)pPdu + iPduLen + 2, iUDL);
		Byte2Str(pPdu + iPduLen, iTemp/2);
	} 
	else
	{
		iTemp = Ucs2Encode(pPdu + iPduLen + 2, pUD, iUDL);
		if(iTemp < 0 )
		{
			return -1;
		}

		/*
		#ifdef MyPrint
		debugPrintf("Hand USC print:\r\n", sizeof("Hand USC print:\r\n"));
		debugPrintf(pPdu + iPduLen + 2, iTemp);
		debugPrintf("\r\n", sizeof("\r\n"));
		#endif
		*/
		
		Byte2Str(pPdu + iPduLen, iTemp/2);
	}

	iPduLen += 2 + iTemp;
	*(pPdu + iPduLen) = '\0';

	return(iPduLen);
}

/*
******************************************************************************
* ���շ�PDU��ʽ:
* ----------------------------------------------------------
* | SCA  | PDUType | OA 	| PID | DCS | SCTS | UDL | UD    |
* |---------------------------------------------------------
* | 1-12 | 1       | 2-12  | 1   |  1  | 7    |  1  | 0-140 |
* ----------------------------------------------------------
* ��ʽ˵����
* SCA: 		�������ĵĵ绰����
* PDUType:	Э�����ݵ�Ԫ����
* OA: 		Originator Adress ���ͷ�SME�ĵ�ַ
* PID: 		��ʾSMSC�Ժ��ַ�ʽ����SM
* DCS: 		��ʾ�û�����(UD)����ʲô���뷽��
* SCTS: 		������ʾSMSC���յ���Ϣʱ��ʱ���
* UDL:
* UD:
******************************************************************************
PDUDecode(NULL,									// ���ŷ������ĺ�
	           (u8 *)sms_buff,             					// �Է����� 			
	           NULL,									// ���ŷ���ʱ���	
	           (u8 *)(sms_buff+MAX_PHONE_STR_LEN-1), 	// �������� 				
	           NULL,        								// �Է����� 				
	           (u8 *)buff, 								// PDU����ʼ��ַ		
	           Size);									// PDU������				
******************************************************************************
*/
s16 PDUDecode(u8 *pSCA, u8 *pDA, u8 *pTime, u8 *pUD, s16 *pDCS, u8 *pPdu, s16 iPduLen)
{
	s16 iTemp;
	u8  bTemp;
	u8  bDCS;
	
	/* SCA, �������ĵĵ绰���� */
	if(Str2Byte(&bTemp, pPdu) != 0)
	{
		return -1;
	}
	pPdu += 2;

	if(bTemp == 0)
	{
		if(pSCA != NULL) 
		{
			*pSCA = '\0';
		}
	} 
	else
	{
		if(pSCA != NULL) 
		{
			if(memcmp(pPdu, "91", 2) == 0)
			{
				if(memcmp(pPdu + 2, "68", 2) == 0)
				{
					bTemp--;
					pPdu += 2;
				} 
				else
				{
					strcpy((s8 *)pSCA, "00");
					pSCA += 2;
				}
			}

			if(PhoneDecode(pSCA, pPdu + 2, bTemp * 2 - 2) <= 0)
			{
				return -1;
			}
		}
		
		pPdu += bTemp * 2;
	}

	/*  PDUType,	Э�����ݵ�Ԫ����*/
	pPdu += 2;

	/*  ���ͷ�SME �ĵ�ַ */
	if(Str2Byte(&bTemp, pPdu) != 0) return -1;
	pPdu += 2;

	if(bTemp & 1)
	{
		bTemp++;
	}
	if(pDA != NULL)
	{
		if(memcmp(pPdu, "91", 2) == 0)
		{
			if(memcmp(pPdu + 2, "68", 2) == 0)
			{
				bTemp -= 2;
				pPdu  += 2;
			} 
			else 
			{
				strcpy((s8 *)pDA, "00");
				pDA += 2;
			}
		}

		iTemp = PhoneDecode(pDA, pPdu + 2, bTemp);
		
		if(iTemp <= 0)
		{
			return -1;
		}
	}

	pPdu += bTemp + 2;
	
	/* PID Protocol Identifier, SMSC�Ժ��ַ�ʽ����SM 	*/
	pPdu += 2;

	/* DCS ��ʾ�û�����(UD)����ʲô���뷸	*/
	if(Str2Byte( &bDCS, pPdu) != 0) return -1;
	pPdu += 2;

	/* SCST ������ʾSMSC ���յ���Ϣʱ��ʱ���*/
	if(pTime != NULL)
	{
		iTemp = PhoneDecode(pTime, pPdu, 12);
	}
	pPdu += 14;
	
	/* UDL, UD �û����ݶγ���, �û����� */
	if(Str2Byte(&bTemp, pPdu) != 0) 
	{
		return -1;
	}
			
	if(bDCS == 0) 
	{
		if(memcmp(pPdu+2, "050003", strlen("050003")) == 0)
		{
			DebugPrintf("Multi sms package!\r\n");
			pPdu += 14;
			bTemp -= 7;
			bTemp /= 2;
			bTemp *= 2;
		}
		DebugPrintf("\r\n7bit decode\r\n");
		iTemp = Bit7Decode((u8 *)pUD, pPdu + 2, (bTemp - (bTemp / 8)) * 2);
		if((bTemp & 0x7) == 0x7) 
		{
			iTemp--;
			*((u8 *)pUD + iTemp ) = 0;
		}
	} 
	else if(bDCS == 4) 
	{
		DebugPrintf("\r\n8bit decode\r\n");
		iTemp = Bit8Decode(pUD, pPdu + 2, bTemp * 2);
	} 
	else
	{
		if(memcmp(pPdu+2, "050003", strlen("050003")) == 0)
		{
			*pUD++ = '(';
			*pUD++ = *(pPdu +2 +11);
			*pUD++ = '/';
			*pUD++ = *(pPdu +2 +9);
			*pUD++ = ')';
			
			pPdu += 12;
			bTemp -= 6;
			
			bDCS = 'M';
		}
		
		iTemp = Ucs2Decode(pUD, pPdu + 2, bTemp * 2);
		if(bDCS == 'M')
		{
			iTemp += 5;
		}
	}

	if(pDCS != NULL) 
	{
		*pDCS = bDCS;
	}
	
	return iTemp;
}

/*
******************************************************************************
* pDA,           	// ���ŷ��ͷ����� 			
* pTime,		// ���ŷ���ʱ���	
* pUD, 		// �������� 				
* pPdu, 		// PDU����ʼ��ַ		
* iPduLen;	// PDU������				
******************************************************************************
*//*
s16 PDUDecode(u8 *pNum, u8 *pTime, u8 *pUD, u8 *pPdu, s16 iPduLen)
{
	s16 iTemp;
	u8  bTemp;
	u8  bDCS;
	
	//����SCA  �������ĵĵ绰����
	if(Str2Byte(&bTemp, pPdu) != 0)
	{
		return -1;
	}
	pPdu += (bTemp*2) +2;

	//PDUType  Э�����ݵ�Ԫ����
	pPdu += 2;

	//���ͷ�SME  �ĵ�ַ
	if(Str2Byte(&bTemp, pPdu) != 0)
	{
		return -1;
	}
	pPdu += 2;
	if(bTemp & 0x01)
	{
		bTemp++;
	}
	if(pNum != NULL)
	{
		if(!memcmp(pPdu,"91",2) || !memcmp(pPdu,"A1",2) || !memcmp(pPdu,"81",2))
		{
			if(memcmp(pPdu + 2, "68", 2) == 0)
			{
				bTemp -= 2;
				pPdu  += 2;
			} 
		}
		if(PhoneDecode(pNum, pPdu + 2, bTemp) <= 0)
		{
			return -1;
		}
	}
	pPdu += (bTemp + 2);
	
	//PID Protocol Identifier, SMSC �Ժ��ַ�ʽ����
	pPdu += 2;

	//DCS ���ݱ��뷽ʽ
	if(Str2Byte(&bDCS, pPdu) != 0) return -1;
	pPdu += 2;

	//SMSC ���յ���Ϣʱ��ʱ���
	if(pTime != NULL)
	{
		PhoneDecode(pTime, pPdu, 12);
	}
	pPdu += 14;
	
	//UDL, UD �û����ݶγ���, �û�����
	if(Str2Byte(&bTemp, pPdu) != 0) 
	{
		return -1;
	}	
	if(bDCS == 0) 
	{
		if(memcmp(pPdu+2, "050003", strlen("050003")) == 0)
		{
			Trace("Multi sms package!\r\n");
			pPdu += 14;
			bTemp -= 7;
			bTemp /= 2;
			bTemp *= 2;
		}
		
		iTemp = Bit7Decode((u8 *)pUD, pPdu + 2, (bTemp - (bTemp / 8)) * 2);
		if((bTemp & 0x7) == 0x7) 
		{
			iTemp--;
			*((u8 *)pUD + iTemp ) = 0;
		}
	} 
	else if(bDCS == 4) 
	{
		iTemp = Bit8Decode(pUD, pPdu + 2, bTemp * 2);
	} 
	else
	{
		if(memcmp(pPdu+2, "050003", strlen("050003")) == 0)
		{
			*pUD++ = '(';
			*pUD++ = *(pPdu +2 +11);
			*pUD++ = '/';
			*pUD++ = *(pPdu +2 +9);
			*pUD++ = ')';
			
			pPdu += 12;
			bTemp -= 6;
			
			bDCS = 'M';
		}
		
		iTemp = Ucs2Decode(pUD, pPdu + 2, bTemp * 2);
		if(bDCS == 'M')
		{
			iTemp += 5;
		}
	}
	return(iTemp);
}*/

/*
GB2312(1980��)һ����¼��7445���ַ�������6763�����ֺ�682���������š�
�����������뷶Χ���ֽڴ�B0-F7�����ֽڴ�A1-FE��ռ�õ���λ��72*94=6768��
������5����λ��D7FA-D7FE
*/

#if 1//def PERIPHERAL_NAVIGATE
s16 USC2PDUEncode(u8 *pPdu, u8 *pSCA, u8 *pDA, u8 *pUD, s16 iUDL, s16 iDCS)
{	
	s16 iPduLen = 0;
	s16 iTemp;
	u32 i;
	u8 *ucs2_pt;
	
	/* SCA, �������ĵĵ绰���� 	*/
	if( pSCA == NULL )
	{
		strcpy( (s8 *)pPdu, "00" );
		iPduLen = strlen( (s8 *)pPdu );
	} 
	else 
	{
		if( *pSCA == '+' ) /* ����ͨ�õĹ��ʺ���ǰ׺*/
		{
			pSCA++;
			Byte2Str( pPdu + 2, 0x91 );
		} 
		else if( memcmp( pSCA, "00", 2 ) == 0 ) /* ����ͨ�õĹ��ʺ���ǰ׺��*/
		{
			pSCA +=2;
			Byte2Str( pPdu + 2, 0x91 );
		} 
		else
		{
			Byte2Str( pPdu + 2, 0x81 );
		}

		iPduLen = PhoneEncode( pPdu + 4, pSCA, strlen((s8 *)pSCA) );
		Byte2Str( pPdu, (iPduLen + 1)/2 );

		if ( iPduLen & 1 )
		{
			iPduLen++;
		}
		iPduLen += 4;
	}

	/*  PDUType,	Э�����ݵ�Ԫ����  */
	Byte2Str( pPdu + iPduLen, 0x11 );
	iPduLen += 2;

	/* MR ���гɹ���SMS-SUBMIT�ο���Ŀ */
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* DA ���շ�SME�ĵ�ַ				*/
	if( *pDA == '+' )                       /* ����ͨ�õĹ��ʺ���ǰ׺��*/
	{
		pDA++;
		Byte2Str( pPdu + iPduLen + 2, 0x91);
	}
	else if( memcmp( pDA, "00", 2 ) == 0 )  /* ����ͨ�õĹ��ʺ���ǰ׺��*/
	{
		pDA += 2;
		Byte2Str( pPdu + iPduLen + 2, 0x91 );
	}
	else
	{
		Byte2Str( pPdu + iPduLen + 2, 0x81 );
	}

	iTemp = PhoneEncode(pPdu + iPduLen + 4, pDA, strlen((s8 *)pDA));
	Byte2Str( pPdu + iPduLen, iTemp);
	iPduLen += iTemp + 4;

	if ( iTemp & 1 )
	{
		iPduLen++;
	}

	/* PID Protocol Identifier, SMSC�Ժ��ַ�ʽ����SM 			*/
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* DCS ��ʾ�û�����(UD)����ʲô���뷽��						*/
	Byte2Str( pPdu + iPduLen, iDCS );
	iPduLen += 2;

	/* VP Validity Period ������ʾ��Ϣ��SMSC�в�����Ч��ʱ��	*/
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* //UDL, UD �û����ݶγ���, �û����� 
	if ( iDCS == 0 ) 
	{
		iTemp = Bit7Encode( pPdu + iPduLen + 2, (u8 *)pUD, iUDL );
		if ( iTemp < 0 ) 
		{
			return -1;
		}
		Byte2Str( pPdu + iPduLen, iUDL );
	} 
	else if ( iDCS == 4 ) 
	{
		iTemp = Bit8Encode( pPdu + iPduLen + 2, pUD, iUDL );
		if ( iTemp < 0 )
		{
			return -1;
		}
		Byte2Str( pPdu + iPduLen, iTemp/2 );
	} 
	else if ( iDCS == 0x0a ) 
	{
		//SystemInfo("Pdu data:", (s8 *)pUD, iUDL, 1);
		iTemp = iUDL;
		memcpy(pPdu + iPduLen + 2, pUD, iUDL);
		iTemp = Hex_2_Ascii((s8 *)pUD, (s8 *)pPdu + iPduLen + 2, iUDL);
		Byte2Str( pPdu + iPduLen, iTemp/2 );
	} 
	else
	{
		iTemp = Ucs2Encode( pPdu + iPduLen + 2, pUD, iUDL );
		if ( iTemp < 0 )
		{
			return -1;
		}

		#ifdef MyPrint
		debugPrintf("Hand USC print:\r\n", sizeof("Hand USC print:\r\n"));
		debugPrintf(pPdu + iPduLen + 2, iTemp);
		debugPrintf("\r\n", sizeof("\r\n"));
		#endif
		
		Byte2Str( pPdu + iPduLen, iTemp/2 );
	}

	iPduLen += 2 + iTemp;
	*( pPdu + iPduLen ) = '\0';

	return iPduLen; */

	//////////////////

	ucs2_pt = pPdu + iPduLen +2;
	
	for(i=0, iTemp=0; i<iUDL; i++)
	{
		ByteToStr(ucs2_pt, *pUD);
		ucs2_pt += 2;
		pUD ++;

		iTemp += 2;
	}

	*ucs2_pt = '\0';

	/*
	#ifdef MyPrint
	debugPrintf("NVG USC print:\r\n", sizeof("NVG USC print:\r\n"));
	debugPrintf(pPdu + iPduLen +2, iTemp);
	debugPrintf("\r\n", sizeof("\r\n"));
	#endif
	*/
		
	Byte2Str( pPdu + iPduLen, iTemp/2 );
	
	iPduLen += 2 + iTemp;
	*( pPdu + iPduLen ) = '\0';

	return iPduLen;
}

/*
******************************************************************************
PDUDecode(NULL,								// ���ŷ������ĺ�
	           (u8 *)sms_buff,             				// �Է����� 			
	           NULL,								// ���ŷ���ʱ���	
	           (u8 *)(sms_buff+MAX_PHONE_STR_LEN-1), 	// �������� 				
	           NULL,        							// �Է����� 				
	           (u8 *)buff, 							// PDU����ʼ��ַ		
	           Size);								// PDU������				
******************************************************************************
*/
#endif

#ifdef PERIPHERAL_NAVIGATE
s16 PDU2USCDecode(u8 *pSCA, u8 *pDA, u8 *pTime, u8 *pUD, s16 *pDCS, u8 *pPdu, s16 iPduLen)
{
	u8 *pUD_tp;
	s16 iTemp;
	u16 i;
	u8  bTemp;
	u8  bDCS;

	*pDA++ = 'N';	/* �ַ�"N" ��ASCII ��, ��ʾ�绰����*/

	*pTime++ = 'T';   /* �ַ�'T'��ASCII��, ��ʾ����ʱ��*/

	*pUD++ = 'C';      /* �ַ�'C'��ASCII��, ��ʾ���Ķ�������*/
	
	for(i=0; i<(MAX_NAVIGATE_PHONE_LEN-1); i++)
	{
		*pDA++ = 0xFF;
	}
	pDA -= (MAX_NAVIGATE_PHONE_LEN-1);
	
	/* SCA, �������ĵĵ绰���� */
	if ( Str2Byte( &bTemp, pPdu ) != 0 )
	{
		return -1;
	}
	pPdu += 2;

	if ( bTemp == 0 )
	{
		if ( pSCA != NULL ) 
		{
			*pSCA = '\0';
		}
	} 
	else
	{
		if ( pSCA != NULL ) 
		{
			if ( memcmp( pPdu, "91", 2 ) == 0 )
			{
				if ( memcmp( pPdu + 2, "68", 2 ) == 0 )
				{
					bTemp--;
					pPdu += 2;
				} 
				else
				{
					strcpy( (s8 *)pSCA, "00" );
					pSCA += 2;
				}
			}

			if ( PhoneDecode( pSCA, pPdu + 2, bTemp * 2 - 2 ) <= 0 )
			{
				return -1;
			}
		}
		
		pPdu += bTemp * 2;
	}

	/*  PDUType,	Э�����ݵ�Ԫ����*/
	pPdu += 2;

	/*  ���ͷ�SME �ĵ�ַ*/
	if ( Str2Byte( &bTemp, pPdu ) != 0 ) return -1;
	pPdu += 2;

	if ( bTemp & 1 )
	{
		bTemp++;
	}
	if ( pDA != NULL )
	{
		if ( memcmp( pPdu, "91", 2 ) == 0 )
		{
			if ( memcmp( pPdu + 2, "68", 2 ) == 0 )
			{
				bTemp -= 2;
				pPdu  += 2;
			} 
			else 
			{
				strcpy( (s8 *)pDA, "00" );
				pDA += 2;
			}
		}

		iTemp = PhoneDecode( pDA, pPdu + 2, bTemp );
		
		if ( iTemp <= 0 )
		{
			return -1;
		}

		pDA[iTemp] = 0xFF;
	}

	pPdu += bTemp + 2;
	
	/* PID Protocol Identifier, SMSC�Ժ��ַ�ʽ����SM 			*/
	pPdu += 2;

	/* DCS ��ʾ�û�����(UD) ����ʲô���뷸		*/
	if ( Str2Byte( &bDCS, pPdu ) != 0 ) return -1;
	pPdu += 2;

	/* SCST ������ʾSMSC���յ���Ϣʱ��ʱ���	*/
	if( pTime != NULL)
	{
		memcpy(pTime, pPdu, MAX_NAVIGATE_MSM_TIME_LEN);
	}
	pPdu += 14;
	
	/* UDL, UD �û����ݶγ���, �û����� */
	if ( Str2Byte( &bTemp, pPdu ) != 0 ) 
	{
		return -1;
	}

			
	if ( bDCS == 0 ) 
	{
		if (memcmp(pPdu+2, "050003", strlen("050003")) == 0)
		{
			DebugPrintf("Multi sms package!\r\n");
			pPdu += 14;
			bTemp -= 7;
			bTemp /= 2;
			bTemp *= 2;
		}
		DebugPrintf( "\r\n7bit decode\r\n" );
		iTemp = Bit7Decode( (u8 *)pUD, pPdu + 2, ( bTemp - ( bTemp / 8 ) ) * 2 );
		if ( ( bTemp & 0x7 ) == 0x7 ) 
		{
			iTemp--;
			*((u8 *)pUD + iTemp ) = 0;
		}
	} 
	else if ( bDCS == 4 ) 
	{
		DebugPrintf( "\r\n8bit decode\r\n" );
		iTemp = Bit8Decode( pUD, pPdu + 2, bTemp * 2 );
	}
	
	/*
	==========================
	else
	{
		iTemp = Ucs2Decode( pUD, pPdu + 2, bTemp * 2 );
	}

	if ( pDCS != NULL ) 
	{
		*pDCS = bDCS;
	}
	
	return iTemp;
	==========================
	*/
	
	else
	{
		pUD_tp = pPdu + 2; 	/* PDU ����������ʼ��ַ*/
		
		for(i=0; i<bTemp; i++)   /* bTemp:  PDU ���ų���*/
		{
			Str2Byte(pUD, pUD_tp);
			pUD_tp += 2;
			pUD ++;
		}
		
		*pUD = '\0';
		
		//iTemp = Ucs2Decode( pUD, pPdu + 2, bTemp * 2 );
		//iTemp = bTemp +1;
		iTemp = bTemp;
	}

	if ( pDCS != NULL ) 
	{
		*pDCS = bDCS;
	}
	
	return iTemp;
}

s16 ASCIIUSCDecode(u8 *pDA, u8 *pTime, u8 *pUD, u8 *pPdu, s16 iPduLen)
{
	s16 iTemp;
	u16 i;
	datetime_t  date_time;

	*pDA++ = 'N';	/* �ַ�"N" ��ASCII ��, ��ʾ�绰����*/

	*pTime++ = 'T';   /* �ַ�'T'��ASCII��, ��ʾ����ʱ��*/

	//app_gps_InterfaceGetLocalDateTime(&date_time);
	app_sys_GetSysClkLocalDatetime(&date_time);

	*pTime++ = HexToAscii(date_time.date.year %10);

	*pTime++ = HexToAscii(date_time.date.year / 10);

	*pTime++ = HexToAscii(date_time.date.month %10);

	*pTime++ = HexToAscii(date_time.date.month / 10);

	*pTime++ = HexToAscii(date_time.date.day %10);

	*pTime++ = HexToAscii(date_time.date.day / 10);

	*pTime++ = HexToAscii(date_time.time.hour  %10);

	*pTime++ = HexToAscii(date_time.time.hour / 10);

	*pTime++ = HexToAscii(date_time.time.minute %10);

	*pTime++ = HexToAscii(date_time.time.minute / 10);

	*pTime++ = HexToAscii(date_time.time.second %10);

	*pTime++ = HexToAscii(date_time.time.second / 10);

	*pUD++ = 'C';      /* �ַ�'C'��ASCII��, ��ʾ���Ķ�������*/
	
	for(i=0; i<(MAX_NAVIGATE_PHONE_LEN-1); i++)
	{
		*pDA++ = 0xFF;
	}
	pDA -= (MAX_NAVIGATE_PHONE_LEN-1);

	
	for(i = 0;i < 4;i++)
	{
		*pDA++ = '#';
	}
		
		
	for(i=0; i<iPduLen; i++)   /* bTemp:  PDU ���ų���*/
	{
		Str2Byte(pUD, pPdu);
		pPdu += 2;
		pUD ++;
	}
	
	*pUD = '\0';
	
	iTemp = iPduLen;
	
	return iTemp;
}
#endif

s16 USCEncode3(u8 *dest,u8 *src,s16 len)
{
	s16 iDstLen;
	u8 bHigh;
	u8 bLow;
	u8  *pUcs2;
	u8 *pDst = dest;
	u32 dwIndex;
	u8  gb2uTable[2];

	for ( iDstLen = 0; len > 0;  ) 
	{
		/* GB2312 to Unicode */
		bHigh = *src;
		bLow  = *(src + 1);
		pUcs2 = NULL;
		if ( ( bLow >= 0xa1 ) && ( bLow <= 0xfe ) ) 
		{
			if ( ( bHigh >= 0xa1 ) && ( bHigh <= 0xa9 ) )
			{
				dwIndex = ( ( bHigh - 0xa1 ) * 94 + ( bLow - 0xa1 ) ) * 2;
				FLASH_ReadDatas(GB2UNICODEADDR + dwIndex, gb2uTable, 2);
				pUcs2   = gb2uTable;
			} 
			else if ( ( bHigh >= 0xb0 ) && ( bHigh <= 0xf7 ) )
			{
				dwIndex = ( ( bHigh - 0xb0 + 9 ) * 94 + ( bLow - 0xa1 ) ) * 2;
				FLASH_ReadDatas(GB2UNICODEADDR + dwIndex, gb2uTable, 2);
				pUcs2   = gb2uTable;
			}
		}
		if ( pUcs2 == NULL )
		{
			bHigh = '\0';
			bLow  = *src;
			src += 1;
			len -= 1;
		} 
		else 
		{
			bHigh = *pUcs2++;
			bLow  = *pUcs2;
			src    += 2;
			len -= 2;
			if ( len < 0 ) 
			{
				return -1;
			}
		}
		/* Unicode to ASCII	 */
		*pDst++ =  bLow;
		*pDst++ =  bHigh;
		iDstLen += 2;
	}

	*pDst = '\0';

	return iDstLen;
}


s16 UcsDecode3(u8 *pDst, u8 *pSrc, s16 iSrcLen)
{
	u32 dwIndex;
	s16 iDstLen;
	u8 bHigh;
	u8 bLow;
	u8 u2gbTable[2];
	
	for(iDstLen = 0; iSrcLen > 0;)
	{
		/* ASCII to Unicode
		if(Str2Byte(&bHigh, pSrc) != 0)
		{
			return -1;
		}
		
		pSrc += 2;
		
		if(Str2Byte(&bLow,  pSrc) != 0)
		{
			return -1;
		}

		pSrc += 2;
		if((iSrcLen -= 4) < 0)
		{
			return -1;
		}
		 */
		if((iSrcLen -= 2) < 0)
		{
			return -1;
		}

		bLow = *pSrc++;
		bHigh = *pSrc++; 
		
		
		/* Unicode to GB2312 */
		if(bHigh == 0) 
		{
			*pDst++ = bLow;
			iDstLen++;
		} 
		else
		{
			dwIndex = ((((u32)bHigh) << 8) + bLow) * 2;
			FLASH_ReadDatas(UNICODE2GBADDR + dwIndex, u2gbTable, 2);
			*pDst++ = u2gbTable[0];
			*pDst++ = u2gbTable[1];
			iDstLen += 2;
		}
	}

	*pDst = '\0';

	return iDstLen;
}


