#include "includes.h"
#include"app_gsm_includes.h"
#include "main.h"
#include "ParameterManage.h"
#include "app_Gsm.h"
#include "Config.h"
#include "MyQueue.h"
#include "MyMalloc.h"
#include "Pdu.h"
#include "app_usbManage.h"
#include "Bsp.h"
#include "ProtocolManage.h"

Gsmmyevent *pstEveTabForGsmRx[GSM_RX_EVENT_TAB_SIZE]; 
static u8 g_tPhoneRingTimeIsLessThanSixSecondFlg=FALSE;
static u8 g_tNetworkDataComingFlag=FALSE;
static u8 g_ucGsmStartedFlag=FALSE;
static GsmTpU8 g_ucSecSBk=0; 
static GsmTpU32 g_dwTimerBk=0; 

GsmTpBool GsmIpDatDebugPrintfIsLimit(GsmTpU8 *pu8Dat, 
											GsmTpU16 u16Len) 
{
	GsmTpBool boRetVal; 
	const GsmTpS8 cs8G610TcpStr[] = "+IPD"; 
	const GsmTpS8 cs8G610UdpStr[] = "+IPD"; 
	GsmTpU8 u8Len; 
	M_ClrBit(boRetVal); 
	/**************************************
		注意,以下比较函数使用了一个条件
		就是TCP和UDP的字符长度一致
	***************************************/
	u8Len = strlen(cs8G610TcpStr); 
	if(u8Len > u16Len) {
		M_ClrBit(boRetVal); 
	}
	else {
		/**************************************
			注意,以下比较函数使用了一个条件
			就是TCP和UDP的字符长度一致
		***************************************/
		if(GsmMemIsEqu((GsmTpU8 *)cs8G610TcpStr, pu8Dat, u8Len)) {
			M_SetBit(boRetVal); 
		}
		else if(GsmMemIsEqu((GsmTpU8 *)cs8G610UdpStr, pu8Dat, u8Len)) {
			M_SetBit(boRetVal); 
		}
		else {
			M_ClrBit(boRetVal); 
		}		
	}

	return (boRetVal); 
}

GsmTpBool GsmDriverGetDevRxDatPend(GsmTpU16 u16PendTimeMs, GsmTpU8 **p2u8DstDat, GsmTpU16 *pu16DstLen)
{
	GsmTpBool boRetVal; 
	Gsmmyevent *pstEveTmp;
	GsmTpU8 *pu8Tmp; 
	GsmTpU16 u16LenTmp; 
	Gsmmyevent stEve;	
	static GsmTpU8 s_acPrintfBuf[56]={0};
	u8 ucPrintfBufLen=0;
	M_ClrBit(boRetVal); 
	boRetVal = DevOsRxQReadPend(pstOsQForGsmRx, (myevent **)&pstEveTmp, u16PendTimeMs); 
	if(IS_FALSE_BIT(boRetVal)) {; 
	}
	else {
		pu8Tmp = pstEveTmp->buff; 
		u16LenTmp = pstEveTmp->size - ST_MYEVENT_SIZE; 

		if(GsmIpDatDebugPrintfIsLimit(pu8Tmp, u16LenTmp)) {; //gsm里面打印
		}
		else {
			if(u16LenTmp>55)
			{
				ucPrintfBufLen=55+1;
			}
			else
			{
				ucPrintfBufLen=u16LenTmp+1;//防止1打印不出来
			}
			memset(s_acPrintfBuf,0,sizeof(s_acPrintfBuf));
			ucPrintfBufLen=snprintf(s_acPrintfBuf,ucPrintfBufLen,"%s",pu8Tmp);
			GsmDebugStringLowLevel(s_acPrintfBuf); 
		}
		
		stEve.size = u16LenTmp; 
		stEve.buff = mymalloc(stEve.size); 
		if(IS_NULL_P(stEve.buff)) {
			M_ClrBit(boRetVal); 
			DebugPrintf("gsm port rd dat malloc err\r\n"); 
		}
		else {
			memcpy(stEve.buff, pu8Tmp, stEve.size); 
			*p2u8DstDat = stEve.buff; 
			*pu16DstLen = stEve.size; 
		}

		myfree((GsmTpU8 *)pstEveTmp, pstEveTmp->size); 
	}
	
	return boRetVal; 
}

