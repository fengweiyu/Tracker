#include "app_gsm_includes.h"

GsmAtUseSocketParaSt sstGsmAtUseSocketPara[IP_DAT_SRC_SCK_NUM];

typedef struct
{
	GsmTpU8 u8Idx; 
	GsmTpU16 u16Len; 
}DataBlockInfoSt; 
#define AUDIO_DATA_BLOCK_MAX_NUM	10	//G610最多10块数据区
typedef enum 
{
	GSM_GET_AUDIO_DAT_ST_IDLE, 
	GSM_GET_AUDIO_DAT_ST_GET, 
	GSM_GET_AUDIO_DAT_ST_WAIT_DAT, 
	GSM_GET_AUDIO_DAT_ST_END, 
} GsmGetAudioDatStEn; 
typedef enum{
	AUDIO_IDLE = 0,
	AUDIO_START_PREPARE, 
	AUDIO_START,
	AUDIO_TAKING,
	AUDIO_TAKPFINISH,
	AUDIO_READING,
	AUDIO_READOK,
	AUDIO_NORMAL_STOP,
	AUDIO_UPLOADOK,
	AUDIO_GIVE_UP_STOP, 
}takeAudio_status;

struct GsmAudioRecordDatSt
{
		takeAudio_status status; 
		struct
		{
			GsmTpU8 bRecordReq : 1; 
			GsmTpU8 bRecordBusyFlg : 1;
			GsmTpU8 bRecordStopReq : 1; 
			
		}; 
		struct GsmAudioRecordReqParaSt stGsmAudioRecordReqPara; 
		
		struct {
			GsmTpU32 u32AudioFileOffset;
			GsmTpU32 u32AudioFileSize; 
			struct
			{
				GsmTpU8 u8WrIdx;
				GsmTpU8 u8RdIdx; 
				DataBlockInfoSt stDataBlockInfoBuf[AUDIO_DATA_BLOCK_MAX_NUM]; 
			}; 
			struct{
				GsmTpU8 bRevEndPktFlg : 1; 		//收到最后一包(收到的数据小于要求的数据)
				GsmTpU8 bRevDatBlkDatAck : 1;       //请求特定区块的数据已经收到应答
			}OperFlg; 
			GsmGetAudioDatStEn enGsmGetAudioDatSt; 
	//本次期望读取到的数据长度如果小于该长度表示读取完毕
			GsmTpU16 u16ExpectReadDatLen; 
			GsmTpU16 u16GotDatLen; //获取单个区块数据时,已经获取长度
		}stGetDat;  
}; 
struct GsmAudioRecordDatSt stGsmAudioRecordDat; 
#define	AUDIO_ONCE_READLEN	512

