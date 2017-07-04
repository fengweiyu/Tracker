#include "app_gsm_includes.h"
#include "main.h"


typedef void (*retInfo_analyse_t)(GsmTpU8 *data_buff, GsmTpU16 data_len);
typedef struct
{
	const GsmTpS8 *ret_info;
	retInfo_analyse_t analyse;
}atRetInfo_analyse_menu_t;



const GsmTpS8 cs8GsmTtsEndStr[] = "+TTS: END"; 
//const GsmTpS8 cs8GsmRxIpDatTcpStr[] = "+MIPRTCP:"; 
const GsmTpS8 cs8GsmRxIpDatTcpStr[] = "+IPD"; 
const GsmTpU8 cu8GsmRxIpDatTcpStrLen = sizeof(cs8GsmRxIpDatTcpStr) - 1; 
const GsmTpS8 cs8GsmRxIpDatUdpStr[] = "+MIPRUDP:"; 
const GsmTpS8 cs8GsmPwrOnStr[] = "Wait...AT command ready"; 

void app_gsm_MipstatHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_IPdataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_ModuleRestart(GsmTpU8 *data_buff, GsmTpU16 data_len); 
static void app_gsm_MIPCALLHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
static void app_gsm_MIPCLOSEHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_AudioRecordResultHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void GsmTtsEndHandler(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
void app_gsm_ManIpDataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);

static const atRetInfo_analyse_menu_t AtRetInfoAnalyseExpandTab[] =
{
	{"+MIPSTAT:",app_gsm_MipstatHandler},
	{cs8GsmRxIpDatUdpStr, app_gsm_IPdataHandler},
	{cs8GsmRxIpDatTcpStr, app_gsm_IPdataHandler},
	{cs8GsmPwrOnStr, app_gsm_ModuleRestart},
	{"Power Down", app_gsm_ModuleRestart},
	{"+MIPCALL:", app_gsm_MIPCALLHandler},
	{"CLOSED", app_gsm_MIPCLOSEHandler},
	{"+PDP: DEACT", app_gsm_MIPCLOSEHandler},//增加突然链接断掉的处理
	{"+MIPXOFF", app_gsm_FlowCtlOfflHandler},
	{"+MIPXON", app_gsm_FlowCtlOnlHandler},
	{cs8GsmTtsEndStr, GsmTtsEndHandler}, 
	{"+ASTOP:",app_gsm_AudioAttriHandler},
	{"+AUDRD:",app_gsm_AudioDataHandler},
	{"+AUDCD:",app_gsm_AudioRecordResultHandler},
	{"+CIPRXGET: 1",app_gsm_ManIpDataHandler},
}; 
const GsmTpU8 cu8AtRetInfoAnalyseExpandTabSize = (sizeof(AtRetInfoAnalyseExpandTab) / sizeof(atRetInfo_analyse_menu_t)); 


void GsmAtAnalyseExpandProc(GsmTpU8 *pu8Dat, GsmTpU16 u16DatLen)
{
	GsmTpU16 u16Idx;
	GsmTpU16 u16Len; 
	
	for(u16Idx = 0; u16Idx < cu8AtRetInfoAnalyseExpandTabSize; u16Idx++) {
		u16Len = strlen(AtRetInfoAnalyseExpandTab[u16Idx].ret_info); 
		if(u16DatLen < u16Len) {; 
		}
		else {
			if(IS_FALSE_BIT(GsmMemIsEqu(pu8Dat, 
									(GsmTpU8 *)AtRetInfoAnalyseExpandTab[u16Idx].ret_info, 
									u16Len))) {; 
			}
			else {
				AtRetInfoAnalyseExpandTab[u16Idx].analyse(pu8Dat, u16DatLen);
				break;
			}
		}
	}	
}


void app_gsm_MipstatHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpS8 *ps8AtAckDat; 
	GsmTpS8 *ps8AtAckDesc; 
	ps8AtAckDat = strstr((GsmTpS8 *)data_buff, "+MIPSTAT: ") + sizeof("+MIPSTAT: ") - 1;
	ps8AtAckDesc = strstr((GsmTpS8 *)data_buff, ",") + sizeof(",") - 1; 
	if('0' == *ps8AtAckDesc) {
		GsmDebugString("sck ack\r\n"); 
	}
	else {
		if((IP_DAT_SRC_SCK_2 + 1) == GsmM_Ascii2u8(ps8AtAckDat[0])) {
			
		}
		else {
			GsmNetBreakProc(); 		
			ATLib_gsm_MIPCALL_DisconnectLink(); 
		}
	}
}