void GsmDriverGetDevRxDatMemFreeIfNeed(GsmTpU8 *pu8DstDat, GsmTpU16 u16DstLen) 
{
	myfree(pu8DstDat, u16DstLen); 
}

extern void GsmTtsProc(void); 
extern void GsmTakeAudioProcess(void); 
static void GsmManGetNetDataHandle()
{
	u8 ucErr;
	OSSemPend(ptOsSemUsedForNeedGetNetData,OS_TICKS_PER_SEC/100,&ucErr);
	if(OS_ERR_NONE!=ucErr)
	{
	}
	else
	{
		ATLib_gsm_CIPRXGETDATA();
	}
}
void GsmDriverGsmTaskIdleHook(void)
{
	//GsmManGetNetDataHandle();//不使用手动网络数据可以去掉
	GsmTtsProc(); 
	GsmTakeAudioProcess();
	app_gsm_PhoneSpkTimeCheck();
	app_gprs_SocketCloseHandler();
}

GsmTpBool GsmRxIpDatHook(const IpDatSt *pcstIpDat)
{	
	GsmTpBool boRetVal; 
	IpDatSt *pstIpDatSt; 
	
	M_ClrBit(boRetVal); 
	pstIpDatSt = (IpDatSt *)mymalloc(sizeof(IpDatSt)); 
	if(NULL == pstIpDatSt) {
		DebugPrintf(ERR"Pro rx malloc err1\r\n"); 
	}
	else {
		memcpy(pstIpDatSt, pcstIpDat, sizeof(IpDatSt)); 
		pstIpDatSt->pu8Dat = mymalloc(pcstIpDat->u16Len);
		
		if(NULL == pstIpDatSt->pu8Dat) {
			DebugPrintf(ERR"MPip recv can't malloc!\n");
			myfree((GsmTpU8 *)pstIpDatSt, sizeof(IpDatSt)); 
		}
		else	{
			memcpy(pstIpDatSt->pu8Dat, pcstIpDat->pu8Dat, pcstIpDat->u16Len);
			pstIpDatSt->u16Len = pcstIpDat->u16Len;			
			g_tGsm.GsmWriteGprsData(pstIpDatSt->pu8Dat,pstIpDatSt->u16Len);
			myfree(pstIpDatSt->pu8Dat,pstIpDatSt->u16Len);
			myfree((GsmTpU8 *)pstIpDatSt, sizeof(IpDatSt)); //头部也要释放
			M_SetBit(boRetVal); 
			M_SetBit(g_tNetworkDataComingFlag);
		}
	}

	return (boRetVal); 
}
/*GSM 电话接通时的回调函数*/
void GsmPhoneChg2TalkHook(void)
{
}

void GsmPhoneNumRingHook(const GsmTpS8 *pcs8Num, const GsmTpU8 cu8NumLen)
{
	GsmTpU16 wSecS=0; 
	static u8 ucEmergencyContactNum1[PHONE_STR_LEN +1];
	static u8 ucEmergencyContactNum2[PHONE_STR_LEN +1];
	static u8 ucEmergencyContactNum3[PHONE_STR_LEN +1];
	u16 wStrLen;
	sms_info_t tSmsInfo={0};
	
	memcpy(tSmsInfo.head.num,pcs8Num,cu8NumLen);
	wSecS = GsmSysGetDurationSecTimer(&g_dwTimerBk); 
	g_ucSecSBk+=wSecS;
	if((g_ucSecSBk< 6)&&(NULL!=pcs8Num)&&(0!=cu8NumLen)) //有来电号码时才回应
	{		
		g_tParameterOpr.GetPara("SmsPara",EmergencyContactNum1,ucEmergencyContactNum1,&wStrLen);		
		g_tParameterOpr.GetPara("SmsPara",EmergencyContactNum2,ucEmergencyContactNum2,&wStrLen);	
		g_tParameterOpr.GetPara("SmsPara",EmergencyContactNum3,ucEmergencyContactNum3,&wStrLen);
		if((0==memcmp(pcs8Num,ucEmergencyContactNum1,cu8NumLen))||
		     (0==memcmp(pcs8Num,ucEmergencyContactNum2,cu8NumLen))||
		     (0==memcmp(pcs8Num,ucEmergencyContactNum3,cu8NumLen)))
		{
			
			g_tGsm.GsmWriteSmsData(&tSmsInfo,NULL);
			OSSchedLock();
			M_SetBit(g_tPhoneRingTimeIsLessThanSixSecondFlg);
			OSSchedUnlock();
		}
		else
		{
			OSSchedLock();
			M_ClrBit(g_tPhoneRingTimeIsLessThanSixSecondFlg);
			OSSchedUnlock();
		}
	}
	else
	{
		OSSchedLock();
		M_ClrBit(g_tPhoneRingTimeIsLessThanSixSecondFlg);
		OSSchedUnlock();
	}
}

