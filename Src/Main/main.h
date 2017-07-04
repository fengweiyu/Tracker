/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 main
* Description		: 	main.h
* Created			: 	2016.08.26.
* Author			: 	Yu Weifeng
* Function List 		: 	none
* Last Modified 	: 	
* History			: 	
******************************************************************************/

#ifndef _MAIN_H
#define _MAIN_H

#include "Ucos_ii.h"
#include "CBasicTypeDefine.h"

#define LED_OUT_TIME						10
#define LED_LIGHT_TIME						1
#define PLATFORM_HEARTBEAT_TIME		300//5分钟心跳
#define BATTERY_STATE_CHECK_TIME		600//10分钟检测一次

/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
#define ALARM_TASK_PRIORITY   							3
#define VIRTUAL_COM_TASK_PRIORITY   					4
#define PLATFORM_CMD_ACK_TASK_PRIORITY			5
#define LOGIN_PLATFORM_TASK_PRIORITY				6
#define FIXED_TIME_REPORT_TASK_PRIORITY			7
#define BATTERY_CHARGE_TASK_PRIORITY   				8
	
#define WIFI_TASK_PRIORITY								9
#define GPS_PROCESS_TASK_PRIORITY					10
#define GSM_TASK_PRIORITY 							11
	
#define LED_INDICATE_TASK_PRIORITY					12
#define CHECK_TRACKER_STATE_TASK_PRIORITY			13
#define SMS_PROCESS_TASK_PRIORITY					14
#define SYSTEM_RESET_TASK_PRIORITY 					18
#define ENTER_LOW_POWER_MODE_TASK_PRIORITY 		19

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
使用256 的栈最大使用率是55%，当内存不够可适当缩减这些任务栈
*********************************************************************************************************
*/
#define GSM_TASK_STACK_LEN							256
#define WIFI_TASK_STACK_LEN							240
#define GPS_PROCESS_TASK_STACK_LEN					192

#define ALARM_TASK_STACK_LEN    						256
#define VIRTUAL_COM_TASK_STACK_LEN					192
#define LOGIN_PLATFORM_TASK_STACK_LEN				256
#define FIXED_TIME_REPORT_TASK_STACK_LEN			256
#define PLATFORM_CMD_ACK_TASK_STACK_LEN			240
#define BATTERY_CHARGE_TASK_STACK_LEN				80
#define LED_INDICATE_TASK_STACK_LEN					192
#define CHECK_TRACKER_STATE_TASK_STACK_LEN		128
#define SMS_PROCESS_TASK_STACK_LEN					256
#define SYSTEM_RESET_TASK_STACK_LEN				256
#define ENTER_LOW_POWER_MODE_TASK_STACK_LEN	80

typedef struct TaskInfo
{
	void   (*task)(void *p_arg); 
        void    *p_arg; 
        OS_STK  *ptos; 
        INT8U    prio; 
        INT16U   id; 
        OS_STK  *pbos; 
        INT32U   stk_size; 
        void    *pext; 
        INT16U   opt; 
	 s8 *NameStr; 	
}T_TaskInfo,*PT_TaskInfo;

struct OsQInfo
{
	void    **start; 
	INT16U    size; 
	OS_EVENT  ** p2stEcb; 
	s8 *NameStr; 
}; 

