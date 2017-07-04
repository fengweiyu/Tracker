/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: GSM  数据解析
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#ifndef _APP_GSM_ANALYSE_H_
#define _APP_GSM_ANALYSE_H_


#define MAX_SMS_SIZE   	(256+65)


#define PHONE_STATUS_NPK 	0
#define PHONE_STATUS_HPK 	1
#define PHONE_STATUS_DEL 	2

#define MAX_PHONE_NUM 	20

void app_gsm_AtRetInfoAnalyse(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_ModuleRestart(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_CMGRHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_CMTHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_CMTIHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_CBMHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_SmsListHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_SmsRecvHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
GsmTpBool app_gsm_SmsInfoDecode(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_SignalVal(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_FlowCtlOfflHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_FlowCtlOnlHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void app_gsm_AudioAttriHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_AudioDataHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void GsmRxQFlush(void); 
void app_gsm_NOCARRIERHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_NOANSWERHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_BUSYHandler(GsmTpU8 *data_buff, GsmTpU16 data_len);
void app_gsm_OKHandler(GsmTpU8 *data_buff, GsmTpU16 data_len); 
void GsmNetBreakProc(void); 

#endif

