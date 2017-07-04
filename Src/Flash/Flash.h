/*
************************************************************************************************************************
*Company	: 航天无线通信技术开发（深圳）有限公司
*Datetime 	: 2011.01.01
*Programmer	: ZhongChuanFeng
************************************************************************************************************************
*/
#ifndef _DRV_FLASH_H
#define _DRV_FLASH_H
#include "CBasicTools.h"
#include "spi_flash.h"

#define FLASH_DEL_MODE_CLR	0
#define FLASH_DEL_MODE_SET	1
#define FLASH_DEL_MODE_MOV	2
#define FLASH_MARK_FOR_UPDATE_FINISH  			0x5B
#define FLASH_MARK_FOR_SELECT_FORMAL_APP  	0x5B

//**************************************************************************************
//--------------------------- FLASH  硬件参数.---------------------------------------
#ifndef W25Q64FV
#define FLASH_ID_SST25VF080 	(0x00BF258E)		/* Flash  08Mbits  识别ID.*/
#define FLASH_ID_SST25VF016  	(0x00BF2541) 	/* Flash  16Mbits  识别ID.*/
#define FLASH_ID_SST25VF032  	(0x00BF254A) 	/* Flash  32Mbits  识别ID.*/
#define FLASH_ID_SST25VF064  	(0x00BF254B) 	/* Flash  64Mbits  识别ID.*/
#else
#define FLASH_ID_W25Q64FV_SPI		(0x00EF4017) 				/* Flash  64Mbits  识别ID.*/
#define FLASH_ID_W25Q64FV_QPI		(0x00EF6017) 				/* Flash  64Mbits  识别ID.*/
#endif

#define FLASH_ADDR_START 		(0x00000000)		/* Flash start address define.*/
#define FLASH_ADDR_END     		(0x00800000)		/* Flash end address define.*/
#define FLASH_PAGE_SIZE 		(256)			/* Flash one page size define.*/
#define FLASH_SECTOR_SIZE		(0x00001000)		/* Flash one sector size define.*/
#define FLASH_SECTOR_MARK		(0xFFFFF000)		/* Flash sector mark.*/


//**************************************************************************************
//--------------------------- 系统配置参数存储区------------------------------
#define FLASH_ADDR_SYS_PARA_MAIN			(0x00001000)
#define FLASH_ADDR_SYS_PARA_BACK			(0x00002000)

//--------------------------- 系统时钟参数存储区------------------------------
#define FLASH_ADDR_SYS_CLK_MAIN 			(0x00003000)
#define FLASH_ADDR_SYS_CLK_BACK 			(0x00004000)

//--------------------------- G P S   接口数据存储区------------------------------
#define FLASH_ADDR_GPS_INTF_MAIN			(0x00005000)
#define FLASH_ADDR_GPS_INTF_BACK			(0x00006000)

//--------------------------- 里程数据记录存储区------------------------------
#define FLASH_ADDR_MILE_SAVE_M			(0x00007000)
#define FLASH_ADDR_MILE_SAVE_B 			(0x00008000)

//--------------------------- 报警信息数据存储区------------------------------
#define FLASH_ADDR_ALM_REPORT_M 			(0x00009000)
#define FLASH_ADDR_ALM_REPORT_B 			(0x0000A000)

//--------------------------- 短信报警数据存储区------------------------------
#define FLASH_ADDR_ALM_SMS_START 		(0x0000B000)
#define FLASH_ADDR_ALM_SMS_END 			(0x0000E000)

#define FLASH_ADDR_TE_SLEEP_MAIN 			(0x0000E000)
#define FLASH_ADDR_TE_SLEEP_BCKP 			(0x0000F000)


//**************************************************************************************
//--------------------------- GSM 短信/来电存储区--------------------------------
#define FLASH_ADDR_SMS_INFO_NEW 			(0x00010000)
#define FLASH_ADDR_SMS_INFO_SAVE 			(0x00011000)
#define FLASH_ADDR_SMS_INFO_BACKUP 		(0x00012000)
#define FLASH_ADDR_CALL_INFO_SAVE 		(0x00013000)
#define FLASH_ADDR_CALL_INFO_BACKUP 		(0x00014000)
#define FLASH_ADDR_SEND_SMS_SAVE 		(0x00015000)
#define FLASH_ADDR_SEND_SMS_BACKUP 		(0x00016000)

