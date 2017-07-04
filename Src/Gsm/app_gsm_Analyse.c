/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: GSM  数据解析
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#include "app_gsm_includes.h"

typedef void (*retInfo_analyse_t)(GsmTpU8 *data_buff, GsmTpU16 data_len);
typedef struct
{
	const GsmTpS8 *ret_info;
	retInfo_analyse_t analyse;
}atRetInfo_analyse_menu_t;

GsmTpU8 IP_openfail_cnt=0;

static void app_gsm_CLIPHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
static void app_gsmCENGHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 

#define MAX_AT_RET_INFO_ANLY_ITEM 	(sizeof(AtRetInfo_analyse_menu) / sizeof(atRetInfo_analyse_menu_t))//17
#define GSM_CSQ_UNKONW_VALUE		99
#define HAND_PHONE_NUM_LEN_FOR_SMS	20

static const atRetInfo_analyse_menu_t AtRetInfo_analyse_menu[] =
{
	{"+CSQ:", app_gsm_SignalVal},
	{"+CMT:",app_gsm_CMTHandler},
	{"+CMTI:",app_gsm_CMTIHandler},
	{"+CBM:", app_gsm_CMGRHandler},
	{"NO CARRIER", app_gsm_NOCARRIERHandler},
	{"NO ANSWER", app_gsm_NOANSWERHandler},
	{"BUSY", app_gsm_BUSYHandler},
	{"OK",app_gsm_OKHandler},
	{"+CLIP:", app_gsm_CLIPHandler},
	{"+CMGR:", app_gsm_CMGRHandler}, 	
	{"+CMGL:", app_gsm_CMGRHandler}, 	
	{"+CENG", app_gsmCENGHandler},	
};

extern GsmTpBool SmsIsRxed(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
void app_gsm_AtRetInfoAnalyse(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU16 u16Idx;
	GsmTpU16 u16Len; 
	GsmTpBool boIsExedFlg; 
	M_ClrBit(boIsExedFlg); 

	if(IS_TRUE_BIT(SmsIsRxed(data_buff, data_len))) {; 
	}
	else {
		for(u16Idx = 0; u16Idx < MAX_AT_RET_INFO_ANLY_ITEM; u16Idx++) {
			u16Len = strlen(AtRetInfo_analyse_menu[u16Idx].ret_info); 
			if(data_len < u16Len) {; 
			}
			else {
				if(IS_FALSE_BIT(GsmMemIsEqu(data_buff, 
											(GsmTpU8 *)AtRetInfo_analyse_menu[u16Idx].ret_info, 
											u16Len))) {; 
				}
				else {
					AtRetInfo_analyse_menu[u16Idx].analyse(data_buff, data_len);
					M_SetBit(boIsExedFlg); 
					break;
				}
			}
		}

		if(IS_TRUE_BIT(boIsExedFlg)) {; 
		}
		else {
			GsmAtAnalyseExpandProc(data_buff, data_len); 
		}
	}
}





/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/

GsmTpBool app_gsm_SmsInfoDecode(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	sms_info_t sms_info;
	GsmTpU8 *sms_buff;
	GsmTpS16 sms_len;
	GsmTpS16 pDCS;
	GsmTpU8 sms_timestr[16];
	GsmTpBool boRetVal; 

	M_ClrBit(boRetVal); 

	memset((GsmTpU8*)&sms_info, 0, sizeof(sms_info_t));
	sms_buff = GsmMalloc(MAX_SMS_SIZE);
	if(IS_NULL_P(sms_buff)) {; 
		M_ClrBit(boRetVal); 
		GsmDebugString("Sms decode can't malloc!\n");
		GsmMallocErrHook();//此处会重启,保证SMS的可操作性
	}
	else {
		sms_len = GsmPDUDecode(NULL, sms_info.head.num, sms_timestr, sms_buff, &pDCS, data_buff,data_len);
		if(sms_len < 0) {
			GsmDebugString("Sms decode err!\n");
		}
		else {
			if(IS_FALSE_BIT(sms_info.head.num[sizeof(sms_info.head.num) - 1])) {; 
			}//防止可能的长度超长
			else {
				M_ClrBit(sms_info.head.num[sizeof(sms_info.head.num) - 1]); 
			}
			sms_info.head.status = SMS_ST_NRD;
			sms_info.head.len = sms_len;
			sms_info.sms_info = sms_buff;
			GsmAscii_2_integer((GsmTpS8*)sms_timestr,(GsmTpS8*)&sms_info.head.datetime,12);

			GsmRxedSmsHook(&sms_info, pDCS); 
		}		
		
		GsmFree(sms_buff,MAX_SMS_SIZE);
		
	}


	return (boRetVal); 
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
void app_gsm_NOCARRIERHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	app_gsm_PhoneReqHangUp();
	gsm_oper_info.flgs.phone_dial = GsmTpFALSE;
	GsmPhoneNoCarreyHook(); 

}

void app_gsm_NOANSWERHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	app_gsm_PhoneReqHangUp(); 

	GsmDriverEnterCritical(); 
	gsm_oper_info.spked_time = 0;
	gsm_oper_info.flgs.phone_dial = GsmTpFALSE;
	gsm_oper_info.phone.status = PHONE_STATUS_IDLE;
	GsmDriverExitCriticical(); 

	GsmPhoneNoAnswerHook(); 
}

