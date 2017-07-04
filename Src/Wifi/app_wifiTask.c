/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266任务程序
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/

#include  "Config.h"
#include "app_wifiTask.h"
#include "Mymalloc.h"

st_WifiTaskParam wifi_task_param;
st_WifiTaskParam *wifi_task_param_get(void)
{
	return (&wifi_task_param);
}

/*
-------------------------------------------------------
-函数功能:	WIFI 模块初始化，任务参数初始化
-参数说明:	NULL
-------------------------------------------------------
*/
void wifi_task_init(void)
{
	app_wifi_init();
	memset(&wifi_task_param, 0, sizeof(st_WifiTaskParam));
}

/*
-------------------------------------------------------
-函数功能:	公用计数申请
-参数说明:	validity，TRUE 为有效，FALSE 为无效
-------------------------------------------------------
*/
void wifi_cmd_timer_apply(e_WifiTimerId id, e_wifi_bool validity)
{
	wifi_task_param.wifi_cmd_timer.id = id;
	wifi_task_param.wifi_cmd_timer.validity = validity;
	wifi_task_param.wifi_cmd_timer.cnt = 0;
}

/*
-------------------------------------------------------
-函数功能:	功能扩展接口
-参数说明:	NULL
-------------------------------------------------------
*/
void app_wifi_cmd_handle(WifiCmdId cmd_id, At_Resualt rev_flag)
{
	if (AT_RESUALT_SUCCESS == rev_flag)
	{
		app_wifi_cmd_success_handle(cmd_id);
	}
	else
	{
		app_wifi_cmd_fail_handle(cmd_id);
	}
}

/*
-------------------------------------------------------
-函数功能:	任务函数
-参数说明:	NULL
-------------------------------------------------------
*/
e_wifi_bool WifiAtCmdProc(WifiCmdId stCmd, 
						const s8 *str_1, const s8 *str_2, u8 Timeout); 

//关机等待时间,防止WIFI错误,无具体要求,暂定1S
#define WIFI_POWER_OFF_WAIT_TIME		1
#define WIFI_POWER_ON_TIME_OVER_TIME		5
enum WifiPwrStEn{
	WIFI_PWR_ST_UNKOWN, 
	WIFI_PWR_ST_OFF_IDLE, 
	WIFI_PWR_ST_ON_START, 
	WIFI_PWR_ST_ON_WAIT, 
	WIFI_PWR_ST_ON_INIT, 
	WIFI_PWR_ST_ON_IDLE, 
	WIFI_PWR_ST_OFF_START, 
	WIFI_PWR_ST_OFF_WAIT, 
}; 
static e_wifi_bool sboWifiAtCmdBusyFlg; 
static WifiCmdId senWifiCmdBuf; 
static e_wifi_bool sboWifiModIsUpdateOk; 

e_wifi_bool WifiPwrIsNeedOn()
{
	e_wifi_bool boRetVal; 
	M_ClrBit(boRetVal); 

	if(IS_TRUE_BIT(sboWifiAtCmdBusyFlg)
	|| (CMD_ID_NULL != senWifiCmdBuf)
	|| IS_FALSE_BIT(sboWifiModIsUpdateOk)) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	
	return boRetVal; 
}
e_wifi_bool WifiGetVersionProc()
{
	e_wifi_bool boRetVal; 
	M_ClrBit(boRetVal); 
	boRetVal = WifiAtCmdProc(CMD_ID_VERSION, "OK", NULL, 3); 
	
	return boRetVal; 
}

e_wifi_bool WifiPowerOnStrChkOk(); 
GsmTpBool WifiDriverFlushRxDat(void); 

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
u16 SysGetDurationSecTimer(u32 *pu32TimerBk)
{//此函数周期要在一秒之内
	u16 u16Secs; 
	u32 u32CurTime; 
	u32 u32DiffTime; 
	M_ClrBit(u16Secs); 
	u32CurTime = OSTimeGet(); 
	if(IS_FALSE_BIT(*pu32TimerBk)
	|| (u32CurTime < *pu32TimerBk)) {; //首次运行或极端异常,初始化
		*pu32TimerBk = u32CurTime; 
	}
	else {; 
	}

	u32DiffTime = (u32CurTime - *pu32TimerBk); 	
	if(u32DiffTime < OS_TICKS_PER_SEC) {; 
	}
	else {
		u32 u32TmpPoint; 
		u16Secs = u32DiffTime / OS_TICKS_PER_SEC; 
		u32TmpPoint = u32DiffTime % OS_TICKS_PER_SEC; //小数部分
		*pu32TimerBk = u32CurTime - u32TmpPoint; 
	}

	return(u16Secs); 	
}