void GsmGetAudioDat(void)
{
	static DataBlockInfoSt sstDatBlkInfo; 
	static GsmTpU32 su32TimerTimeBk; 
	static GsmTpU8 su8GetCnt; 
	GsmTpU16 u16SecS; 
	static GsmTpU16 su16GsmGetAudioDatTimerS; 
	u16SecS = GsmSysGetDurationSecTimer(&su32TimerTimeBk); 
	while(u16SecS--) {
		M_NOT_ZERO_DEC(su16GsmGetAudioDatTimerS);
	}

	switch(stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt) {
		case GSM_GET_AUDIO_DAT_ST_IDLE:
			if(stGsmAudioRecordDat.stGetDat.u8RdIdx != stGsmAudioRecordDat.stGetDat.u8WrIdx) {
				memcpy(&sstDatBlkInfo, 
						&stGsmAudioRecordDat.stGetDat.stDataBlockInfoBuf[stGsmAudioRecordDat.stGetDat.u8RdIdx], 
						sizeof(sstDatBlkInfo)); 
				M_ClrBit(stGsmAudioRecordDat.stGetDat.u16GotDatLen); 
				M_ClrBit(stGsmAudioRecordDat.stGetDat.OperFlg.bRevEndPktFlg); 
				M_ClrBit(su8GetCnt); 
				stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_GET; 
			}
			else {; 
			}
			break; 

		case GSM_GET_AUDIO_DAT_ST_GET:			
			M_ClrBit(stGsmAudioRecordDat.stGetDat.OperFlg.bRevDatBlkDatAck); 
			stGsmAudioRecordDat.stGetDat.u16ExpectReadDatLen = AUDIO_ONCE_READLEN; 
			app_gsm_ReadAudioData(sstDatBlkInfo.u8Idx, 
									stGsmAudioRecordDat.stGetDat.u16GotDatLen, 
									stGsmAudioRecordDat.stGetDat.u16ExpectReadDatLen); 
			su16GsmGetAudioDatTimerS = 5; 
			
			su8GetCnt++; 
			stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_WAIT_DAT; 
			break; 

		case GSM_GET_AUDIO_DAT_ST_WAIT_DAT:
			if(IS_TRUE_BIT(stGsmAudioRecordDat.stGetDat.OperFlg.bRevDatBlkDatAck)
			|| IS_FALSE_BIT(su16GsmGetAudioDatTimerS)) {
				if((stGsmAudioRecordDat.stGetDat.u16GotDatLen >= sstDatBlkInfo.u16Len)
				|| IS_TRUE_BIT(stGsmAudioRecordDat.stGetDat.OperFlg.bRevEndPktFlg)) {
					stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_END; 
				}
				else {
					GsmTpU32 u32BufSize
						= stGsmAudioRecordDat.stGsmAudioRecordReqPara.u32AudioDatSaveMaxSize; 
					if(stGsmAudioRecordDat.stGetDat.u32AudioFileOffset
						< (u32BufSize - stGsmAudioRecordDat.stGetDat.u16ExpectReadDatLen)) {//空间满异常处理
						stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_GET; 
					}
					else {
						stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_END; 
						GsmDebugString("Audio flash full\r\n"); 
					}
				}
			}
			else if(su8GetCnt > 60) {
				GsmDebugString("gsm get audio dat err\r\n"); 
				M_ClrBit(su8GetCnt); 
				stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_END; 
			}
			else {; 
			}
			break; 

		case GSM_GET_AUDIO_DAT_ST_END:
			stGsmAudioRecordDat.stGetDat.u8RdIdx++; 
			if(stGsmAudioRecordDat.stGetDat.u8RdIdx < AUDIO_DATA_BLOCK_MAX_NUM) {; 
			}
			else {//如果溢出则循环覆盖
				M_ClrBit(stGsmAudioRecordDat.stGetDat.u8RdIdx); 
			}
			stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_IDLE; 
			break; 

		default:
			stGsmAudioRecordDat.stGetDat.enGsmGetAudioDatSt = GSM_GET_AUDIO_DAT_ST_IDLE; 
			break; 
	}
}
extern GsmTpU8 app_gsm_TapeStopForOnlyStop(void); 
void GsmTakeAudioProcess(void)
{
	static GsmTpU32 su32TakeAudioDatTimer; //超时处理用等
	static GsmTpU32 su32TakeAudioDatTimeBk;
	GsmTpU16 u16SecS; 
	u16SecS = GsmSysGetDurationSecTimer(&su32TakeAudioDatTimeBk); 
	while(u16SecS--) {
		M_NOT_ZERO_DEC(su32TakeAudioDatTimer); 
	}

	if(IS_FALSE_BIT(stGsmAudioRecordDat.bRecordStopReq)) {; 
	}
	else {
		stGsmAudioRecordDat.status = AUDIO_GIVE_UP_STOP; 
	}
	
	switch(stGsmAudioRecordDat.status)
	{
		case AUDIO_IDLE:
			if(IS_FALSE_BIT(stGsmAudioRecordDat.bRecordReq)) {
			}
			else {
				GsmDriverEnterCritical(); 
				M_ClrBit(stGsmAudioRecordDat.bRecordReq); 
				M_SetBit(stGsmAudioRecordDat.bRecordBusyFlg); 
				memset(&stGsmAudioRecordDat.stGetDat, 
						0, 
						sizeof(stGsmAudioRecordDat.stGetDat)); 
				GsmDriverExitCriticical(); 
				stGsmAudioRecordDat.status = AUDIO_START_PREPARE; 
			}
			break;

		case AUDIO_START_PREPARE:
			app_gsm_TapeStopForOnlyStop(); //stop
			stGsmAudioRecordDat.status = AUDIO_START; 
			break; 

		case AUDIO_START:
			GsmBspAudioChannelBusySet(); 
			app_gsm_TapeCfg(stGsmAudioRecordDat.stGsmAudioRecordReqPara.u8SampleFrequency);
			
			app_gsm_TapeStart();
			su32TakeAudioDatTimer = stGsmAudioRecordDat.stGsmAudioRecordReqPara.u16TimeS; 
			if(IS_NULL_P(stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunHookForAudioStart)) {
			}
			else {
				stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunHookForAudioStart(); 
			}
			stGsmAudioRecordDat.status = AUDIO_TAKING; 
			break;
			
		case AUDIO_TAKING:
			if(IS_FALSE_BIT(su32TakeAudioDatTimer))	//录音计时
			{
				app_gsm_TapeStop();
				GsmBspAudioChannelBusyClr(); 
				stGsmAudioRecordDat.status = AUDIO_TAKPFINISH; 
			}
			GsmGetAudioDat(); 
			break;
			
		case AUDIO_TAKPFINISH:
			stGsmAudioRecordDat.status = AUDIO_READING; 
			//超时处理,连续2分钟不能获取数据当做失败处理
			su32TakeAudioDatTimer = 120; 
			GsmGetAudioDat(); 
			break;
			
		case AUDIO_READING:
			GsmGetAudioDat(); 
			if(stGsmAudioRecordDat.stGetDat.u8RdIdx
			== stGsmAudioRecordDat.stGetDat.u8WrIdx) {
				stGsmAudioRecordDat.status = AUDIO_READOK; 
				GsmDebugString("Audio dat get ok\r\n"); 
			}
			else if(IS_TRUE_BIT(su32TakeAudioDatTimer)) {; 
			}
			else {//超时处理,连续2分钟不能获取数据当做失败处理
				GsmDriverEnterCritical(); 
				M_ClrBit(stGsmAudioRecordDat.bRecordBusyFlg); 
				GsmDriverExitCriticical(); 
				GsmDebugString("Audio dat get timeout\r\n");
				stGsmAudioRecordDat.status = AUDIO_GIVE_UP_STOP;
			}
			break;

		case AUDIO_READOK:
			if(IS_NULL_P(stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunGetDatOkHook)) {; 
			}
			else {			//GsmAudioRecordGetDatOkHook(); 
				GsmTpU32 u32Tmp = stGsmAudioRecordDat.stGetDat.u32AudioFileSize; 
				stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunGetDatOkHook(u32Tmp); 
			}
			stGsmAudioRecordDat.status = AUDIO_NORMAL_STOP;
			break;
			
		case AUDIO_NORMAL_STOP:
			GsmDriverEnterCritical(); 
			M_ClrBit(stGsmAudioRecordDat.bRecordBusyFlg); 
			M_ClrBit(stGsmAudioRecordDat.bRecordReq); 
			GsmDriverExitCriticical(); 
			stGsmAudioRecordDat.status = AUDIO_IDLE; 
			break;
			
		case AUDIO_GIVE_UP_STOP:
			app_gsm_TapeStopForOnlyStop(); 
			GsmDriverEnterCritical(); 
			M_ClrBit(stGsmAudioRecordDat.bRecordBusyFlg); 
			M_ClrBit(stGsmAudioRecordDat.bRecordReq); 
			GsmDriverExitCriticical(); 
			stGsmAudioRecordDat.status = AUDIO_IDLE; 
			break; 
			
		default :
			stGsmAudioRecordDat.status = AUDIO_GIVE_UP_STOP; 
			break;
	}
}
void GsmAudioRecordGsmRxAudioDatHook(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{
/*针对G610新软件返回的文件信息无AMR头问题处理*/	
	if(IS_TRUE_BIT(stGsmAudioRecordDat.stGetDat.u32AudioFileOffset)) {; 
	}
	else {
		const GsmTpS8 cs8FileAmrHeaderStr[] = "#!AMR\n"; 
		GsmTpU8 u8Len; 
		u8Len = sizeof(cs8FileAmrHeaderStr) - 1; 
		if(GsmMemIsEqu(pu8Dat, (GsmTpU8 *)cs8FileAmrHeaderStr, u8Len)) {; 
		}
		else {
			if(IS_NULL_P(stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunHookForAudioDatSave)) {; 
			}
			else {
				stGsmAudioRecordDat\
					.stGsmAudioRecordReqPara\
						.pFunHookForAudioDatSave((GsmTpU8 *)cs8FileAmrHeaderStr, 
												u8Len, 
												stGsmAudioRecordDat.stGetDat.u32AudioFileOffset); 
			}
			stGsmAudioRecordDat.stGetDat.u32AudioFileOffset += u8Len;
			stGsmAudioRecordDat.stGetDat.u32AudioFileSize += u8Len; 
			GsmDebugString("Amr header add\r\n"); 
		}

	}	
/*********************************************************************/
	stGsmAudioRecordDat.stGetDat.u16GotDatLen += u16Len; 
	if(u16Len < stGsmAudioRecordDat.stGetDat.u16ExpectReadDatLen) {
		M_SetBit(stGsmAudioRecordDat.stGetDat.OperFlg.bRevEndPktFlg); 
	}
	else {
		M_ClrBit(stGsmAudioRecordDat.stGetDat.OperFlg.bRevEndPktFlg); 
	}

	M_SetBit(stGsmAudioRecordDat.stGetDat.OperFlg.bRevDatBlkDatAck); 
	
	if(IS_NULL_P(stGsmAudioRecordDat.stGsmAudioRecordReqPara.pFunHookForAudioDatSave)) {; 
	}
	else {
		stGsmAudioRecordDat\
			.stGsmAudioRecordReqPara\
				.pFunHookForAudioDatSave(pu8Dat,
										u16Len, 
										stGsmAudioRecordDat.stGetDat.u32AudioFileOffset); 
		stGsmAudioRecordDat.stGetDat.u32AudioFileOffset += u16Len;
		stGsmAudioRecordDat.stGetDat.u32AudioFileSize += u16Len; 
	}		
}

GsmTpBool GsmAudioRecordReq(GsmTpBool boIsStartFlg, 
						struct GsmAudioRecordReqParaSt stPara)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	GsmDriverEnterCritical(); 
	if(IS_TRUE_BIT(boIsStartFlg)) {
		if(IS_TRUE_BIT(stGsmAudioRecordDat.bRecordReq)
		|| IS_TRUE_BIT(stGsmAudioRecordDat.bRecordBusyFlg)) {
			M_ClrBit(boRetVal); 
		}
		else {
			M_SetBit(stGsmAudioRecordDat.bRecordReq); 
			memcpy(&stGsmAudioRecordDat.stGsmAudioRecordReqPara, 
					&stPara, 
					sizeof(struct GsmAudioRecordReqParaSt)); 
			M_SetBit(boRetVal); 
		}
		
	}
	else {
		M_SetBit(stGsmAudioRecordDat.bRecordStopReq); 
		M_SetBit(boRetVal); 
	}
	GsmDriverExitCriticical(); 

	return (boRetVal); 
}