#define GSM_TX_IP_DAT_REQ_FLAG					(OS_FLAGS)(1 << 0)
#define GSM_TX_IP_DAT_BUSY_FLAG					(OS_FLAGS)(1 << 1)
#define GSM_DAT_FLOW_OFF							(OS_FLAGS)(1 << 2)
#define GPRS_IS_OK_FLAG							(OS_FLAGS)(1 << 3)
#define SEND_DATA_BUSY_FLAG						(OS_FLAGS)(1 << 4)
#define LOGIN_PLATFORM_OK_FLAG					(OS_FLAGS)(1 << 5)
#define VCP_DEBUG_DISABLE_FLAG					(OS_FLAGS)(1 << 6)
#define LOGIN_PLATFORM_ENABLE_FLAG				(OS_FLAGS)(1 << 7)
#define FIXED_TIME_REPORT_ENABLE_FLAG			(OS_FLAGS)(1 << 8)
#define BLUE_LED_LIGHT_ENABLE_FLAG				(OS_FLAGS)(1 << 9)
#define PHONE_IS_IDLE_FLAG							(OS_FLAGS)(1 << 10)
#define GSM_TX_IP_DAT_OK_FLAG					(OS_FLAGS)(1 << 11)
#define GET_GPS_POSITION_FLAG						(OS_FLAGS)(1 << 12)
#define GET_GPS_POSITION_BUSY_FLAG				(OS_FLAGS)(1 << 13)
#define GET_GPS_POSITION_OK_FLAG					(OS_FLAGS)(1 << 14)
#define USB_CONNECT_OK_FLAG						(OS_FLAGS)(1 << 15)
/*
*********************************************************************************************************
*                                            TASK IDLE FLAG
*********************************************************************************************************
*/
#define ALARM_TASK_IDLE_FLAG   					(OS_FLAGS)(1 << 0)
#define VIRTUAL_COM_TASK_IDLE_FLAG   			(OS_FLAGS)(1 << 1)	
#define LOGIN_PLATFORM_TASK_IDLE_FLAG			(OS_FLAGS)(1 << 2)
#define FIXED_TIME_REPORT_TASK_IDLE_FLAG		(OS_FLAGS)(1 << 3)
#define PLATFORM_CMD_ACK_TASK_IDLE_FLAG		(OS_FLAGS)(1 << 4)
#define SMS_TASK_IDLE_FLAG						(OS_FLAGS)(1 << 5)	
#define BATTERY_CHARGE_TASK_IDLE_FLAG   		(OS_FLAGS)(1 << 6)	
#define WIFI_TASK_IDLE_FLAG						(OS_FLAGS)(1 << 7)	
#define GSM_TASK_IDLE_FLAG 						(OS_FLAGS)(1 << 8)			
#define HEARTBEAT_TASK_IDLE_FLAG				(OS_FLAGS)(1 << 9)	
#define PHONE_TASK_IDLE_FLAG						(OS_FLAGS)(1 << 10)
#define LED_INDICATE_TASK_IDLE_FLAG				(OS_FLAGS)(1 << 11)
#define TRACK_STATE_IDLE_FLAG					(OS_FLAGS)(1 << 12)
#define ALL_TASK_IDLE_FLAG						(OS_FLAGS)(1 << 13)

extern OS_FLAG_GRP *pstOsFlagForAppUse; 
extern OS_EVENT *pstOsQForGsmRx; 
extern OS_EVENT *pstOsQForWifiRx;
extern OS_EVENT *pstOsMsgForWifiTask;
extern OS_EVENT *ptOsQForGpsRx;
extern OS_FLAG_GRP *ptOsFlagUsedForEnterLowPowerMode;
extern OS_EVENT *ptOsSemUsedForLoginPlatformOk;
extern OS_EVENT *ptOsSemUsedForPlatformCmdAck;
extern OS_EVENT *ptOsSemUsedForSendSmsOK;
extern OS_EVENT *ptOsSemUsedForGsmNeedRestart;
extern OS_EVENT *ptOsSemUsedForPlatformReceivedAlarm;
extern OS_EVENT *ptOsSemUsedForNetParaSetCompleted;
extern OS_EVENT *ptOsSemUsedForSystemNeedReset;
extern OS_EVENT *ptOsSemUsedForPlatformCommunicateHeartbeatOk;
extern OS_EVENT *ptOsSemUsedForCheckedCutState;
extern OS_EVENT *ptOsSemUsedForCheckedDropState;
extern OS_EVENT *ptOsSemUsedForCheckedGsmState;
extern OS_EVENT *ptOsSemUsedForSmsComing;
extern OS_EVENT *ptOsSemUsedForPhoneComing;
extern OS_EVENT *ptOsSemUsedForLoginPlatform;
extern OS_EVENT *ptOsSemUsedForAlarmTask;
extern OS_EVENT *ptOsSemUsedForResumeGsmTask;
extern OS_EVENT *ptOsSemUsedForNeedGetNetData;
extern OS_EVENT *ptOsSemUsedForGprsDataSendIdle;
extern OS_EVENT *ptOsSemUsedForGprsDataSendReq;
extern OS_EVENT *ptOsSemUsedForVcpCmdAck;
extern OS_EVENT *ptOsSemUsedForGetWifiApSuccess;

#endif

