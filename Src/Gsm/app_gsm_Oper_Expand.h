#ifndef _APP_GSM_OPER_EXPAND_H_
#define _APP_GSM_OPER_EXPAND_H_


extern GsmAtUseSocketParaSt sstGsmAtUseSocketPara[IP_DAT_SRC_SCK_NUM];


void GsmAudioRecordGsmRxAudioDatHook(GsmTpU8 *pu8Dat, GsmTpU16 u16Len); 
void GsmAudioRecordRxedNewDatBlockInfo(GsmTpU8 u8Idx, GsmTpU16 u16Len); 

void GsmIpDatTxProc(GsmTpU8 u8CycleMs); 
void GsmIpDatInit(void); 
void app_gprs_SocketCloseHandler(void); 
void app_gsm_SktClosedHandler(IpDatSrcEn enIpDatSrcNum); 

 void LbsLocationProc(void); 
GsmTpBool LbsLocationUpdateReq(pFunGsmCellTowerInfoProc pstFunHook); 

#endif