void GsmPhoneHangupHook(void)
{
}


void GsmRxedSmsHook(sms_info_t *pstsms_info, GsmTpS16 pDCS)
{
	g_tGsm.GsmWriteSmsData(pstsms_info,(u16)pDCS);
	OSTaskResume(SMS_PROCESS_TASK_PRIORITY);
	OSSemPost(ptOsSemUsedForSmsComing);
}

void GsmPhoneNoAnswerHook(void)
{
}

void GsmPhoneBusyHook(void)
{
}

void GsmPhonePickUpHook(void)

{
}

void GsmPhoneDailFailHook(void)
{
}

void GsmPhoneDailOkHook(void)
{
}

GsmTpU8 GsmGetParaVolume(void)
{
    return TRUE;
}

void GsmSaveParaVolume(GsmTpU8 u8Volume)
{
}

#define VOL_MAX 	7		/* For G610 module. */
#define VOL_MIN 		0
GsmTpU8 GsmGetMaxVolumeLevel(void)
{
	return (VOL_MAX); 
}
GsmTpU8 GsmGetMinVolumeLevel(void)
{
	return (VOL_MIN); 
}

void GsmInitOkHook(void)
{
}

void GprsInitOkHook(void)
{
	u8 ucErr=0;
	GsmIpDatInit(); 	
	OSFlagPost(pstOsFlagForAppUse, GPRS_IS_OK_FLAG, OS_FLAG_SET, &ucErr); 	
}

GsmTpBool GsmModIsNeedOffGsmMod(void) 
{
	u8 ucErr=0;
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	OSSemPend(ptOsSemUsedForGsmNeedRestart,(OS_TICKS_PER_SEC/50),&ucErr); //10ms没回应
	if(OS_ERR_NONE!=ucErr)
	{
		M_ClrBit(boRetVal); 
	}
	else
	{
		M_SetBit(boRetVal); 
	}
	return (boRetVal); 
}

