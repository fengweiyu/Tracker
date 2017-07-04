/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: GSM  AT  指令库
* 模块型号: 华为MC323
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#include "Includes.h"
#include "app_gsm_includes.h"
#include "app_gsm_ATLib.h"
#include "Main.h"
#include "MyMalloc.h"

#define MAX_AT_CMD_SIZE  	MAX_AT_CMD_LEN
const char c8AtCmdStrOk[] = "OK"; 


/*
-------------------------------------------------------------------------------------------------
-函数功能:	设置模块通信串口波特率
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_IPR(void)
{
	dev_gsm_SendWrite("AT+IPR=115200\r\n", strlen("AT+IPR=115200\r\n"));
	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+IPR 设置DTE-DCE 波特率命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_AT(void)
{
	dev_gsm_SendWrite("AT\r\n", strlen("AT\r\n"));
		
	return(GsmTpTRUE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	ATE 设置回显命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_ATE0(void)
{
	dev_gsm_SendWrite("ATE0\r\n", strlen("ATE0\r\n"));
	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpBool bIMSIReqFlag = FALSE;
GsmTpU8 app_waitIMSI(GsmTpU8 *u8Buff, GsmTpU16 u16Len)
{
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);
	if(IS_TRUE_BIT(bIMSIReqFlag))
	{
		app_sysSetImsi(u8Buff,u16Len);
		M_ClrBit(bIMSIReqFlag);
		M_SetBit(u8RetVal);
	}
	else
	{
		M_ClrBit(u8RetVal);
	}

	return u8RetVal;
}
// 查询IMSI
GsmTpU8 app_gsmAT_CIMI(void) {
	dev_gsm_SendWrite("AT+CIMI\r\n", strlen("AT+CIMI\r\n"));
	M_SetBit(bIMSIReqFlag);
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3));
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CGMR/GMR 查询软件版本号命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CGMR(void)
{
	dev_gsm_SendWrite("AT+CGMR\r\n", strlen("AT+CGMR\r\n"));
	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpBool bIMEIReqFlag = GsmTpFALSE;
extern stGsmIMEISt GsmIMEIInfo;
GsmTpU8 app_waitIMEI(GsmTpU8 *u8Buff, GsmTpU16 u16Len) {
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);
	if(IS_TRUE_BIT(bIMEIReqFlag)) {
		memset(GsmIMEIInfo.u8GsmIMEI, 0, sizeof(GsmIMEIInfo.u8GsmIMEI));
		memcpy(GsmIMEIInfo.u8GsmIMEI, u8Buff, u16Len);
		M_SetBit(GsmIMEIInfo.u8IMEIValidFlag);
		M_ClrBit(bIMEIReqFlag);
		M_SetBit(u8RetVal);
	}
	else {
		M_ClrBit(u8RetVal);
	}

	return u8RetVal;
}
GsmTpU8 ATLib_gsm_CGSN(void) {
	dev_gsm_SendWrite("AT+CGSN\r\n", strlen("AT+CGSN\r\n"));
	M_SetBit(bIMEIReqFlag);
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3));
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	查询SIM  卡是否存在
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_TSIM(void)
{
	dev_gsm_SendWrite("AT%TSIM\r\n", strlen("AT%TSIM\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"%TSIM 1",NULL,5));
}

// 设置ME  功能
GsmTpU8 ATLib_gsm_CFUN(int mode)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 *at_cmd_buff;

	M_ClrBit(u8RetVal);
	at_cmd_buff = GsmMalloc(100);
	if( at_cmd_buff == NULL ) {
		GsmMallocErrHook();
		M_ClrBit(u8RetVal);
	}
	else {
		sprintf((char *)at_cmd_buff,"AT+CFUN=%d\r\n", mode);
		dev_gsm_SendWrite(at_cmd_buff, strlen((char*)at_cmd_buff));
		GsmFree(at_cmd_buff, 100);

		u8RetVal = app_gsm_WaitATCmdAck(NULL, "OK", NULL, 5);
	}

	return u8RetVal;
}

GsmTpU8 ATLib_gsm_CPIN(void)
{
	dev_gsm_SendWrite("AT+CPIN?\r\n", strlen("AT+CPIN?\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"READY",NULL,5));
	
}

GsmTpU8 ATLib_gsm_CGSMS(int mode)
{
	GsmTpU8 u8RetVal;
	GsmTpU8 *at_cmd_buff;

	M_ClrBit(u8RetVal);
	at_cmd_buff = GsmMalloc(20);
	if( at_cmd_buff == NULL ) {
		GsmMallocErrHook();
		M_ClrBit(u8RetVal);
	}
	else {
		sprintf((char *)at_cmd_buff,"AT+CGSMS=%d\r\n", mode);
		dev_gsm_SendWrite(at_cmd_buff, strlen((char*)at_cmd_buff));
		GsmFree(at_cmd_buff, 20);

		u8RetVal = app_gsm_WaitATCmdAck(NULL, "OK", NULL, 20);
	}

	return u8RetVal;
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CSQ 查询信号质量命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/

GsmTpU8 ATLib_gsm_CSQ1(void)
{
	dev_gsm_SendWrite("AT+CSQ\r\n", strlen("AT+CSQ\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));

}

GsmTpU8 app_gsmAT_CENG_R(void) 
{
	dev_gsm_SendWrite("AT+CENG?\r\n", strlen("AT+CENG?\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 1));//实际测试一般1S就回应
}

GsmTpU8 ATLib_gsm_CENG(void) {
	dev_gsm_SendWrite("AT+CENG=1,1\r\n", strlen("AT+CENG=1,1\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 5));
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CREG 查询网络注册状态命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CREG0(void)
{
	dev_gsm_SendWrite("AT+CREG=1\r\n", strlen("AT+CREG=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_CREG1(void)
{
	dev_gsm_SendWrite("AT+CREG?\r\n", strlen("AT+CREG?\r\n"));
	

	return(GsmTpTRUE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+COPS? 读取当前已经注册的网络和运营商信息
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_COPS(void)
{
	dev_gsm_SendWrite("AT+COPS?\r\n", strlen("AT+COPS?\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL, c8AtCmdStrOk, NULL, 10));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CMEE 设置终端报错命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CMEE(void)
{
	dev_gsm_SendWrite("AT+CMEE=2\r\n", strlen("AT+CMEE=2\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	开启来电振铃
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_RING(void)
{
	dev_gsm_SendWrite("AT%RING=1\r\n", strlen("AT%RING=1\r\n"));

	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_CRC(void)
{
	dev_gsm_SendWrite("AT+CRC=0\r\n", strlen("AT+CRC=0\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CLIP 显示主叫识别命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CLIP(void)
{
	dev_gsm_SendWrite("AT+CLIP=1\r\n", strlen("AT+CLIP=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CMGF(void)
{
	dev_gsm_SendWrite("AT+CMGF=0\r\n", strlen("AT+CMGF=0\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CNMI(void)
{
	dev_gsm_SendWrite("AT+CNMI=2,2\r\n", strlen("AT+CNMI=2,2\r\n"));
	
	
	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL,3));
}

GsmTpU8 ATLib_gsm_MAVOL2(void) {
	//main audio channel of microphone
	dev_gsm_SendWrite("AT+CMIC=0,7\r\n", strlen("AT+CMIC=0,7\r\n"));
	app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3);

	//Aux audio channel of microphone
	dev_gsm_SendWrite("AT+CMIC=1,7\r\n", strlen("AT+CMIC=1,7\r\n"));
	app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3);

	//main audio channel of side tone
	dev_gsm_SendWrite("AT+SIDET=0,8\r\n", strlen("AT+SIDET=0,8\r\n"));
	app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3);

	//Aux audio channel of side tone
	dev_gsm_SendWrite("AT+SIDET=1,8\r\n", strlen("AT+SIDET=1,8\r\n"));
	app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3);

	return GsmTpTRUE;
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CFGRI(void)//不使用手动网络数据可以改为2
{
	dev_gsm_SendWrite("AT+CFGRI=1\r\n", strlen("AT+CFGRI=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL, 5));
}

/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:执行AT+CFGRI=2，模块收到TCP数据后，RI脚会有拉低	
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/
GsmTpU8 ATLib_gsm_CFGRI_TCP(void)
{
	dev_gsm_SendWrite("AT+CFGRI=2\r\n", strlen("AT+CFGRI=2\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL, 5));
}

/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:开启硬件流控，MCU拉高RTS,即mcu不允许GSM发送串口数据
-参数说明:当拉低RTS，GSM才会发送串口数据
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/
GsmTpU8 ATLib_gsm_IFC(void)
{
	dev_gsm_SendWrite("AT+IFC=2,2\r\n", strlen("AT+IFC=2,2\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL, 5));
}

/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/
GsmTpU8 ATLib_gsm_CMGL(void)
{
	dev_gsm_SendWrite("AT+CMGL=0\r\n", strlen("AT+CMGL=0\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL, 8));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CMGR(GsmTpU16 index)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("CMGR can't malloc!\r\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+CMGR=%d\r\n", index);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	GsmFree(at_cmd_buff, MAX_AT_CMD_LEN); 
	return(app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk,NULL, 8));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------------------------
-函数功能:  	删除短信
-参数说明:	删除所有短信
-------------------------------------------------------------------------------------------------
*/

