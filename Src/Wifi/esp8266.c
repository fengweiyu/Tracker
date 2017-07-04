/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: ESP8266驱动程序
* 开发人员: winter han
* 开发时间: 2016.05.04
* 软件版本: V1.0.3
************************************************************************************************************************
*/
#include "CBasicTools.h"
#include "esp8266.h"
#include "Config.h"
#include "app_WifiSupport.h"
unsigned char at_execmdat(unsigned char *strdata);
unsigned char at_execmdrst(unsigned char *strdata);
unsigned char at_execmdgmr(unsigned char *strdata);
unsigned char at_testcmdmode(unsigned char *strdata);
unsigned char at_querycmdmode(unsigned char *strdata);
unsigned char at_setcmdmode(unsigned char *strdata);
unsigned char at_execmdlap(unsigned char *strdata);
unsigned char at_setcmdlap(unsigned char *strdata);
unsigned char at_setcmdjap(unsigned char *strdata);
unsigned char at_querycmdjap(unsigned char *strdata);
unsigned char at_execmdqap(unsigned char *strdata);
unsigned char at_querycmdqap(unsigned char *strdata);
unsigned char at_setcmdautoconn(unsigned char *strdata);
unsigned char at_setcmdstamac(unsigned char *strdata);
unsigned char at_querycmdstamac(unsigned char *strdata);


AtCmdMessge_st AtCmdMessge[AT_CMD_NUM]=
{
	{AT_CMD_STR_NULL, sizeof(AT_CMD_STR_NULL)-1, NULL, NULL, NULL, NULL},
	{AT_CMD_STR_TEST, sizeof(AT_CMD_STR_TEST)-1, NULL, NULL, at_execmdat, NULL},
	{AT_CMD_STR_RST, sizeof(AT_CMD_STR_RST)-1, NULL, NULL, at_execmdrst, NULL},
	{AT_CMD_STR_GMR, sizeof(AT_CMD_STR_GMR)-1, NULL, NULL, at_execmdgmr, NULL},
	{AT_CMD_STR_CWMODE, sizeof(AT_CMD_STR_CWMODE)-1, at_testcmdmode, at_querycmdmode, NULL, at_setcmdmode},
	{AT_CMD_STR_CWLAP, sizeof(AT_CMD_STR_CWLAP)-1, NULL, NULL, at_execmdlap, at_setcmdlap},
	{AT_CMD_STR_CWJAP, sizeof(AT_CMD_STR_CWJAP)-1, NULL, at_querycmdjap, NULL, at_setcmdjap},
	{AT_CMD_STR_CWQAP, sizeof(AT_CMD_STR_CWQAP)-1, NULL, at_querycmdqap, at_execmdqap, NULL},
	{AT_CMD_STR_CWAUTOCONN, sizeof(AT_CMD_STR_CWAUTOCONN)-1, NULL, NULL, NULL, at_setcmdautoconn},
	{AT_CMD_STR_CIPSTAMAC, sizeof(AT_CMD_STR_CIPSTAMAC)-1, NULL, at_querycmdstamac, NULL, at_setcmdstamac},	
};

#define AT_FLAG_STR_NULL		NULL
#define AT_FLAG_STR_OVER		"\r\n"
#define AT_FLAG_STR_OK		"\r\nOK\r\n"
#define AT_FLAG_STR_ERROR		"\r\nERROR\r\n"
#define AT_FLAG_STR_READY		"ready\r\n"
#define AT_FLAG_STR_FAIL		"\r\nFAIL\r\n"
#define AT_FLAG_STR_BUSY		"busy p...\r\n"

