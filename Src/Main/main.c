/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 main.c
* Description		: 	PT810 operation center
* Created			: 	2016.08.26.
* Author			: 	Yu Weifeng
* Function List 		: 	main
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasictools.h"
#include "Config.h"
#include "stm32f4xx_hal.h"
#include "Ucos_ii.h"
#include "Bsp.h"
#include "main.h"
#include "GsmDriverPort.h"
#include "ProtocolManage.h"
#include "TrackerProtocol.h"
#include "Alarm.h"
#include "app_Led.h"
#include "app_Motor.h"
#include "app_PowerChargeCheck.h"
#include "app_PowerStateCheck.h"
#include "Mymalloc.h"
#include "Wifi_usr_port.h"
#include "app_wifiTask.h"
#include "app_GpsManage.h"
#include "ParameterManage.h"
#include "app_TimeManage.h"
#include "app_Gsm.h"
#include "app_LightDistanceSensor.h"
#include "app_CutStateCheck.h"

static void SystemStartCheck();
static void SetLedIdleTime(u32 i_dwIdleTime);
static u8 LowPowerCheckProcess();
static u8 HeartbeatProcess();
static u8 AppOsQCreate(void) ;
static u8 TaskCreate(void) ;

void MainTask(void *pdata);
void AlarmTask(void *pdata);
void VirtualCommunicatePortTask(void *pdata);
void LoginPlatformTask(void *pdata);
void  FixedTimeReportTask(void *pdata);
void PlatformCmdAckTask(void *pdata);
void BatteryChargingTask(void *pdata);
void LedIndicateTask(void *pdata);
void CheckTrackerStateTask(void *pdata);
void SmsProcessTask(void *pdata);;
void SystemResetTask(void *pdata);
void EnterLowPowerModeTask(void *pdata);


OS_FLAG_GRP *pstOsFlagForAppUse; 
OS_EVENT *pstOsQForGsmRx; 
OS_EVENT *pstOsQForWifiRx;
OS_EVENT *pstOsMsgForWifiTask;
OS_EVENT *ptOsQForGpsRx;
OS_FLAG_GRP *ptOsFlagUsedForEnterLowPowerMode;
OS_EVENT *ptOsSemUsedForLoginPlatformOk;
OS_EVENT *ptOsSemUsedForSendSmsOK;
OS_EVENT *ptOsSemUsedForGprsCommunicate;
OS_EVENT *ptOsSemUsedForVirtualCommunicatePort;
OS_EVENT *ptOsSemUsedForGsmNeedRestart;
OS_EVENT *ptOsSemUsedForPlatformReceivedAlarm;
OS_EVENT *ptOsSemUsedForNetParaSetCompleted;
OS_EVENT *ptOsSemUsedForPlatformCommunicateHeartbeatOk;
OS_EVENT *ptOsSemUsedForResumeGsmTask;
OS_EVENT *ptOsSemUsedForSystemNeedReset;
OS_EVENT *ptOsSemUsedForNeedGetNetData;
OS_EVENT *ptOsSemUsedForGprsDataSendIdle;
OS_EVENT *ptOsSemUsedForGprsDataSendReq;
OS_EVENT *ptOsSemUsedForGetWifiApSuccess;

OS_EVENT *ptOsSemUsedForAlarmTask;
OS_EVENT *ptOsSemUsedForCheckedCutState;
OS_EVENT *ptOsSemUsedForCheckedDropState;
OS_EVENT *ptOsSemUsedForCheckedGsmState;
OS_EVENT *ptOsSemUsedForSmsComing;
OS_EVENT *ptOsSemUsedForPhoneComing;
OS_EVENT *ptOsSemUsedForLoginPlatform;
OS_EVENT *ptOsSemUsedForPlatformCmdAck;
OS_EVENT *ptOsSemUsedForVcpCmdAck;

static OS_STK WifiTaskStack[WIFI_TASK_STACK_LEN];
static OS_STK GpsProcessTaskStack[GPS_PROCESS_TASK_STACK_LEN];
static OS_STK GsmTaskStack[GSM_TASK_STACK_LEN];

static OS_STK AlarmTaskStack[ALARM_TASK_STACK_LEN];
static OS_STK VirtualComTaskStack[VIRTUAL_COM_TASK_STACK_LEN];
static OS_STK LoginPlatformTaskStack[LOGIN_PLATFORM_TASK_STACK_LEN];
static OS_STK FixedTimeReportTaskStack[FIXED_TIME_REPORT_TASK_STACK_LEN];
static OS_STK PlatformCmdAckTaskStack[PLATFORM_CMD_ACK_TASK_STACK_LEN];
static OS_STK BatteryChargeTaskStack[BATTERY_CHARGE_TASK_STACK_LEN];
static OS_STK LedIndicateTaskStack[LED_INDICATE_TASK_STACK_LEN];
static OS_STK CheckTrackerStateTaskStack[CHECK_TRACKER_STATE_TASK_STACK_LEN];
static OS_STK SmsProcessTaskStack[SMS_PROCESS_TASK_STACK_LEN];
static OS_STK SystemResetTaskStack[SYSTEM_RESET_TASK_STACK_LEN];
static OS_STK EnterLowPowerModeTaskStack[ENTER_LOW_POWER_MODE_TASK_STACK_LEN];