GsmTpU8 ATLib_gsm_CMGD_2(GsmTpU16 Index, GsmTpU8 Mode)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_SIZE);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("CMGD no MAB!\n");
		GsmMallocErrHook();
		return(GsmTpFALSE);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+CMGD=%d,%d\r\n",Index,Mode);

	dev_gsm_SendWrite(at_cmd_buff, strlen((const GsmTpS8*)at_cmd_buff));
	GsmFree(at_cmd_buff, MAX_AT_CMD_SIZE); 
	
	return(app_gsm_WaitATCmdAck(NULL, c8AtCmdStrOk, NULL, 8));
}

GsmTpU8 ATLib_gsm_CMGD_All(void)
{
	return(ATLib_gsm_CMGD_2(1, 3)); 
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_CMGS(GsmTpU8 *data_buff, GsmTpU16 data_len)
{
	GsmTpU8 *at_cmd_buff;
	GsmTpU8 u8CtrlZ; 
	GsmTpBool boRetVal; 
	GsmTpU8 u8Len; 
	M_ClrBit(boRetVal); 
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff) {
		GsmDebugString("CMGS can't malloc!\n");
		GsmMallocErrHook();
		M_ClrBit(boRetVal); 
	}
	else {
		u8Len = snprintf((GsmTpS8*)at_cmd_buff, MAX_AT_CMD_LEN,"AT+CMGS=%d\r\n", (data_len - 2) / 2);
		dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, u8Len);
		if(app_gsm_WaitATCmdAck(NULL, ">", NULL, 6)) {
			dev_gsm_SendWrite((const GsmTpU8*)data_buff, data_len);
			u8CtrlZ = 0x1A; //Ctrl+Z
			dev_gsm_SendWrite(&u8CtrlZ, sizeof(u8CtrlZ));
			if(app_gsm_WaitATCmdAck(NULL, "+CMGS:", NULL, 30)) {
				M_SetBit(boRetVal); 
			}
			else {
				M_ClrBit(boRetVal); 
			}
		}
		else {
			GsmDebugString("CMGS err\r\n"); 
			M_ClrBit(boRetVal); 
		}
		
		GsmFree((GsmTpU8 *)at_cmd_buff,MAX_AT_CMD_LEN);
	}

	return(boRetVal);
}


/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:	ATD 发起呼叫命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/
GsmTpU8 ATLib_gsm_ATD(GsmTpU8 *phone_num)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("ATD can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff,"ATD%s;\r\n",phone_num);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,10));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	ATA 接听命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_ATA(void)
{
	dev_gsm_SendWrite("ATA\r\n", strlen("ATA\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	ATH 挂断连接命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_ATH(void)
{
	dev_gsm_SendWrite("ATH\r\n", strlen("ATH\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+VTS 发送DTMF 音调
-参数说明:	字符类型: 0-9, #, *, A, B, C, D 字符集中的ASCII 字符
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_VTS(GsmTpS8 DTMF)
{
	GsmTpU8 *at_cmd_buff;

	if(DTMF < '#') return(0);
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("VTS can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+VTS=%c\r\n", DTMF);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT^SWSPATH 切换音频通道命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MAPATH0(GsmTpU8 chl)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("SWSPATH can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+MAPATH=1,%d\r\n", (GsmTpS8)chl);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));

	app_gsm_WaitATCmdAck(NULL,"OK",NULL, 3);

	sprintf((GsmTpS8*)at_cmd_buff, "AT+MAPATH=2,%d\r\n", (GsmTpS8)chl);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);
	
	app_gsm_WaitATCmdAck(NULL,"OK",NULL, 3);
	GsmDriverSleep(300);
	return(GsmTpTRUE);
}

GsmTpU8 ATLib_gsm_MAPATH1(GsmTpU8 chl)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("SWSPATH can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+MAPATH=1,%d\r\n", (GsmTpS8)chl);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));

	GsmDriverSleep(500);

	sprintf((GsmTpS8*)at_cmd_buff, "AT+MAPATH=2,%d\r\n", (GsmTpS8)chl);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));

	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);
	GsmDriverSleep(300);
	return(GsmTpTRUE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CMIC 调节麦克风增益命令
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MMICG(GsmTpS8 level)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("CMIC can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+MMICG=%d\r\n", level);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);
	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	AT+CLVL 设置扬声器音量
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MAVOL(GsmTpU8 vol)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("CMIC can't malloc!\n");
		GsmMallocErrHook();
		return(0);
	}
	
	if(vol > GsmGetMaxVolumeLevel()) vol = GsmGetMaxVolumeLevel();
	sprintf((GsmTpS8*)at_cmd_buff,"AT+MAVOL=3,7,%u\r\n",(GsmTpS8)vol);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);
	
	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_MOVAL_ForInit(void)
{
	return(ATLib_gsm_MAVOL(GsmGetParaVolume()));
}

