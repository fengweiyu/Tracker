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
#ifndef _APP_GSM_TASK_H_
#define _APP_GSM_TASK_H_

/********************************************************************
1.需要启用GSM任务
2.对于相同的硬件,不同的项目,只需要
			依据GsmDriverPort.h修改对应的数据接口就可以
3.对于硬件不一致,需要修改
				AT命令层,
				GSM逻辑操作层
				GSM硬件电源控制层
*********************************************************************/
#define GSM_TASK_CYCLE_MS	50


/*******必须小于RAM_SIZE_2048***********/
#define GSM_IP_SEND_MAX_DAT_LEN		 2200 //与最大内存块必须能够满足

typedef struct
{
	GsmTpU8 u8Dat[GSM_IP_SEND_MAX_DAT_LEN]; 
}GsmIpSendBufSt; 


#define GSM_ICCID_STD_LEN	20
extern GsmTpU8 u8IccidStr[GSM_ICCID_STD_LEN];//ICCID 长度20，BCD码10 
void GsmModRstReq(void); 
void GsmGprsRstReq(void); 
GsmTpBool GsmModGsmIsOk(void); 
GsmTpBool GsmGprsIsOk(void); 
void GsmGprsNgSet(void); 

#endif