static T_TaskInfo g_tTaskInfoTab[] ={
	{TaskGsmDriver, (void *)0, &GsmTaskStack[GSM_TASK_STACK_LEN -1], 
	  GSM_TASK_PRIORITY, GSM_TASK_PRIORITY, GsmTaskStack, GSM_TASK_STACK_LEN, 
	  NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "GsmTask"},
	  
	{app_wifiTask, (void *)0, &WifiTaskStack[WIFI_TASK_STACK_LEN -1], 
	 WIFI_TASK_PRIORITY, WIFI_TASK_PRIORITY, WifiTaskStack, WIFI_TASK_STACK_LEN, 
	 NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "WifiTask"},

	{GpsProcessTask, (void *)0, &GpsProcessTaskStack[GPS_PROCESS_TASK_STACK_LEN -1], 
	 GPS_PROCESS_TASK_PRIORITY, GPS_PROCESS_TASK_PRIORITY, GpsProcessTaskStack, GPS_PROCESS_TASK_STACK_LEN, 
	 NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "GpsProcessTask"},
	 
	/*由于uCos没有进程的概念，只能这么放，如果使用linux的话
	   就可以把上面的放入一个外部设备进程，下面的放入一个具体业务进程,
	   实现真正的互相独立，业务部分就真正独立出来了,便于各个项目移植*/
	
	{VirtualCommunicatePortTask, NULL, &VirtualComTaskStack[VIRTUAL_COM_TASK_STACK_LEN -1], 
	  VIRTUAL_COM_TASK_PRIORITY, VIRTUAL_COM_TASK_PRIORITY, VirtualComTaskStack, 
	  VIRTUAL_COM_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "VirtualComTask"},
	  	  
	{LoginPlatformTask, NULL, &LoginPlatformTaskStack[LOGIN_PLATFORM_TASK_STACK_LEN -1], 
	  LOGIN_PLATFORM_TASK_PRIORITY, LOGIN_PLATFORM_TASK_PRIORITY, LoginPlatformTaskStack, 
	  LOGIN_PLATFORM_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "LoginPlatformTask"},

	{FixedTimeReportTask, NULL, &FixedTimeReportTaskStack[FIXED_TIME_REPORT_TASK_STACK_LEN -1], 
	  FIXED_TIME_REPORT_TASK_PRIORITY, FIXED_TIME_REPORT_TASK_PRIORITY, FixedTimeReportTaskStack, 
	  FIXED_TIME_REPORT_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "FixedTimeReportTask"},

	{PlatformCmdAckTask, NULL, &PlatformCmdAckTaskStack[PLATFORM_CMD_ACK_TASK_STACK_LEN -1], 
	  PLATFORM_CMD_ACK_TASK_PRIORITY, PLATFORM_CMD_ACK_TASK_PRIORITY, PlatformCmdAckTaskStack, 
	  PLATFORM_CMD_ACK_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "PlatformCmdAckTask"},

	{BatteryChargingTask, NULL, &BatteryChargeTaskStack[BATTERY_CHARGE_TASK_STACK_LEN -1], 
	  BATTERY_CHARGE_TASK_PRIORITY, BATTERY_CHARGE_TASK_PRIORITY, BatteryChargeTaskStack, 
	  BATTERY_CHARGE_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "BatteryChargeTask"},

	{LedIndicateTask, NULL, &LedIndicateTaskStack[LED_INDICATE_TASK_STACK_LEN -1], 
	  LED_INDICATE_TASK_PRIORITY, LED_INDICATE_TASK_PRIORITY, LedIndicateTaskStack, 
	  LED_INDICATE_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "LedIndicateTask"},

	{CheckTrackerStateTask, NULL, &CheckTrackerStateTaskStack[CHECK_TRACKER_STATE_TASK_STACK_LEN -1], 
	  CHECK_TRACKER_STATE_TASK_PRIORITY, CHECK_TRACKER_STATE_TASK_PRIORITY, CheckTrackerStateTaskStack, 
	  CHECK_TRACKER_STATE_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "CheckTrackerStateTask"},

	{SmsProcessTask, NULL, &SmsProcessTaskStack[SMS_PROCESS_TASK_STACK_LEN -1], 
	  SMS_PROCESS_TASK_PRIORITY, SMS_PROCESS_TASK_PRIORITY, SmsProcessTaskStack, 
	  SMS_PROCESS_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "SmsProcessTask"},

	{SystemResetTask, NULL, &SystemResetTaskStack[SYSTEM_RESET_TASK_STACK_LEN -1], 
	  SYSTEM_RESET_TASK_PRIORITY, SYSTEM_RESET_TASK_PRIORITY, SystemResetTaskStack, 
	  SYSTEM_RESET_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "SystemResetTask"},

	{EnterLowPowerModeTask, NULL, &EnterLowPowerModeTaskStack[ENTER_LOW_POWER_MODE_TASK_STACK_LEN -1], 
	  ENTER_LOW_POWER_MODE_TASK_PRIORITY, ENTER_LOW_POWER_MODE_TASK_PRIORITY, EnterLowPowerModeTaskStack, 
	  ENTER_LOW_POWER_MODE_TASK_STACK_LEN, NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR, "EnterLowPowerModeTask"},
};
struct OsQInfo OsQInfoTab[] = 
{
	{(void *)pstEveTabForGsmRx, GSM_RX_EVENT_TAB_SIZE, &pstOsQForGsmRx, "QGsmRx"},
	{(void *)pstEveTabForWifiRx, WIFI_RX_EVENT_TAB_SIZE, &pstOsQForWifiRx, "QWifiRx"},
	{(void *)ptEveTabForGpsRx,  GPS_RX_EVENT_TAB_SIZE,    &ptOsQForGpsRx,  "QGpsRx"}
}; 

/*****************************************************************************
-Fuction		: main
-Description	: main
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
int main(void)
{
	u8 ucError;
	OSInit();
	
	OSTaskCreateExt(AlarmTask, NULL, &AlarmTaskStack[ALARM_TASK_STACK_LEN - 1], 
					ALARM_TASK_PRIORITY, ALARM_TASK_PRIORITY, AlarmTaskStack, ALARM_TASK_STACK_LEN, 
					NULL, OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
	OSTaskNameSet(ALARM_TASK_PRIORITY, "AlarmTask", &ucError); 
	OSStart();


	return(0);
}
/*****************************************************************************
-Fuction		: AlarmTask
-Description	: AlarmTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void AlarmTask(void *pdata)
{
	T_Alarm tAlarm;
	u8 ret=FALSE;
	u8 ucErr=0;
	u8 ucSmsSwitchBuf=0;
	u16 wSmsSwitchBufLen=0;
	s32 i;
	static PT_ProtocolManage ptTrackerProtocolOpr;
	static PT_ProtocolManage ptSmsProtocolOpr;
	static u8 aucAlarmBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};
	static u8 aucSmsAlarmBuf[SMS_PROTOCOL_MAX_SEND_LEN]={0};	
	static u8 aucAlarmSendBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};	
	static u8 aucSmsAlarmSendBuf[SMS_PROTOCOL_MAX_LEN]={0};
	u16 wAlarmSendLen=0;
	u16 wSmsAlarmSendLen=0;
	
	SysInit();
	MAB_MallocInit();
#if (OS_TASK_STAT_EN > 0)
		OSStatInit();	  /* Determine CPU capacity  after systick start,before other tasks be created	 */
