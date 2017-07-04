/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: 功能操作中心
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#include "Includes.h"
#include "app_gsm_includes.h"
#include "MyQueue.h"
#include "Main.h"
GsmIpDatTxBufSt stGsmIpDatTxBuf; 

typedef GsmTpU8 (*AT_CMD)(void);
static GsmTpU8 app_gsm_GsmRegInquire(void);

gsm_oper_t gsm_oper_info;

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
#define CPIN_INIT_ABNORMAL_TIME_MAX	(4)
GsmTpU8 sim_card_check_OK(void)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8InitRet;
	GsmTpU8 u8Cnt;

	M_ClrBit(u8RetVal);
	M_ClrBit(u8InitRet);	
	GsmDriverSleep(3000);
	for(u8Cnt=0; u8Cnt<CPIN_INIT_ABNORMAL_TIME_MAX; u8Cnt++) {
		if(IS_FALSE_BIT(ATLib_gsm_CPIN())) {
			GsmDebugString("Not insert sim card\r\n");
			ATLib_gsm_CFUN(0);
			ATLib_gsm_CFUN(1);
			GsmDriverSleep(3000);
			M_ClrBit(u8InitRet);
		}
		else	{
			M_SetBit(u8InitRet);
			break;
		}
	}

	if(IS_FALSE_BIT(u8InitRet)) {
		GsmDebugString("CPIN Fail\r\n");
		M_ClrBit(u8RetVal);
	}
	else {
		GsmDebugString("CPIN Ok\r\n");
		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

static const AT_CMD SIM800_gsm_init[] =
{
	ATLib_gsm_ATE0,
	ATLib_gsm_CGMR,
	ATLib_gsm_CGSN,
	sim_card_check_OK,
	app_gsmAT_CIMI,
	ATLib_gsm_CENG,
	ATLib_gsm_CREG0,
	ATLib_gsm_CMEE,
	ATLib_gsm_CLIP,
	ATLib_gsm_CMGF,
	ATLib_gsm_CNMI,
	ATLib_gsm_MAVOL2,
	ATLib_gsm_CFGRI_TCP,
//	ATLib_gsm_COPS,
//	ATLib_gsm_CCID, 
//	ATLib_gsm_MOVAL_ForInit, 
};
#define MAX_GSM_INITCMD_ITEM   (sizeof(SIM800_gsm_init) / sizeof(AT_CMD))

static const AT_CMD SIM800_gprs_init[] =
{
	//ATLib_gsm_CIPRXGET,//不使用手动网络数据可以去掉
	ATLib_gsm_CIPHEAD,
	ATLib_gsm_CGREG0,
	ATLib_gsm_CgattEnable,
	ATLib_gsm_CGATT,
	ATLib_gsm_CIPSHUT,
	ATLib_gsm_CSTT,

	ATLib_gsm_CIICR, 
	ATLib_gsm_CIFSR, 
	ATLib_gsm_CIPSTART,

	ATLib_gsm_IFC,
//	ATLib_gsm_MIPCALL_EstablishLink,
//	ATLib_gsm_MIPOPEN_OpenSocket
};
#define MAX_GPRS_INITCMD_ITEM   (sizeof(SIM800_gprs_init) / sizeof(AT_CMD))


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
gsm_oper_t* app_gsm_OperPtr(void)
{
	return(&gsm_oper_info);
}

void app_gsm_OperInfoInit(void)
{
	GsmOperInitHook(); 
	gsm_oper_info.phone.vol_adj = GsmGetParaVolume(); 
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_GsmInit(void)
{
	GsmTpU8 cmd_index;
	GsmTpBool boRetVal; 

	M_SetBit(boRetVal); 
	for(cmd_index = 0; cmd_index < MAX_GSM_INITCMD_ITEM; cmd_index++) {
		if(IS_FALSE_BIT(SIM800_gsm_init[cmd_index]())) {
			M_ClrBit(boRetVal); 
			break; 
		}
		else {			
			if(SIM800_gsm_init[cmd_index] != ATLib_gsm_CREG0) {; 
			}
			else {
				if(IS_TRUE_BIT(app_gsm_GsmRegInquire())) {; 
				}
				else {
					M_ClrBit(boRetVal); 
					break; 
				}
			}
		}
	}
	
	if(IS_FALSE_BIT(boRetVal)) {; 
	}
	else {
		ATLib_gsm_CSQ1();
		ATLib_gsm_CMGL(); 
		ATLib_gsm_Wait(); 
		ATLib_gsm_Wait(); 
		ATLib_gsm_Wait(); 
		ATLib_gsm_CMGD_All(); 
		ATLib_gsm_Wait(); 
		ATLib_gsm_Wait(); 
//		ATLib_gsm_VoiceRecordQuery(); 
	}

	return(boRetVal);
}

#define GSM_INIT_WITHOUT_OPEN_CPU	(1)
/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 GsmInitManager(void)
{
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);

#if GSM_INIT_WITHOUT_OPEN_CPU
	GsmDriverDevModRst();
#endif

	//GsmDriverSleep(6000);
	if(IS_FALSE_BIT(app_gsm_GsmInit())) {
		M_ClrBit(u8RetVal);
	}
	else	{
		GsmInitOkHook();
		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
#define GSM_INIT_ABNORMAL_TIME_MAX	(3)
GsmTpU8 GsmInit(void)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8InitRet;
	GsmTpU8 u8Cnt;

	M_ClrBit(u8RetVal);
	M_ClrBit(u8InitRet);
	//不复位整个系统，这里重试3次就无意义，反而导致这里的执行时间过长
	//for(u8Cnt=0; u8Cnt<GSM_INIT_ABNORMAL_TIME_MAX; u8Cnt++) 
	{
		if(IS_FALSE_BIT(GsmInitManager())) {
			M_ClrBit(u8InitRet);
		}
		else	{
			M_SetBit(u8InitRet);
			//break;
		}
	}

	if(IS_FALSE_BIT(u8InitRet)) {
		GsmDebugString("Gsm Init Fail\r\n");
		GsmBspSysReset();
		M_ClrBit(u8RetVal);
	}
	else {
		GsmDebugString("Gsm Init Ok\r\n"); 
		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_GprsInit(void)
{
	GsmTpU8 cmd_index;
	GsmTpBool boRetVal; 
	M_SetBit(boRetVal); 
	for(cmd_index = 0; cmd_index < MAX_GPRS_INITCMD_ITEM; cmd_index++) {
		if(IS_TRUE_BIT(SIM800_gprs_init[cmd_index]())) {; 
		}
		else {	
			if(SIM800_gprs_init[cmd_index] != ATLib_gsm_MIPCALL_EstablishLink) {; 
			}
			else {
				ATLib_gsm_MIPCALL_DisconnectLink();
			}
			M_ClrBit(boRetVal); 
			break; 
		}

		if(ATLib_gsm_CGREG0 != SIM800_gprs_init[cmd_index]) {; 
		}
		else	{
			if(IS_TRUE_BIT(app_gsm_GprsRegInquire())) {
			}
			else {
				M_ClrBit(boRetVal); 
				break; 
			}
		}
	}

	return(boRetVal);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_WaitModuleReady(void)
{
extern const GsmTpS8 cs8GsmPwrOnStr[]; 
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal);
	boRetVal = app_gsm_WaitATCmdAck(NULL, cs8GsmPwrOnStr, NULL, 5); 
	if(IS_FALSE_BIT(boRetVal)) {; 
		GsmDebugString("Gsm Error,Maybe switch is OFF\r\n"); 
	}
	else	{			
		GsmDriverSleep(2000);
	}
	
	return(boRetVal);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/

/*********!!!!!!!!!!!!!!!注意,防止第一个参数造成的内存泄露**********/
GsmTpU8 app_gsm_WaitATCmdAck(Gsmmyevent *ret_str, const GsmTpS8 *str_1, const GsmTpS8 *str_2, GsmTpU8 Timeout)
{
	Gsmmyevent event;
	GsmTpU16 u16WaitTime;
	GsmTpBool boRetVal; 
	GsmTpBool boNeedBreakFlg; 

	M_ClrBit(boRetVal); 
	M_ClrBit(boNeedBreakFlg); 
	if((str_1 == NULL) && (str_2 == NULL)) {
		GsmDriverSleep(200);
		return(GsmTpTRUE);
	}
	
	for(u16WaitTime = 0; u16WaitTime < (10 * Timeout); u16WaitTime++) {
		GsmTpBool boRxCmdFlg; 
		GsmTpU8 *pu8Dat; 
		GsmTpU16 u16Len; 
		
		boRxCmdFlg = GsmDriverGetDevRxDatPend(100, &pu8Dat, &u16Len); 
		if(IS_FALSE_BIT(boRxCmdFlg)) {; 
		}
		else	{
			event.buff = pu8Dat; 
			event.size = u16Len; 

			if ((event.buff[0]>= '0' && event.buff[0]<= '9') || (event.buff[0]>= 'a' && event.buff[0]<= 'f')
				|| (event.buff[0]>= 'A' && event.buff[0]<= 'F'))
			{
				//app_waitCCID(event.buff,event.size);

				if(event.buff[0]>= '0' && event.buff[0]<= '9')
				{
					app_waitIMEI(event.buff,event.size);				
					app_waitIMSI(event.buff,event.size);
				}
			}
			
			app_gsm_AtRetInfoAnalyse(event.buff,event.size); 			

			//-----------------------------------------------------
			if(((str_1 != NULL) && strstr((const GsmTpS8*)event.buff,str_1))
			|| ((str_2 != NULL) && strstr((const GsmTpS8*)event.buff,str_2))) {
				M_SetBit(boRetVal); 
				M_SetBit(boNeedBreakFlg); 
				if(NULL == ret_str) {;
				}
				else	{
					Gsmmyevent stEve; 
					stEve.size = event.size; 
					stEve.buff = GsmMalloc(stEve.size); 
					if(NULL == stEve.buff) {
						GsmDebugString("Gsm ack dat cann't malloc\r\n"); 
						M_ClrBit(boRetVal); 
					}
					else {
						memcpy(stEve.buff, event.buff, stEve.size); 
						ret_str->buff = stEve.buff;
						ret_str->size = stEve.size;
					}
				}
			}
			else if(strstr((const GsmTpS8*)event.buff,"ERROR")) {
				M_ClrBit(boRetVal); 
				M_SetBit(boNeedBreakFlg); 
			}
			else if(strstr((const GsmTpS8*)event.buff,"RING")) {
				M_ClrBit(boRetVal); //退出以处理电话
				M_SetBit(boNeedBreakFlg); 
			}
			else {; 
			}

			GsmDriverGetDevRxDatMemFreeIfNeed(pu8Dat, u16Len); 
			M_ClrBit(boRxCmdFlg); 

			if(IS_FALSE_BIT(boNeedBreakFlg)) {; 
			}
			else {
				break; 
			}			
		}		
	}
	
	return(boRetVal);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
static GsmTpU8 app_gsm_GsmRegInquire(void)
{
	Gsmmyevent event;
	GsmTpU8 inq_cnt;
	
	for(inq_cnt = 0; inq_cnt < 60; inq_cnt ++) /*2minutes.*/
	{
		ATLib_gsm_CREG1();
		if(app_gsm_WaitATCmdAck(&event,"+CREG:",NULL,5))
		{
			if(strstr((const GsmTpS8*)event.buff,"+CREG: 1,1")
			||strstr((const GsmTpS8*)event.buff,"+CREG: 1,5"))
			{
				GsmFree(event.buff,event.size);
				return(GsmTpTRUE);
			}
			GsmFree(event.buff,event.size);
		}
		if((inq_cnt %5) == 0)
		{
			ATLib_gsm_CSQ1();
		}
		GsmDriverSleep(2000);
	}
	return(GsmTpFALSE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_GprsRegInquire(void)
{
	Gsmmyevent event;
	GsmTpU8 inq_cnt;

	for(inq_cnt = 0; inq_cnt < 20; inq_cnt ++)
	{
		if(IS_FALSE_BIT(GsmPhoneIsIdle()))
		{
			return(GsmTpFALSE);
		}
		
		ATLib_gsm_CGREG1();
		event.buff = NULL; 
		if(app_gsm_WaitATCmdAck(&event,"+CGREG:",NULL,5))
		{
			if(strstr((const GsmTpS8*)event.buff,"+CGREG: 1,1")
			||strstr((const GsmTpS8*)event.buff,"+CGREG: 1,5"))
			{
				GsmFree(event.buff,event.size);
				app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3); 
				return(GsmTpTRUE);
			}
			GsmFree(event.buff,event.size);
		}
		else {
			if(IS_NULL_P(event.buff)) {; 
			}
			else {
				GsmFree(event.buff,event.size);
			}
		}
		if((inq_cnt %5) == 0)
		{
			ATLib_gsm_CSQ1();
		}
		GsmDriverSleep(2000);
	}
	return(GsmTpFALSE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
#define CSQ_INQ_PERIOD  10
void app_gsm_CSQInquire(void)
{	
	if(IS_FALSE_BIT(GsmPhoneIsIdle())) {; 
	}
	else {
		static GsmTpU32 su32CsqTimerBk; 
		static GsmTpU8 su8SecSBk; 
		GsmTpU16 u16SecS; 
		u16SecS = GsmSysGetDurationSecTimer(&su32CsqTimerBk); 
		if((su8SecSBk + u16SecS) < (CSQ_INQ_PERIOD)) {
			su8SecSBk += (GsmTpU8)u16SecS; 
		}
		else {
			M_ClrBit(su8SecSBk); 
			ATLib_gsm_CSQ1();
		}
	}
}


/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/

void app_gsm_CloseModule(void)
{
	if(IS_FALSE_BIT(GsmPhoneIsIdle()))
	{
		app_gsm_PhoneReqHangUp();
	}
	if(GsmGprsIsOk())
	{
		GsmDriverEnterCritical(); 
		GsmGprsNgSet();
		gsm_oper_info.flgs.Ipclose = GsmTpTRUE;
		GsmDriverExitCriticical(); 
		app_gprs_SocketCloseHandler();
	}
	
	GsmBspGsmPwrOff();
	GsmDriverEnterCritical(); 
	gsm_oper_info.Csq_val = 0;
	GsmDriverExitCriticical(); 
}

GsmTpU8 app_gsm_PhoneNumReqToDial(GsmTpU8 *num, GsmTpU8 attri)
{
	GsmTpBool boRetVal; 

	GsmTpU8 len = strlen((char *)num); 
	
	GsmDriverEnterCritical(); 

	if((IS_FALSE_BIT(GsmPhoneIsIdle()))
	||(len > GSM_PHONE_STR_LEN) 
	||(0 == len)
	||(NULL == num)
	)
	{
		M_ClrBit(boRetVal);
	}
	else {
		GsmTpU8 u8Tmp; 

		GsmDriverEnterCritical(); 
		u8Tmp = gsm_oper_info.phone.vol_adj; 
		memset((GsmTpU8*)&gsm_oper_info.phone,0,sizeof(gsm_phone_t));
		gsm_oper_info.phone.vol_adj = u8Tmp; 
		memcpy(gsm_oper_info.phone.num,num,len);
		gsm_oper_info.phone.num[len] = 0;
		gsm_oper_info.phone.status = PHONE_STATUS_DAIL;
		gsm_oper_info.phone.attri = (PhoneAttriEn)attri; 
		M_SetBit(gsm_oper_info.flgs.phone_dial); 
		GsmDriverExitCriticical(); 
		
		M_SetBit(boRetVal); 
	}

	GsmDriverExitCriticical(); 

	return(boRetVal);
	
}

static void app_gsm_PhoneNumDial(void)
{
	if(gsm_oper_info.phone.status != PHONE_STATUS_DAIL) {;
	}
	else	{ 
		if(IS_FALSE_BIT(ATLib_gsm_ATD(gsm_oper_info.phone.num))) {
			GsmPhoneDailFailHook(); 
			memset((GsmTpU8*)&gsm_oper_info.phone,0,sizeof(gsm_phone_t));
		}
		else	{					
			GsmPhoneDailOkHook(); 
			GsmDriverEnterCritical(); 
			M_SetBit(gsm_oper_info.flgs.phone_dial); 
			gsm_oper_info.phone.status = PHONE_STATUS_LINK;
			GsmDriverExitCriticical(); 
		}
	}
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_PhoneDTMFReqToSend(GsmTpU8 dtmf)
{
	GsmTpU8 head, tail;
	GsmTpBool boRetVal; 
	GsmDriverEnterCritical(); 
	if(IS_FALSE_BIT(GsmApiPhoneStIsSpking()))
	{
		M_ClrBit(boRetVal);
	}
	else {		
		head = gsm_oper_info.phone.dtmf.head;
		tail = gsm_oper_info.phone.dtmf.tail;
		if(((head +1) % PHONE_DTMF_MAX) == tail) {
			M_ClrBit(boRetVal);
		}
		else {
			gsm_oper_info.phone.dtmf.key_val[head++] = dtmf;
			head %= PHONE_DTMF_MAX;
			gsm_oper_info.phone.dtmf.head = head;
			
			M_SetBit(boRetVal);
		}		
	}
	GsmDriverExitCriticical(); 
	
	return boRetVal; 
}
static void app_gsm_PhoneDTMFSend(void)
{
	GsmTpU8 dtmf;
	
	if((gsm_oper_info.phone.dtmf.head == gsm_oper_info.phone.dtmf.tail)
	|| (IS_FALSE_BIT(GsmApiPhoneStIsSpking()))) {; 
	}
	else	{
		dtmf = gsm_oper_info.phone.dtmf.key_val[gsm_oper_info.phone.dtmf.tail++];
		gsm_oper_info.phone.dtmf.tail %= PHONE_DTMF_MAX;
		ATLib_gsm_VTS(dtmf);
	}
	
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 app_gsm_PhoneVolReqAdj(enum GsmPhoneVolOprEn VolAdjOpr)
{
	if((IS_FALSE_BIT(GsmApiPhoneStIsSpking()))
	&& (gsm_oper_info.phone.status != PHONE_STATUS_RING)
	&& (gsm_oper_info.phone.status != PHONE_STATUS_LINK))
	{
		return(GsmTpFALSE);
	}

	GsmDriverEnterCritical(); 
	
	if(GSM_PHONE_VOL_UP == VolAdjOpr) {
		gsm_oper_info.phone.vol_adj < GsmGetMaxVolumeLevel() 
			? gsm_oper_info.phone.vol_adj++
			: 0;
	}
	else if(GSM_PHONE_VOL_DW == VolAdjOpr)
	{
		gsm_oper_info.phone.vol_adj > GsmGetMinVolumeLevel() 
			? gsm_oper_info.phone.vol_adj--
			: 0;
	}
	else {;
	}
	
	GsmDriverExitCriticical(); 
	return(GsmTpTRUE);
}
static void app_gsm_PhoneVolAdj(void)
{
	GsmTpU8 volume;
	
	if((gsm_oper_info.phone.vol_adj == GsmGetParaVolume())
	|| (gsm_oper_info.phone.status < PHONE_STATUS_LINK)
	|| (PHONE_ATTRI_LISTEN == gsm_oper_info.phone.attri)) {;
	}
	else {
		volume = gsm_oper_info.phone.vol_adj;
		GsmSaveParaVolume(volume); 		
		ATLib_gsm_MAVOL(volume); 
	}

}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/
void app_gsm_PhoneReqHangUp(void)
{
	GsmDriverEnterCritical(); 
	GsmBspAmpPwrOff();
	M_SetBit(gsm_oper_info.phone.flg.hang_up); 
	GsmDriverExitCriticical(); 
}
static void app_gsm_PhoneHangUp(void)
{
	if(IS_FALSE_BIT(gsm_oper_info.phone.flg.hang_up)) {; 
	}
	else	{
		if(IS_FALSE_BIT(ATLib_gsm_ATH())) {;
		}
		else	{			
			GsmDriverEnterCritical(); 
			M_ClrBit(gsm_oper_info.phone.flg.hang_up);
			gsm_oper_info.phone.status = PHONE_STATUS_IDLE;
			GsmDriverExitCriticical(); 

			GsmPhoneHangupHook(); 
		}	
	}

}


void app_gsm_PhoneReqPickUp(void)
{
	if(gsm_oper_info.phone.status != PHONE_STATUS_RING) {; 
	}
	else {
		GsmDriverEnterCritical(); 
		M_SetBit(gsm_oper_info.phone.flg.pick_up); 
		GsmDriverExitCriticical(); 
	}
}
static void app_gsm_PhonePickUp(void)
{
	if((gsm_oper_info.phone.status != PHONE_STATUS_RING)
	|| IS_FALSE_BIT(gsm_oper_info.phone.flg.pick_up)) {;
	}
	else {
		if(IS_FALSE_BIT(ATLib_gsm_ATA())) {
			GsmPhoneNoCarreyHook(); 
		}
		else {
			GsmDriverEnterCritical(); 
			gsm_oper_info.phone.status = PHONE_STATUS_SPKG;
			M_ClrBit(gsm_oper_info.phone.flg.pick_up); 
			GsmDriverExitCriticical(); 	
			GsmPhonePickUpHook(); 
			GsmPhoneChg2TalkHook();
		}
	}
}


void app_gsm_PhoneHandler(void)
{
	app_gsm_PhoneHangUp();
	app_gsm_PhonePickUp();
	app_gsm_PhoneNumDial();
	app_gsm_PhoneDTMFSend();
	app_gsm_PhoneVolAdj();
	app_gsm_PhoneChlSwitch();
	GsmPhoneDailTimer(); 
}
GsmTpU8 app_gsm_HangUpPhone(void)
{
	GsmBspAmpPwrOff();
	ATLib_gsm_ATH();

	GsmDriverEnterCritical(); 
	gsm_oper_info.phone.attri = PHONE_ATTRI_NULL;
	gsm_oper_info.phone.status = PHONE_STATUS_IDLE;
	GsmDriverExitCriticical(); 
	
	return(GsmTpTRUE);
}



GsmTpU8 app_gsm_PhoneChlReqSwitch(void)
{
	GsmTpU8 chl_sw = GsmTpTRUE;
	
	if((IS_FALSE_BIT(GsmApiPhoneStIsSpking()))
	&& (gsm_oper_info.phone.status != PHONE_STATUS_LINK)
	&& (gsm_oper_info.phone.status != PHONE_STATUS_RING)) {
		return(GsmTpFALSE);
	}
	if(gsm_oper_info.phone.flg.chl_sw == GsmTpTRUE) {
		chl_sw = GsmTpFALSE;
	}
	gsm_oper_info.phone.flg.chl_sw = chl_sw;
	return(GsmTpTRUE);
}
void app_gsm_PhoneChlSwitch(void)
{
	if(IS_TRUE_BIT(gsm_oper_info.phone.flg.chl_sw)) {
		M_ClrBit(gsm_oper_info.phone.flg.chl_sw);
	}
	else {
		return; 
	}
	
	switch(gsm_oper_info.phone.attri)
	{
	case PHONE_ATTRI_HAND:
		gsm_oper_info.phone.attri = PHONE_ATTRI_LCD;
		ATLib_gsm_MAPATH0(2);
		GsmBspAmpPwrOn();
		break;

	case PHONE_ATTRI_PLTF:
	case PHONE_ATTRI_LCD:
		gsm_oper_info.phone.attri = PHONE_ATTRI_HAND;
		GsmBspAmpPwrOff();
		ATLib_gsm_MAPATH0(1);
		break;
	}
}

void GsmPhoneDailTimer(void)
{
	static GsmTpU32 su32GsmPhoneDailTimeBk; 
	GsmTpU16 u16SecS; 
	u16SecS = GsmSysGetDurationSecTimer(&su32GsmPhoneDailTimeBk);
	if(GsmApiPhoneStIsSpking()
	&& GsmApiPhoneIsDailOut()) {
		while(u16SecS--) {
			gsm_oper_info.spked_time++;
		}
	}
	else {; 
	}
}

void app_gsm_TapeCfg(GsmTpU8 u_freq)
{
	ATLib_gsm_AUDPM(u_freq, 1);//freqMode);
}

GsmTpU8 app_gsm_TapeStart(void)
{
	ATLib_gsm_MAPATH0(2);
	ATLib_gsm_AUDCD(1);
	return(app_gsm_WaitATCmdAck(NULL, "OK", NULL, 5));
}

GsmTpU8 app_gsm_TapeStop(void)
{
	Gsmmyevent ret_event;
		
	ATLib_gsm_AUDCD(0);
	if(app_gsm_WaitATCmdAck(&ret_event,"+ASTOP:",NULL,20))
	{
		if(NULL != strstr((const GsmTpS8*)ret_event.buff,"+ASTOP:"))
		{
			app_gsm_AudioAttriHandler(ret_event.buff,ret_event.size);
			GsmFree(ret_event.buff, ret_event.size); 
                        return GsmTpTRUE;
		}
		GsmFree(ret_event.buff, ret_event.size); 
	}
        return GsmTpFALSE;
}

GsmTpU8 app_gsm_TapeStopForOnlyStop(void)
{
	
	ATLib_gsm_AUDCD(0);
       return app_gsm_WaitATCmdAck(NULL,"OK", NULL, 6);
}

void app_gsm_TapePlay(void)
{
	GsmBspAmpPwrOn();
	ATLib_gsm_AUDCD(3);
}

void app_gsm_GetAudioAttri(void)
{
	
	ATLib_gsm_AUDRL(0);
}


void app_gsm_ReadAudioData(GsmTpU8 u8Idx, GsmTpU16 offset,GsmTpU16 read_len)
{
	if(read_len >1024)
	{
		read_len = 1024;
	}
	ATLib_gsm_AUDRD(u8Idx,offset, read_len);
}

void AlarmSmsTxEndHook(GsmTpBool boTxOkFlg)
{
	if(IS_TRUE_BIT(boTxOkFlg)) {
		GsmDebugString("Alarm Sms tx ok\r\n"); 
	}
	else {
		GsmDebugString("Alarm Sms tx fail\r\n"); 
	}
}
GsmTpU8 app_gsm_SmsMsgReqToSend(GsmTpU8 *num, GsmTpU8 num_len, GsmTpU8 *msg, GsmTpU8 msg_len, GsmTpU8 attri, GsmTpU8 pDCS)
{
	SmsTxParaSt stSmsTxPara; 
	if((NULL == num)
	||(NULL == msg)
	||(num_len > (sizeof(stSmsTxPara.stDstNum.s8Num) - 1))
	||(msg_len > SMS_MSG_MAX_LEN))
	{
		return(GsmTpFALSE);
	}

	memset(&stSmsTxPara, 0, sizeof(stSmsTxPara)); 
	memcpy(stSmsTxPara.stDstNum.s8Num, num, num_len);
	stSmsTxPara.stDstNum.u8Len = num_len; 
	stSmsTxPara.SmsTxEndHook = NULL; 
	stSmsTxPara.stSmsDat.pu8Dat = msg; 
	stSmsTxPara.stSmsDat.u16Len = msg_len; 
	stSmsTxPara.SmsTxEndHook = AlarmSmsTxEndHook; 
	return(GsmSmsTxReq(&stSmsTxPara));
}


GsmTpBool GsmSmsTxReq(SmsTxParaSt *pstTxPara)
{
	GsmTpBool boRetVal; 
	Gsmmyevent stEve; 
	M_ClrBit(boRetVal); 
	if(IS_NULL_P(pstTxPara)) {; 
		M_ClrBit(boRetVal); 
	}
	else {
		stEve.size = pstTxPara->stSmsDat.u16Len; 
		stEve.buff = GsmMalloc(stEve.size);
		if(IS_NULL_P(stEve.buff)) {
			GsmDebugString("Gsm sms tx req malloc err\r\n"); 
			M_ClrBit(boRetVal); 
		}
		else {			
			GsmDriverEnterCritical(); 
			if(IS_FALSE_BIT(stGsmSmsOpr.stSmsTxOpr.bSmsTxReq)
			&& IS_FALSE_BIT(stGsmSmsOpr.stSmsTxOpr.bSmsTxBusyFlg)
			&& GsmModGsmIsOk()) {
				M_SetBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxReq); 
				memcpy(&stGsmSmsOpr.stSmsTxOpr.stSmsTxPara, 
						pstTxPara, 
						sizeof(stGsmSmsOpr.stSmsTxOpr.stSmsTxPara)); 
				memcpy(stEve.buff, 
						pstTxPara->stSmsDat.pu8Dat, 
						pstTxPara->stSmsDat.u16Len);
				stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat = stEve.buff; 
				stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.u16Len = stEve.size; 
				M_SetBit(boRetVal); 
			}
			else {
				M_ClrBit(boRetVal); 
			}
			GsmDriverExitCriticical(); 

			if(IS_TRUE_BIT(boRetVal)) {
			}
			else {
				GsmFree(stEve.buff, stEve.size); 
			}
		}
	}

	return (boRetVal); 
}

void GsmSmsTxProc(void)
{
	static enum 
	{
		GSM_SMS_TX_ST_IDLE, 
		GSM_SMS_TX_ST_ENCODE, 
		GSM_SMS_TX_ST_SENDING, 
		GSM_SMS_TX_ST_END, 
	}senGsmSmsTxSt; 
	static Gsmmyevent sstEveBuf; 
	static Gsmmyevent sstSmsDat; 
	static GsmTpBool sboSmsTxOkFlg; 
	static GsmTpU32 su32GsmSmsTxTimer; 
	static GsmTpU32 su32GsmSmsTxTimeBk; 
	GsmTpU16 u16SecS; 
	u16SecS = GsmSysGetDurationSecTimer(&su32GsmSmsTxTimeBk); 
	while(u16SecS--) {
		M_NOT_ZERO_DEC(su32GsmSmsTxTimer); 
	}
	
	switch(senGsmSmsTxSt)	 {
		case GSM_SMS_TX_ST_IDLE:
			GsmDriverEnterCritical(); 
			if(IS_FALSE_BIT(stGsmSmsOpr.stSmsTxOpr.bSmsTxReq)) {; 
			}
			else {
				M_ClrBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxReq); 
				M_SetBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxBusyFlg); 
				M_ClrBit(sboSmsTxOkFlg); 
				su32GsmSmsTxTimer = 10;//10S发布到 
				senGsmSmsTxSt = GSM_SMS_TX_ST_ENCODE; 
			}
			GsmDriverExitCriticical(); 
			break; 

		case GSM_SMS_TX_ST_ENCODE:
			sstEveBuf.size = MAX_SMS_SIZE; 
			sstEveBuf.buff = GsmMalloc(sstEveBuf.size);		
			if(IS_NULL_P(sstEveBuf.buff)) {
				GsmDebugString("Sms send can't malloc!\n");
				if(IS_TRUE_BIT(su32GsmSmsTxTimer)) {; 
				}
				else {
					GsmDebugString("Sms send malloc err timeover\r\n");
					M_ClrBit(sboSmsTxOkFlg); 
					senGsmSmsTxSt = GSM_SMS_TX_ST_END; 
				}
			}
			else {
				memset(sstEveBuf.buff, 0, sstEveBuf.size);  
				sstSmsDat.buff = sstEveBuf.buff; 
				sstSmsDat.size = GsmPDUEncode(sstSmsDat.buff,NULL,
									(GsmTpU8 *)stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stDstNum.s8Num,
									stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat,
									stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.u16Len,
									8);
				senGsmSmsTxSt = GSM_SMS_TX_ST_SENDING; 
			}
			break; 

		case GSM_SMS_TX_ST_SENDING:
			if(IS_FALSE_BIT(GsmModGsmIsOk())) {; 
			}
			else {; 
				sboSmsTxOkFlg = ATLib_gsm_CMGS(sstSmsDat.buff, sstSmsDat.size);
				if(IS_TRUE_BIT(sboSmsTxOkFlg)) {
					GsmDebugString("Sms tx ok\r\n"); 
				}
				else {
					GsmDebugString("Sms tx fail\r\n"); 
				}
				GsmFree(sstEveBuf.buff, sstEveBuf.size); 
				memset(&sstEveBuf, 0, sizeof(sstEveBuf)); 
				senGsmSmsTxSt = GSM_SMS_TX_ST_END; 
			}
			break; 
			
		case GSM_SMS_TX_ST_END:			
			GsmFree(stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat, 
					stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.u16Len); 
			stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat = NULL; 
			GsmDriverEnterCritical(); 
			M_ClrBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxBusyFlg); 
			senGsmSmsTxSt = GSM_SMS_TX_ST_IDLE; 
			GsmDriverExitCriticical(); 
			if(IS_NULL_P(stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.SmsTxEndHook)) {; 
			}
			else {
				stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.SmsTxEndHook(sboSmsTxOkFlg); 
			}
			break; 

		default :
			if(IS_NULL_P(sstEveBuf.buff)) {; 
			}
			else {
				GsmFree(sstEveBuf.buff, sstEveBuf.size); 
			}
			GsmDriverEnterCritical(); 
			M_ClrBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxReq); 
			M_ClrBit(stGsmSmsOpr.stSmsTxOpr.bSmsTxBusyFlg); 
			if(IS_NULL_P(stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat)) {;
			}
			else {
				GsmFree(stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat, 
						stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.u16Len); 
				stGsmSmsOpr.stSmsTxOpr.stSmsTxPara.stSmsDat.pu8Dat = NULL; 
			}
			senGsmSmsTxSt = GSM_SMS_TX_ST_IDLE; 
			GsmDriverExitCriticical(); 
			break; 
	}
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/




/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-返回参数:	
-------------------------------------------------------------------------------------------------
*/

extern void GsmModGprsOffReq(void); 
extern void GsmModGprsOffCancel(void); 



static GsmTpU16 GsmNetOpenTimeS; 
void GsmNetOpenReq(GsmTpU16 u16TimeS)
{
	GsmDriverEnterCritical(); 
	if(GsmNetOpenTimeS > u16TimeS) {; 
	}
	else {
		GsmNetOpenTimeS = u16TimeS; 
	}
	M_SetBit(gsm_oper_info.flgs.bNetOpenReq); 
	GsmDriverExitCriticical(); 
}

void GsmNetCloseReq(GsmTpU16 u16TimeS)
{
	GsmDriverEnterCritical(); 
	M_SetBit(gsm_oper_info.flgs.bNetCloseReq); 
	GsmDriverExitCriticical(); 
}


void GsmNetOpenCloseProc(void)
{
	static GsmTpU32 su32OsTickBk; 
	GsmTpU16 u16Secs; 
	u16Secs = GsmSysGetDurationSecTimer(&su32OsTickBk); 	
	while(IS_TRUE_BIT(u16Secs--)) {
		M_NOT_ZERO_DEC(GsmNetOpenTimeS); 
	}
	
	if(IS_TRUE_BIT(gsm_oper_info.flgs.bNetCloseReq)) {
		GsmDebugString("TE close gprs!\n");
		
		GsmDriverEnterCritical(); 
		gsm_oper_info.flgs.Ipclose = GsmTpTRUE;
		M_ClrBit(gsm_oper_info.flgs.bNetCloseReq); 
		GsmModGprsOffReq(); 
		GsmDriverExitCriticical(); 
	}
	else if(IS_TRUE_BIT(gsm_oper_info.flgs.bNetOpenReq)){
			GsmDebugString("TE open gprs!\n");
			GsmDriverEnterCritical(); 
			M_ClrBit(gsm_oper_info.flgs.bNetOpenReq); 
			GsmModGprsOffCancel(); 
			GsmDriverExitCriticical(); 
		}
	else {;
	}
	}

void GsmTtsProc(void)
{
	static GsmTpU8 su8GsmTtsTimer; 
	static enum {
		GSM_TTS_ST_OFF_IDLE, 
		GSM_TTS_ST_ON_IDLE,
		GSM_TTS_PLAY_WAIT_TX_OK, 
		GSM_TTS_PLAY_WAIT_OK, 
		GSM_TTS_PLAY_END, 
	}senGsmTtsSt; 
	static GsmTpU32 su32OsTickBk; 
	GsmTpU16 u16SecS; 
	GsmTpU8 *pu8Dat; 
	GsmTpU16 u16Len; 
	
	if(GsmTtsIsEnable()
	&& GsmPhoneIsIdle()) {; 
	}
	else {
		return; 
	}
	
	u16SecS = GsmSysGetDurationSecTimer(&su32OsTickBk); 
	
	while(IS_TRUE_BIT(u16SecS--)) {
		M_NOT_ZERO_DEC(su8GsmTtsTimer); 
	}
	
	switch(senGsmTtsSt) {
		case GSM_TTS_ST_OFF_IDLE:
			if(IS_FALSE_BIT(GsmModGsmIsOk())) {; 
			}
			else { 
				senGsmTtsSt = GSM_TTS_ST_ON_IDLE;
			}
			break; 

		case GSM_TTS_ST_ON_IDLE:
			if(IS_FALSE_BIT(GsmPhoneIsIdle())) {;
			}
			else if(IS_FALSE_BIT(GsmModGsmIsOk())) {; 
			}
			else {
				if(IS_FALSE_BIT(GsmTtsGetPlayData(&pu8Dat, &u16Len))) {;
				}
				else {
						if(IS_TRUE_BIT(GsmBspAudioChannelIsBusy())) {; 
						}
						else {
							ATLib_gsm_MAPATH0(2);
							GsmBspAmpPwrOn();
							ATLib_gsm_TTS(GsmTpFALSE, NULL, 0); //先停止上一次
							ATLib_gsm_TTS(GsmTpTRUE, pu8Dat, strlen((GsmTpS8 *)pu8Dat)); //因有结束符
							M_SetBit(gsm_oper_info.flgs.bTtsBusyFlg); 
							su8GsmTtsTimer = 120; //最长播报时间
							senGsmTtsSt = GSM_TTS_PLAY_WAIT_OK;
						}
						
						GsmFree(pu8Dat, u16Len);
				}
			}
			break; 

		case GSM_TTS_PLAY_WAIT_OK:
			if(IS_FALSE_BIT(GsmPhoneIsIdle())) {
				senGsmTtsSt = GSM_TTS_PLAY_END; 
			}
			else if(IS_FALSE_BIT(gsm_oper_info.flgs.bTtsBusyFlg)) {
				senGsmTtsSt = GSM_TTS_PLAY_END; 
			}
			else if(IS_FALSE_BIT(su8GsmTtsTimer)) {
				GsmDebugString("Tts play time out\r\n"); 
				if(IS_FALSE_BIT(GsmPhoneIsIdle())) {;
				}
				else {
					GsmBspAmpPwrOff();
				}
				senGsmTtsSt = GSM_TTS_ST_OFF_IDLE; 
			}
			else {; 
			}
			break; 

		case GSM_TTS_PLAY_END:
			if(IS_FALSE_BIT(GsmPhoneIsIdle())) {;
			}
			else {
				GsmBspAmpPwrOff();
			}
			senGsmTtsSt = GSM_TTS_ST_ON_IDLE; 
			break; 

		default:
			GsmDebugString("GSM Tts fault\r\n"); 
			if(IS_FALSE_BIT(GsmPhoneIsIdle())) {;
			}
			else {
				GsmBspAmpPwrOff();
			}
			senGsmTtsSt = GSM_TTS_ST_OFF_IDLE; 
			break; 
	}
	
}


struct GsmSmsOprSt stGsmSmsOpr; 
void GsmSmsReadByPromptPro(void)
{
	static enum
	{
		GSM_SMS_RD_BY_POR_IDLE, 
		GSM_SMS_RD_BY_POR_START, 
		GSM_SMS_RD_BY_POR_DEL_RDED, 
		GSM_SMS_RD_BY_POR_END, 
		GSM_SMS_RD_BY_POR_OVERFLOW_LIST_ALL, 
		GSM_SMS_RD_BY_POR_OVERFLOW_DEL_ALL, 
	}senGsmSmsRdByPormptSt; 
	static GsmTpU16 su16Index; 
	GsmTpU32 u32Tmp; 
	if(GsmModGsmIsOk()) {; 
	}
	else {
		return; 
	}

	switch(senGsmSmsRdByPormptSt) {
		case GSM_SMS_RD_BY_POR_IDLE:
			if(IS_FALSE_BIT(GsmFifoRead(&stGsmSmsOpr.stSmsRxInfo.stSmsPromptFifo,
									(void *)&u32Tmp))) {
				if(IS_FALSE_BIT(stGsmSmsOpr.stSmsRxInfo.bSmsRxOverflowFlg)) {; 
				}
				else {
					M_ClrBit(stGsmSmsOpr.stSmsRxInfo.bSmsRxOverflowFlg); 
					senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_OVERFLOW_LIST_ALL; 
				}
			}
			else { 
				su16Index = (GsmTpU16)u32Tmp; 
				senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_START; 
			}
			break; 
			
		case GSM_SMS_RD_BY_POR_START:
			ATLib_gsm_CMGR(su16Index); 
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_DEL_RDED; 
			break; 

		case GSM_SMS_RD_BY_POR_DEL_RDED:
			ATLib_gsm_CMGD_2(su16Index, 2);//删除已读
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_END; 
			break; 

		case GSM_SMS_RD_BY_POR_END:
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_IDLE; 
			break; 

		case GSM_SMS_RD_BY_POR_OVERFLOW_LIST_ALL:
			ATLib_gsm_CMGL(); 
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_OVERFLOW_DEL_ALL; 
			break; 

		case GSM_SMS_RD_BY_POR_OVERFLOW_DEL_ALL:
			ATLib_gsm_CMGD_All(); 
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_IDLE; 
			break; 

		default:
			senGsmSmsRdByPormptSt = GSM_SMS_RD_BY_POR_IDLE; 
			break; 
	}
	
}


GsmTpBool SmsIsRxed(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{//改造原来部分函数而来
	GsmTpBool boRetVal; 
	GsmTpU16 u16DatLen; 
	M_ClrBit(boRetVal); 
	if(IS_FALSE_BIT(stGsmSmsOpr.stSmsRxInfo.bSmsRxingFlg)) {
		M_ClrBit(boRetVal); 
	}
	else {		
		u16DatLen = u16Len - (sizeof("\r\n") - 1); 
		if(u16DatLen < stGsmSmsOpr.stSmsRxInfo.u16PduLen) {
			GsmDebugString("Err:sms length err\r\n"); 
		}
		else {
			boRetVal = app_gsm_SmsInfoDecode(pu8Dat, u16DatLen);
		}
		M_ClrBit(stGsmSmsOpr.stSmsRxInfo.bSmsRxingFlg); 
	}

	return boRetVal; 
}

void GsmTaskInit(void)
{
	memset(&stGsmSmsOpr, 0, sizeof(stGsmSmsOpr)); 
	GsmFifoCreat(&stGsmSmsOpr.stSmsRxInfo.stSmsPromptFifo, 
			(void **)&stGsmSmsOpr.stSmsRxInfo.u32SmsPromptBufTab, 
			GSM_SMS_PROMPT_BUF_NUM); 
}
GsmTpBool GsmPhoneIsIdle(void)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 

	if(PHONE_STATUS_IDLE == gsm_oper_info.phone.status) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}

	return (boRetVal); 
}

stGsmIMEISt GsmIMEIInfo;
GsmTpBool getGsmIMEI(GsmTpU8 * u8Buff, GsmTpU16 u16Len)
{
	GsmTpBool bRetVal;

	M_ClrBit(bRetVal);
	OSSchedLock();
	if( (GSM_IMEI_LEN==u16Len) && IS_TRUE_BIT(GsmIMEIInfo.u8IMEIValidFlag) )
	{
		memset(u8Buff, 0, GSM_IMEI_LEN);
		memcpy(u8Buff, GsmIMEIInfo.u8GsmIMEI, GSM_IMEI_LEN);
		M_SetBit(bRetVal);
	}
	else
	{
		M_ClrBit(bRetVal);
	}
	OSSchedUnlock();

	return bRetVal;
}

static GsmParaMaintainSt GsmParaMaintain;
//设置链接参数
GsmTpU8 SetGsmLinkPara(GsmTpU8 * pu8ApnStr, GsmTpU8 * pu8UserNameStr, GsmTpU8 * pu8PassWordStr)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8ApnLen;
	GsmTpU8 u8UserNameLen;
	GsmTpU8 u8PassWordLen;

	M_ClrBit(u8RetVal);
	u8ApnLen = strlen((char const *)pu8ApnStr);
	u8UserNameLen = strlen((char const *)pu8UserNameStr);
	u8PassWordLen = strlen((char const *)pu8PassWordStr);
	if( IS_NULL_P(pu8ApnStr) || IS_NULL_P(pu8UserNameStr) || IS_NULL_P(pu8PassWordStr)
	|| (u8ApnLen > GSM_APN_STR_LEN)
	|| (u8UserNameLen > GSM_USER_NAME_STR_LEN)
	|| (u8PassWordLen > GSM_PASSWORD_STR_LEN) )
	{
		GsmDebugString("Set link para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		OSSchedLock();
		memset(GsmParaMaintain.GsmLinkPara.u8Apn, 0, sizeof(GsmParaMaintain.GsmLinkPara.u8Apn));
		memcpy(GsmParaMaintain.GsmLinkPara.u8Apn, pu8ApnStr, u8ApnLen);
		memset(GsmParaMaintain.GsmLinkPara.u8UserName, 0, sizeof(GsmParaMaintain.GsmLinkPara.u8UserName));
		memcpy(GsmParaMaintain.GsmLinkPara.u8UserName, pu8UserNameStr, u8UserNameLen);
		memset(GsmParaMaintain.GsmLinkPara.u8PassWord, 0, sizeof(GsmParaMaintain.GsmLinkPara.u8PassWord));
		memcpy(GsmParaMaintain.GsmLinkPara.u8PassWord, pu8PassWordStr, u8PassWordLen);
		M_SetBit(GsmParaMaintain.GsmLinkParaValidFlag);
		if(IS_TRUE_BIT(GsmParaMaintain.GsmGprsParaValidFlag))
		{
			M_SetBit(GsmParaMaintain.GsmParaValidFlag);
		}
		else
		{
			NULL;
		}
		OSSchedUnlock();

		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

//获取链接参数
GsmTpU8 GetGsmLinkPara(GsmTpU8 ** pu8ApnStr, GsmTpU8 ** pu8UserNameStr, GsmTpU8 ** pu8PassWordStr)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8ApnLen;
	GsmTpU8 u8UserNameLen;
	GsmTpU8 u8PassWordLen;

	M_ClrBit(u8RetVal);
	if( IS_NULL_P(pu8ApnStr) || IS_NULL_P(pu8UserNameStr) || IS_NULL_P(pu8PassWordStr) )
	{
		GsmDebugString("Get link para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		OSSchedLock();
		u8ApnLen = strlen((char const *)GsmParaMaintain.GsmLinkPara.u8Apn);
		u8UserNameLen = strlen((char const *)GsmParaMaintain.GsmLinkPara.u8UserName);
		u8PassWordLen = strlen((char const *)GsmParaMaintain.GsmLinkPara.u8PassWord);
		if( (u8ApnLen>GSM_APN_STR_LEN)
		|| (u8UserNameLen>GSM_USER_NAME_STR_LEN)
		|| (u8PassWordLen>GSM_PASSWORD_STR_LEN) )
		{
			M_ClrBit(u8RetVal);
		}
		else
		{
			*pu8ApnStr = GsmParaMaintain.GsmLinkPara.u8Apn;
			*pu8UserNameStr = GsmParaMaintain.GsmLinkPara.u8UserName;
			*pu8PassWordStr = GsmParaMaintain.GsmLinkPara.u8PassWord;
			M_SetBit(u8RetVal);
		}
		OSSchedUnlock();
	}

	return u8RetVal;
}

//设置GPRS参数
GsmTpU8 SetGsmGprsPara(GprsCommModeSt GprsCommMode, GsmTpU16 u16Port, GsmTpU8 * pu8IpDomainStr)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8IpDomainLen;

	M_ClrBit(u8RetVal);
	u8IpDomainLen = strlen((char const *)pu8IpDomainStr);
	if( IS_NULL_P(pu8IpDomainStr) || (u8IpDomainLen > GSM_IP_DOMAIN_STR_LEN) || IS_FALSE_BIT(u16Port) )
	{
		GsmDebugString("Set gprs para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		OSSchedLock();
		GsmParaMaintain.GsmGprsPara.GprsCommMode = GprsCommMode;
		GsmParaMaintain.GsmGprsPara.u16CommPort = u16Port;
		memset(GsmParaMaintain.GsmGprsPara.u8IpDomain, 0, sizeof(GsmParaMaintain.GsmGprsPara.u8IpDomain));
		memcpy(GsmParaMaintain.GsmGprsPara.u8IpDomain, pu8IpDomainStr, u8IpDomainLen);
		M_SetBit(GsmParaMaintain.GsmGprsParaValidFlag);
		if(IS_TRUE_BIT(GsmParaMaintain.GsmLinkParaValidFlag))
		{
			M_SetBit(GsmParaMaintain.GsmParaValidFlag);
		}
		else
		{
			NULL;
		}
		OSSchedUnlock();

		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

//获取GPRS参数
GsmTpU8 GetGsmGprsPara(GprsCommModeSt * GprsCommMode, GsmTpU16 * u16Port, GsmTpU8 ** pu8IpDomainStr)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 u8IpDomainLen;

	M_ClrBit(u8RetVal);
	if( IS_NULL_P(pu8IpDomainStr) || IS_NULL_P(u16Port) || IS_NULL_P(pu8IpDomainStr) )
	{
		GsmDebugString("Get gprs para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		OSSchedLock();
		u8IpDomainLen = strlen((char const *)GsmParaMaintain.GsmGprsPara.u8IpDomain);
		if( (u8IpDomainLen>GSM_IP_DOMAIN_STR_LEN)
		|| IS_FALSE_BIT(GsmParaMaintain.GsmGprsPara.u16CommPort) )
		{
			M_ClrBit(u8RetVal);
		}
		else
		{
			*GprsCommMode = GsmParaMaintain.GsmGprsPara.GprsCommMode;
			*u16Port = GsmParaMaintain.GsmGprsPara.u16CommPort;
			*pu8IpDomainStr = GsmParaMaintain.GsmGprsPara.u8IpDomain;
			M_SetBit(u8RetVal);
		}
		OSSchedUnlock();
	}

	return u8RetVal;
}

//设置GSM参数
static GsmTpU8 SetGsmPara(GsmParaSt * GsmPara)
{
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);
	if(IS_NULL_P(GsmPara))
	{
		GsmDebugString("Set gsm para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		SetGsmLinkPara( GsmPara->GsmLinkPara.u8Apn,
						GsmPara->GsmLinkPara.u8UserName,
						GsmPara->GsmLinkPara.u8PassWord );
		SetGsmGprsPara( GsmPara->GsmGprsPara.GprsCommMode,
						GsmPara->GsmGprsPara.u16CommPort,
						GsmPara->GsmGprsPara.u8IpDomain );
	}

	return u8RetVal;
}

//获取GSM参数
static GsmTpU8 GetGsmPara(GsmParaSt * GsmPara)
{
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);
	if(IS_NULL_P(GsmPara))
	{
		GsmDebugString("Get gsm para is err\r\n");
		M_ClrBit(u8RetVal);
	}
	else
	{
		memset((void *)(&GsmPara->GsmLinkPara), 0, sizeof(GsmLinkParaSt));
		memset((void *)(&GsmPara->GsmGprsPara), 0, sizeof(GsmGprsParaSt));
		OSSchedLock();
		memcpy((void *)(&GsmPara->GsmLinkPara), (void *)(&GsmParaMaintain.GsmLinkPara), sizeof(GsmLinkParaSt));
		memcpy((void *)(&GsmPara->GsmGprsPara), (void *)(&GsmParaMaintain.GsmGprsPara), sizeof(GsmGprsParaSt));
		OSSchedUnlock();
	}

	return u8RetVal;
}

GsmTpU8 GsmDriverInit(GsmParaSt * GsmPara)
{
	return (SetGsmPara(GsmPara));
}

GsmTpU8 GsmDriverInfo(GsmParaSt * GsmPara)
{
	return (GetGsmPara(GsmPara));
}


GsmCellTowerInfoProcSt stGsmCellTowerInfoProc = {GsmTpFALSE, NULL};
GsmTpBool GsmCellTowerInfoReq(GsmCellTowerInfoReqSt * pstCellTowerInfoReq)
{
	GsmTpBool bRetVal;

	M_ClrBit(bRetVal);
	if(IS_TRUE_BIT(stGsmCellTowerInfoProc.bGsmCellTowerInfoReqFlag) || IS_NULL_P(pstCellTowerInfoReq))
	{
		GsmDebugString("Gsm lac info req failure\r\n");
		M_ClrBit(bRetVal);
	}
	else
	{
		if(IS_NULL_P(pstCellTowerInfoReq->GsmCellTowerInfoProcFunc))
		{
			GsmDebugString("Gsm lac info proc func is null, req failure\r\n");
			M_ClrBit(bRetVal);
		}
		else
		{
			stGsmCellTowerInfoProc.GsmCellTowerInfoProc = pstCellTowerInfoReq->GsmCellTowerInfoProcFunc;
			M_SetBit(stGsmCellTowerInfoProc.bGsmCellTowerInfoReqFlag);
			GsmDebugStringLowLevel("Gsm lac info req success\r\n");
			M_SetBit(bRetVal);
		}
	}

	return bRetVal;
}

#define GSM_LAC_INFO_PROC_ERR_MAX_CNT	(3)
void GsmCellTowerInfoProc(void)
{
	static GsmTpU8 u8ErrCnt = 0;
	GsmTpU8 ucErr=0;
	if(IS_FALSE_BIT(stGsmCellTowerInfoProc.bGsmCellTowerInfoReqFlag))
	{
		NULL;
	}
	else
	{	
		if(IS_TRUE_BIT(app_gsmAT_CENG_R()))
		{
			M_ClrBit(stGsmCellTowerInfoProc.bGsmCellTowerInfoReqFlag);
		}
		else
		{
			u8ErrCnt++;
			if(u8ErrCnt < GSM_LAC_INFO_PROC_ERR_MAX_CNT)
			{
				NULL;
			}
			else
			{
				stGsmCellTowerInfoProc.GsmCellTowerInfoProc(NULL);
				M_ClrBit(stGsmCellTowerInfoProc.bGsmCellTowerInfoReqFlag);
				GsmDebugString("Gsm lac info proc failure\r\n");
				M_ClrBit(u8ErrCnt);
			}
		}
	}
	
}