#define FLASH_ADDR_TXT_INFO_NEW 			(0x00017000)
#define FLASH_ADDR_TXT_INFO_SAVE 			(0x00018000)
#define FLASH_ADDR_TXT_INFO_TMP 			(0x00019000)

#define FLASH_ADDR_EVENT_INFO_NEW 		(0x0001A000)
#define FLASH_ADDR_EVENT_INFO_SAVE 		(0x0001B000)
#define FLASH_ADDR_EVENT_INFO_TMP 		(0x0001C000)

#define FLASH_ADDR_QUEST_INFO_NEW 		(0x0001D000)
#define FLASH_ADDR_QUEST_INFO_SAVE 		(0x0001E000)
#define FLASH_ADDR_QUEST_INFO_TMP 		(0x0001F000)

#define FLASH_ADDR_NEWS_INFO_NEW 		(0x00020000)
#define FLASH_ADDR_NEWS_INFO_SAVE 		(0x00021000)
#define FLASH_ADDR_NEWS_INFO_TMP 		(0x00022000)

#define FLASH_ADDR_SERVE_INFO_NEW 		(0x00023000)
#define FLASH_ADDR_SERVE_INFO_SAVE 		(0x00024000)
#define FLASH_ADDR_SERVE_INFO_TMP 		(0x00025000)

#define FLASH_ADDR_EXGPS_INFO_NEW 		(0x00026000)
#define FLASH_ADDR_EXGPS_INFO_SAVE 		(0x00027000)
#define FLASH_ADDR_EXGPS_INFO_TMP 		(0x00028000)

#define FLASH_ADDR_EXOBD_INFO_NEW 		(0x00029000)
#define FLASH_ADDR_EXOBD_INFO_SAVE 		(0x0002A000)
#define FLASH_ADDR_EXOBD_INFO_TMP 		(0x0002B000)

#define FLASH_ADDR_PHOTO_ID_TMP 		(0x0002C000)


/********************驾驶员登陆登出事件保存区**************/
#define FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_START	0x0002D000
#define FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_END	0x0002F000

/********************参数修改保存区******************************/
#define FLASH_ADDR_RECORD_MODIFY_PARA_LOG_START	0x00030000
#define FLASH_ADDR_RECORD_MODIFY_PARA_LOG_END		0x00031000

/**********************外部供电记录存储区**********************/
#define FLASH_ADDR_RECORD_PWR_SUPPLY_LOG_START	0x00032000
#define FLASH_ADDR_RECORD_PWR_SUPPLY_LOG_END		0x00033000

/**********************超时驾驶记录存储区**********************/
#define FLASH_ADDR_RECORD_TIRED_DRIVER_LOG_START	0x00034000
#define FLASH_ADDR_RECORD_TIRED_DRIVER_LOG_END	0x00036000

/**********************速度状态日志存储区**********************/
#define FLASH_ADDR_RECORD_SPEED_ST_LOG_START	0x00037000
#define FLASH_ADDR_RECORD_SPEED_ST_LOG_END		0x00038000
/********************** 15min Speed Storage Block ***************************/
#define FLASH_ADDR_RECORD_SPEED_VALUE_LOG_START	0x00038000
#define FLASH_ADDR_RECORD_SPEED_VALUE_LOG_END		0x0003B000
//**************************************************************************************
//--------------------------- 临时数据暂存区------------------------------------
#define FLASH_ADDR_TMP_BUFF1				(0x0005A000)
#define FLASH_ADDR_TMP_BUFF2				(0x0005B000)


//**************************************************************************************
//--------------------------- 远程升级数据存储区------------------------------
#define FLASH_ADDR_UPDATE_MAIN			(0x0005C000)		/* 升级进程信息*/
#define FLASH_ADDR_UPDATE_BACK			(0x0005D000) 	/* 升级进程备份*/

#define FLASH_ADDR_WDLIB_PROGRM 			(0x0005E000)		/* 字库更新标志*/
#define FLASH_ADDR_UPDATE_MARK			(0x0005F000) 	/* 升级更新标志*/
#define FLASH_ADDR_UPDATE_CODE   			(0x00060000) 	/* 升级代码存储*/
#define FLASH_ADDR_UPDATE_END			(0x000A0000)		/* 升级结束地址*/
#define FLASH_SECTOR_NUM_UPDATE   		(64)		 		/* 64Sec*4K = 256KByte */


