/*
******************************************************************************
* Copyright (c) 2005,航天无线通信技术开发（深圳）有限公司
* All rights reserved.
* 
* 文件名称：pdu.c
* 项目标识：MPIP-618
* 摘    要：PDU编解码，用于收发短信
* 
* 当前版本：3.7.2
* 作    者：牟海泉
* 完成日期：2005年12月30日
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
						局部函数实现
******************************************************************************
*/
/*
******************************************************************************
* 功能描述：电话号码格式转换。
* 输入参数：pPhone		电话号码。
*           iPhoneLen	电话号码长度。
* 输出参数：pEncode	转换后的八位组格式。
* 返回参数：转换后的八位组的字节长度。
* 举例说明：输入：pPhone="13851724908", iPhoneLen=11
*           输出：pEncode="3158714209F8"
*           返回：14
* 举例说明：输入：pPhone="13851724908F", iPhoneLen=12
*           输出：pEncode="3158714209F8"
*           返回：14
* 注意事项：iPhoneLen 必须小于20字节。
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
* 功能描述：电话号码格式转换。
* 输入参数：pDecode	转换前的八位组。
*           iDecodeLen	八位组的字节长度。
* 输出参数：pPhone		转换后的电话号码。
* 返回参数：转换后电话号码的字节长度。
* 举例说明：输入：pDecode="3158714209F8"  iDecodeLen= 12
*           输出：pPhone="13851724908"
*           返回：11
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
* 功能描述：ASCII到二进制的转换。
* 输入参数：pStr	  ASCII码数据。
* 输出参数：pByte	  二进制数据。
* 返回参数：成功返回0，失败返回-1。
* 举例说明：输入：pAscii="38"
*           输出：pByte=0x38
*           返回：0。
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
 * 功能描述：二进制到ASCII的转换。
 * 输入参数：bByte	  二进制数据。
 * 输出参数：pStr	  ASCII码数据。
 * 返回参数：成功返回0，失败返回-1。
 * 举例说明：输入：bByte=0x38
 *           输出：pStr ="38"
 *           返回：0。
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
* 功能描述：7bit的编码转换。
* 输入参数：pSrc	  每个字节7bit的数据。
*           iSrcLen  pSrc的字节长度。
* 输出参数：pDst	  8bitASCII的数据。
* 返回参数：成功返回pDst的字节长度， 失败返回-1。
* 举例说明：输入：pSrc="Hello World!", iSrcLen=11
*           输出：pDst="C8329BFD065DDF72363904"
*           返回：22。
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
* 功能描述：7bit的编码转换。
* 输入参数：pSrc	  每个字节8bit的数据。
*           iSrcLen  pSrc的字节长度。
* 输出参数：pDst	  7bitASCII的数据。
* 返回参数：成功返回pDst的字节长度，失败返回-1。
* 举例说明：输入：pSrc="C8329BFD065DDF72363904", iSrcLen=22
*           输出：pDst="Hello World!"
*           返回：11。
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
* 功能描述：8bit的编码转换。
* 输入参数：pSrc	  二进制数据。
*           iSrcLen  pSrc的字节长度。
* 输出参数：pDst	  十六进制ASCII的数据。
* 返回参数：成功返回pDst的字节长度，失败返回-1。
* 举例说明：输入：pSrc={ 0,1,2,3,4,5,6,7,8,9,0xF0,0xFF ), iSrcLen=12
*           输出：pDst="00010203040506070809F0FF"
*           返回：24。
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
* 功能描述：8bit的解码码转换。
* 输入参数：pSrc	  十六进制ASCII的数据。
*           iSrcLen  pSrc的字节长度。
* 输出参数：pDst	  二进制数据。
* 返回参数：成功返回pDst的字节长度，失败返回-1。
* 举例说明：输入：pSrc="00010203040506070809F0FF", iSrcLen=24
*           输出：pDst={ 0,1,2,3,4,5,6,7,8,9,0xF0,0xFF )
*           返回：12。
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
* 功能描述：GB2312 的编码转换.
* 输入参数：pSrc: GB2312 编码的数据.
* 输入参数：iSrcLen:pSrc 的字节长度.
* 输出参数：pDst: Unicode 编码的ASCII 数据.
* 返回参数：成功返回pDst 的字节长度, 失败返回-1
* 举例说明：输入pSrc=, iSrcLen=
* 输出参数：pDst=
* 返回参数：
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
* 功能描述：GB2312 的解码转换.
* 输入参数：pSrc: Unicode 编码的ASCII 数据.
* 输入参数：iSrcLen: pSrc 的字节长度.
* 输出参数：pDst: GB2312 编码的数据
* 返回参数：成功返回pDst 的字节长度, 失败返回-1
* 举例说明：输入pSrc=, iSrcLen=
* 输出参数：pDst=
* 返回参数：
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
						接口函数实现
****************************************************************/
/*
 ******************************************************************************
 * 功能描述：PDU自测程序
 * 输入参数；无
 * 输出参数；无
 * 返回参数：无
 ******************************************************************************
 */
