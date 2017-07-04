/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 main
* Description		: 	main.h
* Created			: 	2016.08.26.
* Author			: 	Yu Weifeng
* Function List 		: 	none
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H

//#include "app_GpsManage.h"
#include "app_UsbManage.h"

#define ROM_APP_START_ADDR 	0xC000
#define NVIC_VECT_TAB_FLASH	((uint32_t)0x08000000)
//#define DebugPrintf(...)  
#define DebugPrintf  		g_tUsb.DbgPrintf
#define DebugDataSend		g_tUsb.DbgSend
//#define DebugDataSend(...)


#define EMERG   		 "<0>"    /* system is unusable */
#define ALERT   		 "<1>"    /* action must be taken immediately */
#define CRIT   		 "<2>"    /* critical conditions */
#define ERR    		 "<3>"    /* error conditions */
#define WARNING	 "<4>"    /* warning conditions */
#define NOTICE   		 "<5>"    /* normal but significant */
#define INFO     		 "<6>"    /* informational */
#define DEBUG   		 "<7>"    /* debug-level messages */

#define DEFAULT_DBGLEVEL 					4
#define DBG_LEVEL_LIMIT 					8
#define SAVE_IN_FLASH_DBG_LEVEL_LIMIT 	3

#define MCU_RUN_FREQUENCY				(42)

#define MAX_NAME_SIZE						(30)
#define LOG_SAVE_IN_FLASH_MAX_LEN		(64)
#endif
