/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM 通信功能模块
* 文件功能: 底层驱动
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#ifndef _DEV_GSM_DATABASE_H_
#define _DEV_GSM_DATABASE_H_
#include"app_gsm_includes.h"
#define GSM_DATA  	0
#define GPRS_DATA 	1

#define MAX_GSM_BUFF_LEN 	(1256)

#pragma pack(1)

typedef struct 
{
	GsmTpU8 data_buff[MAX_GSM_BUFF_LEN];
	GsmTpU16 data_len;
}dev_gsm_t;

#pragma pack()

GsmTpU8 dev_gsm_RecvQueueRead(Gsmmyevent *event);
GsmTpU8 dev_gsm_RecvQueueSize(void);
GsmTpU8 dev_gsm_SendQueueSize(void);
GsmTpBool dev_gsm_SendWrite(const GsmTpU8 *buff, GsmTpU16 len); 

void dev_gsm_UartMonitorCnt(void);
void dev_gsm_DataRecv(GsmTpU8 gsm_data);
void dev_gsm_DataSend(void);

#endif
