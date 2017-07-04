#ifndef _GSM_DRIVER_PORT_H_
#define _GSM_DRIVER_PORT_H_

#include"GsmDriverTypeDef.h"

/************************以下变量声明不是属于接口,属于自行添加************************/
extern GsmIpDatTxBufSt stGsmIpDatTxBuf; 
#define GSM_RX_EVENT_TAB_SIZE  		8
extern Gsmmyevent *pstEveTabForGsmRx[GSM_RX_EVENT_TAB_SIZE]; 
#define GSM_RESUME_TIME				3
/*
*********************************************************************************************************
*                                       获取收到的GSM模块数据
*
* Description: 该函数获取GSM模块数据,无命令到时需要可以挂起
*
* Arguments  : pevent        is a pointer to the event control block associated with the desired mailbox
*
*              pmsg          is a pointer to the message to send.  You MUST NOT send a NULL pointer.
*
*              opt           determines the type of POST performed:
*                            OS_POST_OPT_NONE         POST to a single waiting task
*                                                     (Identical to OSMboxPost())
*                            OS_POST_OPT_BROADCAST    POST to ALL tasks that are waiting on the mailbox
*
*                            OS_POST_OPT_NO_SCHED     Indicates that the scheduler will NOT be invoked
*
* Returns    : OS_ERR_NONE          The call was successful and the message was sent
*              OS_ERR_MBOX_FULL     If the mailbox already contains a message.  You can can only send one
*                                   message at a time and thus, the message MUST be consumed before you
*                                   are allowed to send another one.
*              OS_ERR_EVENT_TYPE    If you are attempting to post to a non mailbox.
*              OS_ERR_PEVENT_NULL   If 'pevent' is a NULL pointer
*              OS_ERR_POST_NULL_PTR If you are attempting to post a NULL pointer
*
* Note(s)    : 1) HPT means Highest Priority Task
*
* Warning    : Interrupts can be disabled for a long time if you do a 'broadcast'.  In fact, the
*              interrupt disable time is proportional to the number of tasks waiting on the mailbox.
*********************************************************************************************************
*/
GsmTpBool GsmDriverGetDevRxDatPend(GsmTpU16 u16PendTimeMs, GsmTpU8 **pu8DstDat, GsmTpU16 *pu16DstLen); 
/**************************************************************************
	GsmDriverGetDevRxDatMemFreeIfNeed
	和GsmDriverGetDevRxDatPend配合使用,如果读取到的数据需要释放内存,
	则由该函数释放,如果不需要释放内存,则该函数实现空即可

	该函数输入和GsmDriverGetDevRxDatPend输出参数名称一致是
	为方便使用人发现
***************************************************************************/
void GsmDriverGetDevRxDatMemFreeIfNeed(GsmTpU8 *pu8DstDat, GsmTpU16 u16DstLen); 

/***************************************************************************************
	GSM主任务中的钩子,可以定期的处理事务,
	后期应当尽可能的取消该函数
***************************************************************************************/
void GsmDriverGsmTaskIdleHook(void); 

/*收到IP数据后的回调*/
GsmTpBool GsmRxIpDatHook(const IpDatSt *pcstIpDat); 
/*GSM 电话接通时的回调函数*/
void GsmPhoneChg2TalkHook(void); 
/*GSM电话来电显示提示回调*/
void GsmPhoneNumRingHook(const GsmTpS8 *pcs8Num, const GsmTpU8 cu8NumLen); 
/*GSM电话挂点时回调状态*/
void GsmPhoneHangupHook(void); 

