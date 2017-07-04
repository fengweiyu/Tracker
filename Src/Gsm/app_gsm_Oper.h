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
#ifndef _APP_GSM_OPER_H_
#define _APP_GSM_OPER_H_
#include"GsmDriverComm.h"

//-----------------------------------------------------
#pragma pack(1)
#define GSM_IP_DAT_MAX_LEN		768

typedef enum
{
	GSM_MODULE_SLEEP,
	GSM_GSM_INIT,
	GSM_GSM_OK,
	GSM_GPRS_INIT,
	GSM_MODULE_OK,
}GsmWorkStEn; 



#define GSM_AT_IDLE   0
#define GSM_AT_BUSY  1

#define PHONE_Q_NULL 		0
#define PHONE_Q_RING 		1
#define PHONE_Q_CONN 		2
#define PHONE_Q_BUSY 		3
#define PHONE_Q_NOCARRY 	4
#define PHONE_Q_NOANSWER 	5
#define PHONE_Q_LIMIT		6
#define PHONE_Q_END 		7
#define PHONE_Q_DIAL		8



typedef struct
{
	GsmTpU8 login_status:1;
	GsmTpU8 Ipclose:1;
	GsmTpU8 bIsBkIpFlg:1;
	GsmTpU8 phone_dial:1;
	GsmTpU8 bNetOpenReq : 1; 
	GsmTpU8 bNetCloseReq : 1; 
	GsmTpU8 bTtsBusyFlg : 1; 
	
}gsm_flags_t;

//-----------------------------------------------------

#define SMS_MSG_MAX_LEN 	140

#define SMS_TIMEOUT   30
typedef struct
{
	GsmTpU8 status;
	GsmTpU8 attri;
	GsmTpU8 len;
	GsmTpU8 pDCS;//因此处发短信时未用,暂时作为编码方式用,1 UNICODE ,0 gbk
	GsmTpU8 *msg;
	GsmTpU8 num[GSM_PHONE_STR_LEN+1];
}gsm_sms_t;



//-----------------------------------------------------
#define GSM_DTMF_LEN  8
typedef struct
{
	GsmTpU8 push;
	GsmTpU8 pull;
	GsmTpU8 dtmf[GSM_DTMF_LEN];
}gsm_dtmf_t;

//-----------------------------------------------------
typedef enum {
	PHONE_STATUS_IDLE, 
	PHONE_STATUS_RING, 	
	PHONE_STATUS_DAIL, 	
	PHONE_STATUS_LINK, 	
	PHONE_STATUS_SPKG, 	
	PHONE_STATUS_ATH, 
}PhoneStatusEn; 

#define PHONE_DTMF_MAX 	8

typedef struct
{
	GsmTpU8 head;
	GsmTpU8 tail;
	GsmTpU8 key_val[PHONE_DTMF_MAX];
}phone_dtmf_t;

typedef struct
{
	GsmTpU8 pick_up:1;
	GsmTpU8 chl_sw:1;
	GsmTpU8 hang_up:1;
	GsmTpU8 user_call:1;
	GsmTpU8 reserv:1;
}phone_flg_t;

typedef struct
{
	phone_dtmf_t dtmf;
	phone_flg_t flg;
	PhoneStatusEn status;
	PhoneAttriEn attri;
	GsmTpU8 vol_adj;
	GsmTpU8 ringtime;
	GsmTpU8 num[GSM_PHONE_STR_LEN + 1];
}gsm_phone_t;

//-----------------------------------------------------

//-----------------------------------------------------
#define GSM_SMS_PROMPT_BUF_NUM 			4
struct SmsRxInfoSt
{
	GsmTpU8 bSmsRxingFlg : 1; 
	GsmTpU8 bSmsRxOverflowFlg : 1;
	
	GsmTpU16 u16PduLen; 

	GsmTpU32 u32SmsPromptBufTab[GSM_SMS_PROMPT_BUF_NUM]; 
	GsmFifoSt stSmsPromptFifo; 
}; 

