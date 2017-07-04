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
#include "app_gsm_Includes.h"
#include "main.h"
#include "Config.h"

static dev_gsm_t Gsm_Recv;
static u8 g_GsmRecvNetDataBuf[1200]={0};//不使用手动获取网络数据可以去掉
static u8 *g_strManRecvNetDataStr="+CIPRXGET: 2,";


static void ManGetGsmNetDataHandler(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *i_pucRxingDataFlag);
/*
--------------------------------------------------------------------------------------------
- 函数功能:	
- 参数说明:	
--------------------------------------------------------------------------------------------
*/



/*
--------------------------------------------------------------------------------------------
- 函数功能:	
- 参数说明:	
--------------------------------------------------------------------------------------------
*/
GsmTpBool dev_gsm_SendWrite(const GsmTpU8 *buff, GsmTpU16 len)
{
	return (GsmDevTxQWrite((GsmTpU8 *)buff, len)); 
}


/*
--------------------------------------------------------------------------------------------
- 函数功能:	
- 参数说明:	
--------------------------------------------------------------------------------------------
*/

static void gsm_buffclear(void)
{
	memset(Gsm_Recv.data_buff, 0, MAX_GSM_BUFF_LEN);
	Gsm_Recv.data_len = 0;
}

#define IPDATA_NULL 	0
#define IPDATA_UDP 		1
#define IPDATA_TCP 		2

extern const GsmTpU8 cu8GsmRxIpDatTcpStrLen; 

/*******************************************************************************
	将收到的网络数据AT命令+IPD,<data lenth>:中的data lenth修改为一个
	更小的长度
		因内存资源限制,将粘包的大数据包分解为小一点的数据包
输入:要修改的长度,数据存放地址,数据总长度,数据BUF空间(防止越界)
返回新数据长度(因为数据长度可能变短,例如:123变为12)
********************************************************************************/

GsmTpU16 Sim800IpdDataLenSet2NewValue(GsmTpU16 u16Len, GsmTpU8*pu8Dat, 
											GsmTpU16 u16DatLen, GsmTpU16 u16BufMaxLen) 
{
	GsmTpBool boRetVal; 
	GsmTpS8 * ps8CommaAddr;
	GsmTpS8 * ps8ColonAddr;
	GsmTpS8 * ps8DataLenAddr;
	GsmTpS8 * ps8DataStartAddr;
	GsmTpU8 * pu8RecvData;
	GsmTpU8 u8DatLenStrLen; 
	GsmTpS8 s8BufTmp[6]; 
	GsmTpS16 s16LenChg; 
	const GsmTpS32 s32BufMaxLen = (GsmTpS32)u16BufMaxLen; 
	const GsmTpS16 s16RecvPackageLen = (GsmTpS16)u16DatLen;
	pu8RecvData = pu8Dat;

	M_SetBit(boRetVal);

	ps8CommaAddr = strchr((char const *)pu8RecvData, ',');
	ps8ColonAddr = strchr((char const *)pu8RecvData, ':');
	if( IS_NULL_P(ps8CommaAddr) || IS_NULL_P(ps8ColonAddr) || (ps8CommaAddr>ps8ColonAddr)) {
		M_ClrBit(boRetVal);
	}
	else {
		ps8DataLenAddr = ps8CommaAddr + 1;
		ps8DataStartAddr = ps8ColonAddr + 1;
		M_SetBit(boRetVal);
	}

	if( IS_FALSE_BIT(boRetVal) || ((ps8DataStartAddr-ps8DataLenAddr)<2) ) {
		;
	}
	else {
		GsmTpU8 u8StrLen; 
		GsmTpS16 s16i; 
		u8DatLenStrLen = ps8ColonAddr - ps8DataLenAddr; 
		memset(s8BufTmp, 0, sizeof(s8BufTmp));
		u8StrLen = sprintf(s8BufTmp, "%d", u16Len); 
		s16LenChg = u8StrLen - u8DatLenStrLen; 
		if((s16LenChg + s16RecvPackageLen) > s32BufMaxLen) {
			GsmDebugString("ERR:buff limit\r\n");
			M_ClrBit(s16LenChg); 
		}
		else {			
			if(IS_FALSE_BIT(s16LenChg)) {;
			}
			else {
				for(s16i = (GsmTpU16)((GsmTpU8 *)ps8ColonAddr - pu8RecvData); s16i < s16RecvPackageLen; s16i++) {
					pu8RecvData[s16i + s16LenChg] = pu8RecvData[s16i]; 
				}
			}
			memcpy(ps8DataLenAddr, s8BufTmp, u8StrLen);
		}
	}	

	return (GsmTpU16)(s16RecvPackageLen + s16LenChg); 
}