//**************************************************************************************
//--------------------------- GPS 区域数据存储区--------------------------------
#define FLASH_ADDR_AREA_CIRCLE 			(0x000A0000)
#define FLASH_ADDR_AREA_RECTGL			(0x000A1000)
#define FLASH_ADDR_AREA_POLYGN			(0x000A2000)

#define FLASH_ADDR_AREA_TMPSV			(0x000A3000)
#define FLASH_ADDR_AREA_BUFFER			(0x000A4000)
#define FLASH_ADDR_AREA_OFFSET			(0x00001000)

//--------------------------- GPS 偏航数据存储区--------------------------------
#define FLASH_ADDR_ROAD1_DATA 			(0x000B0000)
#define FLASH_ADDR_ROAD2_DATA 			(0x000B8000)
#define FLASH_ADDR_ROAD3_DATA 			(0x000C0000)
#define FLASH_ADDR_ROAD4_DATA 			(0x000C8000)
#define FLASH_ADDR_ROAD5_DATA 			(0x000D0000)
#define FLASH_ADDR_ROAD_TMPSV			(0x000D8000)
#define FLASH_ADDR_ROADT_END				(0x000E0000)
#define FLASH_SECTOR_NUM_1ROAD			(8)
#define FLASH_ADDR_ROAD_OFFSET 			(0x00008000)		/*Can suport 1300 point.*/

//0x000E0000 ~ 0x000FFFFF.


//**************************************************************************************
//--------------------------- 图片数据缓存区------------------------------------
#define FLASH_ADDR_PIC_FILE_START 		(0x00100000)
#define FLASH_ADDR_PIC_FILE_END 			(0x001FFFFF)

#define FLASH_SIZE_ONE_PIC_SIZE			(0x00008000)
#define FLASH_SECTOR_NUM_1PIC				(8)

//--------------------------- 视频音频缓存区------------------------------------
#define FLASH_ADDR_MEDIA_SAVE 			(0x00200000)
#define FLASH_ADDR_MEDIA_END 				(0x00280000)

//-----------------行驶记录仪行驶位置信存储区----------------------------
#define FLASH_ADDR_RECORD_POS_INFO_START	(0x00280000)
#define FLASH_ADDR_RECORD_POS_INFO_END		(0x002DC000)

//-----------------行驶记录仪行驶位置缓冲区----------------------------
#define FLASH_ADDR_RECORD_POS_INFO_BUF_START	(0x002DC000)
#define FLASH_ADDR_RECORD_POS_INFO_BUF_END		(0x002DE000)

//**************************************************************************************
//--------------------------- Word lib, between GB2312 and Unicode -------------------------
#define FLASH_ADDR_WDLIB_GB2UNICODE		(0x00300000) 	/*SIZE:7614*2, 4Sectors.*/
#define FLASH_ADDR_WDLIB_UNICODE2GB		(0x00304000) 	/*SIZE:64K*2=128k, 32Sectors.*/

#define FLASH_ADDR_24CHARACTER 			(0x00324000)		/*SIZE:580K.*/
#define FLASH_ADDR_24CHARACTER_A 		(0x003B5000)		/*SIZE:20K.*/
#define FLASH_ADDR_16CHARACTER 			(0x003BA000)		/*SIZE:256K.*/
#define FLASH_ADDR_16CHARACTER_A			(0x003FA000)		/*SIZE:4K.*/


//**************************************************************************************
//--------------------------- 行驶速度记录暂储区------------------------------
#define FLASH_ADDR_SPEED_RECSTART		(0x00400000)
#define FLASH_ADDR_SPEED_RECEND 			(0x0042D000)
#define FLASH_SECTOR_NUM_SPEED			(45)

//--------------------------- 事故疑点数据暂储区------------------------------
#define FLASH_ADDR_ACCIDENT_SAVE 			(0x0042D000)
#define FLASH_ADDR_ACCIDENT_END 			(0x00430000)

//--------------------------- 疲劳驾驶时间存储区------------------------------
#define FLASH_ADDR_TIREDRV_RECORD_M 		(0x00430000)
#define FLASH_ADDR_TIREDRV_RECORD_B		(0x00431000)

#define FLASH_ADDR_TIREDRV_CHECK_M 		(0x00432000)
#define FLASH_ADDR_TIREDRV_CHECK_B 		(0x00433000)