typedef struct
{
	gsm_phone_t phone; 
	gsm_flags_t flgs;
	GsmTpU8 Csq_val;
	GsmTpU32 spked_time;
	gsm_sms_t sms;


}gsm_oper_t;

struct GsmSmsTxOprSt
{
	struct
	{
		GsmTpU8 bSmsTxReq : 1; 
		GsmTpU8 bSmsTxBusyFlg : 1; 
	}; 
	SmsTxParaSt stSmsTxPara; 
}; 
struct GsmSmsOprSt
{
	struct SmsRxInfoSt stSmsRxInfo; 
	struct GsmSmsTxOprSt stSmsTxOpr; 

};

typedef struct
{
	GsmTpS32	s32CellId;				//基站ID,GSM网络的CID，CDMA网 络的BSID
	GsmTpU16	u16LocAreaCode;			//位置区码,GSM网络的LAC，CDMA网 络的NID
	GsmTpU16	u16MobileCountryCode;	//运营商国家编码,GSM网络和CDMA都是MCC
	GsmTpS8	s8MobileNetworkCode;	//网络标识,GSM网络的MNC，CDMA网络的SID
	GsmTpS32	s32Age;					//从检测到基站到请求定位时间,单位：毫秒
	GsmTpS8	s8SignalStrength;			//信号强度,  单位：dBm
	GsmTpU16	u16TimingAdvance;		//定时超前量,由基站向移动台发送，移动台据以确定其发往基站的定时超前量，以补偿传播时延的信号。
}CellTowerSt;

#define MAX_CELL_TOWER_NUM			(3)

typedef struct
{
	CellTowerSt stCellTower[MAX_CELL_TOWER_NUM];
}CellTowerInfoSt;

typedef struct
{
	GsmTpBool bCellIdChanged;
	GsmTpBool bReserve;
	CellTowerInfoSt stCellTowerInfo;
}GsmCellTowerInfoSt;

typedef void (*pFunGsmCellTowerInfoProc)(CellTowerInfoSt * pstCellTowerInfo);

typedef struct
{
	pFunGsmCellTowerInfoProc GsmCellTowerInfoProcFunc;
}GsmCellTowerInfoReqSt;

typedef struct
{
	GsmTpBool bGsmCellTowerInfoReqFlag;
	pFunGsmCellTowerInfoProc GsmCellTowerInfoProc;
}GsmCellTowerInfoProcSt;

#pragma pack()

extern struct GsmSmsOprSt stGsmSmsOpr; 
extern gsm_oper_t gsm_oper_info;


gsm_oper_t* app_gsm_OperPtr(void);
void app_gsm_OperInfoInit(void);
GsmTpU8 GsmInit(void);
GsmTpU8 app_gsm_GprsInit(void);
GsmTpU8 app_gsm_WaitModuleReady(void);
GsmTpU8 app_gsm_WaitATCmdAck(Gsmmyevent *ret_str, const GsmTpS8 *str_1, const GsmTpS8 *str_2, GsmTpU8 Timeout);
GsmTpU8 app_gsm_GprsRegInquire(void);
void app_gsm_CSQInquire(void);
void app_gsm_CloseModule(void);

GsmTpU8 app_gsm_PhoneNumReqToDial(GsmTpU8 *num, GsmTpU8 attri); 
GsmTpU8 app_gsm_PhoneChlReqSwitch(void); 
void app_gsm_PhoneChlSwitch(void);
void GsmPhoneDailTimer(void); 
void app_gsm_PhoneReqHangUp(void); 


void app_gprs_SocketCloseCnt(GsmTpU8 mgr_type);