void GsmAudioRecordRxedNewDatBlockInfo(GsmTpU8 u8Idx, GsmTpU16 u16Len)
{
	stGsmAudioRecordDat.stGetDat.stDataBlockInfoBuf[stGsmAudioRecordDat.stGetDat.u8WrIdx].u8Idx = u8Idx; 
	stGsmAudioRecordDat.stGetDat.stDataBlockInfoBuf[stGsmAudioRecordDat.stGetDat.u8WrIdx].u16Len = u16Len; 
	stGsmAudioRecordDat.stGetDat.u8WrIdx++; 
	if(stGsmAudioRecordDat.stGetDat.u8WrIdx < AUDIO_DATA_BLOCK_MAX_NUM) {; 
	}
	else {//如果溢出则循环覆盖
		M_ClrBit(stGsmAudioRecordDat.stGetDat.u8WrIdx); 
		if(IS_TRUE_BIT(stGsmAudioRecordDat.stGetDat.u8RdIdx)) {; 
		}
		else {
			stGsmAudioRecordDat.stGetDat.u8RdIdx++; 
		}
	}
}

typedef enum 
{
	GSM_IP_DAT_TX_ST_IDLE,
	GSM_IP_DAT_TX_ST_TX_TO_IP_BUF,
	GSM_IP_DAT_TX_ST_TX_TO_NET,
	GSM_IP_DAT_TX_ST_END,

	GSM_IP_DAT_TX_FOR_FAULT,
}GsmIpDatTxStEn; 