void dev_gsm_DataRecv(GsmTpU8 gsm_data)
{
	static GsmTpU8 Ipdata_flag = GsmTpFALSE;
	Gsmmyevent event;

	static GsmTpU8 u8GprsRecStep; 
	static GsmTpU16 uGprsDatalen; 
	static GsmTpU16 uRevdataLen; 
	GsmTpU8 ucRxingNetDataFlag=FALSE;
	GsmTpBool bGprsRecFinish; 
	GsmTpBool bGprsRecvIsBusy; 
	M_ClrBit(bGprsRecFinish); 
	M_ClrBit(bGprsRecvIsBusy); 
	
	if((0 == Gsm_Recv.data_len) && (0 == gsm_data)) {
		return;
	}
	if(('\n' == gsm_data) && (Gsm_Recv.data_len < 2)) {
		
		gsm_buffclear();
		return;
	}

	Gsm_Recv.data_buff[Gsm_Recv.data_len++] = gsm_data;

	switch(u8GprsRecStep)
	{
		case 0:	//scan for start
			if(IS_TRUE_BIT(Ipdata_flag)) {; 
			}
			else {
				if(cu8GsmRxIpDatTcpStrLen != Gsm_Recv.data_len) {;
				}
				else {
		            		if(GsmMemIsEqu(Gsm_Recv.data_buff, 
								(GsmTpU8*)cs8GsmRxIpDatTcpStr, 
								cu8GsmRxIpDatTcpStrLen)) {
						M_SetBit(Ipdata_flag);
						u8GprsRecStep++;
			            	}
					else {; 
					}
				}
			}
			break;
			
		case 1:	//skip ','
			if(gsm_data == ',')
			{
				uGprsDatalen = 0;
				u8GprsRecStep++;
			}
			break;
			
		case 2:	//gprs data packet size
			if(gsm_data == ':')
			{
				uRevdataLen = 0;
				u8GprsRecStep++;
			}
			else
			{
				uGprsDatalen *= 10;
				uGprsDatalen += (gsm_data - '0');
			}
			break;
			
		case 3:	//receive data , condition by size
			if(++uRevdataLen >=uGprsDatalen)
			{
				M_SetBit(bGprsRecFinish); 
				M_ClrBit(Ipdata_flag); 
				u8GprsRecStep = 0;
			}
			else {
				M_SetBit(bGprsRecvIsBusy); 
#if 0//跨层,所以不到没有办法不能采用此方法
//(因大动态内存较少,长指令缓冲困难)
				if(Gsm_Recv.data_len < 25) {;
				}
				else {
					const GsmTpU8 PRO_MARK_BYTE = 0x7E; 
					GsmTpU16 u16Len; 
					if(PRO_MARK_BYTE != Gsm_Recv.data_buff[Gsm_Recv.data_len - 1]) {;
					}
					else	{
						Gsm_Recv.data_len = Sim800IpdDataLenSet2NewValue(uRevdataLen, 
																		Gsm_Recv.data_buff, 
																		Gsm_Recv.data_len, 
																		sizeof(Gsm_Recv.data_buff)); 
						uGprsDatalen -= uRevdataLen; 
						M_ClrBit(uRevdataLen); 
						if(IS_TRUE_BIT(GsmDevRxQWrite(Gsm_Recv.data_buff,
														Gsm_Recv.data_len))) {

						}
						else {
							GsmDebugString("Gsm rx write err0\r\n"); 
						}
						u16Len = (GsmTpU16)(strchr((GsmTpS8 *)Gsm_Recv.data_buff, ':') - (GsmTpS8 *)Gsm_Recv.data_buff + 1); 
						Gsm_Recv.data_len = Sim800IpdDataLenSet2NewValue(uGprsDatalen, 
																			Gsm_Recv.data_buff, 
																			u16Len, 
																			sizeof(Gsm_Recv.data_buff)); 
					}
				}
#endif			
			}
			break;
			
		default:
			M_ClrBit(u8GprsRecStep); 
			M_ClrBit(Ipdata_flag); 
			break;
	}
	
	if(('>' == gsm_data) && (1 == Gsm_Recv.data_len)) {
		Gsm_Recv.data_buff[ Gsm_Recv.data_len + 1] = 0;
		event.size = Gsm_Recv.data_len; 
		event.buff = Gsm_Recv.data_buff;
		if(IS_TRUE_BIT(GsmDevRxQWrite(event.buff, event.size))) {;
		}
		else {
			GsmDebugString("Gsm rx write err\r\n"); 
		}
		
		Ipdata_flag = GsmTpFALSE;
		M_ClrBit(u8GprsRecStep); 
		gsm_buffclear();
	}
	else if(Gsm_Recv.data_len < 2) {; 
	}
	else {
		if(((('\n' == gsm_data) && ('\r' == Gsm_Recv.data_buff[Gsm_Recv.data_len - 2]))
			&& IS_FALSE_BIT(bGprsRecvIsBusy))
		|| IS_TRUE_BIT(bGprsRecFinish)){			
			event.size = Gsm_Recv.data_len; 
			event.buff = Gsm_Recv.data_buff;
			if(IS_TRUE_BIT(GsmDevRxQWrite(event.buff, event.size))) {;
			}
			else {
				GsmDebugString("Gsm rx write err\r\n"); 
			}
			//ManGetGsmNetDataHandler(Gsm_Recv.data_buff,Gsm_Recv.data_len,&ucRxingNetDataFlag);
			Ipdata_flag = GsmTpFALSE;
			M_ClrBit(u8GprsRecStep); 
			//if(IS_FALSE_BIT(ucRxingNetDataFlag))
			{
				gsm_buffclear();
			}
			//else
			{
			}
		}
		else if(Gsm_Recv.data_len > (MAX_GSM_BUFF_LEN -1))
		{
			GsmDebugString("gsm data recv ov!\n");
			Ipdata_flag = GsmTpFALSE;
			M_ClrBit(u8GprsRecStep); 
			gsm_buffclear();
		}
		else {; 
		}
	}

}