void app_gsm_PhoneSpkTimeCheck(void)
{
}
void GsmDriverDevModRst(void)
{
	u8 ucErr;
	OSFlagPost(pstOsFlagForAppUse, GPRS_IS_OK_FLAG, OS_FLAG_CLR, &ucErr); 
	GsmBspGsmPwrOn();
	g_tGsm.GsmReset();	//加入复位的还是重启不行
	M_ClrBit(g_ucGsmStartedFlag);
}
void GsmIdleStateHandle(void)
{
	u8 ucErr;
	u8 ret;
	static u8 s_ucIdleStateCount=0;
	static u8 s_ucNetBusyFlag=FALSE;	
	static u8 s_ucSmsBusyFlag=FALSE;	
	static u8 s_ucGsmEnterSleepCmdSuccessFlag=FALSE;	
	static u8 s_ucSystemStartedFlagForGsmGetTime=FALSE;
	static GsmTpU32 s_dwBackupGsmEnterSleepTime=0; 
	static GsmTpU32 s_dwCurGsmEnterSleepTime=0; 
	if(IS_FALSE_BIT(s_ucSystemStartedFlagForGsmGetTime))//系统刚启动
	{
		s_dwBackupGsmEnterSleepTime=OSTimeGet();
		s_dwCurGsmEnterSleepTime=OSTimeGet();
		M_SetBit(s_ucSystemStartedFlagForGsmGetTime);
	}
	else
	{
		s_dwCurGsmEnterSleepTime=OSTimeGet();
	}
	if((s_dwCurGsmEnterSleepTime-s_dwBackupGsmEnterSleepTime)<(GSM_RESUME_TIME*OS_TICKS_PER_SEC))//空闲状态持续3s则休眠
	{
	}
	else
	{
		if(IS_TRUE_BIT(g_tNetworkDataComingFlag))
		{
			M_ClrBit(g_tNetworkDataComingFlag);//如果3s内有网络数据过来，不睡眠
		}
		else
		{
			if(IS_TRUE_BIT(g_ucGsmStartedFlag))
			{
			}
			else
			{
				ret=ATLib_gsm_CSCLK();//彻底断电后开机执行一次即可
				if(FALSE==ret)
				{
					DebugPrintf(ERR"ATLib_gsm_CSCLK err\r\n");
					M_ClrBit(s_ucGsmEnterSleepCmdSuccessFlag);
				}
				else
				{
					M_SetBit(s_ucGsmEnterSleepCmdSuccessFlag);
					M_SetBit(g_ucGsmStartedFlag);
				}
			}
			if(IS_FALSE_BIT(s_ucGsmEnterSleepCmdSuccessFlag))
			{
			}
			else
			{		
				OSFlagAccept(pstOsFlagForAppUse, GSM_TX_IP_DAT_BUSY_FLAG,OS_FLAG_WAIT_SET_ALL, &ucErr); 
				if(OS_ERR_NONE != ucErr)//网络断了恢复GSM会清掉请求，所以这里也就等不到了 
				{	
					M_ClrBit(s_ucNetBusyFlag);
				}
				else
				{
					M_SetBit(s_ucNetBusyFlag);
				}
				OSSchedLock();
				s_ucSmsBusyFlag=stGsmSmsOpr.stSmsTxOpr.bSmsTxBusyFlg;
				OSSchedUnlock();
				if((3>s_ucIdleStateCount)||IS_TRUE_BIT(s_ucNetBusyFlag)||IS_FALSE_BIT(GsmPhoneIsIdle())||
					IS_TRUE_BIT(s_ucSmsBusyFlag))
				{					
				}//三次才睡，防止突然链接断掉又恢复花费时间，
				else//同时又有数据要发，则不进入睡眠，处理后(清除请求)再睡
				{
					s_ucIdleStateCount=0;
					g_tGsm.GsmEnterSleepMode();
					DebugPrintf("#########GsmEnterSleep!#########\r\n");
					OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,GSM_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
					OSSemPend(ptOsSemUsedForResumeGsmTask,0,&ucErr); 
					OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,GSM_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
					g_tGsm.GsmWakeUp();
					DebugPrintf("##########GsmWakeUp!############\r\n");
				}
			}
		}		
		s_dwBackupGsmEnterSleepTime=OSTimeGet();
	}		
	s_ucIdleStateCount++;
}
void GsmDriverEnterCritical(void)
{
	OSSchedLock(); 
}
void GsmDriverExitCriticical(void)
{
	OSSchedUnlock(); 
}

void GsmDrivermyfree(unsigned char * buff, unsigned int Size)
{
	myfree(buff, Size); 
}

void GsmDriverSleep(GsmTpU32 u32Ms)
{
	OSTimeDly(u32Ms / (1000 / OS_TICKS_PER_SEC)); 
}

void GsmDebugString(GsmTpS8 *pst8Str)
{
	DebugPrintf(CRIT"%s",pst8Str); 
}

void GsmDebugStringLowLevel(GsmTpS8 *pst8Str)
{
	DebugPrintf("%s",pst8Str); 
}
void GsmDebugPrintf(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{
	DebugDataSend(pu8Dat,u16Len);
}
void GsmPhoneNoCarreyHook(void)
{
	u8 ucErr;
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,PHONE_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
	if(IS_FALSE_BIT(g_tPhoneRingTimeIsLessThanSixSecondFlg))
	{
	}
	else
	{	
		M_ClrBit(g_tPhoneRingTimeIsLessThanSixSecondFlg);	
		OSTaskResume(SMS_PROCESS_TASK_PRIORITY);
		OSSemPost(ptOsSemUsedForPhoneComing);
	}	
	g_ucSecSBk=0;
	g_dwTimerBk=0;	
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,PHONE_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
}

GsmTpU8 * GsmMalloc(GsmTpU16 u16Size)
{
	return mymalloc(u16Size); 
}

void GsmFree(GsmTpU8 *pu8Buf, GsmTpU16 u16Size)
{
	myfree(pu8Buf, u16Size); 
}

void GsmMallocErrHook(void)
{
	//debugQueue();
}

void GsmGprsFlowOffHook(void)
{
	GsmTpU8 u8Err; 
	OSFlagPost(pstOsFlagForAppUse, GSM_DAT_FLOW_OFF, OS_FLAG_SET, &u8Err); 
}
void GsmGprsFlowOnHook(void)
{
	GsmTpU8 u8Err; 
	OSFlagPost(pstOsFlagForAppUse, GSM_DAT_FLOW_OFF, OS_FLAG_CLR, &u8Err); 
}

GsmTpBool GsmGprsDebugPrintfIsLimit(void)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 

	return (boRetVal); 
}

