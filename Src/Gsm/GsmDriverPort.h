#ifndef _GSM_DRIVER_PORT_H_
#define _GSM_DRIVER_PORT_H_

#include"GsmDriverTypeDef.h"

/************************���±��������������ڽӿ�,�����������************************/
extern GsmIpDatTxBufSt stGsmIpDatTxBuf; 
#define GSM_RX_EVENT_TAB_SIZE  		8
extern Gsmmyevent *pstEveTabForGsmRx[GSM_RX_EVENT_TAB_SIZE]; 
#define GSM_RESUME_TIME				3
/*
*********************************************************************************************************
*                                       ��ȡ�յ���GSMģ������
*
* Description: �ú�����ȡGSMģ������,�����ʱ��Ҫ���Թ���
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
	��GsmDriverGetDevRxDatPend���ʹ��,�����ȡ����������Ҫ�ͷ��ڴ�,
	���ɸú����ͷ�,�������Ҫ�ͷ��ڴ�,��ú���ʵ�ֿռ���

	�ú��������GsmDriverGetDevRxDatPend�����������һ����
	Ϊ����ʹ���˷���
***************************************************************************/
void GsmDriverGetDevRxDatMemFreeIfNeed(GsmTpU8 *pu8DstDat, GsmTpU16 u16DstLen); 

/***************************************************************************************
	GSM�������еĹ���,���Զ��ڵĴ�������,
	����Ӧ�������ܵ�ȡ���ú���
***************************************************************************************/
void GsmDriverGsmTaskIdleHook(void); 

/*�յ�IP���ݺ�Ļص�*/
GsmTpBool GsmRxIpDatHook(const IpDatSt *pcstIpDat); 
/*GSM �绰��ͨʱ�Ļص�����*/
void GsmPhoneChg2TalkHook(void); 
/*GSM�绰������ʾ��ʾ�ص�*/
void GsmPhoneNumRingHook(const GsmTpS8 *pcs8Num, const GsmTpU8 cu8NumLen); 
/*GSM�绰�ҵ�ʱ�ص�״̬*/
void GsmPhoneHangupHook(void); 

/*GSM�յ����Żص�*/
void GsmRxedSmsHook(sms_info_t *pstsms_info, GsmTpS16 pDCS); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM�绰��Ӧ��ص�
��������:	GSM�绰��Ӧ��ص�
����:��
���:��
ע��:����Ŀ������Ҫ��,��Ϊ����ķ���GSMģ�����,����Ϊ��,��ȡ����API
*************************************************************/
void GsmPhoneNoAnswerHook(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM�绰æ�ص�
��������:	GSM�绰æ�ص�
����:��
���:��
ע��:����Ŀ������Ҫ��,��Ϊ����ķ���GSMģ�����,����Ϊ��,��ȡ����API
*************************************************************/
void GsmPhoneBusyHook(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM�绰ժ���ص�
��������:	GSM�绰ժ���ص�
����:��
���:��
ע��:����Ŀ������Ҫ��,��Ϊ����ķ���GSMģ�����,����Ϊ��,��ȡ����API
*************************************************************/
void GsmPhonePickUpHook(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM����ʧ�ܻص�
��������:	GSM����ʧ�ܻص�
����:��
���:��
ע��:����Ŀ������Ҫ��,��Ϊ����ķ���GSMģ�����,����Ϊ��,��ȡ����API
*************************************************************/
void GsmPhoneDailFailHook(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM���ųɹ��ص�
��������:	GSM���ųɹ��ص�
����:��
���:��
ע��:����Ŀ������Ҫ��,��Ϊ����ķ���GSMģ�����,����Ϊ��,��ȡ����API
*************************************************************/
void GsmPhoneDailOkHook(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM��ȡϵͳ��������
��������:	GSM��ȡϵͳ��������
����:��
���:ϵͳ����
ע��:ϵͳ����Ҫ��ģ������Χ��
*************************************************************/
GsmTpU8 GsmGetParaVolume(void); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM�洢ϵͳ��������
��������:	GSM�洢ϵͳ��������
����:ϵͳ����
���:��
ע��:ϵͳ����Ҫ��ģ������Χ��
*************************************************************/
void GsmSaveParaVolume(GsmTpU8 u8Volume); 
/*************************************************************
����:���չ
ʱ��:2015-5-19 13:52:16
������:	GSM�洢ϵͳ��������
��������:	GSM�洢ϵͳ��������
����:ϵͳ����
���:��
ע��:ϵͳ����Ҫ��ģ������Χ��
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
/*�������ݶ�ȡ*/
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
*����ucos iiϵͳ,�������ϵͳ
��Ҫ�޸Ķ�Ӧ�ڲ����ú���
����:��̬��32λ�Ķ�ʱ���������
	����ñ������Զ���ʼ��,
	��Ҫ���г�ʼ��,��Ҫʹ��
		OSTimeGet(); ��ʼ��,�����ϵͳͬ�������Ķ�ʱ��
		���Բ��ó�ʼ��.

���:�Դ��ϴ����о���������
********************************************************/
GsmTpU16 GsmSysGetDurationSecTimer(GsmTpU32 *pu32TimerBk); 
void GsmOperInitHook(void) ; 
void TaskGsmDriver(void *pdata); 
GsmTpS16 GsmPDUDecode(GsmTpU8 *pSCA, GsmTpU8 *pDA, GsmTpU8 *pTime, 
							GsmTpU8 *pUD, GsmTpS16 *pDCS, GsmTpU8 *pPdu, GsmTpS16 iPduLen); 
/*
���ͷ�PDU��ʽ
----------------------------------------------------------------
| SCA  | PDUType | MR | DA   | PID | DCS | VP    | UDL | UD    |
|---------------------------------------------------------------
| 1-12 | 1       | 1  | 2-12 | 1   | 1   | 0,1,7 | 1   | 0-140 |
----------------------------------------------------------------
*/
GsmTpS16 GsmPDUEncode( GsmTpU8  *pPdu, GsmTpU8  *pSCA, GsmTpU8  *pDA, 
						GsmTpU8  *pUD, GsmTpS16 iUDL, GsmTpS16 iDCS ); 

#endif