/*
--------------------------------------------------------------------------------------------
- 函数功能:	
- 参数说明:	
--------------------------------------------------------------------------------------------
*/


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/


/*****************************************************************************
-Fuction		: ManGetGsmNetDataHandler
-Description	: ManGetGsmNetDataHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2017/04/14	  V1.0.0		 Yu Weifeng 	  Created
******************************************************************************/
static void ManGetGsmNetDataHandler(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *i_pucRxingDataFlag)
{
	static u8 s_ucRecvNetDataState=0;
	static u16 s_wNetDataLen=0;
	static u16 s_wRecvNetDataCmdLen=0;
	static u16 s_wRemainDataLen=0;
	static u8 s_ucRemainNetDataFlag=FALSE;
	static u16 s_wRecvNetDataLen=0;
	static u8 s_IpdHeadBuf[11]={0};//最后需要放\0
	switch(s_ucRecvNetDataState)
	{//+CIPRXGET: 2,35,0\r\n网络数组小于10一下长度忽略
		case 0:
		{
			if(i_wDataLen<strlen(g_strManRecvNetDataStr)+6)
			{
			}
			else
			{
				if(0!=memcmp(i_pucDataBuf,g_strManRecvNetDataStr,strlen(g_strManRecvNetDataStr)))
				{
				}
				else
				{
					memset(g_GsmRecvNetDataBuf,0,sizeof(g_GsmRecvNetDataBuf));
					memcpy(g_GsmRecvNetDataBuf,i_pucDataBuf,i_wDataLen);
					s_wNetDataLen=atoi(&g_GsmRecvNetDataBuf[strlen(g_strManRecvNetDataStr)]);
					if(0==s_wNetDataLen)
					{
					}
					else
					{
						s_wRecvNetDataCmdLen=i_wDataLen;
						s_wRemainDataLen=atoi(&g_GsmRecvNetDataBuf[i_wDataLen-3]);//最后剩余网络数据判断
						if(0!=s_wRemainDataLen)
						{
							DebugPrintf(ERR"RemainGsmNetDataLen:%d\r\n",s_wRemainDataLen);
							M_SetBit(s_ucRemainNetDataFlag);
						}
						else
						{
						}
						s_ucRecvNetDataState++;
					}
				}
			}	
			if(IS_TRUE_BIT(GsmDevRxQWrite(i_pucDataBuf, i_wDataLen))) {;
			}
			else {
				GsmDebugString("Gsm rx write err2\r\n"); 
			}
			break;
		}
		case 1:
		{
			//s_wRecvNetDataLen+=i_wDataLen;
			if(i_wDataLen<s_wNetDataLen)//后续考虑获取的网络数据中里面有两条命令//通过OK判断结束以及长度校验
			{//如两个\R\N的情况,由于升级数据可能有一样的,所以一条AT命令带的数据有多个命令就无法处理
				//DebugPrintf("RecvManNetData,RecvLen:%d,NetLen:%d\r\n",s_wNetDataLen,i_wDataLen);//只能过一会再发数据				
				M_SetBit(*i_pucRxingDataFlag);
			}
			else
			{
				memcpy(&g_GsmRecvNetDataBuf[s_wRecvNetDataCmdLen],i_pucDataBuf,i_wDataLen);
				memset(s_IpdHeadBuf,0,sizeof(s_IpdHeadBuf));				
				s_wNetDataLen=snprintf(s_IpdHeadBuf,sizeof(s_IpdHeadBuf),"+IPD,%d:",i_wDataLen);
				memcpy(&g_GsmRecvNetDataBuf[s_wRecvNetDataCmdLen-s_wNetDataLen],s_IpdHeadBuf,s_wNetDataLen);
				if(IS_TRUE_BIT(GsmDevRxQWrite(&g_GsmRecvNetDataBuf[s_wRecvNetDataCmdLen-s_wNetDataLen],(s_wNetDataLen+i_wDataLen)))) {;
				}//包括头的长度
				else {
					GsmDebugString("Gsm rx write err3\r\n"); 
				}
				if(IS_FALSE_BIT(s_ucRemainNetDataFlag))
				{
				}
				else
				{
					OSSemPost(ptOsSemUsedForNeedGetNetData);
					M_ClrBit(s_ucRemainNetDataFlag);
				}
				s_wRecvNetDataLen=0;
				s_ucRecvNetDataState=0;
			}
			break;
		}
		default:
		{
			DebugPrintf(ERR"ManGetGsmNetDataHandler err:%d\r\n",s_ucRecvNetDataState);

			s_ucRecvNetDataState=0;
			break;
		}
	}
}