//--------------------------- 当天累驾时间存储区------------------------------
#define FLASH_ADDR_DAYDRV_TIME_M 		(0x00434000)
#define FLASH_ADDR_DAYDRV_TIME_B 			(0x00435000)

//--------------------------- 超时停车时间存储区------------------------------
#define FLASH_ADDR_STOP_TIME_MAIN		(0x00436000)
#define FLASH_ADDR_STOP_TIME_BACK		(0x00437000)

//0x00438000 ~ 0x00439000

//--------------------------- 速度数据提取暂储区------------------------------
//#define FLASH_ADDR_SPEE_Get_SAVE			(0x0043A000)
//#define FLASH_ADDR_SPEE_Get_END			(0x00440000)
#define FLASH_ADDR_CAN_UPLOAD_BUF_START 	(0x00437000)
#define FLASH_ADDR_CAN_UPLOAD_BUF_END 		(0x00440000)


//**************************************************************************************
//--------------------------- 位置信息汇报暂储区------------------------------
#define FLASH_ADDR_POST_START				(0x00440000)
#define FLASH_ADDR_POST_END					(0x00454000)//原先(0x00500000)，按需求修改为256*300，对齐再加1K防止读写地址相等

//--------------------------------------------------------------------------------------
//**************************************************************************************
//--------------------------- GPS模块升级file 暂储区------------------------------
//#define FLASH_ADDR_GPS_UPDATE_START (0x00500000)
//#define FLASH_ADDR_GPS_UPDATE_END (0x005F0000)
//#define FLASH_ADDR_GPS_UPDATE_FLG (0x005F0000)

/******行驶记录仪,行驶速度记录存储区域******/
#define FLASH_ADDR_RECORD_SPEED_START		(0x00500000)
#define FLASH_ADDR_RECORD_SPEED_END			(0x0065A000)
/****************************选择正式软件标记存储区域*************************/
#define FLASH_ADDR_SELECT_FORMAL_APP_MARK 	(0x0065A000)/*选择启动正式软件的标记*/
/****************************系统日志存储区域*************************/
#define FLASH_ADDR_SYSTEM_RECORD_START		(0x0065B000)
#define FLASH_ADDR_SYSTEM_RECORD_END			(0x0075B000)//64*16384

#define FLASH_ADDR_RECORD_ACCIDENT_START	(0x0075B000)
#define FLASH_ADDR_RECORD_ACCIDENT_END		(0x00768000)


// ------------------- 辅助服务器参数区------------------//
#define FLASH_ADDR_SECONDARY_LINK_PARA_MAIN_START	(0x00768000)
#define FLASH_ADDR_SECONDARY_LINK_PARA_MAIN_END	(0x00769000)

#define FLASH_ADDR_SECONDARY_LINK_PARA_BACKUP_START	(0x00769000)
#define FLASH_ADDR_SECONDARY_LINK_PARA_BACKUP_END		(0x0076A000)

//--------------------------------------------------------------------------------------
//**************************************************************************************

void Crc32ClkEnable(void);
void FLASH_ReadDatas(u32 addr, u8 *buff, u16 Len);
void FLASH_WriteDatas(u32 addr, u8 *buff, u16 Len);
void FLASH_Check(void);

void SpiFlashStrctWrite(void *pStruct, 
							u16 u16Size, 
							u32 u32FlashMainAdd, 
							u32 u32FlashBakckAdd);
u8 SpiFlashStrctRead(void *pStruct, 
							u16 u16Size, 
							u32 u32FlashMainAdd, 
							u32 u32FlashBakckAdd);
void FlashBlockSaveAddressReadInit(void);
void LocationInfoSaveInFlash(u8 *i_pucBuf, u16 i_wLen);
u8 LocationInfoReadFromFlash(u8 i_ucMaxReadNum,u8 *o_pucBuf,u16 *o_pwLen,u8 *o_ReadAllDataLenBuf,u8 *o_pucReadLocationInfoNum);
void SystemLogSaveInFlash(u8 *i_pucBuf, u16 i_wLen);
u8 SystemLogReadFromFlash(u8 i_ucMaxReadNum,u8 *o_pucBuf,u16 *o_pwLen);
void DeleteAllLocationInfoInFlash();
void FlashRomReadyForSystemUpdate(void);
void FlashSetMarkForSystemUpdate();
void FlashSetMarkForSelectFormalAPP();
void FlashClearMarkForSelectFormalAPP();

#endif /* _DRV_FLASH_H */