/*GSM收到短信回到*/
void GsmRxedSmsHook(sms_info_t *pstsms_info, GsmTpS16 pDCS); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM电话无应答回调
函数功能:	GSM电话无应答回调
输入:无
输出:无
注意:新项目尽量不要用,仅为更快的分离GSM模块而做,不用为空,采取其他API
*************************************************************/
void GsmPhoneNoAnswerHook(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM电话忙回调
函数功能:	GSM电话忙回调
输入:无
输出:无
注意:新项目尽量不要用,仅为更快的分离GSM模块而做,不用为空,采取其他API
*************************************************************/
void GsmPhoneBusyHook(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM电话摘机回调
函数功能:	GSM电话摘机回调
输入:无
输出:无
注意:新项目尽量不要用,仅为更快的分离GSM模块而做,不用为空,采取其他API
*************************************************************/
void GsmPhonePickUpHook(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM拨号失败回调
函数功能:	GSM拨号失败回调
输入:无
输出:无
注意:新项目尽量不要用,仅为更快的分离GSM模块而做,不用为空,采取其他API
*************************************************************/
void GsmPhoneDailFailHook(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM拨号成功回调
函数功能:	GSM拨号成功回调
输入:无
输出:无
注意:新项目尽量不要用,仅为更快的分离GSM模块而做,不用为空,采取其他API
*************************************************************/
void GsmPhoneDailOkHook(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM获取系统音量参数
函数功能:	GSM获取系统音量参数
输入:无
输出:系统音量
注意:系统音量要在模块允许范围内
*************************************************************/
GsmTpU8 GsmGetParaVolume(void); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM存储系统音量参数
函数功能:	GSM存储系统音量参数
输入:系统音量
输出:无
注意:系统音量要在模块允许范围内
*************************************************************/
void GsmSaveParaVolume(GsmTpU8 u8Volume); 
/*************************************************************
作者:李宏展
时间:2015-5-19 13:52:16
函数名:	GSM存储系统音量参数
函数功能:	GSM存储系统音量参数
输入:系统音量
输出:无
注意:系统音量要在模块允许范围内
*************************************************************/
GsmTpU8 GsmGetMaxVolumeLevel(void); 
GsmTpU8 GsmGetMinVolumeLevel(void); 
void GsmInitOkHook(void); 
void GprsInitOkHook(void); 
void dev_gsm_DataBaseInit(void);
GsmTpBool GsmModIsNeedOffGsmMod(void); 
void app_gsm_PhoneSpkTimeCheck(void); 
void GsmDriverDevModRst(void); 
void GsmDriverEnterCritical(void); 
void GsmDriverExitCriticical(void); 
void GsmDrivermyfree(unsigned char * buff, unsigned int Size); 
void GsmDriverSleep(GsmTpU32 u32Ms); 
void GsmDebugString(GsmTpS8 *pst8Str); 
void GsmDebugStringLowLevel(GsmTpS8 *pst8Str); 
void GsmDebugPrintf(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
void GsmPhoneNoCarreyHook(void); 
GsmTpU8 * GsmMalloc(GsmTpU16 u16Size); 
void GsmFree(GsmTpU8 *pu8Buf, GsmTpU16 u16Size); 
void GsmMallocErrHook(void); 
void GsmGprsFlowOffHook(void); 
void GsmGprsFlowOnHook(void); 
GsmTpBool GsmGprsDebugPrintfIsLimit(void); 
void GprsEstablishLinkHook(GsmTpBool boOpenIsOkFlg); 
void GsmGprsOpenSocketFailHook(void);
void GsmGetLinkParaApnUserKey(GsmTpU8 **p2u8Apn, GsmTpU8 **p2u8UserName, GsmTpU8 **p2u8Password); 
void GsmGetIpPara(GsmTpU8 *u8UdpTcp, GsmTpU16 *pu16Port, GsmTpU8 **p2u8IpDomainPtr); 
GsmTpBool GsmTtsIsEnable(void); 
GsmTpBool GsmTtsGetPlayData(GsmTpU8 **p2u8Dat, GsmTpU16 *pu16Len); 
void GsmAudioRecordGetDatOkHook(void); 
GsmTpU32 GsmAudioRecordGetAudioSaveBufSize(void); 
GsmTpBool GsmDevRxQWrite(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
GsmTpBool GsmDevTxQWrite(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
/*串口数据读取*/
GsmTpBool GsmDevRxQReadPend(GsmTpU8 **p2u8Dat, GsmTpU16 u16Len); 
GsmTpBool GsmGprsGetFlowCtlLimitIsOffFlg(void); 
GsmTpBool GsmGprsIpDatTxReqIsSetThenClr(void); 
void GsmGprsIpDatTxEndHook(void); 
void GsmGprsIpDatTxFaultHook(void);
void GsmIdleStateHandle(void);
void GsmGprsIpDatTxOkHook(void);
GsmTpBool GsmGetIccid(GsmTpS8 **p2s8Iccid, GsmTpU16 *pu16Len); 
void GsmBspGsmPwrOff(void); 
void GsmBspGsmPwrOn(void); 
void GsmBspAmpPwrOff(void); 
void GsmBspAmpPwrOn(void); 
GsmTpBool GsmBspAudioChannelIsBusy(void);
void GsmBspAudioChannelBusySet(void); 
void GsmBspAudioChannelBusyClr(void); 

GsmTpU8 dev_gsm_SendQueueRead(Gsmmyevent *event); 
/*******************************************************
*依据ucos ii系统,如果更换系统
需要修改对应内部引用函数
输入:静态的32位的定时器缓存变量
	如果该变量会自动初始化,
	如要自行初始化,需要使用
		OSTimeGet(); 初始化,如果和系统同步启动的定时器
		可以不用初始化.

输出:自从上次运行经过的秒数
********************************************************/
GsmTpU16 GsmSysGetDurationSecTimer(GsmTpU32 *pu32TimerBk); 
void GsmOperInitHook(void) ; 
void TaskGsmDriver(void *pdata); 
GsmTpS16 GsmPDUDecode(GsmTpU8 *pSCA, GsmTpU8 *pDA, GsmTpU8 *pTime, 
							GsmTpU8 *pUD, GsmTpS16 *pDCS, GsmTpU8 *pPdu, GsmTpS16 iPduLen); 
/*
发送方PDU格式
----------------------------------------------------------------
| SCA  | PDUType | MR | DA   | PID | DCS | VP    | UDL | UD    |
|---------------------------------------------------------------
| 1-12 | 1       | 1  | 2-12 | 1   | 1   | 0,1,7 | 1   | 0-140 |
----------------------------------------------------------------
*/
GsmTpS16 GsmPDUEncode( GsmTpU8  *pPdu, GsmTpU8  *pSCA, GsmTpU8  *pDA, 
						GsmTpU8  *pUD, GsmTpS16 iUDL, GsmTpS16 iDCS ); 

#endif