AtCommonFlag_st AtCommonFlag[AT_FLAG_NUM]=
{
	{AT_FLAG_STR_NULL, sizeof(AT_FLAG_STR_NULL)-1},
	{AT_FLAG_STR_OVER, sizeof(AT_FLAG_STR_OVER)-1},
	{AT_FLAG_STR_OK, sizeof(AT_FLAG_STR_OK)-1},
	{AT_FLAG_STR_ERROR, sizeof(AT_FLAG_STR_ERROR)-1},
	{AT_FLAG_STR_READY, sizeof(AT_FLAG_STR_READY)-1},
	{AT_FLAG_STR_FAIL, sizeof(AT_FLAG_STR_FAIL)-1},
	{AT_FLAG_STR_BUSY, sizeof(AT_FLAG_STR_BUSY)-1},
};

AtWifiInfo_st WifiInfo;
AtApAssist_st	AP_assist;

/***************** wifi moudule basic function begin *****************/
/*
-------------------------------------------------------
-函数功能:	WIFI使能
-参数说明:	NULL
-------------------------------------------------------
*/
void wifi_PowerOn(void)
{
	g_tWifi.WifiPowerOn();
}

/*
--------------------------------------------------------
-函数功能:	WIFI关闭
-参数说明:	NULL
--------------------------------------------------------
*/
void wifi_PowerOff(void)
{
	g_tWifi.WifiPowerOff();
}

/*
--------------------------------------------------------
-函数功能:	数据初始化
-参数说明:	NULL
--------------------------------------------------------
*/
void wifi_info_init(void)
{
	memset(&WifiInfo, 0, sizeof(AtWifiInfo_st));
}

void wifi_data_init(void)
{
	memset(WifiInfo.ap_info, 0, sizeof(WifiInfo.ap_info));
}

void wifi_mode_init(void)
{
	memset(&WifiInfo.wifi_module_mode, 0, sizeof(WifiInfo.wifi_module_mode));
}

void wifi_version_init(void)
{
	memset(WifiInfo.at_version, 0, sizeof(WifiInfo.at_version));
}

void wifi_autoconn_init(void)
{
	memset(&WifiInfo.ap_autoconn_stat, 0, sizeof(WifiInfo.ap_autoconn_stat));
}

void wifi_macadr_init(void)
{
	memset(&WifiInfo.wifi_module_mac, 0, sizeof(WifiInfo.wifi_module_mac));
}

/*
--------------------------------------------------------
-函数功能:	清零AT命令返回数据接收数组
-参数说明:	NULL
--------------------------------------------------------
*/
void wifi_ClearAtData(AtData_st *atdata_rev)
{
	memset(atdata_rev,0,sizeof(AtData_st));
}

/***************** wifi moudule basic function end *****************/


/*****************wifi moudule instruction issue begin*****************/
/*
--------------------------------------------------------
-函数功能:	AT命令发送内容生成
-参数说明:	NULL
--------------------------------------------------------
*/
static unsigned char wifi_AtCmd_IdData_Combine(AtCmd_st *atcmd_data, unsigned char *atcmdstr)
{
	unsigned char atcmd_len=0;
	
	if((atcmd_data->AtCmd_Id>AT_CMD_NULL) && (atcmd_data->AtCmd_Id<AT_CMD_NUM))
	{
		if(atcmd_data->AtCmd_Data.At_data_len==0)
		{
			strcpy(atcmdstr, AtCmdMessge[atcmd_data->AtCmd_Id].at_cmdName);
			atcmd_len+=AtCmdMessge[atcmd_data->AtCmd_Id].at_cmdLen;
			
			strcpy(atcmdstr+atcmd_len, AtCommonFlag[AT_FLAG_OVER].at_FlagName);
			atcmd_len+=AtCommonFlag[AT_FLAG_OVER].at_FlagLen;
		}
		else
		{
			strcpy(atcmdstr, AtCmdMessge[atcmd_data->AtCmd_Id].at_cmdName);
			atcmd_len+=AtCmdMessge[atcmd_data->AtCmd_Id].at_cmdLen;
	
			strcpy(atcmdstr+atcmd_len, atcmd_data->AtCmd_Data.At_data);
			atcmd_len+=atcmd_data->AtCmd_Data.At_data_len;
		
			strcpy(atcmdstr+atcmd_len, AtCommonFlag[AT_FLAG_OVER].at_FlagName);
			atcmd_len+=AtCommonFlag[AT_FLAG_OVER].at_FlagLen;
		}
	}
	else
	{;
	}

	return atcmd_len;
}

