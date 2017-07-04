#include"CBasicTools.h"
#include"Config.h"
#include "WifiApi.h"
#include "main.h"

u8 ucCmd; 
#define MUTEX_ID_NULL_FLAG = 0xFF; 
static struct {
	pFunWifiScanHook pFunScanHook; 
	OS_EVENT *mutexId; 
} stWifiScanData; 

static void setWifiScanHook(pFunWifiScanHook pFunHook) 
{
	stWifiScanData.pFunScanHook = pFunHook; 
}
void WifiScanHook(e_wifi_bool boResultIsOk, WifiAccessPointSt *pstWifiApBuf) 
{
	if(IS_NULL_P(stWifiScanData.pFunScanHook)) {;
	}
	else {
		stWifiScanData.pFunScanHook(boResultIsOk, pstWifiApBuf); 
	}

	OSSemPost(stWifiScanData.mutexId); 
}
void WifiApiScanMutexInit()
{
	if(IS_NULL_P(stWifiScanData.mutexId)) {
		DebugPrintf("stWifiScanData sem create\r\n"); 
		stWifiScanData.mutexId = OSSemCreate(1); 
	}
	else {; 
	}
}
u8 WifiApiScanApReq(pFunWifiScanHook pFun)
{
	u8 RetVal; 
	u8 ucErr;
	M_ClrBit(RetVal); 
	WifiApiScanMutexInit(); 
	OSSemPend(stWifiScanData.mutexId,100,&ucErr);
	if(OS_ERR_NONE!=ucErr)
	{
		M_ClrBit(RetVal); 
	}
	else
	{
		setWifiScanHook(pFun); 
		ucCmd = (u8)CMD_ID_LAP; 
		ucErr=OSMboxPost(pstOsMsgForWifiTask,&ucCmd);
		if(OS_ERR_NONE!=ucErr)
		{
			M_ClrBit(RetVal); 
			OSSemPost(stWifiScanData.mutexId);			
		}
		else
		{
			M_SetBit(RetVal); 
		}
	}
	return RetVal; 
}




