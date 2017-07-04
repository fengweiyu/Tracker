/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 SmsManage.c
* Description		: 	SmsManage application operation center
* Created			: 	2016.9.7.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "ParameterManage.h"
#include "Ucos_ii.h"
#include "Config.h"
#include "Flash.h"
static u8 SaveSmsPara(T_SmsPara i_SmsPara);
static u8 SmsParaInit();
static void SmsParaDeInit();
static u8 SetSmsPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len);
static u8 GetSmsPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len);

static T_SmsPara g_tSmsPara={0};
static T_ParameterManage g_tSmsParaOpr={
	.name="SmsPara",
	.ParaOprInit		=SmsParaInit,
	.ParaOprDeInit		=SmsParaDeInit,
	.SetParaOpr		=SetSmsPara,
	.GetParaOpr		=GetSmsPara
};
/*****************************************************************************
-Fuction		: RegisterSmsPara
-Description	: RegisterSmsPara
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RegisterSmsPara()
{
	RegisterParaOpr(&g_tSmsParaOpr);
}
/*****************************************************************************
-Fuction		: SmsParaInit
-Description	: SmsParaInit
-Input			: 
-Output 		:   
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 static u8 SmsParaInit(T_AllSysPara * i_ptAllSysPara)
{
	u8 ret=FALSE;
	if(NULL==i_ptAllSysPara)
	{
	}
	else
	{
		OSSchedLock();
		memcpy(&g_tSmsPara,&i_ptAllSysPara->tSmsPara,sizeof(g_tSmsPara));
		OSSchedUnlock();
		ret=TRUE;
	}
	return ret;
}

/*****************************************************************************
-Fuction		: SmsParaDeInit
-Description	: SmsParaDeInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SmsParaDeInit()
{
	u8 ucDeviceIdLen=0;
	OSSchedLock();
	memset(g_tSmsPara.CenterNum,0,sizeof(g_tSmsPara.CenterNum));
	memset(g_tSmsPara.EmergencyContactNum,0,sizeof(g_tSmsPara.EmergencyContactNum));
	memset(g_tSmsPara.SimCardNum,0,sizeof(g_tSmsPara.SimCardNum));
	memset(g_tSmsPara.SmsSecretKey,'0',sizeof(g_tSmsPara.SmsSecretKey));
	ucDeviceIdLen=strnlen(g_tSmsPara.DeviceID,DEVICE_ID_LEN);
	if(ucDeviceIdLen>sizeof(g_tSmsPara.SmsSecretKey))
	{
		memcpy(g_tSmsPara.SmsSecretKey,&g_tSmsPara.DeviceID[ucDeviceIdLen-sizeof(g_tSmsPara.SmsSecretKey)],sizeof(g_tSmsPara.SmsSecretKey));
	}
	else
	{
		memcpy(g_tSmsPara.SmsSecretKey+sizeof(g_tSmsPara.SmsSecretKey)-ucDeviceIdLen,g_tSmsPara.DeviceID,ucDeviceIdLen);
	}
	OSSchedUnlock();
	SaveSmsPara(g_tSmsPara);
}

/*****************************************************************************
-Fuction		: SetSmsPara
-Description	: SetSmsPara
-Input			: i_SubName		i_Buf	i_Len
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SetSmsPara(ParameterEnum i_SubName,u8 *i_Buf,u16 i_Len)
{
	u8 ret=FALSE;
	u8 ucDeviceIdLen=0;
	u8 ucSmsSecretKeyLen=0;
	ParameterEnum SubSmsParaName=i_SubName;
	switch (SubSmsParaName)
	{
		case CenterNum:
		{
			if((NULL==i_Buf)|| (i_Len>PHONE_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.CenterNum,0,sizeof(g_tSmsPara.CenterNum));
				memcpy(g_tSmsPara.CenterNum,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}		
		case DeviceID:
		{
			if((NULL==i_Buf)|| (i_Len>DEVICE_ID_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.DeviceID,0,sizeof(g_tSmsPara.DeviceID));
				memcpy(g_tSmsPara.DeviceID,i_Buf,i_Len);
				memset(g_tSmsPara.SmsSecretKey,'0',sizeof(g_tSmsPara.SmsSecretKey));
				ucDeviceIdLen=strnlen(g_tSmsPara.DeviceID,DEVICE_ID_LEN);
				if(ucDeviceIdLen>sizeof(g_tSmsPara.SmsSecretKey))
				{
					memcpy(g_tSmsPara.SmsSecretKey,&g_tSmsPara.DeviceID[ucDeviceIdLen-sizeof(g_tSmsPara.SmsSecretKey)],sizeof(g_tSmsPara.SmsSecretKey));
				}
				else
				{
					memcpy(g_tSmsPara.SmsSecretKey+sizeof(g_tSmsPara.SmsSecretKey)-ucDeviceIdLen,g_tSmsPara.DeviceID,ucDeviceIdLen);
				}
				OSSchedUnlock();
				ret=TRUE;			
			}
			break;
		}
		case EmergencyContactNum1:
		{
			if((NULL==i_Buf)|| (i_Len>(PHONE_STR_LEN)))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.EmergencyContactNum[0],0,sizeof(g_tSmsPara.EmergencyContactNum[0]));
				memcpy(g_tSmsPara.EmergencyContactNum[0],i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case EmergencyContactNum2:
		{
			if((NULL==i_Buf)|| (i_Len>(PHONE_STR_LEN)))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.EmergencyContactNum[1],0,sizeof(g_tSmsPara.EmergencyContactNum[1]));
				memcpy(g_tSmsPara.EmergencyContactNum[1],i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case EmergencyContactNum3:
		{
			if((NULL==i_Buf)|| (i_Len>(PHONE_STR_LEN)))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.EmergencyContactNum[2],0,sizeof(g_tSmsPara.EmergencyContactNum[2]));
				memcpy(g_tSmsPara.EmergencyContactNum[2],i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case SimCardNum:
		{
			if((NULL==i_Buf)|| (i_Len>PHONE_STR_LEN))
			{
				ret=FALSE;
			}
			else
			{
				OSSchedLock();
				memset(g_tSmsPara.SimCardNum,0,sizeof(g_tSmsPara.SimCardNum));
				memcpy(g_tSmsPara.SimCardNum,i_Buf,i_Len);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		case SmsSecretKey:
		{
			if((NULL==i_Buf)|| (i_Len>SMS_SECRET_KEY_LEN))
			{
				ret=FALSE;
			}
			else
			{		
				ucSmsSecretKeyLen=strnlen(i_Buf,SMS_SECRET_KEY_LEN);
				OSSchedLock();
				memset(g_tSmsPara.SmsSecretKey,'0',sizeof(g_tSmsPara.SmsSecretKey));
				memcpy(&g_tSmsPara.SmsSecretKey[SMS_SECRET_KEY_LEN-ucSmsSecretKeyLen],i_Buf,ucSmsSecretKeyLen);
				OSSchedUnlock();
				ret=TRUE;
			}
			break;
		}
		default :
		{
			DebugPrintf(ERR"SmsPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	if(TRUE==ret)
	{
		ret&=SaveSmsPara(g_tSmsPara);
	}	
	else
	{
		
	}
	return ret;
}
/*****************************************************************************
-Fuction		: GetSmsPara
-Description	: GetSmsPara
-Input			: i_SubName
-Output 		: o_buf  	o_Len
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 GetSmsPara(ParameterEnum i_SubName,u8 *o_Buf,u16 *o_Len)
{
	u8 ret=FALSE;
	u8 ParaTempLen=0;
	ParameterEnum SubSmsParaName=i_SubName;
	switch (SubSmsParaName)
	{
		case CenterNum:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tSmsPara.CenterNum,PHONE_STR_LEN);
			memcpy(o_Buf,g_tSmsPara.CenterNum,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}		
		case DeviceID:
		{
			OSSchedLock();
			ParaTempLen=DEVICE_ID_LEN-strnlen(g_tSmsPara.DeviceID,DEVICE_ID_LEN);
			memset(&g_tSmsPara.DeviceID[strnlen(g_tSmsPara.DeviceID,DEVICE_ID_LEN)],0,ParaTempLen);// £”‡≥§∂»ÃÓ0
			*o_Len=DEVICE_ID_LEN;
			memcpy(o_Buf,g_tSmsPara.DeviceID,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case EmergencyContactNum1:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tSmsPara.EmergencyContactNum[0],PHONE_STR_LEN);;
			memcpy(o_Buf,g_tSmsPara.EmergencyContactNum[0],*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case EmergencyContactNum2:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tSmsPara.EmergencyContactNum[1],PHONE_STR_LEN);;
			memcpy(o_Buf,g_tSmsPara.EmergencyContactNum[1],*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case EmergencyContactNum3:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tSmsPara.EmergencyContactNum[2],PHONE_STR_LEN);;
			memcpy(o_Buf,g_tSmsPara.EmergencyContactNum[2],*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case SimCardNum:
		{
			OSSchedLock();
			*o_Len=strnlen(g_tSmsPara.SimCardNum,PHONE_STR_LEN);
			memcpy(o_Buf,g_tSmsPara.SimCardNum,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		case SmsSecretKey:
		{
			OSSchedLock();
			*o_Len=sizeof(g_tSmsPara.SmsSecretKey);
			memcpy(o_Buf,g_tSmsPara.SmsSecretKey,*o_Len);
			OSSchedUnlock();
			ret=TRUE;
			break;
		}
		default :
		{
			DebugPrintf(ERR"SmsPara not found!\r\n");
			ret=FALSE;
			break;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: SetSmsPara
-Description	: SetSmsPara
-Input			: i_SmsPara
-Output 		: 
-Return 		: True/False
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/07	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static u8 SaveSmsPara(T_SmsPara i_SmsPara)
{
	u8 ret=FALSE;
	OSSchedLock();
	memcpy(&g_tAllSysPara.tSmsPara,&i_SmsPara,sizeof(T_SmsPara));
	OSSchedUnlock();
	SpiFlashStrctWrite(&g_tAllSysPara,
					sizeof(g_tAllSysPara),
					FLASH_ADDR_SYS_PARA_MAIN,
					FLASH_ADDR_SYS_PARA_BACK);
	ret=TRUE;
	return ret;
}