GsmIpDatTxStEn senGsmIpDatTxSt; 
#define GSM_IP_MAX_FLOW_TIME_S		60//最长的Socket流控关闭时间
#define GSM_IP_RE_SEND_INTERVAL_S		3
#define GSM_IP_RE_PUSH_INTERVAL_S		3
#define GSM_IP_SEND_MAX_CNT			7
#define GSM_IP_PUSH_MAX_CNT			20
#define GSM_IP_DAT_TX_ERR_MAX_CNT	3
void GsmIpDatTxProc(GsmTpU8 u8CycleMs)
{
	GsmAtCmdAckEn enGsmAtCmdAck;
	static GsmTpU8 su8GsmFlowOffTimer; 
	static GsmTpU8 su8IpTxFailCnt; 
	static GsmTpU32 su32IpDatTxTimerBk; 
	GsmTpU16 u16TimerS; 
	static GsmTpU8 su8SendPushTimer; 
	static GsmTpU8 su8SendPushCnt; 

	u16TimerS = GsmSysGetDurationSecTimer(&su32IpDatTxTimerBk); 
	while(IS_TRUE_BIT(u16TimerS))
	{
		u16TimerS--;  
		
		M_NOT_ZERO_DEC(su8GsmFlowOffTimer); 
		M_NOT_ZERO_DEC(su8SendPushTimer); 
		
	}

	if(IS_FALSE_BIT(GsmPhoneIsIdle())) {
		return; 
	}
	else	{
		if(GsmGprsIsOk() 
				&& (IP_DAT_SRC_SCK_1 == stGsmIpDatTxBuf.enIpDatSrcId)) {; 
		}
		else {
			return; 
		}
	}
	if(IS_TRUE_BIT(GsmGprsGetFlowCtlLimitIsOffFlg())) {
		su8GsmFlowOffTimer = GSM_IP_MAX_FLOW_TIME_S; 
	}
	else	{
		if(IS_TRUE_BIT(su8GsmFlowOffTimer)) {		
			return; 
		}
		else	{//强制解除,防止漏掉信息造成一直无法发送数据
			GsmDebugString("Err:manual flow on\n"); 
			app_gsm_FlowCtlOnlHandler(NULL, 0); 

			su8GsmFlowOffTimer = GSM_IP_MAX_FLOW_TIME_S; 
		}
	}

	switch(senGsmIpDatTxSt) {
		case GSM_IP_DAT_TX_ST_IDLE:
			if(IS_FALSE_BIT(GsmGprsIpDatTxReqIsSetThenClr())) {; 
			}
			else	{
				M_ClrBit(su8SendPushTimer); 
				M_ClrBit(su8SendPushCnt); 
				senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_TX_TO_IP_BUF; 
			}
			break; 

		case GSM_IP_DAT_TX_ST_TX_TO_IP_BUF:
			if(IS_TRUE_BIT(su8SendPushTimer)) {//遇到忙3秒Send一次
				;
			}
			else	{				
				enGsmAtCmdAck = ATLib_gsm_IPSEND(stGsmIpDatTxBuf.pu8Dat, 
													stGsmIpDatTxBuf.u16Len, 
													stGsmIpDatTxBuf.enIpDatSrcId); 
				su8SendPushCnt++; 
				su8SendPushTimer = GSM_IP_RE_SEND_INTERVAL_S; 
				if(GSM_AT_CMD_IP_SEND_ACK_BUSY == enGsmAtCmdAck)
				{ 
					if(su8SendPushCnt < GSM_IP_SEND_MAX_CNT) {
						; 
					}
					else	{
						M_ClrBit(su8SendPushCnt); 
						M_ClrBit(su8SendPushTimer); 
						su8IpTxFailCnt++; 
						if(su8IpTxFailCnt < GSM_IP_DAT_TX_ERR_MAX_CNT){//3连续3次任务Socket  断掉
							; 
						}
						else	{
							M_ClrBit(su8IpTxFailCnt); 
							app_gsm_SktClosedHandler(stGsmIpDatTxBuf.enIpDatSrcId); 
						}
						senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END; //give up//牵涉内存释放						
					}
				}
				else if(GSM_AT_CMD_IP_SEND_ACK_OK == enGsmAtCmdAck) {
					M_ClrBit(su8SendPushCnt); 
					M_ClrBit(su8SendPushTimer); 					
					GsmGprsIpDatTxOkHook();
					senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END;
				}
				else if(GSM_AT_CMD_IP_SEND_ACK_NG == enGsmAtCmdAck) {
					M_ClrBit(su8IpTxFailCnt); 
//					app_gsm_SktClosedHandler(stGsmIpDatTxBuf.enIpDatSrcId); 
					senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END; //give up//牵涉内存释放
				}
				else	{
					senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END; //give up//牵涉内存释放
				}
			}
			break; 

		case GSM_IP_DAT_TX_ST_TX_TO_NET:
			if(IS_TRUE_BIT(su8SendPushTimer)) {//遇到忙秒PUSH一次
				; 
			}
			else	{
				enGsmAtCmdAck = AtLib_gsm_MIPPUSH(stGsmIpDatTxBuf.enIpDatSrcId); 
				su8SendPushCnt++; 
				su8SendPushTimer = GSM_IP_RE_PUSH_INTERVAL_S; 
				if(GSM_AT_CMD_IP_SEND_ACK_BUSY == enGsmAtCmdAck)
				{
					if(su8SendPushCnt < GSM_IP_PUSH_MAX_CNT) {
						; 
					}
					else	{
						su8IpTxFailCnt++; 
						if(su8IpTxFailCnt < GSM_IP_DAT_TX_ERR_MAX_CNT){//3连续3次任务Socket  断掉
							; 
						}
						else	{
							M_ClrBit(su8IpTxFailCnt); 
							app_gsm_SktClosedHandler(stGsmIpDatTxBuf.enIpDatSrcId); 
						}
						senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END; //give up//牵涉内存释放
					}
				}
				else if(GSM_AT_CMD_IP_SEND_ACK_OK == enGsmAtCmdAck) {
					M_ClrBit(su8SendPushCnt); 
					M_ClrBit(su8SendPushTimer); 
					M_ClrBit(su8IpTxFailCnt); 
					senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END;
				}
				else	{
					M_ClrBit(su8SendPushCnt); 
					M_ClrBit(su8SendPushTimer); 
					senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_END;
				}
			}
			break; 

		case GSM_IP_DAT_TX_ST_END:
//			app_gsmAT_Mipflush(1); 
			
			GsmDriverEnterCritical(); 
			if(NULL == stGsmIpDatTxBuf.pu8Dat) {; 
			}
			else	{
				GsmFree(stGsmIpDatTxBuf.pu8Dat, stGsmIpDatTxBuf.u16Len); 
				stGsmIpDatTxBuf.pu8Dat = NULL; 
			}
			GsmDriverExitCriticical(); 
			
			GsmGprsIpDatTxEndHook(); 
			senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_IDLE;
			break; 

		case GSM_IP_DAT_TX_FOR_FAULT:
		default:
			GsmDriverEnterCritical(); 
			if(NULL == stGsmIpDatTxBuf.pu8Dat) {
				; 
			}
			else	{
				GsmFree(stGsmIpDatTxBuf.pu8Dat, stGsmIpDatTxBuf.u16Len); 
				stGsmIpDatTxBuf.pu8Dat = NULL; 
			}
			GsmDriverExitCriticical(); 
			GsmGprsIpDatTxFaultHook(); 
			senGsmIpDatTxSt = GSM_IP_DAT_TX_ST_IDLE;
			break; 
	}
}

