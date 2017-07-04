/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 PowerStateCheck.c
* Description		: 	PowerStateCheck application operation center
* Created			: 	2016.09.25.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasicTools.h"
#include "Config.h"
#include "app_PowerStateCheck.h"
#include "main.h"

static PT_PowerStateCheckDevManage g_ptPowerStateCheckDevHead=NULL;

static void PowerStateCheckInit();
static u8 PowerStateCheckConfig(void);
static u8 GetPowerCheckStateOpr(E_PowerCheckStateType *o_pePowerCheckState,u8 *o_ucRemainPowerValue);


T_PowerStateCheckOpr  const g_tPowerStateCheck ={
	.Init					=PowerStateCheckInit,
	.Config				=PowerStateCheckConfig,
	.GetPowerCheckState	=GetPowerCheckStateOpr,
};
/*****************************************************************************
-Fuction		: PowerStateCheckInit
-Description	: PowerStateCheckInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void PowerStateCheckInit()
{
	PowerStateCheckDevInit();	
}
/*****************************************************************************
-Fuction		: PowerStateCheckConfig
-Description	: PowerStateCheckConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 PowerStateCheckConfig(void)
{
	u8 ret=0;
	PT_PowerStateCheckDevManage ptTmp = g_ptPowerStateCheckDevHead;
	if(NULL==ptTmp)
	{
		ret=FALSE;
	}
	else
	{
		while (ptTmp)
		{
			ptTmp->DevConfig();
			ptTmp = ptTmp->ptNext;
		}	
		ret=TRUE;
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPowerValue
-Description	: GetPowerValue
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetPowerValue(u8 i_ucTimes,float *o_pfAverPowerValue)
{
	u8 ret=FALSE;
	PT_PowerStateCheckDevManage ptTmp = g_ptPowerStateCheckDevHead;
	if(NULL==ptTmp)
	{
		DebugPrintf(ERR"PowerStateCheckDevHead RedPowerStateCheckOnOpr Null\r\n");
		ret=FALSE;
	}
	else
	{
		ret=TRUE;
		while (ptTmp)
		{
			ret&=ptTmp->DevGetAverageValue(i_ucTimes,o_pfAverPowerValue);
			ptTmp = ptTmp->ptNext;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetPowerValue
-Description	: GetPowerValue
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetRemainPercentPowerValue(float i_fPowerValue)
{
	u8 ucCurrentRemainPowerValue=0;
	u8 ucErr;
	static u8 s_ucBackupRemainPowerValue=0;
	static u8 s_ucGotRemainPercentPowerValueFlag=FALSE;
	if(i_fPowerValue > FULL_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_FULL_POWER_VALUE;
	}	
	else if(i_fPowerValue > NINETY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_NINETY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > EIGHTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_EIGHTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > SEVENTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_SEVENTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > SIXTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_SIXTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > FIFTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_FIFTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > FOURTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_FOURTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > THIRTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_THIRTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > TWENTY_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_TWENTY_PERCENT_POWER_VALUE;
	}
	else if(i_fPowerValue > TEN_PERCENT_POWER_VALUE) {
		ucCurrentRemainPowerValue = REMAIN_TEN_PERCENT_POWER_VALUE;
	}
	else
	{
		ucCurrentRemainPowerValue = REMAIN_FIVE_PERCENT_POWER_VALUE;
	
	}
	if(IS_FALSE_BIT(s_ucGotRemainPercentPowerValueFlag))
	{
		s_ucBackupRemainPowerValue=ucCurrentRemainPowerValue;
		M_SetBit(s_ucGotRemainPercentPowerValueFlag);
	}
	else
	{
	}
	OSFlagAccept(pstOsFlagForAppUse, USB_CONNECT_OK_FLAG,OS_FLAG_WAIT_CLR_ALL,&ucErr); 
	if(OS_ERR_NONE!=ucErr)//����10���Ӽ��һ�Σ���������ò���
	{//��ô����ɳ���Ҫ�ܾõļ���,����ĳ�ʱ��̼��������󹦺�
		OSSchedLock();//ͬʱ,��س�����ŵ�,����ò���,�ŵ�״̬�ᱻ����Ϊ����״̬
		s_ucBackupRemainPowerValue=ucCurrentRemainPowerValue;//�ֳ��ܿ�,������ָ߱��������һ�¾ͺ���
		OSSchedUnlock();//���Գ��ʹ��ʵʱ״̬
	}
	else//û��USB���ֵ�ѹ����ȡ����״̬
	{
		OSSchedLock();
		if(ucCurrentRemainPowerValue<s_ucBackupRemainPowerValue)
		{
			s_ucBackupRemainPowerValue-=REMAIN_PERCENT_POWER_VALUE_STEP;
		}
		else
		{
		}
		OSSchedUnlock();
	}
	if(s_ucBackupRemainPowerValue>REMAIN_FULL_POWER_VALUE)//��ֹԽ��
	{
		s_ucBackupRemainPowerValue=REMAIN_FULL_POWER_VALUE;
	}
	else if(s_ucBackupRemainPowerValue<REMAIN_FIVE_PERCENT_POWER_VALUE)
	{
		s_ucBackupRemainPowerValue=REMAIN_FIVE_PERCENT_POWER_VALUE;
	}
	else
	{
	}
	ucCurrentRemainPowerValue=s_ucBackupRemainPowerValue;
	return ucCurrentRemainPowerValue;
}

/*****************************************************************************
-Fuction		: GetPowerCheckStateOpr
-Description	: GetPowerCheckStateOpr
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetPowerCheckStateOpr(E_PowerCheckStateType *o_pePowerCheckState,u8 *o_ucRemainPowerValue)
{
	u8 ret=FALSE;
	u8 ucRemainPowerValue=0;
	float fPowerValue;
	u16 wPowerValue=0;
	u8 ucPowerValue=0;
	ret=GetPowerValue(GET_ADC_VAULE_NUM,&fPowerValue);//ȡ��ʮ��ƽ��ֵ
	if(FALSE==ret)
	{
		ret=FALSE;
	}
	else
	{
		//ucPowerValue=(u8)fPowerValue;                            //��ֵ�������ָ�adcx��������ΪadcxΪu16����
		//wPowerValue=(u16)((fPowerValue-ucPowerValue)*1000);
		//DebugPrintf("PowerValue= %d.%d\r\n",ucPowerValue,wPowerValue);
		//С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС����
		if(fPowerValue<=ERROR_POWER_VALUE)
		{
			ret=FALSE;
		}
		else
		{
			ucRemainPowerValue=GetRemainPercentPowerValue(fPowerValue);
			*o_ucRemainPowerValue=ucRemainPowerValue;
			if(REMAIN_FULL_POWER_VALUE==ucRemainPowerValue)
			{
				*o_pePowerCheckState=FullPowerState;
			}
			else if(ucRemainPowerValue>REMAIN_THIRTY_PERCENT_POWER_VALUE)
			{
				*o_pePowerCheckState=NormalPowerState;
			}
			else if(ucRemainPowerValue > REMAIN_TWENTY_PERCENT_POWER_VALUE) {
				*o_pePowerCheckState=FirstLowPowerState;
			}
			else if(ucRemainPowerValue > REMAIN_TEN_PERCENT_POWER_VALUE) {
				*o_pePowerCheckState=SecondLowPowerState;
			}
			else if(ucRemainPowerValue > REMAIN_FIVE_PERCENT_POWER_VALUE) {
				*o_pePowerCheckState=ThirdLowPowerState;
			}
			else
			{
				*o_pePowerCheckState=FourthLowPowerState;
			}
			ret=TRUE;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: RegisterPowerStateCheckDev
-Description	: RegisterPowerStateCheckDev
-Input			: i_ptPowerStateCheckDev
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/23   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterPowerStateCheckDev(PT_PowerStateCheckDevManage i_ptPowerStateCheckDev)
{
	PT_PowerStateCheckDevManage ptTmp;
	if (!g_ptPowerStateCheckDevHead)
	{
		g_ptPowerStateCheckDevHead   = i_ptPowerStateCheckDev;
		g_ptPowerStateCheckDevHead->ptNext = NULL;
	}
	else
	{
		ptTmp = g_ptPowerStateCheckDevHead;
		while (ptTmp->ptNext)
		{
			ptTmp = ptTmp->ptNext;
		}
		ptTmp->ptNext	  = i_ptPowerStateCheckDev;
		i_ptPowerStateCheckDev->ptNext = NULL;
	}
}


