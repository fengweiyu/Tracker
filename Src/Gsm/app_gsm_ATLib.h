/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: GSM  通信功能模块
* 文件功能: GSM  AT  指令库
* 模块型号: 华为EM310
* 开发时间: 2011.06.20.
* 软件设计: ZhongChuanFeng.
************************************************************************************************************************
*/
#ifndef _APP_GSM_ATLIB_H_
#define _APP_GSM_ATLIB_H_

#define MAX_AT_CMD_LEN  40

GsmTpU8 ATLib_gsm_IPR(void);
GsmTpU8 ATLib_gsm_AT(void);
GsmTpU8 ATLib_gsm_ATE0(void);
GsmTpU8 ATLib_gsm_CGMR(void);
GsmTpU8 ATLib_gsm_TSIM(void);
GsmTpU8 ATLib_gsm_CFUN(int mode);
GsmTpU8 ATLib_gsm_CPIN(void);
GsmTpU8 ATLib_gsm_CGSMS(int mode);
GsmTpU8 ATLib_gsm_CSQ0(void);
GsmTpU8 ATLib_gsm_CSQ1(void);
GsmTpU8 ATLib_gsm_CREG0(void);
GsmTpU8 ATLib_gsm_CREG1(void);
GsmTpU8 ATLib_gsm_COPS(void);
GsmTpU8 ATLib_gsm_CMEE(void);
GsmTpU8 ATLib_gsm_RING(void);
GsmTpU8 ATLib_gsm_CRC(void);
GsmTpU8 ATLib_gsm_CLIP(void);
GsmTpU8 ATLib_gsm_CMGF(void);
GsmTpU8 ATLib_gsm_CNMI(void);
GsmTpU8 ATLib_gsm_CFGRI(void);
GsmTpU8 ATLib_gsm_CFGRI_TCP(void);
GsmTpU8 ATLib_gsm_IFC(void);
GsmTpU8 ATLib_gsm_CMGL(void);
GsmTpU8 ATLib_gsm_CMGR(GsmTpU16 index);
GsmTpU8 ATLib_gsm_CMGD(GsmTpU8 index);
GsmTpU8 ATLib_gsm_CMGS(GsmTpU8 *data_buff, GsmTpU16 data_len);
GsmTpU8 ATLib_gsm_CSCLK(void);
GsmTpU8 ATLib_gsm_CIPMODE(void);
GsmTpU8 ATLib_gsm_CIPCCFG(void);
GsmTpU8 ATLib_gsm_CIPRXGET(void);
GsmTpU8 ATLib_gsm_CIPRXGETDATA(void);

GsmTpU8 ATLib_gsm_ATD(GsmTpU8 *phone_num);
GsmTpU8 ATLib_gsm_ATH(void);
GsmTpU8 ATLib_gsm_ATA(void);
GsmTpU8 ATLib_gsm_VTS(GsmTpS8 DTMF);
GsmTpU8 ATLib_gsm_MAPATH0(GsmTpU8 chl);
GsmTpU8 ATLib_gsm_MAPATH1(GsmTpU8 chl);
GsmTpU8 ATLib_gsm_MMICG(GsmTpS8 level);
GsmTpU8 ATLib_gsm_MAVOL(GsmTpU8 vol);
GsmTpU8 ATLib_gsm_MOVAL_ForInit(void); 

GsmTpU8 ATLib_gsm_CGREG0(void);
GsmTpU8 ATLib_gsm_CGREG1(void);
GsmTpU8 ATLib_gsm_MPING(Gsmmyevent *event, GsmTpU8 *DNS_str);
GsmTpU8 ATLib_gsm_MIPCALL_EstablishLink(void);
GsmTpU8 ATLib_gsm_MIPCALL_DisconnectLink(void);
GsmTpU8 ATLib_gsm_MIPCALL_InquireLink(void);
GsmTpU8 ATLib_gsm_MIPOPEN_OpenSocket(void);
GsmTpU8 ATLib_gsm_OpenSocket(GsmTpS8 *IpDomain, GsmTpU16 u16Port, 
								IpProTypeEn enIpType, IpDatSrcEn enIpDatSrc); 
GsmTpU8 ATLib_gsm_MIPOPEN_InquireSocket(GsmTpU8 socket_id);
GsmTpU8 ATLib_gsm_MIPCLOSE_CloseSocket(GsmTpU8 socket_id);
GsmTpU8 ATLib_gsm_MIPCLOSE_InquireClose(void);
typedef enum 
{
	GSM_AT_CMD_IP_SEND_ACK_NG,
	GSM_AT_CMD_IP_SEND_ACK_OK,
	GSM_AT_CMD_IP_SEND_ACK_BUSY,
}GsmAtCmdAckEn; 


GsmAtCmdAckEn ATLib_gsm_IPSEND(GsmTpU8 *data_buff, 
										GsmTpU16 data_len, 
										IpDatSrcEn enIpDatSrc); 
GsmAtCmdAckEn AtLib_gsm_MIPPUSH(IpDatSrcEn enIpDatSrc); 
GsmTpU8 app_gsmAT_Mipflush(GsmTpU8 u8SocketId); 
GsmTpU8 ATLib_gsm_CMGD_2(GsmTpU16 Index, GsmTpU8 Mode); 
GsmTpU8 ATLib_gsm_CMGD_All(void); 
GsmTpU8 ATLib_gsm_Wait(void); 

GsmTpU8 ATLib_gsm_AUDCD(GsmTpU8 mode);
GsmTpU8 ATLib_gsm_AUDPM(GsmTpU8 Dsp_format,GsmTpU8 u_bitrate);
GsmTpU8 ATLib_gsm_AUDRL(GsmTpU8 index);
GsmTpU8 ATLib_gsm_AUDRD(GsmTpU8 index,GsmTpU16 offset,GsmTpU16 read_len);
GsmTpU8 ATLib_gsm_VoiceRecordQuery(void); 
GsmTpU8 ATLib_gsm_CCID(void); 
GsmTpU8 ATLib_gsm_TTS(const GsmTpBool cboStartFlg, const GsmTpU8 *pcu8Dat, const GsmTpU16 cu16DatLen); 
GsmTpBool ATLib_gsm_DNSR(GsmTpU8 *pu8DstIpStr/*[IP_V4_STR_BUF_LEN_MIN]*/,
							GsmTpU8 *pu8IpDomain); 

GsmTpU8 ATLib_gsm_CENG(void);
GsmTpU8 ATLib_gsm_CGSN(void);
GsmTpU8 ATLib_gsm_MAVOL2(void);
GsmTpU8 ATLib_gsm_CIPHEAD(void);
GsmTpU8  ATLib_gsm_CgattEnable(void);
GsmTpU8 ATLib_gsm_CGATT(void);
GsmTpU8 ATLib_gsm_CSTT(void);
GsmTpU8 ATLib_gsm_CIICR(void);
GsmTpU8 ATLib_gsm_CIFSR(void);
GsmTpU8 ATLib_gsm_CIPSTART(void);
GsmTpU8 ATLib_gsm_CIPSHUT(void);
GsmTpU8 ATLib_gsm_CIPSTATUS(void);

GsmTpU8 app_waitIMEI(GsmTpU8 *u8Buff, GsmTpU16 u16Len);
GsmTpU8 app_gsmAT_CIMI(void);
GsmTpU8 app_waitIMSI(GsmTpU8 *u8Buff, GsmTpU16 u16Len);

#endif