void app_gsm_BUSYHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	app_gsm_PhoneReqHangUp(); 
	GsmDriverEnterCritical(); 
	gsm_oper_info.spked_time = 0;
	gsm_oper_info.flgs.phone_dial = GsmTpFALSE;
	GsmDriverExitCriticical(); 
	GsmPhoneBusyHook(); 
	gsm_oper_info.phone.attri = PHONE_ATTRI_NULL;
}

void app_gsm_OKHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{	
	if(gsm_oper_info.phone.status != PHONE_STATUS_LINK) {;
	}
	else {
		GsmDriverEnterCritical(); 
		gsm_oper_info.phone.status = PHONE_STATUS_SPKG;
		gsm_oper_info.spked_time = 0;
		GsmDriverExitCriticical(); 
		
		GsmPhoneChg2TalkHook(); 
	}
}

static void app_gsm_CLIPHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU8 phone_num[GSM_PHONE_STR_LEN + 1]={0};
	GsmTpU8 len;

	if(gsm_oper_info.phone.status != PHONE_STATUS_RING) {
		gsm_oper_info.phone.ringtime = 0;
	}
	gsm_oper_info.phone.ringtime++;
	gsm_oper_info.phone.attri = PHONE_ATTRI_NULL;
	data_buff = (GsmTpU8*)strchr((const GsmTpS8*)data_buff, '"') + 1;
	len = strchr((const GsmTpS8*)data_buff,'"') -(GsmTpS8*)data_buff;
	if(len < GSM_PHONE_STR_LEN) {; 
	}
	else {
		GsmDebugString("phone num too lng!\n");
		len = GSM_PHONE_STR_LEN; 
	}
	memcpy(phone_num, data_buff, len);
	phone_num[len] = 0;
	GsmDriverEnterCritical(); 
	strcpy((GsmTpS8*)gsm_oper_info.phone.num, (const GsmTpS8*)phone_num);
	gsm_oper_info.spked_time = 0;
	gsm_oper_info.flgs.phone_dial = GsmTpFALSE;
	gsm_oper_info.phone.status = PHONE_STATUS_RING;
	GsmDriverExitCriticical(); 

	GsmPhoneNumRingHook((GsmTpS8 *)phone_num, len); 
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
void app_gsm_SignalVal(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU8 u8Csq;
	GsmTpU8 *pu8Tmp; 
	GsmTpU8 u8Len; 
	u8Len = sizeof("+CSQ: ") - 1; 
	if(IS_FALSE_BIT(GsmMemIsEqu(data_buff, "+CSQ: ", u8Len))) {; 
	}
	else {
		pu8Tmp = data_buff + u8Len; 
		u8Csq = atoi((const char*)pu8Tmp);
		if(GSM_CSQ_UNKONW_VALUE != u8Csq) {;
		}//未知信号量
		else {
			M_ClrBit(u8Csq); 
		}
		gsm_oper_info.Csq_val = u8Csq;
	}
	
}


void app_gsm_FlowCtlOfflHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmGprsFlowOffHook(); 
}

void app_gsm_FlowCtlOnlHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmGprsFlowOnHook(); 
}

void GsmNetBreakProc(void) 
{
	GsmDriverEnterCritical();  
	M_SetBit(gsm_oper_info.flgs.Ipclose); 
	GsmGprsRstReq(); 
	GsmGprsNgSet(); 
	GsmDriverExitCriticical(); 
}