void GsmIpDatInit(void)
{
	GsmDriverEnterCritical(); 
	senGsmIpDatTxSt = GSM_IP_DAT_TX_FOR_FAULT; 
	GsmDriverExitCriticical(); 
}

void app_gprs_SocketCloseHandler(void)
{
	static GsmTpU8 Ipclose_failcnt=0;
	GsmTpU8 disconnectLink;
	
	if((IS_FALSE_BIT(GsmPhoneIsIdle()))
	||(gsm_oper_info.flgs.Ipclose == GsmTpFALSE))
	{
		return;
	}

	disconnectLink = GsmTpTRUE;
	if(ATLib_gsm_MIPOPEN_InquireSocket(IP_DAT_SRC_SCK_1) == GsmTpTRUE)
	{
		if(ATLib_gsm_MIPCLOSE_CloseSocket(IP_DAT_SRC_SCK_1))
		{
			Ipclose_failcnt = 0;
		}
		else
		{
			Ipclose_failcnt ++;
			disconnectLink = GsmTpFALSE;
		}
	}
	if(disconnectLink == GsmTpTRUE)
	{
		if(ATLib_gsm_MIPCALL_InquireLink() == GsmTpTRUE)
		{
			if(ATLib_gsm_MIPCALL_DisconnectLink())
			{
				Ipclose_failcnt = 0;
			}
			else
			{
				Ipclose_failcnt ++;
			}
		}
		else
		{
			Ipclose_failcnt = 0;
		}
	}
	
	if(Ipclose_failcnt > 2)
	{
		GsmDebugString("Ipclose fail >= 3, restart gsm module!\n");
		Ipclose_failcnt = 0;
		GsmModRstReq();
	}
	if(Ipclose_failcnt == 0)
	{
		gsm_oper_info.flgs.Ipclose = GsmTpFALSE;
	}
}