/*
----------------------------------------------------------
-函数功能:	AT命令发送
-参数说明:	NULL
----------------------------------------------------------
*/
void wifi_AtCmd_Send(AtCmd_st *atcmd_data)
{
	unsigned char write_len=0;
	unsigned char atcmdstr[100];

	write_len=wifi_AtCmd_IdData_Combine(atcmd_data, atcmdstr);
	DebugPrintf("wifi send:%s\r\n",atcmdstr);
	g_tWifi.WifiSendData(atcmdstr,write_len);
}

/*****************wifi moudule instruction issue end*****************/


/*****************wifi moudule data receive begin*****************/
static At_Resualt wifi_AtData_Analysis(AtData_st *atdata_rev);

/*
-----------------------------------------------------------
-函数功能:	AT命令返回数据接收
-参数说明:	NULL
-----------------------------------------------------------
*/

At_Resualt wifi_AtData_Rev(AtData_st *atdata_rev)
{
	At_Resualt reb=AT_RESUALT_NULL;
	
	reb=wifi_AtData_Analysis(atdata_rev);

	return reb;
}

/*****************wifi moudule data receive end*****************/


/*****************wifi moudule data analysis begin*****************/
static unsigned char chartohex(unsigned char *chr)
{
	unsigned char reb=0;
	
	if((*chr>='0') && (*chr<='9'))
	{
		reb=*chr-'0';
	}
	else if((*chr>='a') && (*chr<='f'))
	{
		reb=*chr-'a'+10;
	}
	else if((*chr>='A') && (*chr<='F'))
	{
		reb=*chr-'A'+10;
	}
	else
	{;
	}

	return reb;
}

/*
-----------------------------------------------------------
-函数功能:	获取AT命令的ID
-参数说明:	NULL
-----------------------------------------------------------
*/
static unsigned char wifi_atcmd_id_search(unsigned char cmdLen, unsigned char *pCmd)
{
	unsigned char i;
	unsigned char atcmd_id=AT_CMD_NULL;

	if(cmdLen == 0)
	{;
	}
	else
	{
		for(i=AT_CMD_TEST; i<AT_CMD_NUM; i++)
		{
			if(cmdLen == AtCmdMessge[i].at_cmdLen)
			{
				if(memcmp(pCmd, AtCmdMessge[i].at_cmdName, cmdLen) == 0)
				{
					atcmd_id= i;
					break;
				}
			}
		}
	}
	
	return atcmd_id;
}

/*
----------------------------------------------------------
-函数功能:	获取AT命令执行结果的ID
-参数说明:	NULL
----------------------------------------------------------
*/
static unsigned char wifi_atcmd_flag_search(unsigned short int data_len, unsigned char *pCmd)
{
	unsigned char i=0,j=0;
	unsigned short int k=data_len;
	unsigned char flag_id=AT_FLAG_NULL;

	pCmd+=data_len;

	while(k--)
	{
		pCmd--;

		if(*pCmd=='\r')
		{
			i++;
		}
		else if(*pCmd=='\n')
		{
			j++;
		}
		else
		{;
		}

		if((i>=2)&&(j>=2))
		{
			break;
		}
		else
		{;
		}
	}

	for(i=AT_FLAG_OK; i<AT_FLAG_NUM; i++)
	{
		if(memcmp(pCmd, AtCommonFlag[i].at_FlagName,AtCommonFlag[i].at_FlagLen) == 0)
		{
			flag_id= i;
			break;
		}
	}	

	return flag_id;
}

 /*
---------------------------------------------------------
-函数功能:	获取AT命令的长度
-参数说明:	NULL
---------------------------------------------------------
*/
static unsigned char wifi_atcmd_len_get(unsigned char *pCmd)
{
	unsigned char len,i;

	len= 0;
	i = AT_CMD_LEN_MAX;

	while(i--)
	{
		if((*pCmd == '\r') || (*pCmd == '=') || (*pCmd == '?') || (*pCmd == ' ')\
			||((*pCmd >= '0')&&(*pCmd <= '9')))
		{
			break;
		}
		else
		{
			pCmd++;
			len++;
		}
	}
	return len;
}