GsmCellTowerInfoSt stGsmCellTowerInfo;
extern GsmCellTowerInfoProcSt stGsmCellTowerInfoProc;
extern void LbsLocationReqClr(); 
u32 char_10_to_int(s8 * ps8Str)
{
	u32 u32ConvertVal = 0;
	if(NULL==ps8Str)
	{
		u32ConvertVal = 0;
	}
	else
	{
		while(*ps8Str != '\0')
		{
			u32ConvertVal = u32ConvertVal *10 + (u32)(*ps8Str - '0');
			ps8Str++;
		}
	}

	return u32ConvertVal;
}
u32 char_16_to_int(s8 * ps8Str)
{
	u32 u32ConvertVal = 0;
	if(NULL==ps8Str)
	{
		u32ConvertVal = 0;
	}
	else
	{
		while(*ps8Str != '\0')
		{
			if (*ps8Str<='9' && *ps8Str>='0')
			{
				u32ConvertVal = u32ConvertVal *16 + (u32)(*ps8Str - '0');
			}
			else if (*ps8Str<='f' && *ps8Str>='a')
			{
				u32ConvertVal = u32ConvertVal *16 + (u32)(*ps8Str - 'a')+10;
			}
			else if (*ps8Str<='F' && *ps8Str>='A')
			{
				u32ConvertVal = u32ConvertVal *16 + (u32)(*ps8Str - 'A')+10;
			}
			ps8Str++;
		}
	}

	return u32ConvertVal;
}
u8 MyStrstr(const u8 *pt,const u8 *pd)
{
	u8 m=0,n=0;
	
	do
	{
		if(*pt==*pd)
		{
			pt++;
			pd++;
			m++;
			n++;
		}
		else
		{
			if(*pt == '\0')
			{
				return(m);
			}
			else
			{
				pt-=n;
				pd-=n;
				m-=n;
				n=0;
				pd++;
				m++;
			}
		}
	}
	while(m < 50);
	
	if(*pt == '\0')
	{
		return(m);
	}
	return(0);
}