void app_gsm_IPdataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
#if 1
	GsmTpU8 u8ParaCheckFlag;
	GsmTpS8 * ps8CommaAddr;
	GsmTpS8 * ps8ColonAddr;
	GsmTpS8 * ps8DataLenAddr;
	GsmTpS8 * ps8DataStartAddr;
	GsmTpU16 u16RecvPackageLen;
	GsmTpU8 * pu8RecvData;
	GsmTpU16 u16RecvDataLen;
	Gsmmyevent IpRecvData;
	IpDatSt *pstIpRecvDataSt;
	IpDatSt stIpRecvDataSt;
	static GsmTpS8 acPrintfBuf[215]={0};
	GsmTpU8 ucPrintfBufLen=0;
	GsmTpU8 ucCount=0;
	pu8RecvData = data_buff;
	u16RecvPackageLen = data_len;
	M_SetBit(u8ParaCheckFlag);

	ps8CommaAddr = strchr((char const *)pu8RecvData, ',');
	ps8ColonAddr = strchr((char const *)pu8RecvData, ':');
	if( IS_NULL_P(ps8CommaAddr) || IS_NULL_P(ps8ColonAddr) || (ps8CommaAddr>ps8ColonAddr)) {
		M_ClrBit(u8ParaCheckFlag);
	}
	else {
		ps8DataLenAddr = ps8CommaAddr + 1;
		ps8DataStartAddr = ps8ColonAddr + 1;
		M_SetBit(u8ParaCheckFlag);
	}

	if( IS_FALSE_BIT(u8ParaCheckFlag) || ((ps8DataStartAddr-ps8DataLenAddr)<2) ) {
		;
	}
	else {
		u16RecvDataLen = atoi(ps8DataLenAddr);
		if( IS_FALSE_BIT(u16RecvDataLen) || u16RecvPackageLen<=u16RecvDataLen ) {
			M_ClrBit(u8ParaCheckFlag);
		}
		else {
			M_SetBit(u8ParaCheckFlag);
		}
	}

	if( IS_FALSE_BIT(u8ParaCheckFlag) ) {
		GsmDebugString("gprs rx len err\r\n");
	}
	else {
#if 1
		GsmTpS8 s8Data2Str;
		GsmTpS8 * s8Data2StrAddr;
		GsmTpU16 u16StrLenTemp;
		u16StrLenTemp = u16RecvDataLen;
		s8Data2StrAddr = ps8DataStartAddr;
		if(u16StrLenTemp>100)//超过100长度不打印
		{
		}
		else
		{	
			memset(acPrintfBuf,0,sizeof(acPrintfBuf));
			ucPrintfBufLen=ps8DataStartAddr - (char *)pu8RecvData;
			memcpy(acPrintfBuf,pu8RecvData,ucPrintfBufLen);
			for(ucCount=0;ucCount<2*u16StrLenTemp&&ucCount<sizeof(acPrintfBuf);ucCount+=2)
			{
				s8Data2Str = HexToAscii( (*s8Data2StrAddr) >> 4 );
				acPrintfBuf[ucPrintfBufLen+ucCount]=s8Data2Str;
				s8Data2Str = HexToAscii( (*s8Data2StrAddr) & 0xF);
				acPrintfBuf[ucPrintfBufLen+ucCount+1]=s8Data2Str;
				s8Data2StrAddr++;
			}
			memcpy(&acPrintfBuf[ucPrintfBufLen+ucCount],"\r\n",strlen("\r\n"));
			GsmDebugStringLowLevel(acPrintfBuf);
		}
#endif
		pstIpRecvDataSt = &stIpRecvDataSt;
		IpRecvData.size = u16RecvDataLen;
		IpRecvData.buff = GsmMalloc(IpRecvData.size);
		if(NULL == IpRecvData.buff) {
			GsmDebugString("Mpip recv can't malloc!\n");
			//GsmMallocErrHook();
		}
		else {
			memset(IpRecvData.buff, 0, IpRecvData.size);
			pstIpRecvDataSt->enSrc = IP_DAT_SRC_SCK_1;
			memcpy(IpRecvData.buff, ps8DataStartAddr, IpRecvData.size);
			pstIpRecvDataSt->pu8Dat = IpRecvData.buff;
			pstIpRecvDataSt->u16Len = IpRecvData.size;
			if(IS_TRUE_BIT(GsmRxIpDatHook(pstIpRecvDataSt))) {
#if 0
				GsmDebugString("Ip data recv:");
				debugPrintfHex2Ascii(pstIpRecvDataSt->pu8Dat,pstIpRecvDataSt->u16Len);
				GsmDebugString("\r\n");
#endif
			}
			else {
				GsmDebugString("Gsm ip dat write err\r\n");
			}
			GsmFree(IpRecvData.buff, IpRecvData.size);
		}
	}