/*
---------------------------------------------------------
-函数功能:	AT命令返回数据解析
-参数说明:	NULL
---------------------------------------------------------
*/
static At_Resualt wifi_AtData_Analysis(AtData_st *atdata_rev)
{
	unsigned char data[4]={0};
	unsigned char atcmd_len, atcmd_id, flag_id;
	unsigned char i;
	At_Resualt reb = AT_RESUALT_NULL;
	
	atcmd_len=wifi_atcmd_len_get(atdata_rev->buff);
	atcmd_id=wifi_atcmd_id_search(atcmd_len, atdata_rev->buff);
	flag_id=wifi_atcmd_flag_search(atdata_rev->size, atdata_rev->buff);

	if((atcmd_id <= AT_CMD_NULL) || (atcmd_id >= AT_CMD_NUM))
	{;
	}
	else if (AT_FLAG_NULL == flag_id)
	{
		reb = AT_RESUALT_WAIT;
	}
	else
	{
		if (flag_id != AT_FLAG_OK)
		{
			reb = AT_RESUALT_FAIL;
		}
		else
		{
			reb = AT_RESUALT_SUCCESS;
		}

		switch(*(atdata_rev->buff+atcmd_len))
		{
			case '\r':
				AtCmdMessge[atcmd_id].cmddata_rev_execute(atdata_rev->buff);
				break;
			
			case ' ':
				AtCmdMessge[atcmd_id].cmddata_rev_execute(atdata_rev->buff);
				break;
			
			case '=':
				if(*(atdata_rev->buff+atcmd_len+1) == '?')
				{
					AtCmdMessge[atcmd_id].cmddata_rev_test(atdata_rev->buff);
				}
				else
				{
					AtCmdMessge[atcmd_id].cmddata_rev_set(atdata_rev->buff);
				}
				break;
			
			case '?':
				AtCmdMessge[atcmd_id].cmddata_rev_query(atdata_rev->buff);
				break;
			
			default:	reb = AT_RESUALT_FAIL;
				break;
				
		}
	}
	return reb;
}

/*****************wifi moudule data analysis end*****************/


/*****************wifi moudule data handle begin ********************/
/*
-------------------------------------------------------------
-函数功能:	判断多个AP点中信号最弱的
-参数说明:	NULL
-------------------------------------------------------------
*/
static void wifi_ApStrenMinJudge(void)

{
	unsigned char i;

	AP_assist.ap_stren_val_min=WifiInfo.ap_info[0].signal_strength;
	
	for(i=1; i<AT_WIFI_AP_NUM; i++)
	{
		if(AP_assist.ap_stren_val_min>WifiInfo.ap_info[i].signal_strength)
		{
			AP_assist.ap_stren_id_min=i;
			AP_assist.ap_stren_val_min=WifiInfo.ap_info[i].signal_strength;
		}
		else
		{
			;
		}
	}
}