#endif
	TaskCreate();	
	AppOsQCreate();
	
	BaseSupportInit();

	ptOsSemUsedForPlatformReceivedAlarm=OSSemCreate(0);	
	ptOsSemUsedForResumeGsmTask=OSSemCreate(0);	
	ptOsSemUsedForSendSmsOK=OSSemCreate(0);			
	ptOsSemUsedForAlarmTask=OSSemCreate(0);
	ptOsSemUsedForGprsDataSendReq=OSSemCreate(0);
	ptOsSemUsedForGprsDataSendIdle=OSSemCreate(0);
	ptOsSemUsedForGetWifiApSuccess=OSSemCreate(0);
	SystemStartCheck();
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,20);
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,ALARM_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
		OSSemPend(ptOsSemUsedForAlarmTask,0,&ucErr);	
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,ALARM_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
		/*获取具体有效报警状态*/
		ret=g_tAlarmOpr.GetEffectiveState(&tAlarm);
		/*判断后上传到平台或短信*/
		if(FALSE==ret)
		{
			DebugPrintf(ERR"GetEffectiveState err\r\n");
		}
		else
		{
			g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
			for(i=0;i<3;i++)
			{		
				ptTrackerProtocolOpr->ProtocolReportAlarm(tAlarm,aucAlarmBuf,sizeof(aucAlarmBuf),aucAlarmSendBuf,&wAlarmSendLen);
				
				OSSemPend(ptOsSemUsedForPlatformReceivedAlarm,18*OS_TICKS_PER_SEC,&ucErr);	
				if(OS_ERR_NONE!=ucErr)
				{
					DebugPrintf(ERR"Receive Alarm Ack Err:%d,Count:%d \r\n",ucErr,i);
				}
				else
				{
					break;
				}
			}			
			//短信报警
			g_tParameterOpr.GetPara("AlarmPara",SmsAlarmSwitch,&ucSmsSwitchBuf,&wSmsSwitchBufLen);
			if(0==ucSmsSwitchBuf)
			{
			}
			else
			{
				g_tProtocol.GetProtocolOpr("SmsProtocol",&ptSmsProtocolOpr);
				ptSmsProtocolOpr->ProtocolReportAlarm(tAlarm,aucSmsAlarmBuf,sizeof(aucSmsAlarmBuf),aucSmsAlarmSendBuf,&wSmsAlarmSendLen);
			}
		}
	}
}
/*****************************************************************************
-Fuction		: TaskCreate
-Description	: TaskCreate
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 TaskCreate(void) 
{
	u8 ret=0; 
	u8 ucError; 
	s32 i; 
	u16 wTaskNum; 
	PT_TaskInfo ptTaskInfo; 
	M_SetBit(ret); 
	wTaskNum = sizeof(g_tTaskInfoTab) / sizeof(T_TaskInfo); 
	for(i = 0; i < wTaskNum; i++) 
	{
		ptTaskInfo = &g_tTaskInfoTab[i]; 
		ucError = OSTaskCreateExt(ptTaskInfo->task, 
							        ptTaskInfo->p_arg,
							        ptTaskInfo->ptos, 
							        ptTaskInfo->prio, 
							        ptTaskInfo->id, 
							        ptTaskInfo->pbos, 
							        ptTaskInfo->stk_size, 
							        ptTaskInfo->pext, 
							        ptTaskInfo->opt);
		if(OS_ERR_NONE != ucError)
		{
			M_ClrBit(ret); 
			DebugPrintf(ERR"task create err\r\n"); 
			break; 
		}
		else 
		{
			OSTaskNameSet(ptTaskInfo->prio, (INT8U *)ptTaskInfo->NameStr, &ucError); 
			if(OS_ERR_NONE != ucError) 
			{
				DebugPrintf(ERR"task name err\r\n"); 
			}
			else 
			{				
			}
		}		
	}	
	return ret; 
}
/*****************************************************************************
-Fuction		: VirtualCommunicatePortTask
-Description	: VirtualCommunicatePortTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void VirtualCommunicatePortTask(void *pdata)
{
	u8 ret=FALSE;
	u8 ucErr;
	PT_ProtocolManage ptTrackerProtocolOpr;
	static u8 aucVcpBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
	static u8 acuVcpAckBuf[TRACKER_PROTOCOL_ACK_MAX_LEN]={0};	
	static u8 acuVcpAckSendBuf[TRACKER_PROTOCOL_ACK_MAX_LEN]={0};
	static T_HandleCmdAck s_tHandCmdAck={0};
	u16 wVcpBufLen=0;
	s_tHandCmdAck.pucDataSendBuf=acuVcpAckSendBuf;
	s_tHandCmdAck.pucDataBuf=acuVcpAckBuf;
	s_tHandCmdAck.wMaxDataLen=sizeof(acuVcpAckBuf);
	ptOsSemUsedForVcpCmdAck=OSSemCreate(0);	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,10);
		OSSemPend(ptOsSemUsedForVcpCmdAck,0,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			wVcpBufLen=0;
			ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
			ret&=ptTrackerProtocolOpr->ProtocolGetData(PROTOCOL_USED_FOR_VCP,aucVcpBuf,&wVcpBufLen,sizeof(aucVcpBuf));
			if(FALSE==ret)
			{
				DebugPrintf(ERR"VcpProtocolGetData err \r\n");
			}
			else
			{
				ret=ptTrackerProtocolOpr->ProtocolCheckData(aucVcpBuf,wVcpBufLen);
				if(FALSE==ret)
				{
					DebugPrintf(ERR"VcpProtocolCheckData err \r\n");
				}
				else
				{
					ret=ptTrackerProtocolOpr->ProtocolAnalyseData(PROTOCOL_USED_FOR_VCP,aucVcpBuf,wVcpBufLen,&s_tHandCmdAck);
					if(FALSE==ret)
					{
						DebugPrintf(ERR"VcpProtocolAnalyseData err \r\n");
					}
					else
					{
					}
				}
			}
		}
	}
}
/*****************************************************************************
-Fuction		: PlatformCmdAckTask
-Description	: PlatformCmdAckTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void PlatformCmdAckTask(void *pdata)
{
	u8 ret=FALSE;	
	u8 ucErr=0;
	PT_ProtocolManage ptTrackerProtocolOpr;		
	static u8 aucPlatformCmdBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
	static u8 aucPlatformCmdAckBuf[TRACKER_PROTOCOL_ACK_MAX_LEN]={0};	
	static u8 aucPlatformCmdAckSendBuf[TRACKER_PROTOCOL_ACK_MAX_LEN]={0};
	static T_HandleCmdAck s_tHandCmdAck={0};
	u16 wPlatformCmdBufLen=0;
	s_tHandCmdAck.pucDataSendBuf=aucPlatformCmdAckSendBuf;
	s_tHandCmdAck.pucDataBuf=aucPlatformCmdAckBuf;
	s_tHandCmdAck.wMaxDataLen=sizeof(aucPlatformCmdAckBuf);
	ptOsSemUsedForPlatformCmdAck=OSSemCreate(0);	
	while(1)
	{
		OSTimeDly(5);		
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,PLATFORM_CMD_ACK_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
		OSSemPend(ptOsSemUsedForPlatformCmdAck,0,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{	
			OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,PLATFORM_CMD_ACK_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);

			wPlatformCmdBufLen=0;
			ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
			ret&=ptTrackerProtocolOpr->ProtocolGetData(PROTOCOL_USED_FOR_GPRS,aucPlatformCmdBuf,&wPlatformCmdBufLen,sizeof(aucPlatformCmdBuf));
			if(FALSE==ret)
			{
				DebugPrintf(ERR"PlatformProtocolGetData err \r\n");
			}
			else
			{
				ret=ptTrackerProtocolOpr->ProtocolCheckData(aucPlatformCmdBuf,wPlatformCmdBufLen);
				if(FALSE==ret)
				{
					DebugPrintf(ERR"PlatformProtocolCheckData err \r\n");
				}
				else
				{
					ret=ptTrackerProtocolOpr->ProtocolAnalyseData(PROTOCOL_USED_FOR_GPRS,aucPlatformCmdBuf,wPlatformCmdBufLen,&s_tHandCmdAck);
					if(FALSE==ret)
					{
						DebugPrintf(ERR"PlatformProtocolAnalyseData err \r\n");
					}
					else
					{
					}
				}
			}
		}
	}
}
/*****************************************************************************
-Fuction		: LoginPlatformTask
-Description	: LoginPlatformTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void LoginPlatformTask(void *pdata)
{
	u8 ucErr=0;	
	u8 ret=FALSE;
	PT_ProtocolManage ptTrackerProtocolOpr;	
	static u8 aucLoginBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};
	static u8 aucLoginSendBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};
	u16 wLoginSendLen=0;
	ptOsSemUsedForLoginPlatformOk=OSSemCreate(0);
	OSFlagPost(pstOsFlagForAppUse, LOGIN_PLATFORM_ENABLE_FLAG, OS_FLAG_SET, &ucErr);			
	while(1)
	{
		OSFlagPend(pstOsFlagForAppUse, LOGIN_PLATFORM_ENABLE_FLAG,OS_FLAG_WAIT_SET_ALL, 0, &ucErr); //10秒醒一次醒来喂狗
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{		
			OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,LOGIN_PLATFORM_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
			ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
			ret=ptTrackerProtocolOpr->ProtocolLogin(aucLoginBuf,sizeof(aucLoginBuf),aucLoginSendBuf,&wLoginSendLen);
			if(FALSE==ret)
			{
				DebugPrintf("Login Platform err!\r\n");//减少打印到FALSH减少擦写flash
			}
			else
			{		
				DebugPrintf(CRIT">>>>>>>>Login platform ok!\r\n");
				OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,LOGIN_PLATFORM_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
				OSFlagPost(pstOsFlagForAppUse, LOGIN_PLATFORM_OK_FLAG, OS_FLAG_SET, &ucErr);						
				OSFlagPost(pstOsFlagForAppUse, LOGIN_PLATFORM_ENABLE_FLAG, OS_FLAG_CLR, &ucErr);	
				ret=TRUE;	
			}
		}
		OSTimeDlyHMSM(0,0,5,20);//不断登录间隔
	}
}
/*****************************************************************************
-Fuction		: FixedTimeReportTask
-Description	: FixedTimeReportTask
-Input			: 
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void  FixedTimeReportTask(void *pdata)
{
	u8 ReportIntervalBuf[4];
	u16 wReportIntervalLen=4;	
	u32 dwReportInterval=0;		
	PT_ProtocolManage ptTrackerProtocolOpr;	
	static u32 s_dwBackupFixReportTime=0;
	u32 dwCurrentFixReportTime=0;
	u8 ret=FALSE;
	u8 ucErr=0,PendErr=0;	
	u8 ucFixedTimeReportCloseFlag=FALSE;
	static u8 s_ucIsNotFirstFixedTimeReportFlag=FALSE;
	static u8 aucFixedTimeReportBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
	static u8 aucFixedTimeReportSendBuf[TRACKER_PROTOCOL_MAX_LEN]={0};
	u16 wFixedTimeReportSendLen=0;
	while(1)
	{
		g_tParameterOpr.GetPara("CommonPara",ReportInterval,ReportIntervalBuf,&wReportIntervalLen);
		memcpy(&dwReportInterval,ReportIntervalBuf,wReportIntervalLen);
		if(dwReportInterval<1)
		{
			if(IS_FALSE_BIT(ucFixedTimeReportCloseFlag))//防止打印过多
			{
				DebugPrintf(ERR"Report Interval too short,FixTimeReportClose!\r\n");
				M_SetBit(ucFixedTimeReportCloseFlag);
			}
			else
			{
			}
		}
		else
		{
			M_ClrBit(ucFixedTimeReportCloseFlag);
			if(IS_FALSE_BIT(s_ucIsNotFirstFixedTimeReportFlag))
			{
				g_tTime.WakeUpConfig(dwReportInterval);
				M_SetBit(s_ucIsNotFirstFixedTimeReportFlag);
			}
			else
			{
			}
			//至少要加100也就是1s(时间越短加的越多),因为前面设置了唤醒
			//同样的时间,等待则会超时，所以多加等待时间,可以考虑任务挂起的方式			
			OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,FIXED_TIME_REPORT_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
			OSFlagPend(pstOsFlagForAppUse, FIXED_TIME_REPORT_ENABLE_FLAG,OS_FLAG_WAIT_SET_ALL, (dwReportInterval*OS_TICKS_PER_SEC+100),&PendErr);
			OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,FIXED_TIME_REPORT_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);

			OSFlagPost(pstOsFlagForAppUse, FIXED_TIME_REPORT_ENABLE_FLAG, OS_FLAG_CLR, &ucErr); 						
			g_tTime.WakeUpConfig(dwReportInterval);			
			if(OS_ERR_NONE!=PendErr)
			{
				DebugPrintf(ERR"PendFixedTimeReportEnable err:%d\r\n",PendErr);//校准了RTC可能会出现
			}
			else
			{
			}	
			ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
			ret&=ptTrackerProtocolOpr->ProtocolReport(aucFixedTimeReportBuf,sizeof(aucFixedTimeReportBuf),aucFixedTimeReportSendBuf,&wFixedTimeReportSendLen); 
			if(FALSE==ret)
			{
				DebugPrintf(ERR"FixedTimeReportTask err\r\n");
			}
			else
			{
			}
		}
		OSTimeDlyHMSM(0,0,0,20);
	}
}
/*****************************************************************************
-Fuction		: BatteryChargingTask
-Description	: BatteryChargingTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void BatteryChargingTask(void *pdata)
{
        u8 ret=0;
	u8 ucErr;	
	u8 ucPowerChargeState=0;
	u8 ucPowerCheckState=0;	
	u8 ucUsbState;
	u8 ucUsbIsConfigedFlag=FALSE;
	T_Alarm tAlarm;
	E_PowerCheckStateType ePowerCheckState;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,20);//消抖
		g_tUsb.GetUsbState(&ucUsbState);
		if(USB_IS_CONNECT_STATE==ucUsbState)
		{
			OSTimeDlyHMSM(0,0,0,20);//消抖				
			g_tUsb.GetUsbState(&ucUsbState);				
			if(USB_IS_CONNECT_STATE==ucUsbState)
			{
				g_tLed.RedLedOn();
				if(IS_FALSE_BIT(ucUsbIsConfigedFlag))
				{
					g_tUsb.Config();//接上USB才配置USB功能				
					M_SetBit(ucUsbIsConfigedFlag);
					OSFlagPost(pstOsFlagForAppUse, USB_CONNECT_OK_FLAG, OS_FLAG_SET, &ucErr);			
				}
				else
				{
				}
				g_tPowerStateCheck.GetPowerCheckState(&ePowerCheckState,&ucPowerCheckState);
				if(NormalPowerState==ePowerCheckState)
				{
					OSTimeDly(1);//上面执行ADC采样需要1s
					g_tLed.RedLedOff(); 			
					OSTimeDly(100);
				}
				else if(NormalPowerState<ePowerCheckState)
				{
					OSTimeDly(50);
					g_tLed.RedLedOff();
					OSTimeDly(500);
				}
				else
				{
				}			
				g_tAlarmOpr.GetState(&tAlarm);
				tAlarm.LowPowerAlarm=0;
				g_tAlarmOpr.SetState(tAlarm);//充电一直醒着所以不需要中断唤醒		
				ret=TRUE;
			}
			else
			{
			}
		}
		else if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
		{			
			OSTimeDlyHMSM(0,0,0,20);//消抖				
			g_tUsb.GetUsbState(&ucUsbState);				
			if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
			{
				if(IS_FALSE_BIT(ucUsbIsConfigedFlag))
				{				
				}
				else
				{
					g_tUsb.DeConfig();//没接上USB不配置USB功能					
					M_ClrBit(ucUsbIsConfigedFlag);
					OSFlagPost(pstOsFlagForAppUse, USB_CONNECT_OK_FLAG, OS_FLAG_CLR, &ucErr);			
				}
				g_tLed.RedLedOff(); 						
				OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,BATTERY_CHARGE_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
				g_tUsb.VbusSetFallExti();
				OSTaskSuspend(BATTERY_CHARGE_TASK_PRIORITY);
				OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,BATTERY_CHARGE_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
			}
			else
			{
			}
			//复位后门不启用
		}		
		else
		{
			DebugPrintf(ERR"UsbStateUnKnow\r\n");
			ret=FALSE;
		}		
	}
}
/*****************************************************************************
-Fuction		: LedIndicateTask
-Description	: LedIndicateTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void LedIndicateTask(void *pdata)
{
	u8 ucErr;
	u8 ucPlatformCommunicationHeartbeatCount=0;
	u8 ucBatteryStateCheckCount=0;
	u8 ucLowerPowerStateFlag=FALSE;
	ptOsSemUsedForPlatformCommunicateHeartbeatOk=OSSemCreate(0);	
	ptOsSemUsedForGsmNeedRestart=OSSemCreate(0);
	while(1)
	{	
		IWDG_Feed();//11秒醒一次醒来喂狗
		OSFlagPend(pstOsFlagForAppUse, LOGIN_PLATFORM_OK_FLAG,OS_FLAG_WAIT_SET_ALL, (10*OS_TICKS_PER_SEC), &ucErr);	//10秒醒一次醒来喂狗
		if(OS_ERR_NONE!=ucErr)
		{
			if(FALSE==ucLowerPowerStateFlag)
			{
				SetRtcAlarmTime(LED_LIGHT_TIME);
				OSFlagPend(pstOsFlagForAppUse, BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC+100),&ucErr);
				OSFlagPost(pstOsFlagForAppUse,BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_CLR,&ucErr);
			}
			else
			{
				g_tLed.RedLedOn();
				SetRtcAlarmTime(LED_LIGHT_TIME);
				OSFlagPend(pstOsFlagForAppUse, BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_WAIT_SET_ALL, (OS_TICKS_PER_SEC+100),&ucErr);
				OSFlagPost(pstOsFlagForAppUse,BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_CLR,&ucErr);
				g_tLed.RedLedOff();
			}
			IWDG_Feed();//11秒醒一次醒来喂狗
			ucPlatformCommunicationHeartbeatCount++;//计时停止模式无效改为计次
			ucBatteryStateCheckCount++;
		}
		else
		{
			g_tLed.BlueLedOn();			
			SetRtcAlarmTime(LED_LIGHT_TIME);
			SetLedIdleTime(LED_LIGHT_TIME);
			g_tLed.BlueLedOff();
			if(FALSE==ucLowerPowerStateFlag)
			{
				SetRtcAlarmTime(LED_OUT_TIME);
			}
			else
			{
				g_tLed.RedLedOn();
				SetRtcAlarmTime(LED_LIGHT_TIME);				
				SetLedIdleTime(LED_LIGHT_TIME);
				g_tLed.RedLedOff();
				SetRtcAlarmTime(LED_OUT_TIME-LED_LIGHT_TIME);
			}
			SetLedIdleTime(LED_OUT_TIME);
			IWDG_Feed();//11秒醒一次醒来喂狗
			ucPlatformCommunicationHeartbeatCount++;
			ucBatteryStateCheckCount++;
		}		
		if(ucPlatformCommunicationHeartbeatCount<(u8)((float)PLATFORM_HEARTBEAT_TIME/(LED_OUT_TIME+LED_LIGHT_TIME)+0.5))
		{
		}
		else
		{	
			ucPlatformCommunicationHeartbeatCount=0;
			if(FALSE==HeartbeatProcess())/*心跳处理*/
			{
				ucPlatformCommunicationHeartbeatCount++;
				ucBatteryStateCheckCount++;
			}
			else
			{
			}
		}
		if(ucBatteryStateCheckCount<(u8)((float)BATTERY_STATE_CHECK_TIME/(LED_OUT_TIME+LED_LIGHT_TIME)+0.5)-1)//由于心跳执行时间这里减去一次11s
		{
		}
		else
		{
			if(FALSE==LowPowerCheckProcess())
			{
				M_ClrBit(ucLowerPowerStateFlag);
			}
			else
			{
				M_SetBit(ucLowerPowerStateFlag);
			}
			ucBatteryStateCheckCount=0;
		}
	}
}
/*****************************************************************************
-Fuction		: CheckTrackerStateTask
-Description	: CheckTrackerStateTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void CheckTrackerStateTask(void *pdata)
{
	u8 ret=FALSE;
	u8 ucErr=0;		
	u16 ps=0,ir=0,als=0;
	ptOsSemUsedForCheckedCutState=OSSemCreate(0);	
	ptOsSemUsedForCheckedDropState=OSSemCreate(0);
	ptOsSemUsedForCheckedGsmState=OSSemCreate(0);

	ptOsSemUsedForNeedGetNetData=OSSemCreate(0);
	while(1)
	{
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,TRACK_STATE_IDLE_FLAG,OS_FLAG_SET,&ucErr);
		OSTaskSuspend(CHECK_TRACKER_STATE_TASK_PRIORITY);		
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,TRACK_STATE_IDLE_FLAG,OS_FLAG_CLR,&ucErr);

		OSSemPend(ptOsSemUsedForCheckedCutState,OS_TICKS_PER_SEC/50,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			g_tCutStateCheck.CheckCutState();
		}
		OSSemPend(ptOsSemUsedForCheckedDropState,OS_TICKS_PER_SEC/100,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			g_tLightDistanceSensor.CheckDropState();
			g_tLightDistanceSensor.GetSensorData(&ir,&ps,&als); 	
			DebugPrintf(CRIT"TaskLD FLAG:%xh,PS:%xh,ALS:%xh\r\n",ir,ps,als);
		}
		OSSemPend(ptOsSemUsedForCheckedGsmState,OS_TICKS_PER_SEC/100,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			ret=g_tGsm.GsmCheckGsmState();
			if(FALSE==ret)
			{
			}
			else
			{
				OSFlagAccept(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL,&ucErr);
				if(OS_ERR_NONE!=ucErr)//GPRS不ok，gsm是一直醒的
				{	
				}
				else
				{	
					OSFlagAccept(ptOsFlagUsedForEnterLowPowerMode, GSM_TASK_IDLE_FLAG,OS_FLAG_WAIT_SET_ALL, &ucErr);
					if(OS_ERR_NONE!=ucErr)//GSM睡眠后才需要唤醒
					{	
					}
					else
					{
						OSSemPost(ptOsSemUsedForResumeGsmTask);//不使用手动网络数据
					}
				}
			}
		}
	}
}
/*****************************************************************************
-Fuction		: SystemResetTask
-Description	: SystemResetTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void SmsProcessTask(void *pdata)
{
	u8 ret=FALSE;	
	u8 ucErr;
	
	static u8 aucSmsReportBuf[SMS_PROTOCOL_MAX_SEND_LEN]={0};	
	static u8 aucSmsReportSendBuf[SMS_PROTOCOL_MAX_LEN]={0};
	u16 wSmsReportSendLen=0;
	
	static PT_ProtocolManage ptTrackerProtocolOpr;
	static u8 aucSmsAckSendBuf[SMS_PROTOCOL_MAX_LEN]={0};
	static u8 aucSmsHandleBuf[SMS_PROTOCOL_MAX_LEN]={0};
	static T_HandleCmdAck s_tHandCmdAck={0};
	s_tHandCmdAck.pucDataSendBuf=aucSmsAckSendBuf;
	s_tHandCmdAck.pucDataBuf=aucSmsHandleBuf;
	s_tHandCmdAck.wMaxDataLen=sizeof(aucSmsHandleBuf);
	ptOsSemUsedForPhoneComing=OSSemCreate(0);
	ptOsSemUsedForSmsComing=OSSemCreate(0);
	while(1)
	{
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,SMS_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
		OSTaskSuspend(SMS_PROCESS_TASK_PRIORITY);	
		OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,SMS_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
		
		ret=g_tProtocol.GetProtocolOpr("SmsProtocol",&ptTrackerProtocolOpr);
		OSSemPend(ptOsSemUsedForPhoneComing,OS_TICKS_PER_SEC/100,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			ret&=ptTrackerProtocolOpr->ProtocolReport(aucSmsReportBuf,sizeof(aucSmsReportBuf),aucSmsReportSendBuf,&wSmsReportSendLen);	
			if(FALSE==ret)
			{
				DebugPrintf(ERR"SmsProtocolReport err \r\n");
			}
			else
			{
			}
		}
		
		OSSemPend(ptOsSemUsedForSmsComing,OS_TICKS_PER_SEC/100,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			ret&=ptTrackerProtocolOpr->ProtocolGetData(PROTOCOL_USED_FOR_SMS,NULL,NULL,NULL);
			if(FALSE==ret)
			{
				DebugPrintf(ERR"SmsProtocolGetData err \r\n");
			}
			else
			{
				ret=ptTrackerProtocolOpr->ProtocolCheckData(NULL,NULL);
				if(FALSE==ret)
				{
					DebugPrintf(ERR"SmsProtocolCheckData err \r\n");
				}
				else
				{
					ret=ptTrackerProtocolOpr->ProtocolAnalyseData(PROTOCOL_USED_FOR_SMS,NULL,NULL,&s_tHandCmdAck);
					if(FALSE==ret)
					{
						DebugPrintf(ERR"SmsProtocolAnalyseData err \r\n");
					}
					else
					{
					}
				}
			}
		}	
		OSTimeDlyHMSM(0,0,0,20);
	}
}

/*****************************************************************************
-Fuction		: SystemResetTask
-Description	: SystemResetTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void SystemResetTask(void *pdata)
{
	u8 ucErr=0;	
	ptOsSemUsedForSystemNeedReset=OSSemCreate(0);
	static u8 aucResetBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};
	static u8 aucResetSendBuf[TRACKER_PROTOCOL_LOCATION_INFO_MAX_LEN]={0};
	u16 wResetSendLen=0;
	while(1)
	{
		OSSemPend(ptOsSemUsedForSystemNeedReset,0,&ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			SystemReset(aucResetBuf,sizeof(aucResetBuf),aucResetSendBuf,&wResetSendLen);
		}
		OSTimeDlyHMSM(0,0,0,20);
	}
}
/*****************************************************************************
-Fuction		: EnterLowPowerModeTask
-Description	: EnterLowPowerModeTask
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
void EnterLowPowerModeTask(void *pdata)
{
	u8 ucErr=0;
	u8 ucEnterStopModeCount=0;
	OSTimeDlyHMSM(0,0,6,120);		
	DebugDataSend(g_pcLogo,strlen(g_pcLogo));
	
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,HEARTBEAT_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,SMS_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,PHONE_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
	while(1)
	{
		//OSTimeDlyHMSM(0,0,0,50);	
		OSFlagPend(ptOsFlagUsedForEnterLowPowerMode, ALARM_TASK_IDLE_FLAG |
													   LOGIN_PLATFORM_TASK_IDLE_FLAG |
													   FIXED_TIME_REPORT_TASK_IDLE_FLAG |
													   PLATFORM_CMD_ACK_TASK_IDLE_FLAG |
													   SMS_TASK_IDLE_FLAG |
													   PHONE_TASK_IDLE_FLAG |
													   BATTERY_CHARGE_TASK_IDLE_FLAG |
													   WIFI_TASK_IDLE_FLAG |
													   GSM_TASK_IDLE_FLAG |
													   HEARTBEAT_TASK_IDLE_FLAG |
													   TRACK_STATE_IDLE_FLAG |
													   LED_INDICATE_TASK_IDLE_FLAG ,
				OS_FLAG_WAIT_SET_ALL, 0, &ucErr);
		if(OS_ERR_NONE!=ucErr)
		{
		}
		else
		{
			OSTimeDlyHMSM(0,0,0,1);
			//再次确认是否进去停止模式
			OSFlagPend(ptOsFlagUsedForEnterLowPowerMode, ALARM_TASK_IDLE_FLAG |
														   LOGIN_PLATFORM_TASK_IDLE_FLAG |
														   FIXED_TIME_REPORT_TASK_IDLE_FLAG |
														   PLATFORM_CMD_ACK_TASK_IDLE_FLAG |
														   SMS_TASK_IDLE_FLAG |
														   PHONE_TASK_IDLE_FLAG |
														   BATTERY_CHARGE_TASK_IDLE_FLAG |
														   WIFI_TASK_IDLE_FLAG |
														   GSM_TASK_IDLE_FLAG |
														   HEARTBEAT_TASK_IDLE_FLAG |	
														   TRACK_STATE_IDLE_FLAG |
														   LED_INDICATE_TASK_IDLE_FLAG ,
					OS_FLAG_WAIT_SET_ALL, 0, &ucErr);
			if(OS_ERR_NONE!=ucErr)
			{
			}
			else
			{
				ucEnterStopModeCount++;
				if(240>ucEnterStopModeCount)//减少打印到flash中
				{
					DebugPrintf("AllTaskIsIdle,SystemEnterStopMode:%d!\r\n",ucEnterStopModeCount);			
				}
				else
				{
					DebugPrintf(CRIT"AllTaskIsIdle,SystemEnterStopMode!\r\n");	
					ucEnterStopModeCount=0;
				}
				SystemEnterStopMode();
			}
		}
	}
}
/************************主流程操作支持函数************************/
/*****************************************************************************
-Fuction		: AppOsQCreate
-Description	: COPY BD的代码
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 AppOsQCreate(void) 
{
	u8 boRetVal123; 
	u8 u8Error; 
	u8 u8i; 
	u8 u8Num; 
	struct OsQInfo *pstInfo; 

	pstOsFlagForAppUse = OSFlagCreate(0, &u8Error); //For app use
	OSFlagNameSet(pstOsFlagForAppUse, "FlagApp", &u8Error); 
	ptOsFlagUsedForEnterLowPowerMode= OSFlagCreate(0, &u8Error); //For LowPowerMode use
	OSFlagNameSet(ptOsFlagUsedForEnterLowPowerMode, "FlagLowPowerMode", &u8Error); 
	
	pstOsMsgForWifiTask=OSMboxCreate((void *)NULL);
		
	M_SetBit(boRetVal123); 
	u8Num = sizeof(OsQInfoTab) / sizeof(struct OsQInfo); 
	for(u8i = 0; u8i < u8Num; u8i++) {
		pstInfo = &OsQInfoTab[u8i]; 
		*pstInfo->p2stEcb = OSQCreate(pstInfo->start, pstInfo->size); 
		if(NULL == *pstInfo->p2stEcb) {
			DebugPrintf("Os q create err\r\n"); 
			M_ClrBit(boRetVal123); 
			break; 
		}
		else {
			OSEventNameSet(*pstInfo->p2stEcb, (INT8U *)pstInfo->NameStr, &u8Error); 
		}
	}
	return boRetVal123; 
}
/*****************************************************************************
-Fuction		: HeartbeatProcess
-Description	: HeartbeatProcess
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 HeartbeatProcess()
{
	u8 ret=FALSE;
	u8 ucErr;
	static u8 s_ucHeartbeatNoOkCount=0;
	static u8 aucSendDataBuf[TRACKER_PROTOCOL_BASE_LEN];
	u16 wSendLen=0;
	PT_ProtocolManage ptTrackerProtocolOpr;
	
	DebugPrintf(">>>>>>>>Heartbeat>>>>>>>>>\r\n");			
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,HEARTBEAT_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
	ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);		
	ret&=ptTrackerProtocolOpr->ProtocolHeartBeat(aucSendDataBuf,&wSendLen);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"ProtocolHeartBeat  err \r\n");				
		s_ucHeartbeatNoOkCount++;
	}
	else
	{			
		IWDG_Feed();//10秒醒一次醒来喂狗
		OSSemPend(ptOsSemUsedForPlatformCommunicateHeartbeatOk,(10*OS_TICKS_PER_SEC+100),&ucErr); //11秒没回应
		if(OS_ERR_NONE!=ucErr)
		{
			s_ucHeartbeatNoOkCount++;
			ret=FALSE;
		}
		else
		{
			s_ucHeartbeatNoOkCount=0;
			ret=TRUE;
		}				
		IWDG_Feed();//10秒醒一次醒来喂狗
	}
	if(s_ucHeartbeatNoOkCount<2)//两次四分钟则认为断开连接
	{
	}
	else
	{
		g_tLed.BlueLedOn();
		OSFlagPost(pstOsFlagForAppUse, LOGIN_PLATFORM_OK_FLAG, OS_FLAG_CLR, &ucErr);			
		OSFlagPend(pstOsFlagForAppUse, GPRS_IS_OK_FLAG,OS_FLAG_WAIT_SET_ALL, 1, &ucErr);
		if(OS_ERR_NONE!=ucErr)//GPRS不ok，gsm是一直醒的
		{	
		}
		else
		{
			OSSemPost(ptOsSemUsedForResumeGsmTask);
		}
		OSSemPost(ptOsSemUsedForGsmNeedRestart);		
		OSFlagPost(pstOsFlagForAppUse, LOGIN_PLATFORM_ENABLE_FLAG, OS_FLAG_SET, &ucErr);			
		DebugPrintf(ERR"HeartBeatTimeout:%d,LoginAgain\r\n",s_ucHeartbeatNoOkCount);
		s_ucHeartbeatNoOkCount=0;
	}			
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,HEARTBEAT_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
	return ret;
}
/*****************************************************************************
-Fuction		: BatteryStateCheckProcess
-Description	: BatteryStateCheckProcess
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static u8 LowPowerCheckProcess()
{
	static u8 s_ucFirstLowPowerAlarmFlag=FALSE;	
	static u8 s_ucSecondLowPowerAlarmFlag=FALSE;
	static u8 s_ucThirdLowPowerAlarmFlag=FALSE;	
	u8 ucPowerCheckState=0;
	T_Alarm tAlarm;
	E_PowerCheckStateType ePowerCheckState;
	u8 ret=FALSE;
	g_tPowerStateCheck.GetPowerCheckState(&ePowerCheckState,&ucPowerCheckState);		
	if(NormalPowerState<ePowerCheckState)
	{
		ret=TRUE;
		DebugPrintf(CRIT">>>>>Warning:LowPower<<<<<\r\n");
	}
	else
	{
		ret=FALSE;
		//电量恢复正常则后续掉电依然会报警				
		M_ClrBit(s_ucFirstLowPowerAlarmFlag);
		M_ClrBit(s_ucSecondLowPowerAlarmFlag);
		M_ClrBit(s_ucThirdLowPowerAlarmFlag);
	}
	if(FirstLowPowerState==ePowerCheckState)
	{
		g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.LowPowerAlarm=1;
		g_tAlarmOpr.SetState(tAlarm);	
		if(IS_FALSE_BIT(s_ucFirstLowPowerAlarmFlag))
		{
			M_SetBit(s_ucFirstLowPowerAlarmFlag);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSTimeDly(50);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSSemPost(ptOsSemUsedForAlarmTask);
		}
		else
		{
		}
	}		
	else if(SecondLowPowerState==ePowerCheckState)
	{
		g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.LowPowerAlarm=1;
		g_tAlarmOpr.SetState(tAlarm);	
		if(IS_FALSE_BIT(s_ucSecondLowPowerAlarmFlag))
		{					
			M_SetBit(s_ucSecondLowPowerAlarmFlag);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSTimeDly(50);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSSemPost(ptOsSemUsedForAlarmTask);
		}
		else
		{
		}
	}
	else if(ThirdLowPowerState==ePowerCheckState)
	{			
		g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.LowPowerAlarm=1;
		g_tAlarmOpr.SetState(tAlarm);	
		if(IS_FALSE_BIT(s_ucThirdLowPowerAlarmFlag))
		{
			M_SetBit(s_ucThirdLowPowerAlarmFlag);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSTimeDly(50);
			g_tMotor.MotorOn();
			OSTimeDly(50);
			g_tMotor.MotorOff();
			OSSemPost(ptOsSemUsedForAlarmTask);
		}
		else
		{
		}
	}		
	else if(FourthLowPowerState==ePowerCheckState)
	{
		g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.LowPowerAlarm=1;
		g_tAlarmOpr.SetState(tAlarm);	
		g_tMotor.MotorOn();
		OSTimeDly(200);
		g_tMotor.MotorOff();
		DebugPrintf(ERR"VeryLowPowerSystemStandbySoon:%d\r\n",ePowerCheckState);
		/*由于极端情况下登出包会打开GPS会导致超过32s会复位
		所以使用其他任务关机*/
		OSSemPost(ptOsSemUsedForSystemNeedReset);
	}
	else
	{
		g_tAlarmOpr.GetState(&tAlarm);
		tAlarm.LowPowerAlarm=0;
		g_tAlarmOpr.SetState(tAlarm);	
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetLedIdleTime
-Description	: SetLedIdleTime
-Input			: i_dwIdleTime    unit s
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void SetLedIdleTime(u32 i_dwIdleTime)
{
	u8 ucErr;
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,LED_INDICATE_TASK_IDLE_FLAG,OS_FLAG_SET,&ucErr);
	OSFlagPend(pstOsFlagForAppUse, BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_WAIT_SET_ALL, (i_dwIdleTime*OS_TICKS_PER_SEC+100),&ucErr);
	OSFlagPost(pstOsFlagForAppUse,BLUE_LED_LIGHT_ENABLE_FLAG,OS_FLAG_CLR,&ucErr);
	OSFlagPost(ptOsFlagUsedForEnterLowPowerMode,LED_INDICATE_TASK_IDLE_FLAG,OS_FLAG_CLR,&ucErr);
}
/*****************************************************************************
-Fuction		: SystemStartCheck
-Description	: SystemStartCheck
-Input			: i_dwIdleTime    unit s
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/26	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void SystemStartCheck()
{
	E_PowerCheckStateType ePowerCheckState;
	u8 ucPowerCheckState=0;	
	g_tPowerStateCheck.GetPowerCheckState(&ePowerCheckState,&ucPowerCheckState);
	if(FourthLowPowerState==ePowerCheckState)
	{
		OSTimeDlyHMSM(0,0,0,200);//消抖	
		g_tPowerStateCheck.GetPowerCheckState(&ePowerCheckState,&ucPowerCheckState);
		if(FourthLowPowerState==ePowerCheckState)
		{
			g_tTime.WakeUpConfig(21600);//低电下设置长时间防止被唤醒
			SetRtcAlarmTime(21600);//由看门狗来复位启动
			SystemEnterStandByMode();//由于RTC会唤醒防止关机后还会起来工作
		}
		else
		{
			g_tLed.BlueLedOn();//关机下不亮灯，防止低电不断闪烁
		}
	}
	else
	{
		g_tLed.BlueLedOn();//关机下不亮灯，防止低电不断闪烁
	}
	DebugPrintf(CRIT">>>>>SystemStart!>>>>>\r\n");
}