#else
	GsmTpU8 *com_para;
	GsmTpU8 *prt_ptr;
	GsmTpU16 prt_len;
	GsmTpU8 u8i; 
	GsmTpU16 u16Len; 
	IpDatSt stIpDatSt; 
	GsmTpU8 u8SktId; 
	GsmTpBool boIsUdpDatFlg; 
	M_ClrBit(u8SktId); 
	u16Len = data_len; 
	
	com_para = data_buff;

	if(GsmMemIsEqu((const GsmTpU8*)data_buff, 
				(const GsmTpU8*)cs8GsmRxIpDatUdpStr, 
				strlen(cs8GsmRxIpDatUdpStr))) {
		M_SetBit(boIsUdpDatFlg); 
	}
	else {
		M_ClrBit(boIsUdpDatFlg); 
	}

	for(u8i = 0; u8i < 3; u8i++) {
		GsmTpS8 s8Sign; 
		s8Sign = (IS_FALSE_BIT(u8i) ? ':' : ','); 
		
		while(*com_para != s8Sign) {
			if(IS_FALSE_BIT(u16Len)) {		
				//此处返回不会造成内存溢出
				return; 
			}
			else	{
				u16Len--; 
			}
			com_para++; 
		};

		// check socket. 1 or 2
		if(u8i == 1) {
			GsmTpU8 *pu8Dat; 
			if(IS_FALSE_BIT(boIsUdpDatFlg)) {; 
			}//upd dat has ip str and port
			else {
				GsmTpU8 u8j; 
				for(u8j = 0; u8j < 2; u8j++) {
					com_para++; 
					M_NOT_ZERO_DEC(u16Len); 
					while(*com_para != ',') {
						if(IS_FALSE_BIT(u16Len)) {		
							//此处返回不会造成内存溢出
							return; 
						}
						else	{
							u16Len--; 
						}
						com_para++; 
					};
				}
			}
			pu8Dat = com_para; 
			if(pu8Dat[0] == ',') {
				pu8Dat--; 
				u8SktId = GsmM_Ascii2u8(pu8Dat[0]); 
			}
		}
		
		com_para++; //skip sign
		if(IS_FALSE_BIT(u16Len)) {		
			//此处返回不会造成内存溢出
			return; 
		}
		else	{
			u16Len--; 
		}
	}
	
	if(u16Len < 2)	{		
		//此处返回不会造成内存溢出
		return; 
	}
	else	{
		u16Len--; //\r
	}


	prt_len = (GsmTpU16)(com_para - data_buff); 
	GsmDebugString("\r\n");
	GsmDebugPrintf(data_buff, prt_len);
	
	prt_ptr = com_para;
	prt_len = u16Len;
	if(IS_FALSE_BIT(GsmGprsDebugPrintfIsLimit()) || (prt_len < 40)) {; 
	}
	else {
		prt_len = 20; 
	}

	while(IS_TRUE_BIT(prt_len))
	{
		GsmTpU8 u8PrintBuf[2]; 
		u8PrintBuf[0] = GsmHexToAscii(prt_ptr[0] >> 4);
		u8PrintBuf[1] = GsmHexToAscii(prt_ptr[0] &0xF);
		GsmDebugPrintf(u8PrintBuf, sizeof(u8PrintBuf));
		prt_len--;
		prt_ptr++;
	}
	GsmDebugString("\r\n"); 

	stIpDatSt.enSrc = (IpDatSrcEn)(u8SktId - 1); 
	stIpDatSt.u16Len = u16Len; 
	stIpDatSt.pu8Dat = com_para; 
	if(IS_TRUE_BIT(GsmRxIpDatHook(&stIpDatSt))) {; 
	}
	else {
		GsmDebugString("Gsm ip dat write err\r\n"); 
	}