void WifiPwrStProc() 
{
	static enum WifiPwrStEn senWifiPwrSt; 
	static u16 su16WifiPwrTimerS; 
	static u32 su32WifiPwrTimerBk; 
	u16 u16SecS; 
	u16 u16i; 
	u8 ucErr=0;
	static u8 su8ErrCnt; 
	
	u16SecS = SysGetDurationSecTimer(&su32WifiPwrTimerBk); 
	for(u16i = 0; u16i < u16SecS; u16i++) {
		M_NOT_ZERO_DEC(su16WifiPwrTimerS); 
	}

	switch(senWifiPwrSt) {
		case WIFI_PWR_ST_UNKOWN:
			DebugPrintf(ERR"wifi power st is unkown, to off star\r\n"); 
			senWifiPwrSt = WIFI_PWR_ST_OFF_START; 
			break; 
			
		case WIFI_PWR_ST_OFF_IDLE:
			if(IS_FALSE_BIT(WifiPwrIsNeedOn())) {
			}
			else {
				WifiDriverFlushRxDat(); 
				senWifiPwrSt = WIFI_PWR_ST_ON_START; 
			}
			break; 

		case WIFI_PWR_ST_ON_START:
			DebugPrintf("Wifi power is on\r\n"); 
			wifi_PowerOn(); 
			su16WifiPwrTimerS = WIFI_POWER_ON_TIME_OVER_TIME; 
			senWifiPwrSt = WIFI_PWR_ST_ON_WAIT; 
			break; 

		case WIFI_PWR_ST_ON_WAIT:
			if(IS_TRUE_BIT(WifiPowerOnStrChkOk())) {
				DebugPrintf("Wifi to power on init\r\n"); 
				M_ClrBit(su8ErrCnt); 
				senWifiPwrSt = WIFI_PWR_ST_ON_INIT; 
			}
			else if(IS_FALSE_BIT(su16WifiPwrTimerS)) {
				//暂时没有高标准要求,超时不再做异常处理,直接
				//认为打开完毕,目标是现有一个可以使用的WIFI模块
				//先不要求高可靠性(如果不打开,可能造成信号一直不处理)
				DebugPrintf(ERR"ERROR:wifi power on time over, recognize power is on\r\n"); 
				M_ClrBit(su8ErrCnt); 
				senWifiPwrSt = WIFI_PWR_ST_ON_INIT; 
			}
			else {; 
			}
			break; 

		case WIFI_PWR_ST_ON_INIT:
			if(IS_TRUE_BIT(sboWifiModIsUpdateOk)) {
				DebugPrintf("Wifi to power on ok\r\n"); 
				senWifiPwrSt = WIFI_PWR_ST_ON_IDLE; 
			}
			else {
				WifiGetVersionProc(); 
				if(IS_TRUE_BIT(WifiAtCmdProc(CMD_ID_MODE_SET_2_STA, "OK", NULL, 3))) {
					DebugPrintf("Wifi to power on ok\r\n"); 
					senWifiPwrSt = WIFI_PWR_ST_ON_IDLE; 
					M_SetBit(sboWifiModIsUpdateOk); 
				}
				else {
					su8ErrCnt++; 
					if(su8ErrCnt < 3) {; 
					}
					else {
						DebugPrintf(ERR"Wifi to power on init fail restart\r\n"); 
						senWifiPwrSt = WIFI_PWR_ST_OFF_START;
					}
				}
			}
			break; 

		case WIFI_PWR_ST_ON_IDLE:
			if(IS_TRUE_BIT(WifiPwrIsNeedOn())) {
				if(CMD_ID_NULL == senWifiCmdBuf) {;
				}
				else {
					e_wifi_bool boOkFlg; 
					boOkFlg = WifiAtCmdProc(senWifiCmdBuf, "OK", NULL, 6);
					if(IS_TRUE_BIT(boOkFlg)) {
						app_wifi_cmd_success_handle(senWifiCmdBuf); 
					}
					else {
						app_wifi_cmd_fail_handle(senWifiCmdBuf); 
					}
					senWifiCmdBuf = CMD_ID_NULL; 
				}
			}
			else {
				senWifiPwrSt = WIFI_PWR_ST_OFF_START; 
			}
			break; 
			
		case WIFI_PWR_ST_OFF_START:
			DebugPrintf("Wifi power off start\r\n"); 
			wifi_PowerOff();
			su16WifiPwrTimerS = WIFI_POWER_OFF_WAIT_TIME; 
			senWifiPwrSt = WIFI_PWR_ST_OFF_WAIT; 
			break; 

		case WIFI_PWR_ST_OFF_WAIT:
			if(IS_TRUE_BIT(su16WifiPwrTimerS)) 
			{; 
			}
			else 
			{
				DebugPrintf("Wifi power off OK\r\n"); 
				if(WIFI_TRUE==WifiPwrIsNeedOn()) 
				{
				}
				else
				{
					OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,WIFI_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
					OSTaskSuspend(WIFI_TASK_PRIORITY);	
					OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,WIFI_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
				}
				senWifiPwrSt = WIFI_PWR_ST_OFF_IDLE; 
			}
			break; 

		default:
			senWifiPwrSt = WIFI_PWR_ST_UNKOWN; 
			break; 
	}
}