void GprsEstablishLinkHook(GsmTpBool boOpenIsOkFlg)
{
	static GsmTpU8 callfail_cnt; 
	if(IS_TRUE_BIT(boOpenIsOkFlg)) {
		M_ClrBit(callfail_cnt); 
	}
	else {
		callfail_cnt++; 
	}
	
	if(callfail_cnt >= 3)
	{
		GsmDebugString("Establish link fail >= 3, mybe apn err!\n");
		callfail_cnt = 0;
	}
}

void GsmGprsOpenSocketFailHook(void)
{
	GsmDebugString("TE cannot link server, switch to other server!\n");
}

void GsmGetLinkParaApnUserKey(GsmTpU8 **p2u8Apn, GsmTpU8 **p2u8UserName, GsmTpU8 **p2u8Password)
{	
	static u8 strApn[APN_STR_LEN+1]={0};
	static u8 strUserName[USERNAME_STR_LEN+1]={0};
	static u8 strPassword[PASSWORD_STR_LEN+1]={0};
	u16 wStrLen=0;

	g_tParameterOpr.GetPara("NetPara",Apn,strApn,&wStrLen);	
	g_tParameterOpr.GetPara("NetPara",UsrName,strUserName,&wStrLen);
	g_tParameterOpr.GetPara("NetPara",Pwd,strPassword,&wStrLen);
	*p2u8Apn = strApn; 
	*p2u8UserName = strUserName; 
	*p2u8Password = strPassword; 
}


//获取IP链接参数
void GsmGetIpPara(GsmTpU8 *u8UdpTcp, GsmTpU16 *pu16Port, GsmTpU8 **p2u8IpDomainPtr)
{
	GsmTpU16 wPort; 
	GsmTpU8 udp_tcp; 	
	GsmTpU8 *tmp_ptr; 
	
	GsmTpU8 ucDailMode; 
	GsmTpU8 aucPort[2];
	static u8 strDomain[DOMAIN_STR_LEN+1]={0};
	static u8 strIp[IP_STR_LEN+1]={0};
	u16 wStrLen=0;
	g_tParameterOpr.GetPara("NetPara",DailMode,&ucDailMode,&wStrLen);	
	if(DOMAIN_DIAL_MODE==ucDailMode)
	{
		g_tParameterOpr.GetPara("NetPara",Domain,strDomain,&wStrLen);
		tmp_ptr=strDomain;
	}
	else
	{
		g_tParameterOpr.GetPara("NetPara",Ip,strIp,&wStrLen);
		tmp_ptr=strIp;
	}
	g_tParameterOpr.GetPara("NetPara",Port,aucPort,&wStrLen);	
	wPort=Lsb2U8ToU16(aucPort);
	udp_tcp = GPRS_TCP;
	*u8UdpTcp = udp_tcp; 
	*pu16Port = wPort; 
	*p2u8IpDomainPtr = tmp_ptr; 
}

GsmTpBool GsmTtsIsEnable(void)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	return (boRetVal); 
}


GsmTpBool GsmTtsGetPlayData(GsmTpU8 **p2u8Dat, GsmTpU16 *pu16Len)
{
	GsmTpBool boRetVal; 
	Gsmmyevent stEve; 
	memset(&stEve, 0, sizeof(stEve)); 
	M_SetBit(boRetVal); 
	return (boRetVal); 
}