static void app_gsmCENGHandler(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU8 u8CmpRet;
	GsmTpS8 * s8TempAddr;
	CellTowerInfoSt stCellTowerTemp;
	GsmTpU8 u8i = 0;
	
	M_ClrBit(u8CmpRet);

	if(data_len < 15)
	{
		M_ClrBit(u8CmpRet);
	}
	else if(IS_NULL_P(strstr((GsmTpU8 *)data_buff, "+CENG:")))
	{
		M_ClrBit(u8CmpRet);
	}
	else
	{	
		data_buff = data_buff + MyStrstr("+CENG:", (GsmTpU8 *)data_buff);
		data_buff++;

		strtok(data_buff, "\"");
		strtok(NULL, ",");	// arfcn
//		strtok(NULL, ",");	// rxl
		
		if(*data_buff < '0')
		{
			M_ClrBit(u8CmpRet);
		}
		else if('0' == *data_buff)
		{
			u8i = 0;

			s8TempAddr = strtok(NULL, ",");	// rxl
			stCellTowerTemp.stCellTower[0].s8SignalStrength = char_10_to_int(s8TempAddr) - 113;
			
			s8TempAddr = strtok(NULL, ",");	// rxq
			
			s8TempAddr = strtok(NULL, ",");	// mcc
			stCellTowerTemp.stCellTower[0].u16MobileCountryCode = char_10_to_int(s8TempAddr);
			s8TempAddr = strtok(NULL, ",");	// mnc
			stCellTowerTemp.stCellTower[0].s8MobileNetworkCode = char_10_to_int(s8TempAddr);
			strtok(NULL, ",");	// bsic
			s8TempAddr = strtok(NULL, ",");	// cellid
			stCellTowerTemp.stCellTower[0].s32CellId = char_16_to_int(s8TempAddr);
			strtok(NULL, ",");	// rla
			strtok(NULL, ",");	// txp
			s8TempAddr = strtok(NULL, ",");	// lac
			stCellTowerTemp.stCellTower[0].u16LocAreaCode = char_16_to_int(s8TempAddr);
			s8TempAddr = strtok(NULL, "\"");	// ta
			stCellTowerTemp.stCellTower[0].u16TimingAdvance = char_10_to_int(s8TempAddr);
			stCellTowerTemp.stCellTower[0].s32Age = 0;
			M_SetBit(u8CmpRet);
		}
		else if(*data_buff >= ('0' + MAX_CELL_TOWER_NUM ))
		{
			M_ClrBit(u8CmpRet);
		}
		else
		{
			u8i = *data_buff - '0';

			s8TempAddr = strtok(NULL, ",");	// rxl
			stCellTowerTemp.stCellTower[u8i].s8SignalStrength = char_10_to_int(s8TempAddr) -113;
			strtok(NULL, ",");	// bsic
			s8TempAddr = strtok(NULL, ",");	// cellid
			stCellTowerTemp.stCellTower[u8i].s32CellId = char_16_to_int(s8TempAddr);
			s8TempAddr = strtok(NULL, ",");	// mcc
			stCellTowerTemp.stCellTower[u8i].u16MobileCountryCode = char_10_to_int(s8TempAddr);
			s8TempAddr = strtok(NULL, ",");	// mnc
			stCellTowerTemp.stCellTower[u8i].s8MobileNetworkCode = char_10_to_int(s8TempAddr);
			s8TempAddr = strtok(NULL, "\"");	// lac
			stCellTowerTemp.stCellTower[u8i].u16LocAreaCode = char_16_to_int(s8TempAddr);
			
			stCellTowerTemp.stCellTower[u8i].s32Age = 0;
			stCellTowerTemp.stCellTower[u8i].u16TimingAdvance = 0;
			M_SetBit(u8CmpRet);
		}			
	}

	if(IS_TRUE_BIT(u8CmpRet))
	{
		memset((void *)(&stGsmCellTowerInfo.stCellTowerInfo.stCellTower[u8i]), 0, sizeof(CellTowerSt));
		memcpy((void *)(&stGsmCellTowerInfo.stCellTowerInfo.stCellTower[u8i]), (void *)(&stCellTowerTemp.stCellTower[u8i]), sizeof(CellTowerSt));
		
		if(IS_NULL_P(stGsmCellTowerInfoProc.GsmCellTowerInfoProc))
		{
			NULL;
		}
		else
		{
			if(MAX_CELL_TOWER_NUM == (u8i + 1))
			{
				if(IS_NULL_P(stGsmCellTowerInfoProc.GsmCellTowerInfoProc)) {;
				}
				else {
					stGsmCellTowerInfoProc.GsmCellTowerInfoProc(&stGsmCellTowerInfo.stCellTowerInfo);
				}
 				LbsLocationReqClr(); 
				
				for(u8i = 0;u8i < MAX_CELL_TOWER_NUM;u8i++)
				{
					memset((void *)(&stGsmCellTowerInfo.stCellTowerInfo.stCellTower[u8i]), 0, sizeof(CellTowerSt));
				}
			}
			else
			{
				;
			}
		}
		if(stGsmCellTowerInfo.stCellTowerInfo.stCellTower[0].s32CellId != stCellTowerTemp.stCellTower[0].s32CellId)
		{
			M_SetBit(stGsmCellTowerInfo.bCellIdChanged);
		}
		else
		{

		}
	}
	else
	{
		NULL;
	}
#if 0
	u8 *ceng_buff;
	static u16 u16LastAreaCode = 0;
	static u16 u16LastCellID = 0;

	ceng_buff = buff + MyStrstr("+CENG:", (u8 *)buff);
	ceng_buff++;
	if('0' == *ceng_buff)
	{
		u8 temp = 0;
		u16 id = 0;
		while(temp < 7)
		{
			if(',' == *ceng_buff)
			{
				temp++;
			}
			ceng_buff++;
		}
		while(*ceng_buff != ',')
		{
			id *= 0x10;
			if(*ceng_buff >= 'a')
			{
				id += *ceng_buff + 0x0A - 'a';
			}
			else if(*ceng_buff >= 'A')
			{
				id += *ceng_buff + 0x0A - 'A';
			}
			else
			{
				id += *ceng_buff - '0';
			}
			ceng_buff++;
		}

		stGsmCellTowerInfo.u16CellId = id;
		temp = 0;
		while(temp < 3)
		{
			if(',' == *ceng_buff)
			{
				temp++;
			}
			ceng_buff++;
		}
		id = 0;
		while(*ceng_buff != ',')
		{
			id *= 0x10;
			if(*ceng_buff >= 'a')
			{
				id += *ceng_buff + 0x0A - 'a';
			}
			else if(*ceng_buff >= 'A')
			{
				id += *ceng_buff + 0x0A - 'A';
			}
			else
			{
				id += *ceng_buff - '0';
			}
			ceng_buff++;
		}

		stGsmCellTowerInfo.u16BaseId = id;
	}

	if((stGsmCellTowerInfo.u16BaseId != u16LastAreaCode) || (stGsmCellTowerInfo.u16CellId))
	{
		u16LastAreaCode = stGsmCellTowerInfo.u16BaseId;
		u16LastCellID = stGsmCellTowerInfo.u16CellId;
		M_SetBit(stGsmCellTowerInfo.bCellIdChanged);
		debugPrintfString("Cell id has changed!");
	}
	else
	{
		M_ClrBit(stGsmCellTowerInfo.bCellIdChanged);
	}

	if(IS_NULL_P(stGsmCellTowerInfoProc.GsmCellTowerInfoProc))
	{
		NULL;
	}
	else
	{
		stGsmCellTowerInfoProc.GsmCellTowerInfoProc(&stGsmCellTowerInfo);
		stGsmCellTowerInfoProc.GsmCellTowerInfoProc = NULL;
	}
#endif
}