s16 PduTest( void )
{
	u8 cBuf[] = "测试";
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
发送方PDU格式
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
	
	/* SCA, 服务中心的电话号码 	*/
	if(pSCA == NULL)
	{
		strcpy((s8*)pPdu, "00");
		iPduLen = strlen((s8*)pPdu);
	} 
	else 
	{
		if(*pSCA == '+') 	/* 国内通用的国际号码前缀。*/
		{
			pSCA++;
			Byte2Str( pPdu + 2, 0x91 );
		} 
		else if(memcmp(pSCA, "00", 2) == 0) 	/* 国际通用的国际号码前缀。*/
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

	/*  PDUType,	协议数据单元类型  */
	Byte2Str(pPdu + iPduLen, 0x11);
	iPduLen += 2;

	/* MR 所有成功的SMS-SUBMIT参考数目 */
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* DA 接收方SME的地址*/
	if(*pDA == '+')	/* 国内通用的国际号码前缀。*/
	{
		pDA++;
		Byte2Str(pPdu + iPduLen + 2, 0x91);
	}
	else if(memcmp(pDA, "00", 2) == 0)  /* 国际通用的国际号码前缀。*/
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

	/* PID Protocol Identifier, SMSC以何种方式处理SM 			*/
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* DCS 表示用户数据(UD)采用什么编码方案						*/
	Byte2Str(pPdu + iPduLen, iDCS);
	iPduLen += 2;

	/* VP Validity Period 参数表示消息在SMSC中不再有效的时长	*/
	Byte2Str(pPdu + iPduLen, 0x00);
	iPduLen += 2;

	/* UDL, UD 用户数据段长度, 用户数据 						*/
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
* 接收方PDU格式:
* ----------------------------------------------------------
* | SCA  | PDUType | OA 	| PID | DCS | SCTS | UDL | UD    |
* |---------------------------------------------------------
* | 1-12 | 1       | 2-12  | 1   |  1  | 7    |  1  | 0-140 |
* ----------------------------------------------------------
* 格式说明：
* SCA: 		服务中心的电话号码
* PDUType:	协议数据单元类型
* OA: 		Originator Adress 发送方SME的地址
* PID: 		显示SMSC以何种方式处理SM
* DCS: 		表示用户数据(UD)采用什么编码方案
* SCTS: 		参数表示SMSC接收到消息时的时间戳
* UDL:
* UD:
******************************************************************************
PDUDecode(NULL,									// 短信服务中心号
	           (u8 *)sms_buff,             					// 对方号码 			
	           NULL,									// 短信发送时间戳	
	           (u8 *)(sms_buff+MAX_PHONE_STR_LEN-1), 	// 短信内容 				
	           NULL,        								// 对方编码 				
	           (u8 *)buff, 								// PDU串起始地址		
	           Size);									// PDU串长度				
******************************************************************************
*/
s16 PDUDecode(u8 *pSCA, u8 *pDA, u8 *pTime, u8 *pUD, s16 *pDCS, u8 *pPdu, s16 iPduLen)
{
	s16 iTemp;
	u8  bTemp;
	u8  bDCS;
	
	/* SCA, 服务中心的电话号码 */
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

	/*  PDUType,	协议数据单元类型*/
	pPdu += 2;

	/*  发送方SME 的地址 */
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
	
	/* PID Protocol Identifier, SMSC以何种方式处理SM 	*/
	pPdu += 2;

	/* DCS 表示用户数据(UD)采用什么编码犯	*/
	if(Str2Byte( &bDCS, pPdu) != 0) return -1;
	pPdu += 2;

	/* SCST 参数表示SMSC 接收到消息时的时间戳*/
	if(pTime != NULL)
	{
		iTemp = PhoneDecode(pTime, pPdu, 12);
	}
	pPdu += 14;
	
	/* UDL, UD 用户数据段长度, 用户数据 */
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
* pDA,           	// 短信发送方号码 			
* pTime,		// 短信发送时间戳	
* pUD, 		// 短信内容 				
* pPdu, 		// PDU串起始地址		
* iPduLen;	// PDU串长度				
******************************************************************************
*//*
s16 PDUDecode(u8 *pNum, u8 *pTime, u8 *pUD, u8 *pPdu, s16 iPduLen)
{
	s16 iTemp;
	u8  bTemp;
	u8  bDCS;
	
	//短信SCA  服务中心的电话号码
	if(Str2Byte(&bTemp, pPdu) != 0)
	{
		return -1;
	}
	pPdu += (bTemp*2) +2;

	//PDUType  协议数据单元类型
	pPdu += 2;

	//发送方SME  的地址
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
	
	//PID Protocol Identifier, SMSC 以何种方式处理
	pPdu += 2;

	//DCS 数据编码方式
	if(Str2Byte(&bDCS, pPdu) != 0) return -1;
	pPdu += 2;

	//SMSC 接收到消息时的时间戳
	if(pTime != NULL)
	{
		PhoneDecode(pTime, pPdu, 12);
	}
	pPdu += 14;
	
	//UDL, UD 用户数据段长度, 用户数据
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
GB2312(1980年)一共收录了7445个字符，包括6763个汉字和682个其它符号。
汉字区的内码范围高字节从B0-F7，低字节从A1-FE，占用的码位是72*94=6768。
其中有5个空位是D7FA-D7FE
*/

#if 1//def PERIPHERAL_NAVIGATE
s16 USC2PDUEncode(u8 *pPdu, u8 *pSCA, u8 *pDA, u8 *pUD, s16 iUDL, s16 iDCS)
{	
	s16 iPduLen = 0;
	s16 iTemp;
	u32 i;
	u8 *ucs2_pt;
	
	/* SCA, 服务中心的电话号码 	*/
	if( pSCA == NULL )
	{
		strcpy( (s8 *)pPdu, "00" );
		iPduLen = strlen( (s8 *)pPdu );
	} 
	else 
	{
		if( *pSCA == '+' ) /* 国内通用的国际号码前缀*/
		{
			pSCA++;
			Byte2Str( pPdu + 2, 0x91 );
		} 
		else if( memcmp( pSCA, "00", 2 ) == 0 ) /* 国际通用的国际号码前缀。*/
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

	/*  PDUType,	协议数据单元类型  */
	Byte2Str( pPdu + iPduLen, 0x11 );
	iPduLen += 2;

	/* MR 所有成功的SMS-SUBMIT参考数目 */
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* DA 接收方SME的地址				*/
	if( *pDA == '+' )                       /* 国内通用的国际号码前缀。*/
	{
		pDA++;
		Byte2Str( pPdu + iPduLen + 2, 0x91);
	}
	else if( memcmp( pDA, "00", 2 ) == 0 )  /* 国际通用的国际号码前缀。*/
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

	/* PID Protocol Identifier, SMSC以何种方式处理SM 			*/
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* DCS 表示用户数据(UD)采用什么编码方案						*/
	Byte2Str( pPdu + iPduLen, iDCS );
	iPduLen += 2;

	/* VP Validity Period 参数表示消息在SMSC中不再有效的时长	*/
	Byte2Str( pPdu + iPduLen, 0x00 );
	iPduLen += 2;

	/* //UDL, UD 用户数据段长度, 用户数据 
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
PDUDecode(NULL,								// 短信服务中心号
	           (u8 *)sms_buff,             				// 对方号码 			
	           NULL,								// 短信发送时间戳	
	           (u8 *)(sms_buff+MAX_PHONE_STR_LEN-1), 	// 短信内容 				
	           NULL,        							// 对方编码 				
	           (u8 *)buff, 							// PDU串起始地址		
	           Size);								// PDU串长度				
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

	*pDA++ = 'N';	/* 字符"N" 的ASCII 码, 表示电话号码*/

	*pTime++ = 'T';   /* 字符'T'的ASCII码, 表示短信时间*/

	*pUD++ = 'C';      /* 字符'C'的ASCII码, 表示中文短信内容*/
	
	for(i=0; i<(MAX_NAVIGATE_PHONE_LEN-1); i++)
	{
		*pDA++ = 0xFF;
	}
	pDA -= (MAX_NAVIGATE_PHONE_LEN-1);
	
	/* SCA, 服务中心的电话号码 */
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

	/*  PDUType,	协议数据单元类型*/
	pPdu += 2;

	/*  发送方SME 的地址*/
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
	
	/* PID Protocol Identifier, SMSC以何种方式处理SM 			*/
	pPdu += 2;

	/* DCS 表示用户数据(UD) 采用什么编码犯		*/
	if ( Str2Byte( &bDCS, pPdu ) != 0 ) return -1;
	pPdu += 2;

	/* SCST 参数表示SMSC接收到消息时的时间戳	*/
	if( pTime != NULL)
	{
		memcpy(pTime, pPdu, MAX_NAVIGATE_MSM_TIME_LEN);
	}
	pPdu += 14;
	
	/* UDL, UD 用户数据段长度, 用户数据 */
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
		pUD_tp = pPdu + 2; 	/* PDU 短信内容起始地址*/
		
		for(i=0; i<bTemp; i++)   /* bTemp:  PDU 短信长度*/
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

	*pDA++ = 'N';	/* 字符"N" 的ASCII 码, 表示电话号码*/

	*pTime++ = 'T';   /* 字符'T'的ASCII码, 表示短信时间*/

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

	*pUD++ = 'C';      /* 字符'C'的ASCII码, 表示中文短信内容*/
	
	for(i=0; i<(MAX_NAVIGATE_PHONE_LEN-1); i++)
	{
		*pDA++ = 0xFF;
	}
	pDA -= (MAX_NAVIGATE_PHONE_LEN-1);

	
	for(i = 0;i < 4;i++)
	{
		*pDA++ = '#';
	}
		
		
	for(i=0; i<iPduLen; i++)   /* bTemp:  PDU 短信长度*/
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