/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/
GsmTpU8 ATLib_gsm_CIPHEAD(void) {
	dev_gsm_SendWrite("AT+CIPHEAD=1\r\n", strlen("AT+CIPHEAD=1\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 20));
}

GsmTpU8 ATLib_gsm_CGREG0(void)
{
	dev_gsm_SendWrite("AT+CGREG=1\r\n", strlen("AT+CGREG=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,3));
}

GsmTpU8 ATLib_gsm_CGREG1(void)
{
	dev_gsm_SendWrite("AT+CGREG?\r\n", strlen("AT+CGREG?\r\n"));
	

	return(GsmTpTRUE);
}

GsmTpU8 ATLib_gsm_CgattEnable(void) {
	const GsmTpS8 cs8AtCgattEnableCmd[] = "AT+CGATT=1\r\n";

	dev_gsm_SendWrite((GsmTpU8 *)cs8AtCgattEnableCmd, strlen(cs8AtCgattEnableCmd));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 11));
}

GsmTpU8 ATLib_gsm_CGATT(void) {
	GsmTpU8 u8RetVal;

	M_ClrBit(u8RetVal);
	dev_gsm_SendWrite("AT+CGATT?\r\n", strlen("AT+CGATT?\r\n"));
	if(app_gsm_WaitATCmdAck(NULL, "+CGATT: 1", NULL, 3)) {
		M_SetBit(u8RetVal);
	}

	return u8RetVal;
}

GsmTpU8 ATLib_gsm_CIPSHUT(void) {
	dev_gsm_SendWrite("AT+CIPSHUT\r\n", strlen("AT+CIPSHUT\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "SHUT OK", NULL, 30));

}

GsmTpU8 ATLib_gsm_CSTT(void) {
	GsmTpU8 u8RetVal;
	GsmTpS8 *at_cmd_buff;
	GsmTpU8 *pu8Apn;
	GsmTpU8 *pu8UserName;
	GsmTpU8 *pu8Password;
	apns tApn;
	M_ClrBit(u8RetVal);
	at_cmd_buff = (GsmTpS8 *)mymalloc(90);
	if( at_cmd_buff == NULL ) {
		GsmDebugString("AT CSTT can't malloc!\n");
		GsmMallocErrHook();
		M_ClrBit(u8RetVal);
	}
	else {
		GetGsmLinkPara(&pu8Apn, &pu8UserName, &pu8Password);
		if(IS_FALSE_BIT(app_sysGetApn(pu8Apn,&tApn)))
		{
			
		}
		else
		{
			pu8Apn=tApn.apn;
			pu8UserName=tApn.user;
			pu8Password=tApn.password;
		}
		sprintf(   at_cmd_buff,
				"AT+CSTT=\"%s\",\"%s\",\"%s\"\r\n",
				(GsmTpS8 *)pu8Apn,
				(GsmTpS8 *)pu8UserName,
				(GsmTpS8 *)pu8Password );
		GsmDebugStringLowLevel((GsmTpS8 *)at_cmd_buff);
		dev_gsm_SendWrite((GsmTpU8 *)at_cmd_buff, strlen(at_cmd_buff));
		myfree((GsmTpU8 *)at_cmd_buff, 90);
		return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 50));
	}

	return u8RetVal;
}

GsmTpU8 ATLib_gsm_CIICR(void) {
	dev_gsm_SendWrite("AT+CIICR\r\n", strlen("AT+CIICR\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 20));

}

GsmTpU8 ATLib_gsm_CIFSR(void) {
	dev_gsm_SendWrite("AT+CIFSR\r\n", strlen("AT+CIFSR\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, ".", NULL, 150));

}

GsmTpU8 ATLib_gsm_CIPSTATUS(void) {
	GsmTpU8 u8RetVal;
	Gsmmyevent stATRet_Evn;

	M_ClrBit(u8RetVal);
	dev_gsm_SendWrite("AT+CIPSTATUS\r\n", strlen("AT+CIPSTATUS\r\n"));

	if(app_gsm_WaitATCmdAck(&stATRet_Evn, "STATE", NULL, 30)) {
		if(NULL != strstr((const GsmTpS8*)stATRet_Evn.buff, "INITIAL")) {
			myfree(stATRet_Evn.buff, stATRet_Evn.size);
			M_SetBit(u8RetVal);
		}
		else {
			myfree(stATRet_Evn.buff, stATRet_Evn.size);
			if(IS_TRUE_BIT(ATLib_gsm_CIPSHUT())) {
				M_SetBit(u8RetVal);
			}
			else {
				M_ClrBit(u8RetVal);
			}
		}
	}
	else {
		M_ClrBit(u8RetVal);
	}

	return u8RetVal;
}


GsmTpU8 ATLib_gsm_CIPSTART(void) {
	GsmTpU8 u8RetVal;
	GsmTpS8 *at_cmd_buff;
	Gsmmyevent evn_ret;
	GprsCommModeSt GprsCommMode;
	GsmTpU16 u16Port;
	GsmTpU8 *pu8IpDomain;
	const GsmTpS8 cs8UdpStr[] = "UDP";
	const GsmTpS8 cs8TcpStr[] = "TCP";
	const GsmTpS8 *pcs8TcpUdpStr;
	GsmTpU8 u8Len;

	M_ClrBit(u8RetVal);
	at_cmd_buff = (GsmTpS8 *)mymalloc(100);
	if(NULL == at_cmd_buff) {
		GsmDebugString("AT ipopen can't malloc!\n");
		GsmMallocErrHook();
		M_ClrBit(u8RetVal);
	}
	else {
		GetGsmGprsPara(&GprsCommMode, &u16Port, &pu8IpDomain);
		if(GPRS_COMM_MODE_TCP == GprsCommMode) {
			pcs8TcpUdpStr = cs8TcpStr;
		}
		else {
			pcs8TcpUdpStr = cs8UdpStr;
		}
		u8Len = sprintf(at_cmd_buff,
					  "AT+CIPSTART=\"%s\",\"%s\",%d\r\n",
					  pcs8TcpUdpStr,
					  (s8 *)pu8IpDomain,
					  u16Port);
		//GsmDebugString((GsmTpS8 *)at_cmd_buff);
		dev_gsm_SendWrite((GsmTpU8 *)at_cmd_buff, u8Len);
		myfree((GsmTpU8 *)at_cmd_buff, 100);
		if(app_gsm_WaitATCmdAck(&evn_ret, "CONNECT OK", "CONNECT FAIL", 100)) {
			if(NULL != strstr((const GsmTpS8*)evn_ret.buff, "CONNECT OK")) {
				M_SetBit(u8RetVal);
			}
			else if(NULL != strstr((const GsmTpS8*)evn_ret.buff, "CONNECT FAIL")) {
				ATLib_gsm_CIPSTATUS();
				M_ClrBit(u8RetVal);
			}
			else {
				M_ClrBit(u8RetVal);
			}
			myfree(evn_ret.buff, evn_ret.size);
		}
		else {
			M_ClrBit(u8RetVal);
		}
	}

	return u8RetVal;

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
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
extern void GsmGetLinkParaApnUserKey(GsmTpU8 **p2u8Apn, GsmTpU8 **p2u8UserName, GsmTpU8 **p2u8Password); 

GsmTpU8 ATLib_gsm_MIPCALL_EstablishLink(void)
{
	GsmTpU8 *at_cmd_buff;
	GsmTpU8 *tmp_ptr;
	GsmTpU8 *pu8UserName; 
	GsmTpU8 *pu8Password; 
	Gsmmyevent event;
	GsmTpU8 u8Len; 
	GsmTpBool boExeOkFlg; 
	M_ClrBit(boExeOkFlg); 
#define MAX_AT_CMD_LEN_FOR_LONG	100
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN_FOR_LONG);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AT+MIPCALL can't malloc!\n");
		GsmMallocErrHook();
		return(GsmTpFALSE);
	}

	GsmGetLinkParaApnUserKey(&tmp_ptr, &pu8UserName, &pu8Password); 
		
	u8Len = sprintf((GsmTpS8*)at_cmd_buff, 
			"AT+MIPCALL=1,\"%s\",\"%s\",\"%s\"\r\n",
			tmp_ptr, 
			pu8UserName, 
			pu8Password);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, u8Len);
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN_FOR_LONG);

	event.buff = NULL; 
	if(app_gsm_WaitATCmdAck(&event,"+MIPCALL:",NULL,180) == GsmTpFALSE) //Add by zcf. 2012.12.06//
	{
		if(IS_NULL_P(event.buff)) {; 
		}
		else {
			GsmFree(event.buff, event.size); 
		}
		M_ClrBit(boExeOkFlg); 
	}
	else
	{
		tmp_ptr += strlen("+MIPCALL: ");
		if(strchr((const GsmTpS8*)event.buff,'.'))
		{
			M_SetBit(boExeOkFlg); 
		}
		else
		{
			M_ClrBit(boExeOkFlg); 
		}
		GsmFree(event.buff,event.size);
	}

	GprsEstablishLinkHook(boExeOkFlg); 
	
	return(boExeOkFlg);
}


GsmTpU8 ATLib_gsm_MIPCALL_DisconnectLink(void)
{
//	dev_gsm_SendWrite("AT+MIPCALL=0\r\n", strlen("AT+MIPCALL=0\r\n"));
//	return(app_gsm_WaitATCmdAck(NULL,"+MIPCALL: 0",NULL,60));
	dev_gsm_SendWrite("AT+CIPCLOSE\r\n", strlen("AT+CIPCLOSE\r\n"));
	return(app_gsm_WaitATCmdAck(NULL,"CLOSE OK",NULL,60));
}

GsmTpU8 ATLib_gsm_MIPCALL_InquireLink(void)
{
//	Gsmmyevent event;
//	GsmTpU8 *tmp_ptr;
	
//	dev_gsm_SendWrite("AT+MIPCALL?\r\n", strlen("AT+MIPCALL?\r\n"));
	dev_gsm_SendWrite("AT+CIFSR\r\n", strlen("AT+CIFSR\r\n"));

//	if(app_gsm_WaitATCmdAck(&event,"+MIPCALL:",NULL,10) == GsmTpFALSE)
//	if(app_gsm_WaitATCmdAck(&event,"OK",NULL,10) == GsmTpFALSE)
//	{
//		return(GsmTpFALSE);
//	}
	
//	tmp_ptr = (GsmTpU8*)strstr((const GsmTpS8*)event.buff,"+MIPCALL:");
//	if(strchr((const GsmTpS8*)tmp_ptr,'.'))
//	{
//		GsmFree(event.buff,event.size);
//		return(GsmTpTRUE);
//	}
//	GsmFree(event.buff,event.size);
//	return(GsmTpFALSE);

	return(GsmTpTRUE);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MIPOPEN_OpenSocket(void)
{
	GsmTpU8 *tmp_ptr;
	GsmTpU16 port;
	GsmTpU8 udp_tcp;
	GsmTpBool boOpenOkFlg; 
	IpProTypeEn enProType; 

	GsmGetIpPara(&udp_tcp, &port, &tmp_ptr); 	
	if(GPRS_UDP == udp_tcp) {
		enProType = IP_PRO_TYPE_UDP; 
	}
	else {
		enProType = IP_PRO_TYPE_TCP; 
	}

	boOpenOkFlg = ATLib_gsm_OpenSocket((GsmTpS8 *)tmp_ptr, port, 
										enProType, IP_DAT_SRC_SCK_1); 
	
	if(IS_TRUE_BIT(boOpenOkFlg)) {
	}
	else {
		GsmGprsOpenSocketFailHook(); 
	}
	
	return(boOpenOkFlg);
}

int
ipaddr_aton(const char *cp, GsmTpU32 *pu32Addr); 

/*广和通新指令*/
GsmTpBool ATLib_gsm_Mipdns(GsmTpU8 *pu8DstIpStr/*[IP_V4_STR_BUF_LEN_MIN]*/,
							GsmTpU8 *pu8IpDomain); 
GsmTpU8 ATLib_gsm_OpenSocket(GsmTpS8 *IpDomain, GsmTpU16 u16Port, 
								IpProTypeEn enIpType, IpDatSrcEn enIpDatSrc)
{
	GsmTpU8 *at_cmd_buff;
	GsmTpU8 u8Len; 
	GsmTpS8 s8IpStrBuf[IP_V4_STR_BUF_LEN_MIN]; 
	Gsmmyevent event;
	GsmTpU8 u8SckIdForGsm; 
	GsmTpBool boRetVal; 
	GsmTpBool boIsUdpFlg; 
	const GsmTpS8 s8MipOpenAckBasicStr[] = "+MIPOPEN: "; 
	M_ClrBit(boRetVal); 
	GsmTpU32 u32IpV4AddrTmp; 
#define MIPOPEN_AT_CMD_MAX_LEN		80
	at_cmd_buff = GsmMalloc(MIPOPEN_AT_CMD_MAX_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AT+MIPOPEN can't malloc!\n");
		GsmMallocErrHook();
		return(GsmTpFALSE);
	}
	enIpDatSrc > IP_DAT_SRC_SCK_5 ? (enIpDatSrc = IP_DAT_SRC_SCK_1) : 0; 

	sstGsmAtUseSocketPara[enIpDatSrc].enIpProType = enIpType; 
	sstGsmAtUseSocketPara[enIpDatSrc].u16Port = u16Port; 
	memset(s8IpStrBuf, 0, sizeof(s8IpStrBuf)); 

	M_ClrBit(sstGsmAtUseSocketPara[enIpDatSrc].u32IpV4Addr); 
	if(IS_TRUE_BIT(ATLib_gsm_DNSR((GsmTpU8 *)s8IpStrBuf,
									(GsmTpU8 *)IpDomain))) {;
		GsmDebugString("Dns 1 ok\r\n"); 
	}
	else { 		
		GsmDebugString("Dns 1 Err\r\n"); 
		if(IS_TRUE_BIT(ATLib_gsm_Mipdns((GsmTpU8 *)s8IpStrBuf,
									(GsmTpU8 *)IpDomain))) {; 
			GsmDebugString("Dns 2 ok\r\n"); 
		}
		else {
			GsmDebugString("Dns 2 Err\r\n"); 
			memset(s8IpStrBuf, 0, sizeof(s8IpStrBuf)); 
			memcpy(s8IpStrBuf, IpDomain, M_MIN(sizeof(s8IpStrBuf) - 1, strlen((GsmTpS8 *)IpDomain))); 
		}
	}
	
	M_ClrBit(u32IpV4AddrTmp); 
	if(IS_FALSE_BIT(ipaddr_aton(s8IpStrBuf, &u32IpV4AddrTmp))) {
		GsmDebugString("ipaddr_aton Err\r\n"); 
	}
	else {
		sstGsmAtUseSocketPara[enIpDatSrc].u32IpV4Addr = u32IpV4AddrTmp; 
	}
	
	u8SckIdForGsm = enIpDatSrc + 1; //form 1,gsm module manual
	if(enIpType != IP_PRO_TYPE_UDP) {
		enIpType = IP_PRO_TYPE_TCP; 
		M_ClrBit(boIsUdpFlg); 
	}
	else {
		M_SetBit(boIsUdpFlg); 
	}
	u8Len = sprintf((GsmTpS8*)at_cmd_buff,"AT+MIPOPEN=%d,,\"%s\",%d,%d\r\n", 
											u8SckIdForGsm, IpDomain, u16Port, boIsUdpFlg);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, u8Len);
	
	GsmFree(at_cmd_buff, MIPOPEN_AT_CMD_MAX_LEN);


	event.buff = NULL; 
	M_ClrBit(event.size); 
	if(IS_FALSE_BIT(app_gsm_WaitATCmdAck(&event,s8MipOpenAckBasicStr,"+MIPSTAT:",30))) {
		M_ClrBit(boRetVal);
	}
	else {
		GsmTpS8 *ps8AtAckDat; 
		ps8AtAckDat = strstr((const GsmTpS8*)event.buff, s8MipOpenAckBasicStr); 
		if(IS_NULL_P(ps8AtAckDat)) {; 
			M_ClrBit(boRetVal);
		}
		else {
			ps8AtAckDat += sizeof(s8MipOpenAckBasicStr) - 1; 
			if((GsmM_Ascii2u8(ps8AtAckDat[0]) == u8SckIdForGsm)
			&& ((GsmM_Ascii2u8(ps8AtAckDat[2]) == 1)
				|| (GsmM_Ascii2u8(ps8AtAckDat[2]) == 2))) {
//State 0 Inactive 1 Active 2 SSL secured
				M_SetBit(boRetVal); 
			}
			else {
				M_ClrBit(boRetVal);
			}
		}
	}
	
	if(IS_NULL_P(event.buff)) {; 
	}
	else {
		GsmFree(event.buff, event.size); 
	}
	return(boRetVal);
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MIPOPEN_InquireSocket(GsmTpU8 socket_id)
{
	Gsmmyevent event;
//	GsmTpS8 *tmp_ptr;
	GsmTpBool boRetVal; 
//	dev_gsm_SendWrite("AT+MIPOPEN?\r\n", strlen("AT+MIPOPEN?\r\n"));
	dev_gsm_SendWrite("AT+CIPSTART=?\r\n", strlen("AT+CIPSTART=?\r\n"));
	M_ClrBit(boRetVal); 
//	if(app_gsm_WaitATCmdAck(&event,"+MIPOPEN:",NULL,10) == GsmTpFALSE)
	if(app_gsm_WaitATCmdAck(&event,"OK",NULL,10) == GsmTpFALSE)

	{
		M_ClrBit(boRetVal); 
	}
	else {
//		tmp_ptr = (GsmTpS8*)strstr((const GsmTpS8*)event.buff,"+MIPOPEN:");
//		if(strchr((const GsmTpS8*)tmp_ptr, socket_id + 1 + '0')) {
//			M_ClrBit(boRetVal);
//		}
//		else {
//			M_SetBit(boRetVal);
//		}
		GsmFree(event.buff,event.size);
		M_SetBit(boRetVal);
	}
	
	return(boRetVal); /*socket opened*/
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
GsmTpU8 ATLib_gsm_MIPCLOSE_CloseSocket(GsmTpU8 socket_id)
{
#define CMD_BUF_LEN	(22)
	GsmTpS8 s8CmdBuf[CMD_BUF_LEN];
	GsmTpU8 u8Len;
	memset(s8CmdBuf, 0, CMD_BUF_LEN);
//	u8Len = sprintf(s8CmdBuf, "AT+MIPCLOSE=%d\r\n", socket_id + 1);
	u8Len = sprintf(s8CmdBuf, "AT+CIPCLOSE=%d\r\n", socket_id + 1); 
	dev_gsm_SendWrite((GsmTpU8 *)s8CmdBuf, u8Len);

//	return (app_gsm_WaitATCmdAck(NULL,"+MIPCLOSE:",NULL,10));
	return (app_gsm_WaitATCmdAck(NULL,"CLOSE OK",NULL,10));
}

GsmTpU8 ATLib_gsm_MIPCLOSE_InquireClose(void)
{
	dev_gsm_SendWrite("AT+MIPCLOSE?\r\n", strlen("AT+MIPCLOSE?\r\n"));
	
	
	return(1);
}


/*
-------------------------------------------------------------------------------------------------
*************************************************************************************************
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*************************************************************************************************
*/

/*
-------------------------------------------------------------------------------------------------
-函数功能:  	GPRS 数据发送
-参数说明:	
				lhz
-------------------------------------------------------------------------------------------------
*/
GsmAtCmdAckEn ATLib_gsm_IPSEND(GsmTpU8 *data_buff, 
										GsmTpU16 data_len, 
										IpDatSrcEn enIpDatSrc)
{
#if 1
	GsmAtCmdAckEn enRetVal;
	Gsmmyevent ip_send_cmd;
	GsmTpU8 u8CtlrZ;

	enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG;
	if((NULL == data_buff) || (0 == data_len)) {
		enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG;
	}
	else {
		ip_send_cmd.buff = mymalloc(MAX_AT_CMD_SIZE);
		if(NULL == ip_send_cmd.buff) {
			GsmDebugString("ip_send_cmd can't malloc!\n");
			GsmMallocErrHook();
			myfree(data_buff,data_len);
			enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG;
		}
		else {
			ip_send_cmd.size = sprintf((s8*)ip_send_cmd.buff, "AT+CIPSEND=%d\r\n", data_len);

			dev_gsm_SendWrite(ip_send_cmd.buff, ip_send_cmd.size);
			myfree(ip_send_cmd.buff, ip_send_cmd.size);
			if(IS_FALSE_BIT(app_gsm_WaitATCmdAck(NULL, ">", NULL, 10))) {
				GsmDebugString("ip data send fail\r\n");
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG;
			}
			else {
				//GsmDebugPrintf(data_buff, data_len);
				//GsmDebugString("\r\n");
				dev_gsm_SendWrite(data_buff, data_len);
				u8CtlrZ = 0x1A;	//Ctrl+Z
				dev_gsm_SendWrite(&u8CtlrZ, sizeof(u8CtlrZ));
				if(IS_TRUE_BIT(app_gsm_WaitATCmdAck(NULL, "SEND", NULL, 30))) {
					enRetVal = GSM_AT_CMD_IP_SEND_ACK_OK;
				}
				else {
					enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG;
				}
			}
		}
	}

	return enRetVal; 
#else
	GsmTpU8 *pu8IpSendCmdBuf;
	GsmTpU8 *pu8IpDatTmp;
	GsmTpU16 u16LenTmp;
	GsmTpU16 u16IpDatLen;
	Gsmmyevent stEve; 
	GsmAtCmdAckEn enRetVal; 
	GsmTpU8 u8Len; 
	GsmTpU8 u8SckIdForGsm; 
	const GsmTpS8 s8MipSendAckBasicStr[] = "+MIPSEND: "; 

	static GsmIpSendBufSt sstGsmIpSendBuf; 

	enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
	if(((data_len << 1) + 20) > sizeof(sstGsmIpSendBuf.u8Dat)) {
		
		GsmDebugString("Ipsend dat len is too max, fail!\n");
		return enRetVal; 
	}
	else	{; 
	}	
	
	if(NULL != data_buff) {; 
	}
	else	{
		GsmDebugString("Ipsend con null\n");
		return enRetVal; 
	}
	
	pu8IpSendCmdBuf = sstGsmIpSendBuf.u8Dat; 

	enIpDatSrc > IP_DAT_SRC_SCK_5 ? (enIpDatSrc = IP_DAT_SRC_SCK_1) : 0; 
	u8SckIdForGsm = enIpDatSrc + 1; //from 1,gsm module manual
	pu8IpDatTmp = pu8IpSendCmdBuf;

	
	u8Len = sprintf((GsmTpS8 *)pu8IpDatTmp, "AT+CIPSEND=%d,\"", u8SckIdForGsm); 
	pu8IpDatTmp += u8Len;
	u16IpDatLen = u8Len; 
	
	
	u16LenTmp = GsmHex_2_Ascii((GsmTpS8*)data_buff, (GsmTpS8*)pu8IpDatTmp, data_len) - 1;
	pu8IpDatTmp += u16LenTmp;
	u16IpDatLen += u16LenTmp;
	
	pu8IpDatTmp[0] = '"';
	pu8IpDatTmp[1] = '\r';
	pu8IpDatTmp[2] = '\n';
	pu8IpDatTmp[3] = '\0';
	u16IpDatLen += 3;
	dev_gsm_SendWrite((const GsmTpU8*)pu8IpSendCmdBuf, u16IpDatLen);

	stEve.buff = NULL; 
	M_ClrBit(stEve.size); 
	if(IS_FALSE_BIT(app_gsm_WaitATCmdAck(&stEve, s8MipSendAckBasicStr, NULL, 8))) {
		enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
	}
	else {
		GsmTpU8 *pu8AckDat; 
		pu8AckDat = (GsmTpU8 *)strstr((const GsmTpS8*)stEve.buff, s8MipSendAckBasicStr); 
		pu8AckDat +=sizeof(s8MipSendAckBasicStr) - 1; 
		if(u8SckIdForGsm != (pu8AckDat[0] - '0')) {
			enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
		}
		else {
			if(IS_FALSE_BIT(pu8AckDat[2] - '0')) {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_OK; 
			}
			else if(IS_TRUE_BIT(pu8AckDat[2] - '0')) {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_BUSY; 
			}
			else {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
			}
		}
	}
	
	if(NULL == stEve.buff) {; 
	}
	else	{
		GsmFree(stEve.buff, stEve.size); 
	}
	return enRetVal;
#endif
}

char *ipaddr_ntoa_r(const GsmTpU32 *addr, char *buf, int buflen); 
GsmAtCmdAckEn AtLib_gsm_MIPPUSH(IpDatSrcEn enIpDatSrc)
{
#define MIPOPEN_AT_CMD_MAX_LEN		80
	GsmTpU8 *at_cmd_buff;
	Gsmmyevent stEve; 
	GsmAtCmdAckEn enRetVal; 
	GsmTpU8 u8SckIdForGsm; 
	const GsmTpS8 s8MipPushAckBasicStr[] = "+MIPPUSH: "; 
	GsmTpBool boIsUdpFlg; 
	GsmTpS8 s8IpStrBuf[IP_V4_STR_BUF_LEN_MIN]; 
	M_ClrBit(boIsUdpFlg); 
	enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
	
	at_cmd_buff = GsmMalloc(MIPOPEN_AT_CMD_MAX_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("Ipclose can't malloc!\n");
		GsmMallocErrHook();
		return(enRetVal);
	}
	enIpDatSrc > IP_DAT_SRC_SCK_5 ? (enIpDatSrc = IP_DAT_SRC_SCK_1) : 0; 
	u8SckIdForGsm = enIpDatSrc + 1; //form 1,gsm module manual

	(IP_PRO_TYPE_TCP == sstGsmAtUseSocketPara[enIpDatSrc].enIpProType)
		? M_ClrBit(boIsUdpFlg)
		: M_SetBit(boIsUdpFlg);
	
	if(IS_TRUE_BIT(boIsUdpFlg)
	&& IS_TRUE_BIT(sstGsmAtUseSocketPara[enIpDatSrc].u32IpV4Addr)) {
		memset(s8IpStrBuf, 0, sizeof(s8IpStrBuf));
		ipaddr_ntoa_r(&sstGsmAtUseSocketPara[enIpDatSrc].u32IpV4Addr, 
						s8IpStrBuf, 
						sizeof(s8IpStrBuf)); 	
		sprintf((GsmTpS8*)at_cmd_buff,"AT+MIPPUSH=%d,\"%s\",%d\r\n", 
							u8SckIdForGsm, 
							s8IpStrBuf, 
							sstGsmAtUseSocketPara[enIpDatSrc].u16Port);
	}
	else {
		sprintf((GsmTpS8*)at_cmd_buff,"AT+MIPPUSH=%d\r\n", u8SckIdForGsm);
	}

	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, strlen((GsmTpS8 *)at_cmd_buff));
	GsmFree(at_cmd_buff, strlen((GsmTpS8*)at_cmd_buff)); 
	
	stEve.buff = NULL; 
	M_ClrBit(stEve.size); 
	if(IS_FALSE_BIT(app_gsm_WaitATCmdAck(&stEve, s8MipPushAckBasicStr, NULL, 5)))
	{
		enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
	}
	else
	{
		GsmTpU8 *pu8AckDat; 
		pu8AckDat = (GsmTpU8 *)strstr((const GsmTpS8*)stEve.buff, s8MipPushAckBasicStr); 
		pu8AckDat += sizeof(s8MipPushAckBasicStr) - 1; 
		if(u8SckIdForGsm != (pu8AckDat[0] - '0')) {
			enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
		}
		else {
			if((2 == (pu8AckDat[2] - '0')) || IS_FALSE_BIT(pu8AckDat[2] - '0')) {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_OK; 
			}
			else if(1 == (pu8AckDat[2] - '0')) {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_BUSY; 
			}
			else {
				enRetVal = GSM_AT_CMD_IP_SEND_ACK_NG; 
			}
		}
	}
	
	if(NULL == stEve.buff) {; 
	}
	else	{
		GsmFree(stEve.buff, stEve.size); 
	}
	return enRetVal;
}

GsmTpU8 app_gsmAT_Mipflush(GsmTpU8 u8SocketId)
{
	GsmTpU8 *at_cmd_buff;
	
	at_cmd_buff = (GsmTpU8 *)GsmMalloc(MAX_AT_CMD_SIZE);
	if( at_cmd_buff == NULL )
	{
		GsmDebugString("AT MIPFLUSH can't malloc!\r\n");
		GsmMallocErrHook();
		return GsmTpFALSE;
	}
	sprintf((char *)at_cmd_buff, "AT+MIPFLUSH=%d\r\n", u8SocketId);
	dev_gsm_SendWrite((GsmTpU8 *)at_cmd_buff,strlen((char *)at_cmd_buff));
	GsmFree(at_cmd_buff, strlen((const GsmTpS8*)at_cmd_buff)); 

	return (app_gsm_WaitATCmdAck(NULL,c8AtCmdStrOk, NULL, 50));
}

GsmTpU8 ATLib_gsm_Wait(void)
{
	GsmDriverSleep(500); 
	return GsmTpTRUE; 
}

GsmTpU8 ATLib_gsm_VoiceRecordQuery(void)
{
	const GsmTpS8 s8VoiceRecord[] = "AT+AUDCD=?\r\n"; 
	dev_gsm_SendWrite((GsmTpU8 *)s8VoiceRecord, strlen(s8VoiceRecord));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_AUDCD(GsmTpU8 mode)
{
	GsmTpU8 *at_cmd_buff;

	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AUDCD can't malloc!\r\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+AUDCD=%d\r\n", mode);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);
        
        return GsmTpTRUE;
	//return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_AUDPM(GsmTpU8 u_bitrate,GsmTpU8 Dsp_format)
{
	GsmTpU8 *at_cmd_buff;

	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AUDPM can't malloc!\r\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+AUDPM=%d,%d\r\n", u_bitrate,Dsp_format);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_AUDRL(GsmTpU8 index)
{
	GsmTpU8 *at_cmd_buff;
	Gsmmyevent ret_event;
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AUDRL can't malloc!\r\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+AUDRL=%d\r\n", index);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);

	if(app_gsm_WaitATCmdAck(&ret_event,"+AUDRL:",NULL,20))
	{
		if(NULL != strstr((const GsmTpS8*)ret_event.buff,"+AUDRL:"))
		{
			app_gsm_AudioAttriHandler(ret_event.buff,ret_event.size);
 			GsmFree(ret_event.buff, ret_event.size); 
			return GsmTpTRUE;
		}
		else {
			GsmFree(ret_event.buff, ret_event.size); 
		}
	}
        return GsmTpFALSE;
	//return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_AUDRD(GsmTpU8 index,GsmTpU16 offset,GsmTpU16 read_len)
{
	GsmTpU8 *at_cmd_buff;

	if(read_len > 1023) //the max read len
	{
		read_len = 1023;
	}
	
	at_cmd_buff = GsmMalloc(MAX_AT_CMD_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("AUDRD can't malloc!\r\n");
		GsmMallocErrHook();
		return(0);
	}
	
	sprintf((GsmTpS8*)at_cmd_buff, "AT+AUDRD=%d,%d,%d\r\n", index,offset,read_len);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff,strlen((const GsmTpS8*)at_cmd_buff));
	
	GsmFree(at_cmd_buff,MAX_AT_CMD_LEN);

	return(GsmTpTRUE);//app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}

GsmTpU8 ATLib_gsm_CCID(void)
{
	Gsmmyevent stEve; 
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	dev_gsm_SendWrite("AT+CCID\r\n", strlen("AT+CCID\r\n"));
	

	if(IS_FALSE_BIT(app_gsm_WaitATCmdAck(&stEve,"+CCID: ",NULL,3))) {; 
		M_ClrBit(boRetVal); 
	}
	else {
		GsmTpS8 *ps8Tmp; 
		GsmTpU8 u8Len; 
		ps8Tmp = strstr((GsmTpS8 *)stEve.buff, "+CCID: "); 
		ps8Tmp +=  strlen("+CCID: "); 
		for(u8Len = 0; u8Len < GSM_ICCID_STD_LEN; u8Len++) {
			if('\r' == *ps8Tmp) {
				break; 
			}
			else {; 
			}
		}
		
		GsmDriverEnterCritical(); 
		memcpy(u8IccidStr, ps8Tmp, u8Len); 
		GsmDriverExitCriticical(); 
		GsmFree(stEve.buff, stEve.size); 
		M_SetBit(boRetVal); 
	}
	return boRetVal; 
}

GsmTpU8 ATLib_gsm_CSCLK(void) {
	dev_gsm_SendWrite("AT+CSCLK=1\r\n", strlen("AT+CSCLK=1\r\n"));
	return (app_gsm_WaitATCmdAck(NULL, "OK", NULL, 3));

}

GsmTpU8 ATLib_gsm_CIPMUX(void)//设置单个连接
{
	dev_gsm_SendWrite("AT+CIPMUX=0\r\n", strlen("AT+CIPMUX=0\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,3));
}

GsmTpU8 ATLib_gsm_CIPMODE(void)//设置透传模式
{
	dev_gsm_SendWrite("AT+CIPMODE=1\r\n", strlen("AT+CIPMODE=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,3));
}

GsmTpU8 ATLib_gsm_CIPCCFG(void)//配置透传模式,结果收不到数据
{
	dev_gsm_SendWrite("AT+CIPCCFG=5,2,1460,1,1,1460,300\r\n", strlen("AT+CIPCCFG=5,2,1460,1,1,1460,300\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,5));
}


GsmTpU8 ATLib_gsm_CIPRXGET(void)
{
	dev_gsm_SendWrite("AT+CIPRXGET=1\r\n", strlen("AT+CIPRXGET=1\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"OK",NULL,3));
}

GsmTpU8 ATLib_gsm_CIPRXGETDATA(void)
{
	dev_gsm_SendWrite("AT+CIPRXGET=2,1200\r\n", strlen("AT+CIPRXGET=2,1200\r\n"));
	

	return(app_gsm_WaitATCmdAck(NULL,"+CIPRXGET",NULL,3));
}

#define G610_TTS_MAX_SIZE		(509)
#define G610_TTS_CMD_START	(1)
#define G610_TTS_CMD_STOP		(0)
static const GsmTpU8 cu8AtGttsCmdStr[] = "AT+GTTS=1,\"\"\r\n"; 
GsmTpU8 ATLib_gsm_TTS(const GsmTpBool cboStartFlg, const GsmTpU8 *pcu8Dat, const GsmTpU16 cu16DatLen)
{
	GsmTpBool boRetVal; 
	GsmTpU8 u8TtsCmd; //1:start, 0:stop
	Gsmmyevent stEve; 
	GsmTpU16 u16Len; 
	GsmTpU8 *pu8Tmp;
	if(IS_FALSE_BIT(cboStartFlg)
	|| IS_NULL_P(pcu8Dat) 
	|| IS_FALSE_BIT(cu16DatLen)
	|| (cu16DatLen > G610_TTS_MAX_SIZE)) {
		u8TtsCmd = G610_TTS_CMD_STOP; //stop
	}
	else {
		u8TtsCmd = G610_TTS_CMD_START; //start
	}
	stEve.size = cu16DatLen + strlen((char const *)cu8AtGttsCmdStr) + 1;//add 1 冗余 
	stEve.buff = GsmMalloc(stEve.size); 
	if(IS_NULL_P(stEve.buff)) {
		M_ClrBit(boRetVal); 
	}
	else {
		memset(stEve.buff, 0, stEve.size); 
		pu8Tmp = stEve.buff; 
		pu8Tmp += sprintf((GsmTpS8 *)pu8Tmp, "AT+GTTS=%d,\"", u8TtsCmd); 
		pu8Tmp += M_MEM_CPY(pu8Tmp, pcu8Dat, cu16DatLen); 
		pu8Tmp += sprintf((GsmTpS8 *)pu8Tmp, "\"\r\n"); 

		u16Len = (GsmTpU16)(pu8Tmp - stEve.buff);  
		dev_gsm_SendWrite((GsmTpU8 *)stEve.buff, u16Len);
		
		GsmFree(stEve.buff, stEve.size); 
		
		boRetVal = app_gsm_WaitATCmdAck(NULL,"OK",NULL,5); 
	}
	
	return(boRetVal);
}

#define MIPOPEN_AT_CMD_MAX_LEN		80

GsmTpBool ATLib_gsm_DNSR(GsmTpU8 *pu8DstIpStr/*[IP_V4_STR_BUF_LEN_MIN]*/,
							GsmTpU8 *pu8IpDomain)
{
	GsmTpU8 *at_cmd_buff;
	Gsmmyevent stEve; 
	GsmTpBool boRetVal; 
	const GsmTpS8 cs8MpingStatsStr[] = "+MPINGSTAT: "; 
	M_ClrBit(boRetVal); 
	if(IS_NULL_P(pu8DstIpStr)
	|| IS_NULL_P(pu8IpDomain)) {
		return boRetVal; 
	}
	else {; 
	}
	
	at_cmd_buff = GsmMalloc(MIPOPEN_AT_CMD_MAX_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("Domain name can't malloc!\n");
		GsmMallocErrHook();
		return(boRetVal);
	}
	memset(at_cmd_buff, 0, MIPOPEN_AT_CMD_MAX_LEN); 

	sprintf((GsmTpS8*)at_cmd_buff, "AT+MPING=1,\"%s\",1,1372,1\r\n", pu8IpDomain);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, strlen((const GsmTpS8*)at_cmd_buff));
	GsmFree(at_cmd_buff, MIPOPEN_AT_CMD_MAX_LEN); 
	memset(&stEve, 0, sizeof(stEve)); 
	boRetVal = app_gsm_WaitATCmdAck(&stEve, cs8MpingStatsStr, NULL, 45); 
	if(IS_FALSE_BIT(boRetVal)) {; 
	}
	else {
		GsmTpU8 *pu8Tmp; 
		GsmTpU8 ipLen;  
		GsmTpU8 *pu8Dat;		
		GsmTpU8 *data_buff; 
		GsmTpU16 data_len; 
		M_ClrBit(ipLen); 
		data_buff = stEve.buff; 
		data_len = stEve.size; 
		pu8Tmp = (GsmTpU8*)strstr((const GsmTpS8*)data_buff, cs8MpingStatsStr); 
		pu8Tmp += sizeof(cs8MpingStatsStr) - 1; 
		pu8Tmp++; //next
		if('0' != *pu8Tmp) {; //'1'time out ,'0'
			M_ClrBit(boRetVal); 
			GsmDebugString("ping time out\r\n"); 
		}
		else {
			data_buff = (GsmTpU8*)strchr((const GsmTpS8*)data_buff, '"') + 1;
			pu8Dat = (GsmTpU8*)strchr((const GsmTpS8*)data_buff, '"');
			ipLen = pu8Dat - data_buff;
			if((data_len > ipLen) && (ipLen < IP_V4_STR_BUF_LEN_MIN)) {
				memcpy(pu8DstIpStr, data_buff, ipLen);
			}	
			else {; 
			}
		}
	}
	
	if(IS_NULL_P(stEve.buff)) {; 
	}
	else {
		GsmFree(stEve.buff, stEve.size); 
	}
	return(boRetVal);
}

/*广和通新指令*/
GsmTpBool ATLib_gsm_Mipdns(GsmTpU8 *pu8DstIpStr/*[IP_V4_STR_BUF_LEN_MIN]*/,
							GsmTpU8 *pu8IpDomain)
{
	const GsmTpS8 cs8StrMipDns[] = "MIPDNS"; 
	GsmTpU8 *at_cmd_buff;
	Gsmmyevent stEve; 
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	if(IS_NULL_P(pu8DstIpStr)
	|| IS_NULL_P(pu8IpDomain)) {
		return boRetVal; 
	}
	else {; 
	}
	
	at_cmd_buff = GsmMalloc(MIPOPEN_AT_CMD_MAX_LEN);
	if(NULL == at_cmd_buff)
	{
		GsmDebugString("Domain name can't malloc!\n");
		GsmMallocErrHook();
		return(boRetVal);
	}
	memset(at_cmd_buff, 0, MIPOPEN_AT_CMD_MAX_LEN); 

	sprintf((GsmTpS8*)at_cmd_buff, "AT+%s=\"%s\"\r\n", cs8StrMipDns, pu8IpDomain);
	dev_gsm_SendWrite((const GsmTpU8*)at_cmd_buff, strlen((const GsmTpS8*)at_cmd_buff));
	GsmFree(at_cmd_buff, MIPOPEN_AT_CMD_MAX_LEN); 
	memset(&stEve, 0, sizeof(stEve)); 
	boRetVal = app_gsm_WaitATCmdAck(&stEve, cs8StrMipDns, NULL, 45); 
	if(IS_FALSE_BIT(boRetVal)) {; 
	}
	else {
		GsmTpU8 ipLen;  
		GsmTpU8 *data_buff; 
		GsmTpU16 data_len; 
		M_ClrBit(ipLen); 
		data_buff = stEve.buff; 
		data_len = stEve.size; 
		data_buff = (GsmTpU8*)strstr((const GsmTpS8*)data_buff, (GsmTpS8 *)pu8IpDomain);
		if(IS_NULL_P(data_buff)) {
			GsmDebugString("domain err\r\n"); 
		}
		else {
			data_buff = (GsmTpU8*)strchr((const GsmTpS8*)data_buff, ',') + 1; 
			ipLen = stEve.size - (data_buff - stEve.buff) - strlen("\r\n");
			if((data_len > ipLen) && (ipLen < IP_V4_STR_BUF_LEN_MIN)) {
				memcpy(pu8DstIpStr, data_buff, ipLen);
			}	
			else {; 
			}
		}
	}
	
	if(IS_NULL_P(stEve.buff)) {; 
	}
	else {
		GsmFree(stEve.buff, stEve.size); 
	}
	return(boRetVal);
}


#ifndef isprint
#define in_range(c, lo, up)  ((GsmTpU8)c >= lo && (GsmTpU8)c <= up)
#define isprint(c)           in_range(c, 0x20, 0x7f)
#define isdigit(c)           in_range(c, '0', '9')
#define isxdigit(c)          (isdigit(c) || in_range(c, 'a', 'f') || in_range(c, 'A', 'F'))
#define islower(c)           in_range(c, 'a', 'z')
#define isspace(c)           (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v')
#endif

/**
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 *
 * @param cp IP address in ascii represenation (e.g. "127.0.0.1")
 * @param addr pointer to which to save the ip address in network order
 * @return 1 if cp could be converted to addr, 0 on failure
 */
int
ipaddr_aton(const char *cp, GsmTpU32 *pu32Addr)
{
  GsmTpU32 val;
  GsmTpU8 base;
  char c;
  GsmTpU32 parts[4];
  GsmTpU32 *pp = parts;

  c = *cp;
  for (;;) {
    /*
     * Collect number up to ``.''.
     * Values are specified as for C:
     * 0x=hex, 0=octal, 1-9=decimal.
     */
    if (!isdigit(c))
      return (0);
    val = 0;
    base = 10;
    if (c == '0') {
      c = *++cp;
      if (c == 'x' || c == 'X') {
        base = 16;
        c = *++cp;
      } else
        base = 8;
    }
    for (;;) {
      if (isdigit(c)) {
        val = (val * base) + (int)(c - '0');
        c = *++cp;
      } else if (base == 16 && isxdigit(c)) {
        val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
        c = *++cp;
      } else
        break;
    }
    if (c == '.') {
      /*
       * Internet format:
       *  a.b.c.d
       *  a.b.c   (with c treated as 16 bits)
       *  a.b (with b treated as 24 bits)
       */
      if (pp >= parts + 3) {
        return (0);
      }
      *pp++ = val;
      c = *++cp;
    } else
      break;
  }
  /*
   * Check for trailing characters.
   */
  if (c != '\0' && !isspace(c)) {
    return (0);
  }
  /*
   * Concoct the address according to
   * the number of parts specified.
   */
  switch (pp - parts + 1) {

  case 0:
    return (0);       /* initial nondigit */

  case 1:             /* a -- 32 bits */
    break;

  case 2:             /* a.b -- 8.24 bits */
    if (val > 0xffffffUL) {
      return (0);
    }
    val |= parts[0] << 24;
    break;

  case 3:             /* a.b.c -- 8.8.16 bits */
    if (val > 0xffff) {
      return (0);
    }
    val |= (parts[0] << 24) | (parts[1] << 16);
    break;

  case 4:             /* a.b.c.d -- 8.8.8.8 bits */
    if (val > 0xff) {
      return (0);
    }
    val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
    break;
  default:
    //LWIP_ASSERT("unhandled", 0);
    break;
  }
  if (pu32Addr) {
  	*pu32Addr = val; 
  }
  return (1);
}


/**
 * Same as ipaddr_ntoa, but reentrant since a user-supplied buffer is used.
 *
 * @param addr ip address in network order to convert
 * @param buf target buffer where the string is stored
 * @param buflen length of buf
 * @return either pointer to buf which now holds the ASCII
 *         representation of addr or NULL if buf was too small
 */
char *ipaddr_ntoa_r(const GsmTpU32 *addr, char *buf, int buflen)
{ 
  GsmTpU32 s_addr;
  GsmTpU8 u8TmpBuf[sizeof(s_addr)]; 
  char inv[3];
  char *rp;
  GsmTpU8 *ap;
  GsmTpU8 rem;
  GsmTpU8 n;
  GsmTpU8 i;
  int len = 0;

  s_addr = *addr;

  rp = buf;
  GsmMsbU32To4U8(u8TmpBuf, s_addr); 
  ap = u8TmpBuf;
  for(n = 0; n < 4; n++) {
    i = 0;
    do {
      rem = *ap % (GsmTpU8)10;
      *ap /= (GsmTpU8)10;
      inv[i++] = '0' + rem;
    } while(*ap);
    while(i--) {
      if (len++ >= buflen) {
        return NULL;
      }
      *rp++ = inv[i];
    }
    if (len++ >= buflen) {
      return NULL;
    }
    *rp++ = '.';
    ap++;
  }
  *--rp = 0;
  return buf;
}