void WifiDriverGetDevRxDatMemFreeIfNeed(GsmTpU8 *pu8DstDat, GsmTpU16 u16DstLen) ; 
GsmTpBool WifiDriverFlushRxDat(void)
{
	GsmTpBool boRetVal; 
	myevent *pstEveTmp;
	GsmTpU8 *pu8Tmp; 
	GsmTpU16 u16LenTmp; 
	
	M_ClrBit(boRetVal); 

	while(1) {
		boRetVal = DevOsRxQReadPend(pstOsQForWifiRx, (myevent **)&pstEveTmp,WIFI_TASK_CYCLE_MS); 
		if(IS_FALSE_BIT(boRetVal)) {
			M_SetBit(boRetVal); 
			break;  
		}
		else {
			pu8Tmp = pstEveTmp->buff; 
			u16LenTmp = pstEveTmp->size-ST_MYEVENT_SIZE; 
			DebugPrintf("wifi flushed:"); 
			GsmDebugPrintf(pstEveTmp->buff, u16LenTmp); 
			WifiDriverGetDevRxDatMemFreeIfNeed((GsmTpU8 *)pstEveTmp, pstEveTmp->size); 
		}
	}
	
	return boRetVal; 
}


GsmTpBool WifiDriverGetDevRxDatPend(GsmTpU16 u16PendTimeMs, GsmTpU8 **p2u8DstDat, GsmTpU16 *pu16DstLen)
{
	GsmTpBool boRetVal; 
	myevent *pstEveTmp;
	GsmTpU8 *pu8Tmp; 
	GsmTpU16 u16LenTmp; 
	myevent stEve;
	M_ClrBit(boRetVal); 

	boRetVal = DevOsRxQReadPend(pstOsQForWifiRx, (myevent **)&pstEveTmp,u16PendTimeMs); 
	if(IS_FALSE_BIT(boRetVal)) {; 
	}
	else {		
		pu8Tmp = pstEveTmp->buff; 
		u16LenTmp = pstEveTmp->size - ST_MYEVENT_SIZE; 
		stEve.size = u16LenTmp; 
		stEve.buff = mymalloc(stEve.size); 
		if(IS_NULL_P(stEve.buff)) {
			M_ClrBit(boRetVal); 
			DebugPrintf(ERR"wifi  rd dat malloc err\r\n"); 
		}
		else {
			memcpy(stEve.buff, pu8Tmp, stEve.size); 
			*p2u8DstDat = stEve.buff; 
			*pu16DstLen = stEve.size; 
			GsmDebugPrintf(stEve.buff,stEve.size);
			//DebugPrintf("wifi RxedLen:%d\r\n",stEve.size); 
		}
		myfree((GsmTpU8 *)pstEveTmp, pstEveTmp->size); 
	}
	
	return boRetVal; 
}
void WifiDriverGetDevRxDatMemFreeIfNeed(GsmTpU8 *pu8DstDat, GsmTpU16 u16DstLen) 
{
	myfree(pu8DstDat, u16DstLen); 
}

void WifiAtRetInfoAnalyse(u8 *pu8Dat, u16 u16Len)
{
	
}

extern e_wifi_bool app_wifi_cmd_sendById(WifiCmdId stCmd); 
static e_wifi_bool WifiWaitATCmdAck(pWifiCmdRetHookType pFunAtCmdHook, 
								const s8 *str_1, const s8 *str_2, u8 Timeout); 
extern pWifiCmdRetHookType app_wifi_GetHookById(WifiCmdId stCmd); 
e_wifi_bool WifiPowerOnStrChkOk()
{
	return WifiWaitATCmdAck(NULL, WIFI_POWER_ON_STR, NULL, 4); 
}
e_wifi_bool WifiAtCmdProc(WifiCmdId stCmd, 
						const s8 *str_1, const s8 *str_2, u8 Timeout)
{
	e_wifi_bool boRetVal; 
	M_ClrBit(boRetVal); 

	M_SetBit(sboWifiAtCmdBusyFlg); 
	
	if(IS_FALSE_BIT(app_wifi_cmd_sendById(stCmd))) {
		DebugPrintf(ERR"wifi cmd send ERROR\r\n"); 
		M_ClrBit(boRetVal); 
	}
	else {
		boRetVal = WifiWaitATCmdAck(app_wifi_GetHookById(stCmd), 
									str_1, str_2, Timeout); 
	}

	M_ClrBit(sboWifiAtCmdBusyFlg); 
	
	return boRetVal; 
}

