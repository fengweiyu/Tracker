/*
************************************************************************************************************************
* 								��������ͨ�ż����������޹�˾
------------------------------------------------------------------------------------------------------------------------
* �ļ�ģ��: ESP8266�������
* ����ʱ��: 2016.05.04
* ����汾: V1.0.3
************************************************************************************************************************
*/

#include "CBasicTools.h"
#include "Config.h"
#include "Esp8266.h"
#include "Dev_wifi.h"
#include "Myqueue.h"
#include "Wifi_usr_port.h"
#include "app_wifiTask.h"

myevent*pstEveTabForWifiRx[WIFI_RX_EVENT_TAB_SIZE]; 
/*
-------------------------------------------------------
-��������:	����WIFI ָ��
-����˵��:	cmd_id, WIFI ָ��ID��
				param, ָ���������
-------------------------------------------------------
*/
e_wifi_bool app_module_cmd_issue(WifiCmdId cmd_id, unsigned char param)
{		
	if ((cmd_id < CMD_ID_CONNECT) || (cmd_id > CMD_ID_STAMAC))
	{
		return WIFI_FALSE;
	}
	else
	{
		switch (cmd_id)
		{
			case CMD_ID_LAP:
				wifi_data_init();
				break;

			case CMD_ID_VERSION:
				wifi_version_init();
				break;

			case CMD_ID_STAMAC:
				wifi_macadr_init();
				break;			
				
			case CMD_ID_MODE_SET:
				if ((param >= WIFI_MODE_STA) && (param <= WIFI_MODE_APSTA))
				{
					wifi_module_mode_preset(param);
					wifi_cmd_param_get()->mode = param;
				}
				else
				{
					return WIFI_FALSE;
				}
				
			case CMD_ID_MODE_GET:
				wifi_mode_init();
				break;

			case CMD_ID_AUTOCONN:
				if ((param >= AP_AUTO_CLOSE) && (param <= AP_AUTO_OPEN))
				{
					wifi_module_autoconn_preset(param);
					wifi_cmd_param_get()->autoconn_stat = param;
				}
				else
				{
					return WIFI_FALSE;
				}
				wifi_autoconn_init();
				break;
				
			default: break;		
		}

		if (WIFI_FALSE == cmd_queue_new_member_join(cmd_id))
		{
			return WIFI_FALSE;
		}
		
		app_wifi_start();
	}

	return WIFI_TRUE;
}

/*
-------------------------------------------------------
-��������:	���ü�������
-����˵��:	��Ҫ�ڼ�ʱ������м�ʱ���ܵĽӿں���
				�е��ã���λΪ��
-------------------------------------------------------
*/
void wifi_cmd_timeout_handle(void)
{
	st_WifiTaskParam *param;

	param = wifi_task_param_get();

	if (WIFI_TRUE == param->wifi_cmd_timer.validity)
	{
		param->wifi_cmd_timer.cnt++;

		switch(param->wifi_cmd_timer.id)
		{
			case WIFI_TIMER_CONNECT:
				
				if (0 == (param->wifi_cmd_timer.cnt%WIFI_CONNET_INTERVAL))
				{
					//MsgSend2Task(eat_get_task_id(), WIFI_TASK_ID, TRUE, 0, EAT_NULL,NULL);
				}

			case WIFI_TIMER_WORK:

				if (param->wifi_cmd_timer.cnt >= WIFI_CMD_TIMEOUT_CNT)
				{
					app_wifi_end();		
					app_wifi_cmd_fail_handle(app_wifi_cmd_id_get());
					memset(param, 0, sizeof(st_WifiTaskParam));
					//MsgSend2Task(eat_get_task_id(), WIFI_TASK_ID, TRUE, 0, EAT_NULL,NULL);					
				}
				
				break;
				
			default:	break;
		}
	}
}

extern void WifiScanHook(e_wifi_bool boResultIsOk, WifiAccessPointSt *pstWifiApBuf); 
/*
-------------------------------------------------------
-��������:	������չ�ӿڣ�ָ��ʧ�ܴ���
-����˵��:	ָ��ID
-------------------------------------------------------
*/
void app_wifi_cmd_fail_handle(WifiCmdId cmd_id)
{
	switch(cmd_id)
	{
		case CMD_ID_LAP:
			WifiScanHook(FALSE, NULL); 
			break;
			
		case CMD_ID_CONNECT:
			break;
			
		case CMD_ID_VERSION:
			break;

//����Ϊ����������������ֶ����롣
		default:	break;
	}
}

/*
-------------------------------------------------------
-��������:	������չ�ӿڣ�ָ��ɹ�����
-����˵��:	ָ��ID
-------------------------------------------------------
*/
void app_wifi_cmd_success_handle(WifiCmdId cmd_id)
{
	static WifiAccessPointSt stWifiApBuf; //static ��ֹ��ջ���
	e_wifi_bool boGetApOkFlg=WIFI_FALSE; ; 
	
	switch(cmd_id)
	{
		case CMD_ID_LAP:
			memset(&stWifiApBuf,0,sizeof(WifiAccessPointSt));
			boGetApOkFlg = app_location_ap_get(&stWifiApBuf); 
			WifiScanHook(boGetApOkFlg, &stWifiApBuf); 
			break;
			
		case CMD_ID_CONNECT:
			break;
			
		case CMD_ID_VERSION:
			break;

//����Ϊ����������������ֶ����롣
		default:	break;
	}
}

// AP �ȵ���Ϣ�ڷ�������AP ָ��󼴿ɵ��û�ȡ��
// �ȵ���Ϣ�Ƿ���ȷ��鿴����ֵ
e_wifi_bool app_location_ap_get(WifiAccessPointSt *stWifiAccessPointInfo)
{
	return app_wifi_ap_get(stWifiAccessPointInfo);
}

// ����������Ϣ��AP ����ģʽ��MAC ��ַ��
// �ڳ�ʼ��ʱ���Ѿ����û��ѯ��ϣ�
// ����ֱ�ӵ��á�
e_wifi_bool app_wifi_mode_get(unsigned char *mode)
{
	return app_module_mode_inquiry(mode);
}

e_wifi_bool app_wifi_macadr_get(unsigned char *mac_adr)
{
	return app_module_macadr_inquiry(mac_adr);
}

// �����Ҫ��ȡ����������Ϣ��
// �����ڳ�ʼ��ʱ���Ӳ�ѯָ�
// ������������ѯָ����ٻ�ȡ��
e_wifi_bool app_wifi_autoconn_get(unsigned char *stat) 
{
	return app_module_autoconn_inquiry(stat);
}

e_wifi_bool app_wifi_version_get(unsigned char *version)
{
	return app_module_version_inquiry(version);
}
	

