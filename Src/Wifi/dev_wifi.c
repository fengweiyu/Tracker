/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266应用程序
* 开发人员: winter han
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/

#include "CBasicTools.h"
#include "Config.h"
#include "Esp8266.h"
#include "Dev_wifi.h"
#include "GsmDriverTypeDef.h"
#include "main.h"
#include "Myqueue.h"
#include "app_WifiSupport.h"
//AtData_st atdata_rev;
WifiCmdParam wifi_cmd_param;
DataBuffParam cmd_queue_param;
static e_wifi_bool wifi_power_status;
extern AtWifiInfo_st WifiInfo;

WifiCmdParam *wifi_cmd_param_get(void)
{
	return &wifi_cmd_param;
}

/*****************wifi moudule instruction issue begin*****************/

void wifi_module_version_get(void)
{
	AtCmd_st 	atcmd_write;
		
	atcmd_write.AtCmd_Id = AT_CMD_GMR;
	atcmd_write.AtCmd_Data.At_data_len = CMD_WITHOUT_DATA_LEN;

	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_module_connect_test(void)
{
	AtCmd_st 	atcmd_write;
		
	atcmd_write.AtCmd_Id = AT_CMD_TEST;
	atcmd_write.AtCmd_Data.At_data_len = CMD_WITHOUT_DATA_LEN;

	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_module_softrest(void)
{
	AtCmd_st 	atcmd_write;
				
	atcmd_write.AtCmd_Id = AT_CMD_RST;
	atcmd_write.AtCmd_Data.At_data_len = CMD_WITHOUT_DATA_LEN;

	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_module_mode_set(WifiModuleMode mode)
{
	AtCmd_st 	atcmd_write;
	
	atcmd_write.AtCmd_Id = AT_CMD_CWMODE;
	atcmd_write.AtCmd_Data.At_data_len = 2;
	atcmd_write.AtCmd_Data.At_data[0] = '=';
	atcmd_write.AtCmd_Data.At_data[1] = mode+'0';
	 
	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_module_mode_get(void)
{
	AtCmd_st 	atcmd_write;
	
	atcmd_write.AtCmd_Id = AT_CMD_CWMODE;
	atcmd_write.AtCmd_Data.At_data_len = 1;
	atcmd_write.AtCmd_Data.At_data[0] = '?';
		
	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_ap_list_get(void)
{
	AtCmd_st 	atcmd_write;
				
	atcmd_write.AtCmd_Id=AT_CMD_CWLAP;
	atcmd_write.AtCmd_Data.At_data_len=CMD_WITHOUT_DATA_LEN;

	wifi_AtCmd_Send(&atcmd_write);
}

unsigned char ap_join_ssid_code_ESC(unsigned char *data, unsigned char data_len, unsigned char *data_dest)
{
	unsigned char reb_len = 0;
	unsigned char i;
	unsigned char *data_temp;

//	debugPrintfHex2Ascii(data, data_len);
//	debugPrintfString("\n----- I am an evil cut-off rule -------\n");
	data_temp = data_dest;
	
	*data_temp++ = '"';
	reb_len ++;
	
	for(i=0; i<data_len; i++)
	{
		if ((*(data + i) == ',') || (*(data + i) == '"') || (*(data + i) == '\\'))
		{
			*data_temp++ = '\\';
			reb_len++;
		}
		*data_temp++ = *(data + i);
		reb_len++;
	}

	*data_temp = '"';
	reb_len ++;

	return reb_len;
}

void wifi_ap_join(void)
{
	AtCmd_st 	atcmd_write;
	unsigned char *data_dest;

	atcmd_write.AtCmd_Id = AT_CMD_CWJAP;
	atcmd_write.AtCmd_Data.At_data_len = 0;

	*(atcmd_write.AtCmd_Data.At_data + atcmd_write.AtCmd_Data.At_data_len) = '=';
	atcmd_write.AtCmd_Data.At_data_len++;

	atcmd_write.AtCmd_Data.At_data_len += ap_join_ssid_code_ESC(wifi_cmd_param.AP_join_info.ssid,
		wifi_cmd_param.AP_join_info.ssid_len, 
		(atcmd_write.AtCmd_Data.At_data + atcmd_write.AtCmd_Data.At_data_len));

	*(atcmd_write.AtCmd_Data.At_data + atcmd_write.AtCmd_Data.At_data_len) = ',';
	atcmd_write.AtCmd_Data.At_data_len++;

	atcmd_write.AtCmd_Data.At_data_len += ap_join_ssid_code_ESC(wifi_cmd_param.AP_join_info.code,
		wifi_cmd_param.AP_join_info.code_len, 
		(atcmd_write.AtCmd_Data.At_data + atcmd_write.AtCmd_Data.At_data_len));

	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_ap_quiet(void)
{
	AtCmd_st 	atcmd_write;
	
	atcmd_write.AtCmd_Id = AT_CMD_CWQAP;		
	atcmd_write.AtCmd_Data.At_data_len=CMD_WITHOUT_DATA_LEN;

	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_ap_autoconn(AP_AutoConnStat autoconn_stat)
{
	AtCmd_st 	atcmd_write;
	
	atcmd_write.AtCmd_Id = AT_CMD_CWAUTOCONN;

	atcmd_write.AtCmd_Data.At_data_len = 2;
	atcmd_write.AtCmd_Data.At_data[0] = '=';
	atcmd_write.AtCmd_Data.At_data[1] = autoconn_stat+'0';
	
	wifi_AtCmd_Send(&atcmd_write);
}

void wifi_module_mac(void)
{
	AtCmd_st 	atcmd_write;
	
	atcmd_write.AtCmd_Id = AT_CMD_CIPSTAMAC;

	atcmd_write.AtCmd_Data.At_data_len = 1;
	atcmd_write.AtCmd_Data.At_data[0] = '?';
	
	wifi_AtCmd_Send(&atcmd_write);
}

void app_wifi_cmd_send(void)
{
	switch(wifi_cmd_param.wifi_cmd_id)
	{
		case CMD_ID_CONNECT:
			wifi_module_connect_test();
			break;
		
		case CMD_ID_MODE_GET:
			wifi_module_mode_get();
			break;
		
		case CMD_ID_MODE_SET:
			wifi_module_mode_set(wifi_cmd_param.mode);
			break;
		
		case CMD_ID_RST:
			wifi_module_softrest();
			break;
		
		case CMD_ID_VERSION:
			wifi_module_version_get();
			break;
		
		case CMD_ID_LAP:
			wifi_ap_list_get();
			break;
	
		case CMD_ID_JAP:
			wifi_ap_join();
			break;

		case CMD_ID_QAP:
			wifi_ap_quiet();
			break;

		case CMD_ID_AUTOCONN:
			wifi_ap_autoconn(wifi_cmd_param.autoconn_stat);
			break;

		case CMD_ID_STAMAC:
			wifi_module_mac();
			break;

		default: break;
	}
}
e_wifi_bool app_wifi_cmd_sendById(WifiCmdId stCmd)
{
	e_wifi_bool boRetVal; 
	M_SetBit(boRetVal); 
	switch(stCmd)
	{
		case CMD_ID_CONNECT:
			wifi_module_connect_test();
			break;
		
		case CMD_ID_MODE_GET:
			wifi_module_mode_get();
			break;
		
		case CMD_ID_MODE_SET:
			wifi_module_mode_set(wifi_cmd_param.mode);
			break;

		case CMD_ID_MODE_SET_2_STA:
			wifi_module_mode_set(1);
			break;
		
		case CMD_ID_RST:
			wifi_module_softrest();
			break;
		
		case CMD_ID_VERSION:
			wifi_module_version_get();
			break;
		
		case CMD_ID_LAP:
			wifi_ap_list_get();
			break;
	
		case CMD_ID_JAP:
			wifi_ap_join();
			break;

		case CMD_ID_QAP:
			wifi_ap_quiet();
			break;

		case CMD_ID_AUTOCONN:
			wifi_ap_autoconn(wifi_cmd_param.autoconn_stat);
			break;

		case CMD_ID_STAMAC:
			wifi_module_mac();
			break;

		default:
			M_ClrBit(boRetVal); 
			break;
	}

	return boRetVal; 
}
e_wifi_bool WifiCmdHookLap(u8 *pu8Dat, u16 u16Len)
{
	e_wifi_bool boCmdExeEnd; 
	const s8 cs8WifiCmdLapEndSign[] = "OK"; 
	const u8 u8StrLen = sizeof(cs8WifiCmdLapEndSign) - 1; 

	struct LapCmdBufSt {
		u8 su8WifiCmdBuf[1536];//为快速兼容,使用原来的解析函数需要
		u16 su16Len; 
	}; 
	static struct LapCmdBufSt sstLapCmdBuf; 
	e_wifi_bool boEndSign; 
	e_wifi_bool boStartSign; 
	static u8 su8ApNum; 
	e_wifi_bool boWifiScanEndFlg; 
	//DebugPrintf("wifi hook rxed:"); 
	//debugPrintf(pu8Dat, u16Len); 
	static enum {
		WIFI_CMD_LAP_HOOK_ST_IDLE,
		WIFI_CMD_LAP_HOOK_ST_RXING,
		WIFI_CMD_LAP_HOOK_ST_END, 
	} senWifiCmdLapHookSt; 
	
	M_ClrBit(boEndSign); 
	M_ClrBit(boStartSign); 
	M_ClrBit(boWifiScanEndFlg); 

	M_ClrBit(boCmdExeEnd); 
	
	if(u16Len < u8StrLen) {; 
	}
	else {
		if(0==memcmp(cs8WifiCmdLapEndSign, pu8Dat, u8StrLen)) {
			DebugPrintf("rx end sign \r\n"); 
			M_SetBit(boEndSign); 
		}
		else {
			M_ClrBit(boEndSign); 
		}
	}

	if(u16Len < strlen(AT_CMD_STR_CWLAP)) {; 
	}
	else {
		if(0==memcmp(AT_CMD_STR_CWLAP, pu8Dat, strlen(AT_CMD_STR_CWLAP))) {
			M_SetBit(boStartSign); 
			DebugPrintf("rx scan start sign\r\n"); 
		}
		else {
			M_ClrBit(boStartSign); 
		}
	}
	
	if(IS_FALSE_BIT(boStartSign)) {
	}
	else {
		senWifiCmdLapHookSt = WIFI_CMD_LAP_HOOK_ST_IDLE; 
	}

	switch(senWifiCmdLapHookSt) {
		case WIFI_CMD_LAP_HOOK_ST_IDLE:
			if(IS_FALSE_BIT(boStartSign)) {; 
			}
			else {
				DebugPrintf("wifi scan start clr buf\r\n"); 
				memset(&sstLapCmdBuf, 0, sizeof(sstLapCmdBuf)); 
				M_ClrBit(su8ApNum); 
				{
					u16 u16LenFreeInBuf;
					u16LenFreeInBuf = (sizeof(sstLapCmdBuf.su8WifiCmdBuf) - 1) - sstLapCmdBuf.su16Len; //-1, '\0'
					if(u16LenFreeInBuf < u16Len) {
						DebugPrintf(ERR"ERROR:wifi lab cmd buff ERROR\r\n"); 
						DebugPrintf(CRIT"wifi scan buf not enough, end scan\r\n"); 
						M_SetBit(boWifiScanEndFlg);
					}
					else {
						sstLapCmdBuf.su16Len += 
							M_MEM_CPY(&sstLapCmdBuf.su8WifiCmdBuf[sstLapCmdBuf.su16Len], pu8Dat, u16Len); 
					}
				}	
				senWifiCmdLapHookSt = WIFI_CMD_LAP_HOOK_ST_RXING; 
			}
			break; 

		case WIFI_CMD_LAP_HOOK_ST_RXING:
			{
				u16 u16LenFreeInBuf;				
				u16LenFreeInBuf = (sizeof(sstLapCmdBuf.su8WifiCmdBuf) - 1) - sstLapCmdBuf.su16Len; //-1, '\0'
				if(u16LenFreeInBuf < u16Len) {
					DebugPrintf(ERR"ERROR:wifi lab cmd buff ERROR\r\n"); 
					DebugPrintf(CRIT"wifi scan buf not enough, end scan\r\n"); 
					M_SetBit(boWifiScanEndFlg);
				}
				else {
					sstLapCmdBuf.su16Len += 
						M_MEM_CPY(&sstLapCmdBuf.su8WifiCmdBuf[sstLapCmdBuf.su16Len], pu8Dat, u16Len); 
					/******************个数是否足够**************************/
					if(u16Len < strlen(AT_CMD_ACK_CWLAP)) {; 
					}
					else {
						if(0==memcmp(AT_CMD_ACK_CWLAP, pu8Dat, strlen(AT_CMD_ACK_CWLAP))) {
							su8ApNum++; 
							if(su8ApNum < (WIFI_SCAN_AP_LIST_ADDR_MAX_NUM)) {
							}
							else {
								DebugPrintf("wifi scan ap num ok,manual end scan\r\n"); 
								M_SetBit(boWifiScanEndFlg); 
							}
						}
						else {; 
						}
					}
				}
			}	

			if(IS_TRUE_BIT(boEndSign)
			|| IS_TRUE_BIT(boWifiScanEndFlg)) {
				at_execmdlap(sstLapCmdBuf.su8WifiCmdBuf); 
				memset(&sstLapCmdBuf, 0, sizeof(sstLapCmdBuf)); 
				DebugPrintf("wifi scan end\r\n"); 
				if(su8ApNum < (WIFI_SCAN_AP_LIST_ADDR_MAX_NUM)) {
					DebugPrintf(CRIT"WifiScanApNotEnoughBufEnd\r\n"); 
				}
				else {
				}
				senWifiCmdLapHookSt = WIFI_CMD_LAP_HOOK_ST_END;
			}
			else {; 
			}
				
			break; 

		case WIFI_CMD_LAP_HOOK_ST_END:
			//放在此处而不放在上面是
			//倾向于接收到结束符再结束命令
			M_SetBit(boCmdExeEnd); 
			DebugPrintf("wifi rxed end, give up\r\n"); 
			break; 

		default:
			senWifiCmdLapHookSt = WIFI_CMD_LAP_HOOK_ST_IDLE; 
			DebugPrintf(ERR"ERROR:wifi rxed status default,2 idle\r\n"); 
			break; 
	}

	return boCmdExeEnd; 
}

pWifiCmdRetHookType app_wifi_GetHookById(WifiCmdId stCmd)
{
	pWifiCmdRetHookType pRetVal; 
	pRetVal = NULL; 

	switch(stCmd)
	{
		case CMD_ID_LAP:
			pRetVal = WifiCmdHookLap; 
			break; 

			
		default:
			pRetVal = NULL; 
			break;
	}

	return pRetVal; 
}

/*串口数据接收*/
GsmTpBool WifiDevRxQWrite(GsmTpU8 *pu8Dat, GsmTpU16 u16Len)
{
	Gsmmyevent event; 
	event.buff = pu8Dat; 
	event.size = u16Len; 
#if 0
	if(u16Len<130)
	{
		DebugPrintf("Wifi recv:%s!\r\n",event.buff);
	}
#endif
	return DevOsRxQWrite(pstOsQForWifiRx, (myevent *)&event); 
}
static AtData_st sstWfiRecvBuf; 
void wifiRecv(u8 u8Dat)
{
	Gsmmyevent event;
	static u8 ucWifiRecvCount=0;
	if((0 == sstWfiRecvBuf.size) && (0 == u8Dat)) {
		return;
	}
	if(('\n' == u8Dat) && (sstWfiRecvBuf.size < 2)) {
		
		memset(&sstWfiRecvBuf, 0, sizeof(sstWfiRecvBuf)); 
		return;
	}

	sstWfiRecvBuf.buff[sstWfiRecvBuf.size++] = u8Dat;
	
	if(sstWfiRecvBuf.size < 2) {; 
	}
	else {
		if(('\n' == u8Dat)
		&& ('\r' == sstWfiRecvBuf.buff[sstWfiRecvBuf.size - 2])){			
			event.size = sstWfiRecvBuf.size; 
			event.buff = sstWfiRecvBuf.buff;			
			//超过八个热点则过滤，防止不断写队列
			if(0==memcmp("AT+CWLAP",sstWfiRecvBuf.buff,strlen("AT+CWLAP")))
			{
				ucWifiRecvCount=0;
			}
			else
			{
			}
			if(0==memcmp("+CWLAP:",sstWfiRecvBuf.buff,strlen("+CWLAP:")))
			{
				ucWifiRecvCount++;
			}
			else
			{
			}
			if((ucWifiRecvCount>WIFI_SCAN_AP_LIST_ADDR_MAX_NUM)&&(0==memcmp("+CWLAP:",sstWfiRecvBuf.buff,strlen("+CWLAP:"))))
			{
				//DebugPrintf("WifiApInfo is enough!\r\n");
			}
			else
			{
				if(IS_TRUE_BIT(WifiDevRxQWrite(event.buff, event.size))) {;
				}
				else {
					DebugPrintf(ERR"wifi rx write err\r\n"); 
				}
			}		
			memset(&sstWfiRecvBuf, 0, sizeof(sstWfiRecvBuf)); 
		}
		else if(sstWfiRecvBuf.size > (AT_WIFI_DATA_MAX -1))
		{
			DebugPrintf(ERR"Wifi data recv ov!\n");
			memset(&sstWfiRecvBuf, 0, sizeof(sstWfiRecvBuf)); 
		}
		else {; 
		}
	}
}
/*****************wifi moudule data receive begin*****************/

/*void app_wifi_rev(EatUart_enum uart)
{
	unsigned short int AtData_len = 0;

	do
	{
		u16 u16ReadLen = 0;
		u16 u16RequestLen = 0;
		
		if(AT_WIFI_DATA_MAX <= atdata_rev.size)
		{
			memset(&atdata_rev, 0, sizeof(atdata_rev)); 
		}
		
		AtData_len = eat_uart_read(uart, &atdata_rev.buff[atdata_rev.size], AT_WIFI_DATA_MAX - atdata_rev.size);
		atdata_rev.size += AtData_len;

	}while(AtData_len > 0);

}
void wifiData2Recv()
{
	if(IS_FALSE_BIT(atdata_rev.size)) {;
	}
	else {			
		u16 u16i; 
		for(u16i = 0; u16i < atdata_rev.size; u16i++) {
			wifiRecv(atdata_rev.buff[u16i]); 
		}
		wifi_ClearAtData(&atdata_rev);
	}
}*/
/*****************wifi moudule data recerve end*****************/


/*****************wifi moudule base function begin*****************/

/*
-------------------------------------------------------------------------------------------------
-函数功能:	初始化WIFI模块
-参数说明:	NULL
-------------------------------------------------------------------------------------------------
*/

void time_delay_ms(unsigned short int u16ms)
{
	if (u16ms < 5)
	{;
	}
	else
	{
		OSTimeDly(u16ms);
	}
}

void app_wifi_init(void)
{
	wifi_info_init();
	//wifi_ClearAtData(&atdata_rev);
	memset(&wifi_cmd_param, 0, sizeof(WifiCmdParam));
	
	cmd_queue_init();	
	app_module_cmd_issue(CMD_ID_MODE_SET, (unsigned char)WIFI_MODE_STA);
	app_module_cmd_issue(CMD_ID_STAMAC, 0);
	//MsgSend2Task(eat_get_task_id(), WIFI_TASK_ID, TRUE, 0, EAT_NULL,NULL);
}

e_wifi_bool wifi_power_is_on(void)
{
	return wifi_power_status;
}

void app_wifi_start(void)
{
	if (CMD_LIST_WORK == wifi_cmd_param.cmd_list_status)
	{;
	}
	else
	{	
		cmd_queue_member_get_fifo(&wifi_cmd_param.wifi_cmd_id);
		
		wifi_cmd_param.cmd_list_status = CMD_LIST_WORK;
		
		if (AT_RESUALT_NULL== wifi_cmd_param.rev_flag[CMD_ID_READY])
		{
			wifi_PowerOn();
			wifi_power_status = TRUE;
			DebugPrintf(CRIT"------- wifi power on -------\n");
		}
	}
}

void app_wifi_end(void)
{
	//wifi_ClearAtData(&atdata_rev);
	app_wifi_cmd_id_clr();
	app_wifi_rev_flag_clr();
	
	if(WIFI_TRUE == cmd_queue_member_get_fifo(&wifi_cmd_param.wifi_cmd_id))
	{
		wifi_cmd_param.rev_flag[CMD_ID_READY] = AT_RESUALT_SUCCESS;
	}
	else
	{
		wifi_cmd_param.cmd_list_status = CMD_LIST_IDLE;
		
		if (ANTI_LOST_ON == wifi_cmd_param.anti_lost_status)
		{
			wifi_cmd_param.rev_flag[CMD_ID_READY] = AT_RESUALT_SUCCESS;
		}
		else
		{
			time_delay_ms(10);
			wifi_PowerOff();
			wifi_power_status = FALSE;
			DebugPrintf(CRIT"------- wifi power off -------\n");
		}
	}
}

/*****************wifi moudule base function end*****************/


/*****************wifi moudule param set and get begin*****************/

void app_wifi_connection_establish(void)
{
	wifi_module_connect_test();
}

void app_wifi_connection_flag_set(At_Resualt resualt)
{
	wifi_cmd_param.rev_flag[CMD_ID_READY] = resualt;
}

At_Resualt app_wifi_connection_flag_get(void)
{
	return wifi_cmd_param.rev_flag[CMD_ID_READY];
}

void app_wifi_connection_flag_clr(void)
{
	wifi_module_connect_test();
}

WifiCmdId app_wifi_cmd_id_get(void)
{
	return wifi_cmd_param.wifi_cmd_id;
}

void app_wifi_cmd_id_clr(void)
{
	wifi_cmd_param.wifi_cmd_id = CMD_ID_NULL;
}

void app_wifi_rev_flag_clr(void)
{
	memset(wifi_cmd_param.rev_flag, AT_RESUALT_NULL, sizeof(wifi_cmd_param.rev_flag));
}

void app_wifi_rev_flag_set(At_Resualt resualt)
{
	wifi_cmd_param.rev_flag[wifi_cmd_param.wifi_cmd_id] = resualt;
}

void app_wifi_rev_flag_get(At_Resualt *rev_flag)
{
	memcpy(rev_flag, wifi_cmd_param.rev_flag, CMD_ID_NUM);
}

void app_ap_join_info_fill(AP_JoinInfo *AP_join_info) 
{
	memcpy(wifi_cmd_param.AP_join_info.code, AP_join_info->code, AP_join_info->code_len);
	wifi_cmd_param.AP_join_info.code_len = AP_join_info->code_len;
	memcpy(wifi_cmd_param.AP_join_info.ssid, AP_join_info->ssid, AP_join_info->ssid_len);
	wifi_cmd_param.AP_join_info.ssid_len = AP_join_info->ssid_len;
}

void app_anti_lost_status_fill(AntiLostStatus anti_lost_status)
{
	wifi_cmd_param.anti_lost_status = anti_lost_status;
}

/*****************wifi moudule param set and get end*****************/


/*****************wifi moudule info inquiry begin*****************/

e_wifi_bool app_module_mode_inquiry(unsigned char *mode)
{
	e_wifi_bool reb;

	reb = wifi_module_mode_inquiry(mode);
	
	return reb;
}

e_wifi_bool app_module_autoconn_inquiry(unsigned char *stat)
{
	e_wifi_bool reb;
	
	reb = wifi_module_autoconn_inquiry(stat);
	
	return reb;
}

e_wifi_bool app_module_version_inquiry(unsigned char *version)
{
	e_wifi_bool reb;
	
	reb = wifi_module_version_inquiry(version);
	
	return reb;
}

e_wifi_bool app_module_macadr_inquiry(unsigned char *mac_adr)
{
	e_wifi_bool reb;

	reb = wifi_module_macadr_inquiry(mac_adr);
	
	return reb;
}

e_wifi_bool app_wifi_ap_get(WifiAccessPointSt *stWifiAccessPointInfo)
{
	e_wifi_bool reb= WIFI_FALSE;
	unsigned char i = 0;

	for(i = 0;i < AT_WIFI_AP_NUM;i++)
	{
		if (0 == strlen(WifiInfo.ap_info[i].ssid))
		{
			reb = WIFI_FALSE;
			break;
		}
		else
		{
			strcpy(stWifiAccessPointInfo->stAccessPointArray[i].ssid,WifiInfo.ap_info[i].ssid);
			memcpy(stWifiAccessPointInfo->stAccessPointArray[i].mac_address,WifiInfo.ap_info[i].mac_address,AT_WIFI_AP_MAC_NUM);
			stWifiAccessPointInfo->stAccessPointArray[i].signal_strength= WifiInfo.ap_info[i].signal_strength;	
			stWifiAccessPointInfo->stAccessPointArray[i].age= WifiInfo.ap_info[i].age;	
			stWifiAccessPointInfo->stAccessPointArray[i].channel= WifiInfo.ap_info[i].channel;	
			stWifiAccessPointInfo->stAccessPointArray[i].signal_to_noise_ratio= WifiInfo.ap_info[i].signal_to_noise_ratio;
			reb = WIFI_TRUE;
		}
	}
	return reb;
}

/*****************wifi moudule info inquiry end*****************/


/*****************wifi moudule cmd queue handle begin*****************/

void cmd_queue_init(void)	//初始化命令队列
{
	cmd_queue_param.read_offset = 0;
	cmd_queue_param.write_offset = 0;
	memset(cmd_queue_param.cmd_queue, 0, sizeof(cmd_queue_param)/sizeof(unsigned char));
}

e_wifi_bool cmd_queue_has_new_member(void)
	//判断命令队列是否有新成员
{
	if (cmd_queue_param.read_offset == cmd_queue_param.write_offset)
	{
		return WIFI_FALSE;
	}
	else
	{
		return WIFI_TRUE;
	}
}

e_wifi_bool cmd_queue_new_member_join(WifiCmdId cmd_id)
	//命令队列有新成员加入
{
	unsigned char next_pos;

	next_pos = (cmd_queue_param.write_offset + 1) % CMD_QUEUE_LEN_MAX;
	
	if (next_pos == cmd_queue_param.read_offset) //判断队列是否已满
	{
		return WIFI_FALSE;		
	}
	else
	{
		cmd_queue_param.cmd_queue[cmd_queue_param.write_offset] = cmd_id;
		cmd_queue_param.write_offset = next_pos;
		return WIFI_TRUE;		
	}
}

e_wifi_bool cmd_queue_member_get_fifo(WifiCmdId *cmd_id)
	//按照先入先出的方式获取队列成员
{
	unsigned char next_pos;

	next_pos = (cmd_queue_param.read_offset + 1) % CMD_QUEUE_LEN_MAX;
	
	if (WIFI_TRUE == cmd_queue_has_new_member()) //判断队列是否为空
	{
		*cmd_id = cmd_queue_param.cmd_queue[cmd_queue_param.read_offset];
		cmd_queue_param.read_offset = next_pos;
		return WIFI_TRUE;		
	}
	else
	{
		return WIFI_FALSE;		
	}
}


/*****************wifi moudule cmd queue handle end*****************/

void app_ap_test(void)
{
	app_module_cmd_issue(CMD_ID_LAP, NULL);
	//MsgSend2Task(eat_get_task_id(), WIFI_TASK_ID, TRUE, 0, EAT_NULL,NULL);
}