static e_wifi_bool WifiWaitATCmdAck(pWifiCmdRetHookType pFunAtCmdHook, 
								const s8 *str_1, const s8 *str_2, u8 Timeout)
{
#define WIFI_ATCMD_ACK_ERROR_STR		"ERROR"
	myevent event;
	u16 u16WaitTime;
	e_wifi_bool boRetVal; 
	e_wifi_bool boNeedBreakFlg; 

	u16 u16i; 
	u16 u16SecS; 
	u16 u16TimeOutS; 
	u32 su32WifiWaitATCmdAckTimerBk; 
	e_wifi_bool boCmdEndByManual; 
	
	M_ClrBit(boRetVal); 
	M_ClrBit(boNeedBreakFlg); 
	M_ClrBit(boCmdEndByManual); 
	if((str_1 == NULL) && (str_2 == NULL)) {
		OSTimeDly(200);
		return(TRUE);
	}

	u16TimeOutS = Timeout; 
	
	M_ClrBit(su32WifiWaitATCmdAckTimerBk); 
	SysGetDurationSecTimer(&su32WifiWaitATCmdAckTimerBk); 
	while(IS_TRUE_BIT(u16TimeOutS)) {
		u16SecS = SysGetDurationSecTimer(&su32WifiWaitATCmdAckTimerBk); 
		for(u16i = 0; u16i < u16SecS; u16i++) {
			M_NOT_ZERO_DEC(u16TimeOutS); 
		}

		{
			e_wifi_bool boRxCmdFlg; 
			u8 *pu8Dat; 
			u16 u16Len; 
			
			boRxCmdFlg = WifiDriverGetDevRxDatPend(WIFI_TASK_CYCLE_MS, &pu8Dat, &u16Len); 
			if(IS_FALSE_BIT(boRxCmdFlg)) {; 
			}
			else	{
				event.buff = pu8Dat; 
				event.size = u16Len; 
				
				WifiAtRetInfoAnalyse(event.buff,event.size); 			
				if(IS_NULL_P(pFunAtCmdHook)) {
				}
				else {
					if(IS_FALSE_BIT(pFunAtCmdHook(event.buff, event.size))) {; 
					}
					else {
						DebugPrintf("at cmd can end by manual\r\n"); 
						M_SetBit(boCmdEndByManual); 
					}
				}
				//-----------------------------------------------------
				if(((str_1 != NULL) && strstr((const s8*)event.buff,str_1))
				|| ((str_2 != NULL) && strstr((const s8*)event.buff,str_2))) {
					M_SetBit(boRetVal); 
					M_SetBit(boNeedBreakFlg); 
				}
				else if(0==memcmp((const s8*)event.buff, 
								WIFI_ATCMD_ACK_ERROR_STR, 
								M_MIN(strlen(WIFI_ATCMD_ACK_ERROR_STR), event.size))) {
					M_ClrBit(boRetVal); 
					M_SetBit(boNeedBreakFlg); 
				}
				else {; 
				}

				WifiDriverGetDevRxDatMemFreeIfNeed(pu8Dat, u16Len); 
				M_ClrBit(boRxCmdFlg); 

				if(IS_TRUE_BIT(boNeedBreakFlg)) {; 
				}
				else {
					if(IS_FALSE_BIT(boCmdEndByManual)) {
					}
					else {
						M_SetBit(boRetVal); 
						M_SetBit(boNeedBreakFlg); 
						DebugPrintf(CRIT"wifi cmd not rx end sign, ended by manual\r\n"); 
					}
				}

				if(IS_FALSE_BIT(boNeedBreakFlg)) {; 
				}
				else {
					break; 
				}			
			}		
		}		
	}
	
	return(boRetVal);
}


void app_wifiTask(void *pdata)
{
	WifiCmdId enWifiCmd; 
	u8 *pucWifiCmd;
	u8 ucErr;
	OSTimeDly(300);//延时3s防止重启过多打印到FLASH中
	while(1)
	{
		pucWifiCmd=OSMboxPend(pstOsMsgForWifiTask,WIFI_TASK_CYCLE_MS,&ucErr);
		if(OS_ERR_NONE!=ucErr)	
		{
		}
		else
		{
			enWifiCmd = CMD_ID_NULL; 
			enWifiCmd=*pucWifiCmd;
			if(CMD_ID_NULL != senWifiCmdBuf) {
				DebugPrintf(CRIT"wifi cmd busy give up new\r\n");
			}
			else {
				senWifiCmdBuf = enWifiCmd; 
			}
		}
		WifiPwrStProc(); 
				
	}
}