#endif
}

void app_gsm_ModuleRestart(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmModRstReq(); 	
}

static void app_gsm_MIPCALLHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	if(memcmp(data_buff, "+MIPCALL: 0", sizeof("+MIPCALL: 0") - 1) == 0) {
		GsmNetBreakProc(); 
	}
	else {; 
	}
}

static void app_gsm_MIPCLOSEHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	u8 strBuf[8]={0};
	/*由于来短信等其他业务，这里也会调用，这时候
	并不是网络断了，为防止误判断屏蔽下面语句，如果是
	短信等其他业务导致的发送失败等错误，GSM工作机制也会处理，所以屏蔽*/
	//OSFlagPost(pstOsFlagForAppUse, GPRS_IS_OK_FLAG, OS_FLAG_CLR, &ucErr);	
	snprintf(strBuf,sizeof(strBuf),data_buff,data_len);
	GsmDebugString(strBuf);
	//GsmNetBreakProc(); 
	GsmDriverEnterCritical();  
	//M_SetBit(gsm_oper_info.flgs.Ipclose); //链接已经断了不需要再关
	GsmGprsRstReq(); 
	GsmGprsNgSet(); 
	GsmDriverExitCriticical(); 
}

void app_gsm_CMGRHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 u16LenTmp; 
	GsmTpU8 *pu8Tmp; 
	u16LenTmp = data_len; 
	pu8Tmp = NULL; 
	while(u16LenTmp--) {
		if(',' != data_buff[u16LenTmp]) {; 
		}
		else {
			pu8Tmp = &data_buff[u16LenTmp]; 
		}
	}
	if(IS_NULL_P(pu8Tmp)) {
		GsmDebugString("sms rx cmd err\r\n"); 
	}
	else {
		pu8Tmp++;//skip ','
		u16LenTmp = atoi((GsmTpS8 *)pu8Tmp); 
	}

	M_SetBit(stGsmSmsOpr.stSmsRxInfo.bSmsRxingFlg); 
	stGsmSmsOpr.stSmsRxInfo.u16PduLen = u16LenTmp; 
}

void app_gsm_CMTIHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 u16Index;
	GsmTpS8 *ps8Dat; 
	ps8Dat = strchr((const GsmTpS8*)data_buff, ',') + 1;
	u16Index = atoi(ps8Dat);
	
	if(IS_TRUE_BIT(GsmFifoPost(&stGsmSmsOpr.stSmsRxInfo.stSmsPromptFifo, 
							(void *)u16Index))) {; 
	}
	else {
		GsmDebugString("CMT sms fifo write err\r\n"); 
		M_SetBit(stGsmSmsOpr.stSmsRxInfo.bSmsRxOverflowFlg); 
	}
}

void app_gsm_CMTHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 u16LenTmp;
	GsmTpS8 *ps8Dat; 
	ps8Dat = strchr((const GsmTpS8*)data_buff, ',') + 1;
	u16LenTmp = atoi(ps8Dat);

	M_SetBit(stGsmSmsOpr.stSmsRxInfo.bSmsRxingFlg); 
	stGsmSmsOpr.stSmsRxInfo.u16PduLen = u16LenTmp; 
}


void GsmTtsEndHandler(GsmTpU8 *pu8Dat, GsmTpU16 u16Len) 
{
	M_ClrBit(gsm_oper_info.flgs.bTtsBusyFlg); 
}