/*串口数据接收*/
GsmTpBool GsmDevRxQWrite(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{
	Gsmmyevent event; 
	event.buff = pu8Dat; 
	event.size = u16Len; 
#if 0
	debugString("Gsm recv:");
	debugPrintf(event.buff, event.size);
	debugString("\r\n");
#endif
	return DevOsRxQWrite(pstOsQForGsmRx, (myevent *)&event); 
}


static myqueue Gsm_SendQueue;
GsmTpU8 dev_gsm_SendQueueRead(Gsmmyevent *event)
{
	Gsmmyevent stEve; 
	GsmTpU8 u8RetVal; 
	u8RetVal = (GsmTpU8)myQueueRead((myevent *)&stEve, &Gsm_SendQueue); 
	event->buff = stEve.buff; 
	event->size = stEve.size; 
	return (u8RetVal);
}
GsmTpU8 dev_gsm_SendQueueWrite(Gsmmyevent *event)
{
	Gsmmyevent stEve; 
	GsmTpU8 u8RetVal; 
	stEve.buff = event->buff; 
	stEve.size = event->size; 
	u8RetVal = (GsmTpU8)myQueueWrite(&Gsm_SendQueue, (myevent *)&stEve); 
	return (u8RetVal);
}
GsmTpU8 dev_gsm_SendQueueSize(void)
{
	return(Gsm_SendQueue.item_num);
}
static GsmTpU8 dev_gsm_SendQueueCreate(void)
{
	return(myQueueCreate(&Gsm_SendQueue));
}


/*串口数据发送*/
GsmTpBool GsmDevTxQWrite(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{
	Gsmmyevent event;
	static GsmTpU8 su8NgCnt; 
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	event.buff = GsmMalloc(u16Len);
	if(NULL == event.buff) {
		GsmDebugString("gsm write can't malloc!\n");
		su8NgCnt++; 
		if(su8NgCnt < 3) {;
		}
		else {
			M_ClrBit(su8NgCnt); 
			GsmMallocErrHook();
		}
		M_ClrBit(boRetVal); 
	}
	else {
		M_ClrBit(su8NgCnt); 
		event.size = u16Len;
		memcpy(event.buff, pu8Dat, u16Len);
		if(u16Len<26)
		{
			GsmDebugStringLowLevel(pu8Dat);
		}
		else if(u16Len<260)
		{
			GsmDebugPrintf(pu8Dat, u16Len);
		}
		else
		{
			GsmDebugPrintf(pu8Dat, 260);
		}
		boRetVal = g_tGsm.GsmSendData(event.buff,(u16)event.size); 
		myfree(event.buff,event.size);
#if 0
			debugString("GSM send Q write:");
			debugPrintf(event.buff, event.size);
#endif
	}
	
	return (boRetVal);
}

void dev_gsm_DataBaseInit(void)
{
	dev_gsm_SendQueueCreate();
}

/*流控开不能发送数据,要等流控限制关*/
GsmTpBool GsmGprsGetFlowCtlLimitIsOffFlg(void)
{
	GsmTpU8 u8Err; 
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	OSFlagAccept(pstOsFlagForAppUse, GSM_DAT_FLOW_OFF, OS_FLAG_WAIT_CLR_ALL, &u8Err); 
	if(OS_ERR_NONE == u8Err) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}

	return boRetVal; 
}

GsmTpBool GsmGprsIpDatTxReqIsSetThenClr(void)
{
	GsmTpBool boRetVal; 
	GsmTpU8 u8Err; 
	GsmTpU8 ucErr;	
	M_ClrBit(boRetVal); 	
	OSSemPend(ptOsSemUsedForGprsDataSendReq,1,&ucErr);
	if(OS_ERR_NONE!=ucErr)	
	{	
		M_ClrBit(boRetVal); 		
	}
	else	
	{
		OSFlagPost(pstOsFlagForAppUse, GSM_TX_IP_DAT_BUSY_FLAG, OS_FLAG_SET, &u8Err); 
		//OSFlagPost(pstOsFlagForAppUse, GSM_TX_IP_DAT_REQ_FLAG, OS_FLAG_CLR, &u8Err); 
		M_SetBit(boRetVal); 
	}	
	return (boRetVal); 
}

void GsmGprsIpDatTxEndHook(void)
{
	GsmTpU8 u8Err; 
	GsmTpU16 wIdleCnt=0;
	OSFlagPost(pstOsFlagForAppUse, GSM_TX_IP_DAT_BUSY_FLAG, OS_FLAG_CLR, &u8Err); 
	OSTimeDly(10);//给等待标记任务执行处理时机
	wIdleCnt=OSSemAccept(ptOsSemUsedForGprsDataSendIdle);
	if(wIdleCnt>1)
	{
		DebugPrintf(ERR"GsmGprsIpDatTxEnd:%d!\r\n",wIdleCnt);
	}
	else
	{
	}
	while(wIdleCnt>1)//防止多次post空闲状态
	{
		wIdleCnt=OSSemAccept(ptOsSemUsedForGprsDataSendIdle);
	}
	OSSemPost(ptOsSemUsedForGprsDataSendIdle);
}

void GsmGprsIpDatTxFaultHook(void)
{
	GsmTpU8 u8Err; 
	GsmTpU16 wIdleCnt=0;
	OSFlagPost(pstOsFlagForAppUse, 
				GSM_TX_IP_DAT_REQ_FLAG + GSM_TX_IP_DAT_BUSY_FLAG, 
				OS_FLAG_CLR, 
				&u8Err); 
	OSTimeDly(10);//给等待标记任务执行处理时机
	wIdleCnt=OSSemAccept(ptOsSemUsedForGprsDataSendIdle);
	if(wIdleCnt>1)
	{
		DebugPrintf(ERR"GsmGprsIpDatTxFault:%d!\r\n",wIdleCnt);
	}
	else
	{
	}
	while(wIdleCnt>1)//防止多次post空闲状态
	{
		wIdleCnt=OSSemAccept(ptOsSemUsedForGprsDataSendIdle);
	}
	OSSemPost(ptOsSemUsedForGprsDataSendIdle);
}
void GsmGprsIpDatTxOkHook(void)
{
	GsmTpU8 ucErr; 
	OSFlagPost(pstOsFlagForAppUse,GSM_TX_IP_DAT_OK_FLAG,OS_FLAG_SET,&ucErr);
}
GsmTpBool GsmGetIccid(GsmTpS8 **p2s8Iccid, GsmTpU16 *pu16Len)
{
	GsmTpBool boRetVal; 
	*p2s8Iccid = (GsmTpS8 *)u8IccidStr; 
	*pu16Len = sizeof(u8IccidStr); 
	M_SetBit(boRetVal); 

	return (boRetVal); 
}
void GsmBspSysReset(void)
{
	//SystemReset();//不复位整个系统
	/*GsmBspGsmPwrOff();
	OSTimeDly(300);
	GsmBspGsmPwrOn();
	OSTimeDly(400);*/
}
void GsmBspGsmPwrOff(void)
{
	g_tGsm.GsmPowerOff(); //关模块使用掉电
	//OSTimeDly(300);
}
void GsmBspGsmPwrOn(void)
{
	g_tGsm.GsmPowerOn(); 
}
void GsmBspAmpPwrOff(void)
{
}
void GsmBspAmpPwrOn(void)
{
}

GsmTpBool GsmBspAudioChannelIsBusy(void)
{
  return FALSE;
}
void GsmBspAudioChannelBusySet(void)
{
}
void GsmBspAudioChannelBusyClr(void)
{
}



/*******************************************************
*依据ucos ii系统,如果更换系统
需要修改对应内部引用函数
输入:静态的32位的定时器缓存变量
	如果该变量会自动初始化,
	如要自行初始化,需要使用
		OSTimeGet(); 初始化,如果和系统同步启动的定时器
		可以不用初始化.

输出:自从上次运行经过的秒数
********************************************************/
GsmTpU16 GsmSysGetDurationSecTimer(GsmTpU32 *pu32TimerBk)
{//此函数周期要在一秒之内
	GsmTpU16 u16Secs; 
	u16Secs=GetDurationSecTimer(pu32TimerBk);
	return(u16Secs); 	
}

void GsmOperInitHook(void) 
{
//每次都初始化为主IP(为和原来兼容,待进一步优化)
	GsmDriverEnterCritical(); 
	//M_ClrBit(boLinkIpIsBackIpFlag);
	GsmDriverExitCriticical(); 
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
GsmTpS16 GsmPDUDecode(GsmTpU8 *pSCA, GsmTpU8 *pDA, GsmTpU8 *pTime, 
							GsmTpU8 *pUD, GsmTpS16 *pDCS, GsmTpU8 *pPdu, GsmTpS16 iPduLen)
{
	return(PDUDecode(pSCA, pDA, pTime, pUD, pDCS, pPdu, iPduLen)); 
}

GsmTpS16 GsmPDUEncode( GsmTpU8  *pPdu, GsmTpU8  *pSCA, GsmTpU8  *pDA, 
							GsmTpU8  *pUD, GsmTpS16 iUDL, GsmTpS16 iDCS )
{

	return (PDUEncode(pPdu, pSCA, pDA, pUD, iUDL, iDCS )); 
}