GsmTpU8 app_gsm_TapeStart(void);
GsmTpU8 app_gsm_TapeStop(void);
void app_gsm_TapeCfg(GsmTpU8 u_freq);
void app_gsm_GetAudioAttri(void);
void app_gsm_ReadAudioData(GsmTpU8 u8Idx, GsmTpU16 offset,GsmTpU16 read_len); 
GsmTpU8 app_gsm_SmsMsgReqToSend(GsmTpU8 *num, GsmTpU8 num_len, GsmTpU8 *msg, GsmTpU8 msg_len, GsmTpU8 attri, GsmTpU8 pDCS); 
GsmTpU8 app_gsm_PhoneDTMFReqToSend(GsmTpU8 dtmf); 
GsmTpU8 app_gsm_PhoneVolReqAdj(enum GsmPhoneVolOprEn VolAdjOpr); 
void app_gsm_PhoneHandler(void); 
void app_gsm_TapePlay(void);
void GsmNetOpenReq(GsmTpU16 u16TimeS); 
void GsmNetCloseReq(GsmTpU16 u16TimeS); 
void GsmNetOpenCloseProc(void); 
void GsmTtsProc(void); 
void GsmGetIpPara(GsmTpU8 *u8UdpTcp, GsmTpU16 *pu16Port, GsmTpU8 **pu8IpDomainPtr); 
GsmTpBool GsmSmsTxReq(SmsTxParaSt *pstTxPara); 
void GsmSmsTxProc(void); 
void GsmSmsReadByPromptPro(void); 
GsmTpBool GsmPhoneIsIdle(void); 
GsmTpBool GsmCellTowerInfoReq(GsmCellTowerInfoReqSt * pstCellTowerInfoReq);
void GsmCellTowerInfoProc(void);

#define GSM_IMEI_LEN	(15)
typedef struct
{
	GsmTpU8 u8IMEIValidFlag;
	GsmTpU8 u8Reserve;
	GsmTpU8 u8GsmIMEI[GSM_IMEI_LEN+1];
}stGsmIMEISt;

#define GSM_APN_STR_LEN			(50)
#define GSM_USER_NAME_STR_LEN	(20)
#define GSM_PASSWORD_STR_LEN		(20)
#define GSM_IP_DOMAIN_STR_LEN		(50)
typedef enum
{
	GPRS_COMM_MODE_TCP = 0x00,
	GPRS_COMM_MODE_UDP = 0x01,
}GprsCommModeSt;
typedef struct
{
	GsmTpU8 u8Apn[GSM_APN_STR_LEN+1];
	GsmTpU8 u8UserName[GSM_USER_NAME_STR_LEN+1];
	GsmTpU8 u8PassWord[GSM_PASSWORD_STR_LEN+1];
}GsmLinkParaSt;
typedef struct
{
	GsmTpU8 u8IpDomain[GSM_IP_DOMAIN_STR_LEN+1];
	GsmTpU16 u16CommPort;
	GprsCommModeSt GprsCommMode;
}GsmGprsParaSt;
typedef struct
{
	GsmLinkParaSt GsmLinkPara;
	GsmGprsParaSt GsmGprsPara;
}GsmParaSt;

typedef struct
{
	GsmLinkParaSt GsmLinkPara;
	GsmGprsParaSt GsmGprsPara;
	GsmTpBool GsmLinkParaValidFlag;
	GsmTpBool GsmGprsParaValidFlag;
	GsmTpBool GsmParaValidFlag;
}GsmParaMaintainSt;

GsmTpBool getGsmIMEI(GsmTpU8 * u8Buff, GsmTpU16 u16Len);
GsmTpU8 SetGsmLinkPara(GsmTpU8 * pu8ApnStr, GsmTpU8 * pu8UserNameStr, GsmTpU8 * pu8PassWordStr);
GsmTpU8 GetGsmLinkPara(GsmTpU8 ** pu8ApnStr, GsmTpU8 ** pu8UserNameStr, GsmTpU8 ** pu8PassWordStr);
GsmTpU8 SetGsmGprsPara(GprsCommModeSt GprsCommMode, GsmTpU16 u16Port, GsmTpU8 * pu8IpDomainStr);
GsmTpU8 GetGsmGprsPara(GprsCommModeSt * GprsCommMode, GsmTpU16 * u16Port, GsmTpU8 ** pu8IpDomainStr);
static GsmTpU8 SetGsmPara(GsmParaSt * GsmPara);
static GsmTpU8 GetGsmPara(GsmParaSt * GsmPara);
GsmTpU8 GsmDriverInit(GsmParaSt * GsmPara);
GsmTpU8 GsmDriverInfo(GsmParaSt * GsmPara);

#endif