/*
----------------------------------------------------------
-函数功能:	WIFI数据解析，获取WIFI信息
-参数说明:	NULL
----------------------------------------------------------
*/
static void wifi_InfoAnalysis(unsigned char *ap_data)
{
	unsigned char i;
	unsigned char *pstr_start, *pstr_end;
	unsigned char comma_num;
	signed char signal_strength_temp;
	unsigned char mac_temp;
		
	pstr_start=ap_data;
	pstr_end=ap_data;
	
	if(AP_assist.ap_num_had <  AT_WIFI_AP_NUM)
	{
		pstr_start=strchr(pstr_end, '"')+1;	//get ap name
		pstr_end=strchr(pstr_start, '"');
		memcpy(WifiInfo.ap_info[AP_assist.ap_num_had].ssid, pstr_start, (pstr_end-pstr_start));
		*(WifiInfo.ap_info[AP_assist.ap_num_had].ssid+(pstr_end-pstr_start))='\0';
			
		pstr_start=strchr(pstr_end, '-')+1;	//get ap strength
		pstr_end=strchr(pstr_start, ',');
		
		signal_strength_temp=0-(signed char)atoi(pstr_start);
		WifiInfo.ap_info[AP_assist.ap_num_had].signal_strength=signal_strength_temp;

		pstr_start=strchr(pstr_end, '"')+1;
		pstr_end=strchr(pstr_start, '"');
		for(i=0; i<AT_WIFI_AP_MAC_NUM; i++)	//get mac addr
		{
			mac_temp = chartohex(pstr_start++);
			mac_temp = (mac_temp<<4)+chartohex(pstr_start);
				
			WifiInfo.ap_info[AP_assist.ap_num_had].mac_address[i]=mac_temp;

			if(i==(AT_WIFI_AP_MAC_NUM-1))
			{;
			}
			else
			{
				pstr_start=strchr(pstr_start, ':')+1;
			}
		}

		pstr_start=strchr(pstr_end, ',')+1;	//get ap channel
		WifiInfo.ap_info[AP_assist.ap_num_had].channel=(unsigned char)atoi(pstr_start);
		
		AP_assist.ap_num_had++;
		if(AP_assist.ap_num_had ==  AT_WIFI_AP_NUM)
		{
			wifi_ApStrenMinJudge();
		}
	}
	else
	{		
		comma_num=0;
		while(comma_num < 2)	
		{
			comma_num += (',' == *pstr_end++)?(1):(0);
		}

		pstr_start=strchr(pstr_end, '-')+1;	//get ap strength
		signal_strength_temp=0-(signed char)atoi(pstr_start);

		if(0)//signal_strength_temp>AP_assist.ap_stren_val_min)
		{
			pstr_start=ap_data;
			pstr_end=ap_data;
			
			pstr_start=strchr(pstr_end, '"')+1;	//get ap name
			pstr_end=strchr(pstr_start, '"');
			memcpy(WifiInfo.ap_info[AP_assist.ap_stren_id_min].ssid, pstr_start, (pstr_end-pstr_start));
			*(WifiInfo.ap_info[AP_assist.ap_stren_id_min].ssid+(pstr_end-pstr_start))='\0';

			WifiInfo.ap_info[AP_assist.ap_stren_id_min].signal_strength=signal_strength_temp; //get ap strength

			pstr_start=strchr(pstr_end, '"')+1;
			pstr_end=strchr(pstr_start, '"');
			for(i=0; i<AT_WIFI_AP_MAC_NUM; i++)	//get mac addr
			{
				mac_temp = chartohex(pstr_start++);
				mac_temp = (mac_temp<<4)+chartohex(pstr_start);
					
				WifiInfo.ap_info[AP_assist.ap_stren_id_min].mac_address[i]=mac_temp;

				if(i==(AT_WIFI_AP_MAC_NUM-1))
				{;
				}
				else
				{
					pstr_start=strchr(pstr_start, ':')+1;
				}
			}

			pstr_start=strchr(pstr_end, ',')+1;	//get ap channel
			WifiInfo.ap_info[AP_assist.ap_stren_id_min].channel=(unsigned char)atoi(pstr_start);
			
			wifi_ApStrenMinJudge();
		}
		else
		{;
		}
	}
}

/*
-----------------------------------------------------------
-函数功能:	处理"AT"(执行)命令返回数据
-参数说明:	NULL
-----------------------------------------------------------
*/
unsigned char at_execmdat(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT\n");
}

/*
-----------------------------------------------------------
-函数功能:	处理"AT+RST"(执行)命令返回数据
-参数说明:	NULL
-----------------------------------------------------------
*/
unsigned char at_execmdrst(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+RST\n");
}

