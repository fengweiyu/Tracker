/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: 任务主进程
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#include "app_gsm_Includes.h"


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 u8IccidStr[GSM_ICCID_STD_LEN];//ICCID 长度20，BCD码10 


enum GsmModStEn
{
	GSM_MOD_ST_IDLE,
	GSM_MOD_ST_GSM_INIT, 
	GSM_MOD_ST_GSM_OK, 
	GSM_MOD_ST_GPRS_INIT, 
	GSM_MOD_ST_OK, 
	GSM_MOD_ST_CLOSE_PWR_START,
	GSM_MOD_ST_CLOSE_PWR_IDLE, 
}; 

static struct
{
	GsmTpU8 bGsmRstReq : 1; 
	GsmTpU8 bGsmIsOkFlg : 1; 
	GsmTpU8 bGprsRstReq : 1; 
	GsmTpU8 bGprsIsOkFlg : 1; 
	GsmTpU8 bGprsIsOffFlg : 1; 
}sstGsmModFlg; 

void GsmModWorkProc(void)
{
#define GSM_GPRS_INIT_ERR_NUM_RST_GSM			6

	static GsmTpU8 su8ErrCnt; 
	GsmTpU8 ucErr;
	static enum GsmModStEn senGsmModSt; 
	switch(senGsmModSt) {
		case GSM_MOD_ST_IDLE:
			M_ClrBit(sstGsmModFlg.bGsmIsOkFlg); 
			M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
			if(GsmModIsNeedOffGsmMod()) {
				senGsmModSt = GSM_MOD_ST_CLOSE_PWR_START; 
			}
			else {
				senGsmModSt = GSM_MOD_ST_GSM_INIT; 
			}
			break; 

		case GSM_MOD_ST_GSM_INIT:
			if(GsmModIsNeedOffGsmMod()) {
				senGsmModSt = GSM_MOD_ST_CLOSE_PWR_START; 
			}
			else {
				app_gsm_OperInfoInit();
				if(IS_FALSE_BIT(GsmInit())) {
					;
				}
				else	{
					GsmDriverEnterCritical(); 
					M_ClrBit(sstGsmModFlg.bGsmRstReq); 
					M_SetBit(sstGsmModFlg.bGsmIsOkFlg); 
					M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
					GsmDriverExitCriticical(); 
					GsmInitOkHook(); //放在这里，防止回调发生调度，标记未生效的情况						
					senGsmModSt = GSM_MOD_ST_GSM_OK;
				}
			}
			break; 

		case GSM_MOD_ST_GSM_OK:
			app_gsm_CSQInquire(); 				
			if(GsmModIsNeedOffGsmMod()) {
				senGsmModSt = GSM_MOD_ST_CLOSE_PWR_START; 
			}
			else {
				if(IS_TRUE_BIT(sstGsmModFlg.bGprsIsOffFlg)) {; 
				}
				else {
					M_ClrBit(su8ErrCnt); 
					senGsmModSt = GSM_MOD_ST_GPRS_INIT; 
				}
			}
			break; 

		case GSM_MOD_ST_GPRS_INIT:
			if(GsmModIsNeedOffGsmMod()) {
				senGsmModSt = GSM_MOD_ST_CLOSE_PWR_START; 
			}
			else if(IS_TRUE_BIT(sstGsmModFlg.bGsmRstReq)) {; 
				M_ClrBit(sstGsmModFlg.bGsmRstReq); 
				senGsmModSt = GSM_MOD_ST_IDLE; 
			}
			else if(IS_TRUE_BIT(sstGsmModFlg.bGprsIsOffFlg)) {
				senGsmModSt = GSM_MOD_ST_GSM_OK; 
			}
			else {
				if(IS_TRUE_BIT(gsm_oper_info.flgs.Ipclose)
				|| IS_FALSE_BIT(GsmPhoneIsIdle())) {; 
				}
				else {
					if(IS_FALSE_BIT(app_gsm_GprsInit())) {						
						su8ErrCnt++; 
						if(su8ErrCnt < GSM_GPRS_INIT_ERR_NUM_RST_GSM) {; 
							GsmDriverSleep(5000);
							GsmDebugStringLowLevel("Gprs init fail,rst again\r\n");
						}
						else {
							M_ClrBit(su8ErrCnt); 
							senGsmModSt = GSM_MOD_ST_IDLE; 
							GsmDebugString("Gprs init fail num over,rst gsm\r\n"); 
						}
					}
					else {
						GprsInitOkHook(); 						
						GsmDriverEnterCritical(); 
						M_ClrBit(sstGsmModFlg.bGprsRstReq);
						M_SetBit(sstGsmModFlg.bGprsIsOkFlg); 
						GsmDriverExitCriticical(); 
						senGsmModSt = GSM_MOD_ST_OK; 
						GsmDebugString("Gprs init ok\r\n"); 
					}
				}
			}
			break; 

		case GSM_MOD_ST_OK:
			//app_gsm_CSQInquire(); 				
			if(GsmModIsNeedOffGsmMod()) {
				senGsmModSt = GSM_MOD_ST_CLOSE_PWR_START; 
			} 
			else if(IS_TRUE_BIT(sstGsmModFlg.bGsmRstReq)) {; 
				GsmDriverEnterCritical(); 
				M_ClrBit(sstGsmModFlg.bGsmIsOkFlg); 
				M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
				GsmDriverExitCriticical(); 
				senGsmModSt = GSM_MOD_ST_IDLE; 
			}
			else if(IS_TRUE_BIT(sstGsmModFlg.bGprsRstReq)) {; 
				OSFlagPost(pstOsFlagForAppUse, GPRS_IS_OK_FLAG, OS_FLAG_CLR, &ucErr); 
				M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
				senGsmModSt = GSM_MOD_ST_GPRS_INIT; 
			}
			else if(IS_TRUE_BIT(sstGsmModFlg.bGprsIsOffFlg)) {
				GsmDriverEnterCritical(); 
				M_SetBit(gsm_oper_info.flgs.Ipclose); 
				GsmDriverExitCriticical(); 
				senGsmModSt = GSM_MOD_ST_GSM_OK; 
			}
			else {//GSM IDLE STATE
				GsmIdleStateHandle();
			}				
			break; 

		case GSM_MOD_ST_CLOSE_PWR_START:
			GsmDriverEnterCritical(); 
			M_ClrBit(sstGsmModFlg.bGsmIsOkFlg); 
			M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
			GsmDriverExitCriticical(); 
			app_gsm_CloseModule(); 
			senGsmModSt = GSM_MOD_ST_CLOSE_PWR_IDLE; 
			GsmDebugString("Gsm off start\r\n"); 
			break; 

		case GSM_MOD_ST_CLOSE_PWR_IDLE:
			if(GsmModIsNeedOffGsmMod()) {; 
			}
			else {
				senGsmModSt = GSM_MOD_ST_IDLE; 
				GsmDebugString("Gsm off cancel\r\n"); 
			}
			break; 

		default:
			GsmDriverEnterCritical(); 
			M_ClrBit(sstGsmModFlg.bGsmIsOkFlg); 
			M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
			GsmDriverExitCriticical(); 
			senGsmModSt = GSM_MOD_ST_IDLE; 
			break; 			
	}
}