void app_gsm_AudioAttriHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU8 *com_para =NULL;
//	GsmTpU8 time_min;
//	GsmTpU8 time_sec;
	GsmTpU32 fileLen = 0;
	GsmTpU16 i = 0;
	GsmTpU8 u8IdxTmp; 
	if((data_buff == NULL)) //||(data_len < 5))  //基本参数的长度5
	{
		return ;
	}

	if(NULL !=(com_para  = (GsmTpU8*)strstr((GsmTpS8 const *)data_buff,"Index")))
	{
		
		for(i=0;*com_para !=' ';i++,com_para++)
		{
			if(i > data_len)break;
		}
		com_para++;	//跳过空格
		
		u8IdxTmp = (GsmTpU8)atoi((GsmTpS8 *)com_para); 
	}
	else
	{
		return ;
	}

	if(NULL !=(com_para  = (GsmTpU8*)strstr((GsmTpS8 const *)data_buff,"Format")))
	{
		
		for(i=0;*com_para !=' ';i++,com_para++)
		{
			if(i > data_len)break;
		}
		com_para++;	//跳过空格	
	}
	else
	{
		return ;
	}
	
	if(NULL !=(com_para  = (GsmTpU8*)strstr((GsmTpS8 const *)data_buff,"Time")))
	{
		//Time 00:26
		for(i=0;*com_para !=' ';i++,com_para++)
		{
			if(i > data_len)break;
		}
		com_para++;	//跳过空格
//		time_min = (10*(com_para[0]-'0')+com_para[1]-'0');
		com_para+=3;
//		time_sec = (10*(com_para[0]-'0')+com_para[1]-'0');
	}
	else
	{
		return ;
	}

	if(NULL !=(com_para  = (GsmTpU8*)strstr((GsmTpS8 const *)data_buff,"Len")))
	{
		
		for(i=0;*com_para !=' ';i++,com_para++)
		{
			if(i > data_len)break;
		}
		com_para++;	//跳过空格

		while(*com_para != '\r')
		{
			if((*com_para>= '0') &&(*com_para<='9'))
			{
				fileLen *= 10;
				fileLen +=*com_para -'0';
			}
			else
			{
				//break;
			}
			com_para ++;
		}

		GsmAudioRecordRxedNewDatBlockInfo(u8IdxTmp, (GsmTpU16)fileLen); 
	}
	else
	{
		return ;
	}

}

//static 
void app_gsm_AudioDataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 datalen = 0;
	GsmTpU8 *com_para =data_buff;
	GsmTpU16 i= 0;
	GsmTpU8 *prt_ptr;
	GsmTpU16 prt_len;
	GsmTpU8 prt_byte;
	if((data_buff == NULL) ) 
	{
		return ;
	}

	for(i=0;*com_para != ':';i++,com_para++) //参数分隔符:
	{
		if(i > data_len)
		{
			return ;
		}
	}

	com_para++; //跳过空格

	for(i=0;*com_para != '*';i++,com_para++) //参数分隔符:
	{
		if(i > data_len)
		{
			return ;
		}
		if((*com_para>= '0') &&(*com_para<='9'))
		{
			datalen *= 10;
			datalen +=*com_para -'0';
		}
		else
		{
			//
		}
	}
	com_para++;
	if(*com_para == '\n')
	{
		com_para++;
		prt_ptr = com_para;
		prt_len = 0;
		while(prt_len < datalen)
		{
			prt_byte = GsmAsciiToHex(prt_ptr[0]  )& 0x0f;
			prt_byte <<=4;
			prt_byte +=GsmAsciiToHex(prt_ptr[1] )& 0x0f;
			com_para[prt_len ] = prt_byte;
			prt_len++;
			prt_ptr+=2;
		}
		
		GsmAudioRecordGsmRxAudioDatHook(com_para, datalen); 
	}
}

void app_gsm_AudioRecordResultHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 datalen = 0;
	GsmTpU8 *com_para =data_buff;
	GsmTpU16 i= 0;
	GsmTpS8 s8Buf[6]; 
	if((data_buff == NULL) ) 
	{
		return ;
	}

	for(i=0;*com_para != ':';i++,com_para++) //参数分隔符:
	{
		if(i > data_len)
		{
			return ;
		}
	}

	com_para++; //跳过空格

	for(i=0;*com_para != ',';i++,com_para++) //参数分隔符:
	{
		if(i > data_len)
		{
			return ;
		}
		if((*com_para>= '0') &&(*com_para<='9'))
		{
			datalen *= 10;
			datalen +=*com_para -'0';
		}
		else
		{
			//
		}
	}
	com_para++;
	memset(s8Buf, 0, sizeof(s8Buf)); 
	memcpy(s8Buf, com_para, M_MIN(com_para - data_buff, sizeof(s8Buf) - 1)); //结束符
	GsmAudioRecordRxedNewDatBlockInfo((GsmTpU8)(datalen), (GsmTpU16)atoi(s8Buf)); 
}
void app_gsm_ManIpDataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	ATLib_gsm_CIPRXGETDATA();
}