/*
----------------------------------------------------------
-函数功能:	处理"AT+GMR"(执行)命令返回数据
-参数说明:	NULL
----------------------------------------------------------
*/
unsigned char at_execmdgmr(unsigned char *strdata)
{
	unsigned char *pstr;
	unsigned char i = 0;
	
//	debugPrintfString("rev cmd: AT+GMR\n");

	pstr = strdata;
	pstr = strchr(pstr, ':');

	while((*(++pstr) != '\n') && (AT_WIFI_VERSION_LEN > i))
	{
		WifiInfo.at_version[i] = *pstr;
		i++;
	}
	
	DebugPrintf(CRIT"wifi version:%s\r\n",WifiInfo.at_version);
}

/*
---------------------------------------------------------------
-函数功能:	处理"AT+CWMODE=?"(测试)命令返回数据
-参数说明:	NULL
---------------------------------------------------------------
*/
unsigned char at_testcmdmode(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWMODE?\n");
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CWMODE?"(查询)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_querycmdmode(unsigned char *strdata)
{
	unsigned char *pstr;

//	debugPrintfString("rev cmd: AT+CWMODE?\n");
	pstr = strdata;
	pstr = strchr(pstr, ':')+1;
	WifiInfo.wifi_module_mode = *pstr-'0';
//	debugPrintfHex2Ascii(&WifiInfo.wifi_module_mode, 1);
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CWMIDE="(设置)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_setcmdmode(unsigned char *strdata)
{
	DebugPrintf(CRIT"rev cmd: AT+CWMODE=\n");
	WifiInfo.wifi_module_mode = AP_assist.wifi_module_mode;
}

/*
------------------------------------------------------------
-函数功能:	处理"AT+CWLAP"(执行)命令返回数据
-参数说明:	NULL
------------------------------------------------------------
*/
unsigned char at_execmdlap(unsigned char *strdata)
{
	unsigned char *pstr_start, *pstr_end;
	unsigned char *	pstr_end_backup;
	unsigned char ap_data[AT_WIFI_AP_DATA_LEN];
	unsigned char i;
	
//	debugPrintfString("rev cmd: AT+CWLAP\n");

	AP_assist.ap_num_had=0;

	pstr_start = strdata;
	pstr_end = strdata;
	pstr_end_backup = pstr_end;
	
	while(1)
	{
		pstr_start=strchr(pstr_end, '\n')+1;
		if('+' !=*pstr_start)
		{
			break;
		}
		else
		{
			pstr_end=strchr(pstr_start, '\n');
			
			if ((pstr_end - pstr_start) < (AT_WIFI_DATA_MAX - AtCmdMessge[AT_CMD_CWLAP].at_cmdLen - 1))
			{
				pstr_end_backup = pstr_end;
			}
			else
			{
				pstr_end = pstr_end_backup;
			}
			
			memcpy(ap_data, pstr_start, (pstr_end-pstr_start));
			wifi_InfoAnalysis(ap_data);
		}
	}
}

/*
-------------------------------------------------------------
-函数功能:	处理"AT+CWLAP="(设置)命令返回数据
-参数说明:	NULL
-------------------------------------------------------------
*/
unsigned char at_setcmdlap(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWLAP=\n");
}

/*
-------------------------------------------------------------
-函数功能:	处理"AT+CWJAP="(设置)命令返回数据
-参数说明:	NULL
-------------------------------------------------------------
*/
unsigned char at_setcmdjap(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWJAP=\n");
}

/*
-------------------------------------------------------------
-函数功能:	处理"AT+CWJAP?"(查询)命令返回数据
-参数说明:	NULL
-------------------------------------------------------------
*/
unsigned char at_querycmdjap(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWJAP?\n");
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CWQAP"(执行)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_execmdqap(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWQAP\n");
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CWQAP?"(查询)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_querycmdqap(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CWQAP?\n");
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CWAUTOCONN"(设置)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_setcmdautoconn(unsigned char *strdata)
{
	DebugPrintf(CRIT"rev cmd: AT+CWAUTOCONN\n");

	WifiInfo.ap_autoconn_stat = AP_assist.ap_autoconn_stat;
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CIPSTAMAC="(执行)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_setcmdstamac(unsigned char *strdata)
{
//	debugPrintfString("rev cmd: AT+CIPSTAMAC=\n");
}

/*
--------------------------------------------------------------
-函数功能:	处理"AT+CIPSTAMAC?"(查询)命令返回数据
-参数说明:	NULL
--------------------------------------------------------------
*/
unsigned char at_querycmdstamac(unsigned char *strdata)
{
	unsigned char *pstr;
	unsigned char i = 0;
	unsigned char mac_temp;
	
//	debugPrintfString("rev cmd: AT+CIPSTAMAC?\n");

	pstr = strdata;
	pstr = strchr(pstr, '"') + 1;

	for(i=0; i<AT_WIFI_AP_MAC_NUM; i++)	//get mac addr
	{
		mac_temp = chartohex(pstr++);
		mac_temp = (mac_temp<<4)+chartohex(pstr);
			
		WifiInfo.wifi_module_mac[i] = mac_temp;

		if(i==(AT_WIFI_AP_MAC_NUM-1))
		{;
		}
		else
		{
			pstr = strchr(pstr, ':')+1;
		}
	}
	
	DebugPrintf(CRIT"wifi stamac:%s\r\n",WifiInfo.wifi_module_mac);
	//debugPrintfHex2Ascii(WifiInfo.wifi_module_mac, AT_WIFI_AP_MAC_NUM);
}

/*****************wifi moudule data handle end ********************/


/*****************wifi moudule data inquiry begin ********************/

e_wifi_bool wifi_module_mode_inquiry(unsigned char *mode)
{
	e_wifi_bool reb = WIFI_TRUE;
	
	if ((WifiInfo.wifi_module_mode >= WIFI_MODE_STA) 
		&& (WifiInfo.wifi_module_mode <= WIFI_MODE_APSTA))
	{;
	}
	else
	{
		reb = WIFI_FALSE;
	}

	*mode = WifiInfo.wifi_module_mode;
	
	return reb;
}

e_wifi_bool wifi_module_autoconn_inquiry(unsigned char *stat)
{
	e_wifi_bool reb = WIFI_TRUE;
	
	if ((WifiInfo.ap_autoconn_stat >= AP_AUTO_CLOSE) 
		&& (WifiInfo.ap_autoconn_stat <= AP_AUTO_OPEN))
	{;
	}
	else
	{
		reb = WIFI_FALSE;
	}

	*stat = WifiInfo.ap_autoconn_stat;
	
	return reb;
}

e_wifi_bool wifi_module_version_inquiry(unsigned char *version)
{
	unsigned char reb = WIFI_TRUE;
	
	if (strlen(WifiInfo.at_version) == 0)
	{
		reb = WIFI_FALSE;
	}
	else
	{
		memcpy(version, WifiInfo.at_version, AT_WIFI_VERSION_LEN);
	}
	
	return reb;
}

e_wifi_bool wifi_module_macadr_inquiry(unsigned char *mac_adr)
{
	unsigned char reb = WIFI_TRUE;
	
	if (strlen(WifiInfo.wifi_module_mac) == 0)
	{
		reb = WIFI_FALSE;
	}
	else
	{
		memcpy(mac_adr, WifiInfo.wifi_module_mac, AT_WIFI_AP_MAC_NUM);
	}
	
	return reb;
}

/*****************wifi moudule data inquiry end ********************/


/*****************wifi moudule assistant handle begin ********************/

unsigned char wifi_module_mode_preset(unsigned char mode)
{
	AP_assist.wifi_module_mode = mode;
}

unsigned char wifi_module_autoconn_preset(unsigned char stat)
{
	AP_assist.ap_autoconn_stat = stat;
}


/*****************wifi moudule assistant handle end ********************/