extern void GsmTaskInit(void); 
void TaskGsmDriver(void *pdata)
{	
	GsmTpBool boRxCmdFlg;  
	GsmTpU8 *pu8Dat; 
	GsmTpU16 u16Len; 
	GsmTaskInit();
	while(1)
	{
		boRxCmdFlg = GsmDriverGetDevRxDatPend(GSM_TASK_CYCLE_MS, &pu8Dat, &u16Len); 
		if(IS_TRUE_BIT(boRxCmdFlg)) {			
			app_gsm_AtRetInfoAnalyse(pu8Dat, u16Len);
			GsmDriverGetDevRxDatMemFreeIfNeed(pu8Dat, u16Len); 
			M_ClrBit(boRxCmdFlg); 
		}
		else	{		
			GsmDriverGsmTaskIdleHook(); 
			app_gsm_PhoneHandler(); 
			GsmIpDatTxProc(GSM_TASK_CYCLE_MS); 
			GsmSmsTxProc();
		}
		GsmSmsReadByPromptPro(); 
		LbsLocationProc(); 
		GsmCellTowerInfoProc();
		GsmNetOpenCloseProc(); 	
		GsmModWorkProc(); 
	}
}

void GsmModRstReq(void)
{
	GsmDriverEnterCritical(); 
	M_SetBit(sstGsmModFlg.bGsmRstReq); 
	GsmDriverExitCriticical(); 
}

void GsmGprsRstReq(void)
{
	GsmDriverEnterCritical(); 
	M_SetBit(sstGsmModFlg.bGprsRstReq); 
	GsmDriverExitCriticical(); 
}

GsmTpBool GsmModGsmIsOk(void)
{
	return sstGsmModFlg.bGsmIsOkFlg; 
}

GsmTpBool GsmGprsIsOk(void)
{
	return sstGsmModFlg.bGprsIsOkFlg; 
}

void GsmGprsNgSet(void)
{
	GsmDriverEnterCritical(); 
	M_ClrBit(sstGsmModFlg.bGprsIsOkFlg); 
	GsmDriverExitCriticical(); 
}

void GsmModGprsOffReq(void)
{
	GsmDriverEnterCritical(); 
	M_SetBit(sstGsmModFlg.bGprsIsOffFlg); 
	GsmGprsNgSet(); 
	GsmDriverExitCriticical(); 
}

void GsmModGprsOffCancel(void)
{
	GsmDriverEnterCritical(); 
	M_ClrBit(sstGsmModFlg.bGprsIsOffFlg); 
	GsmDriverExitCriticical(); 
}