void app_gsm_SktClosedHandler(IpDatSrcEn enIpDatSrcNum)
{
	if(enIpDatSrcNum == IP_DAT_SRC_SCK_2) {
		GsmDriverEnterCritical(); 
		//M_ClrBit(gsm_link2_oper_info.OperFlg.gprs_right); 
		GsmDriverExitCriticical(); 
	}
	else {
		GsmNetBreakProc(); 
		ATLib_gsm_MIPCLOSE_CloseSocket(IP_DAT_SRC_SCK_1);  
		ATLib_gsm_MIPCALL_DisconnectLink(); 
	}
}


static struct{
	GsmTpBool boIsLbsReqFlag; 
	pFunGsmCellTowerInfoProc pFunHook; 
	
}sstGsmLbsData; 

GsmTpBool LbsLocationUpdateReq(pFunGsmCellTowerInfoProc pstFunHook)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	GsmDriverEnterCritical(); 
	if(IS_FALSE_BIT(sstGsmLbsData.boIsLbsReqFlag)) {
		sstGsmLbsData.pFunHook = pstFunHook; 
		M_SetBit(sstGsmLbsData.boIsLbsReqFlag);
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	GsmDriverExitCriticical(); 

	return boRetVal; 
}

static GsmTpBool boGetLbsLocationReq()
{
	return sstGsmLbsData.boIsLbsReqFlag; 
}
void LbsLocationReqClr()
{
	GsmDriverEnterCritical(); 
	M_ClrBit(sstGsmLbsData.boIsLbsReqFlag);
	GsmDriverExitCriticical(); 
}

void LbsLocationProc(void)
{
	GsmTpBool boLbsReqFlg; 
	GsmDriverEnterCritical(); 
	boLbsReqFlg = boGetLbsLocationReq(); 
	if(IS_FALSE_BIT(boLbsReqFlg)) {;
	}
	else {
		LbsLocationReqClr(); 
	}
	GsmDriverExitCriticical(); 
	
	if(IS_FALSE_BIT(boLbsReqFlg)) {
	}
	else {
		GsmCellTowerInfoReqSt stGsmCellTowerInfoReq;

		stGsmCellTowerInfoReq.GsmCellTowerInfoProcFunc = sstGsmLbsData.pFunHook;
		GsmCellTowerInfoReq(&stGsmCellTowerInfoReq);
	}
	
}

