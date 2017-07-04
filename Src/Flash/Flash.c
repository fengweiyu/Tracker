/*
************************************************************************************************************************
*Company	: 航天无线通信技术开发（深圳）有限公司
*Datetime 	: 2011.01.01
*Programmer	: ZhongChuanFeng, modify by lihongzhan
************************************************************************************************************************
*/
#define  FLASH_GBL

#include "Config.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_crc.h"
#include "ucos_ii.h"
#include "spi_flash.h"
#include "Mymalloc.h"
#include "Myqueue.h"
#include "Flash.h"
#include "MiddlewareFlash.h"
#include "TrackerProtocol.h"
#include "app_GpsManage.h"
#include "app_TimeManage.h"

struct FlashBlockSaveInfo
{
	u32 u32AddStart; 
	u32 u32AddEnd; 
	u16 u16UnitSaveLen;
	FlashSaveAddSt *pstFlashSaveAdd; 	
}; 
static FlashSaveAddSt g_tReportAddr={0};
static FlashSaveAddSt g_tSystemLogAddr={0};
static u8   g_LocationInfoReadFromFlashBuf[TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN]={0};

struct FlashBlockSaveInfo g_tFlashBlockSaveInfoTab[] = 
{
	{FLASH_ADDR_POST_START, 
				FLASH_ADDR_POST_END, 
				TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN, 
				&g_tReportAddr}, 
	{FLASH_ADDR_SYSTEM_RECORD_START, 
				FLASH_ADDR_SYSTEM_RECORD_END, 
				LOG_SAVE_IN_FLASH_MAX_LEN, 
				&g_tSystemLogAddr}, 
};

CRC_HandleTypeDef hcrc;
/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
void FLASH_ReadDatas(u32 addr, u8 *buff, u16 Len)
{
	if((buff == NULL) ||(Len == 0) ||(addr > (FLASH_ADDR_END -1)))
	{
		return;
	}
	if((addr + Len) > (FLASH_ADDR_END-1))     
	{
		Len = (u16)(FLASH_ADDR_END - addr);
	}

	OSSchedLock();
	SPI_FLASH_BufferRead(buff, addr, Len);
	OSSchedUnlock();
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
void FLASH_WriteDatas(u32 addr, u8 *buff, u16 Len)
{
	u8 i, page_num;
	u8 head_len;
	
	if((buff == NULL) ||(Len == 0) ||(addr > (FLASH_ADDR_END -1)))
	{
		return;
	}
	if((addr + Len) > (FLASH_ADDR_END -1))
	{
	    	Len = (u16)(FLASH_ADDR_END - addr);
	}

	head_len = FLASH_PAGE_SIZE - (addr %FLASH_PAGE_SIZE);
	if((head_len > 0) && (Len > head_len))
	{
		OSSchedLock();
		SPI_FLASH_PageWrite(buff, addr, head_len);
		OSSchedUnlock();
		Len -= head_len;
		buff += head_len;
		addr += head_len;
	}
	
	page_num = (Len /FLASH_PAGE_SIZE);
	for(i = 0; i < page_num; i ++)
	{
		OSSchedLock();
		SPI_FLASH_PageWrite(buff, addr, FLASH_PAGE_SIZE);
		OSSchedUnlock();
		Len -= FLASH_PAGE_SIZE;
		buff += FLASH_PAGE_SIZE;
		addr += FLASH_PAGE_SIZE;
	}
	
	OSSchedLock();
	SPI_FLASH_PageWrite(buff, addr, Len);
	OSSchedUnlock();
}

/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
void FLASH_Check(void)
{
	u32 FLASH_JEDEC_ID;
	u8 tmp_buffer[12];

	SPI_FLASH_CS_HIGH();
	SPI_FLASH_WriteDisable(); 
	SPI_FLASH_SoDisable(); 

  	FLASH_JEDEC_ID = SPI_FLASH_ReadID();
#ifndef W25Q64FV
  	if((FLASH_JEDEC_ID == FLASH_ID_SST25VF080) ||(FLASH_JEDEC_ID == FLASH_ID_SST25VF016)
	||(FLASH_JEDEC_ID == FLASH_ID_SST25VF032) ||(FLASH_JEDEC_ID == FLASH_ID_SST25VF064))
#else
  	if( (FLASH_JEDEC_ID == FLASH_ID_W25Q64FV_SPI) || (FLASH_JEDEC_ID == FLASH_ID_W25Q64FV_QPI) )
#endif
  	{
    		DebugPrintf("Flash ID OK!\n");
  	}
  	else
  	{
    		DebugPrintf("Flash ID Err!\n");
  	}

	tmp_buffer[0] = SPI_FLASH_ReadStatus();
#ifndef W25Q64FV
	if((tmp_buffer[0] & 0xBC) != 0x00)
#else		
	if((tmp_buffer[0] & 0x1C) != 0x00)
#endif		
	{
		DebugPrintf("Flash Sector protected!\n");
		SPI_FLASH_WriteStatus(0x00);
	}
	tmp_buffer[0] = SPI_FLASH_ReadStatus();
#ifndef W25Q64FV
	if((tmp_buffer[0] & 0xBC) != 0x00)
#else		
	if((tmp_buffer[0] & 0x1C) != 0x00)
#endif		
	{
		DebugPrintf("Flash Status R Write Err!\n");
	}

	SPI_FLASH_SectorErase(FLASH_ADDR_TMP_BUFF1);
	FLASH_WriteDatas(FLASH_ADDR_TMP_BUFF1, (u8*)"Flash Test", strlen("Flash Test"));
	
	FLASH_ReadDatas(FLASH_ADDR_TMP_BUFF1, tmp_buffer, strlen("Flash Test"));
	if(memcmp(tmp_buffer, "Flash Test", strlen("Flash Test")) == 0)
	{
		DebugPrintf("Flash Read Write OK!\n");
	}
	else
	{
		DebugPrintf("Flash Read Write Err!\n");
	}
}
/**
  * 函数功能: CRC循环冗余校验初始化配置
  * 输入参数: hcrc：CRC外设句柄指针
  * 返 回 值: 无
  * 说    明: Hal库内部调用
  */
void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
{

  if(hcrc->Instance==CRC)
  {
    /* 外设时钟使能 */
    __HAL_RCC_CRC_CLK_ENABLE();
  }
}
void Crc32ClkEnable(void)
{
	hcrc.Instance = CRC;
	HAL_CRC_Init(&hcrc);
}
u32 Crc32Cal(u8 *pu8Src, u16 u16Len)//自校验用,不理会字节序
{
	u16 u16i; 
	u16 u16Tmp; 
	u32 u32Tmp; 
	u16Tmp = (u16Len >> 2); //按字计算
	HAL_CRC_Calculate(&hcrc, (u32*)(pu8Src), u16Tmp);

	u16i =  (u16Tmp << 2); 
	
	M_ClrBit(u32Tmp); 
	memcpy((u8 *)&u32Tmp, pu8Src + u16i, u16Len - u16i);  
	return HAL_CRC_Accumulate(&hcrc, &u32Tmp, 1);
}

/*************************************************
SPI FLASH  存储,
pStruct 最后四字节必须是CRC32 验证
(pStruct最后至少空4字节)
**************************************************/
void SpiFlashStrctWrite(void *pStruct, 
							u16 u16Size, 
							u32 u32FlashMainAdd, 
							u32 u32FlashBakckAdd)
{
	u8 u8i; 
	u8 u8j; 
	u8 u8SecNum; 
	u8 *pu8Tmp; 
	u8 u8NoBkFlg; 
	u8 u8RomSmallFlg; 
	u32 u32Crc; 
	u32 u32AddMax; 
	u32 u32AddMin; 
	u32 u32Add; 
	myevent stEve; 
	u8 mallocRetErr = TRUE;

	pu8Tmp = (u8 *)pStruct; 

	memset(&stEve, 0, sizeof(stEve)); 
	stEve.size = u16Size; 
	stEve.buff = mymalloc(stEve.size); 
	if(IS_NULL_P(stEve.buff)) {
		mallocRetErr = TRUE;
		DebugPrintf("struct write malloc err\r\n"); 
	}
	else {
		OSSchedLock(); 
		memcpy(stEve.buff, pu8Tmp, stEve.size); 
		OSSchedUnlock(); 
		mallocRetErr = FALSE;
		
		pu8Tmp = stEve.buff; 
	}

	if(TRUE == mallocRetErr) {
		OSSchedLock();
	}
	
	OSSchedLock(); 
	u32Crc = Crc32Cal(pu8Tmp, u16Size - sizeof(u32Crc)); 
	OSSchedUnlock(); 
	memcpy(pu8Tmp + u16Size - sizeof(u32Crc), (u8 *)&u32Crc, sizeof(u32Crc));
	u8SecNum = u16Size / FLASH_SECTOR_SIZE; 
	IS_TRUE_BIT(u16Size % FLASH_SECTOR_SIZE)
		? u8SecNum++ : 0; 
	
	u32AddMax = M_MAX(u32FlashMainAdd, u32FlashBakckAdd);
	u32AddMin = M_MIN(u32FlashMainAdd, u32FlashBakckAdd); 
	M_ClrBit(u8NoBkFlg); 
	M_ClrBit(u8RomSmallFlg); 
	if(u32AddMax != u32AddMin)	{
		(u32AddMax - u32AddMin) < u16Size ? M_SetBit(u8RomSmallFlg) : 0; 
	}
	else	{
		M_SetBit(u8NoBkFlg); 
	}

	if(IS_FALSE_BIT(u8RomSmallFlg)) {; 
	}
	else	{
		DebugPrintf("Flash rom too small\r\n"); 
	}
	
	if(IS_FALSE_BIT(u8NoBkFlg)) {; 
	}
	else	{
		DebugPrintf("Flash save no bk\r\n"); 
	}

	u32Add = u32AddMin; 
	for(u8j = 0; u8j < 2; u8j++)	{
		for(u8i = 0; u8i < u8SecNum; u8i++) {
			u16 u16WriteLen; 
			u16WriteLen = (u8i == (u8SecNum - 1) ? u16Size % FLASH_SECTOR_SIZE : FLASH_SECTOR_SIZE); 

			SPI_FLASH_SectorErase(u32Add + u8i * FLASH_SECTOR_SIZE);
			FLASH_WriteDatas(u32Add + u8i * FLASH_SECTOR_SIZE, 
								pu8Tmp + u8i * FLASH_SECTOR_SIZE, 
								u16WriteLen); 							
		}
		if(IS_FALSE_BIT(u8NoBkFlg)
		&& IS_FALSE_BIT(u8RomSmallFlg)) {
			u32Add = u32AddMax; 
		}
		else	{
			break; 
		}
	}

	if(TRUE == mallocRetErr) {
		OSSchedUnlock();
	}

	if(IS_NULL_P(stEve.buff)) {; 
	}
	else {
		myfree(stEve.buff, stEve.size); 
	}
	
}
/*************************************************
SPI FLASH  存储,
pStruct 最后四字节必须是CRC32 验证
(pStruct最后至少空4字节)
返回读取成功或失败

一个区域失败用另一个区域恢复
**************************************************/
u8 SpiFlashStrctRead(void *pStruct, 
							u16 u16Size, 
							u32 u32FlashMainAdd, 
							u32 u32FlashBakckAdd)
{
	u8 boRetVal; 
	u32 u32Crc; 
	u32 u32CrcBk; 
	u32 u32CrcRead; 
	u8 u8SecNum; 
	u8 *pu8Tmp; 
	u8 u8NoBkFlg; 
	u8 u8RomSmallFlg; 
	u32 u32AddMax; 
	u32 u32AddMin; 
	myevent stEve; 
	u8 mallocRetErr = TRUE;

	M_ClrBit(boRetVal); 

	if(NULL == pStruct) {
		return FALSE; 
	}
	else {; 
	}

	pu8Tmp = (u8 *)pStruct; 
	
	memset(&stEve, 0, sizeof(stEve)); 
	stEve.size = u16Size; 
	stEve.buff = mymalloc(stEve.size); 
	if(IS_NULL_P(stEve.buff)) {
		mallocRetErr = TRUE;
		DebugPrintf("struct read malloc err\r\n"); 
	}
	else {		
		memset(stEve.buff, 0, stEve.size); 
		pu8Tmp = stEve.buff; 
		mallocRetErr = FALSE;
	}

	u8SecNum = u16Size / FLASH_SECTOR_SIZE; 
	IS_TRUE_BIT(u16Size % FLASH_SECTOR_SIZE)
		? u8SecNum++ : 0; 
	
	u32AddMax = M_MAX(u32FlashMainAdd, u32FlashBakckAdd);
	u32AddMin = M_MIN(u32FlashMainAdd, u32FlashBakckAdd); 
	M_ClrBit(u8NoBkFlg); 
	M_ClrBit(u8RomSmallFlg); 
	if(u32AddMax != u32AddMin)	{
		(u32AddMax - u32AddMin) < u16Size ? M_SetBit(u8RomSmallFlg) : 0; 
	}
	else	{
		M_SetBit(u8NoBkFlg); 
	}

	if(IS_FALSE_BIT(u8RomSmallFlg)) {; 
	}
	else	{
		DebugPrintf("Flash rom too small\r\n"); 
	}
	
	if(IS_FALSE_BIT(u8NoBkFlg)) {; 
	}
	else	{
		DebugPrintf("Flash save no bk\r\n"); 
	}

	if(TRUE == mallocRetErr) {
		OSSchedLock();
	}

	FLASH_ReadDatas(u32AddMin, pu8Tmp, u16Size);
	u32Crc = Crc32Cal(pu8Tmp, u16Size - sizeof(u32Crc)); 
	memcpy((u8 *)&u32CrcRead, pu8Tmp + u16Size - sizeof(u32Crc), sizeof(u32Crc)); 
	if(u32Crc != u32CrcRead) {
		DebugPrintf("Spi flash read error 1\r\n"); 
		if(IS_TRUE_BIT(u8NoBkFlg) || IS_TRUE_BIT(u8RomSmallFlg)) {
			M_ClrBit(boRetVal); 
		}
		else	{
			FLASH_ReadDatas(u32AddMax, pu8Tmp, u16Size);
			u32Crc = Crc32Cal(pu8Tmp, u16Size - sizeof(u32Crc)); 
			memcpy((u8 *)&u32CrcRead, pu8Tmp + u16Size - sizeof(u32Crc), sizeof(u32Crc)); 
			if(u32Crc != u32CrcRead) {
				DebugPrintf("Spi flash read error 2\r\n"); 
				M_ClrBit(boRetVal); 
			}
			else	{
				SpiFlashStrctWrite(pu8Tmp, u16Size, u32AddMin, u32AddMin); 
				DebugPrintf("Spi flash recover ok\r\n"); 
				M_SetBit(boRetVal); 
			}
		}
	}
	else {
		if(IS_TRUE_BIT(u8NoBkFlg) || IS_TRUE_BIT(u8RomSmallFlg)) {; 
		}
		else	{
			FLASH_ReadDatas(u32AddMax, pu8Tmp, u16Size);
			u32CrcBk = Crc32Cal(pu8Tmp, u16Size - sizeof(u32Crc)); 
			memcpy((u8 *)&u32CrcRead, pu8Tmp + u16Size - sizeof(u32Crc), sizeof(u32Crc)); 
			if(u32CrcBk != u32CrcRead
			|| u32CrcBk != u32Crc)	{
				DebugPrintf("Spi flash read error 3,recover\r\n"); 
				FLASH_ReadDatas(u32AddMin, pu8Tmp, u16Size);
				SpiFlashStrctWrite(pu8Tmp, u16Size, u32AddMax, u32AddMax); 
			}
			else	{
				DebugPrintf("Spi flash read ok\r\n"); 
			}
		}
		M_SetBit(boRetVal); 
	}

	if(TRUE == mallocRetErr) {
		OSSchedUnlock();
	}

	if(IS_NULL_P(stEve.buff)) {; 
	}
	else {
		OSSchedLock(); 
		memcpy((u8 *)pStruct, stEve.buff, u16Size); 
		OSSchedUnlock(); 
		
		myfree(stEve.buff, stEve.size); 
	}

	return boRetVal; 
}
/*****************************************************************************
-Fuction		: FlashBlockSaveAddressReadInit
-Description	: FlashBlockSaveAddressReadInit
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void FlashBlockSaveAddressReadInit(void)
{
	u8 u8i; 
	u8 u8Num; 
	struct FlashBlockSaveInfo *pstInfo; 

	u8Num = sizeof(g_tFlashBlockSaveInfoTab) / sizeof(struct FlashBlockSaveInfo);
	for(u8i = 0; u8i < u8Num; u8i++) {
		pstInfo = &g_tFlashBlockSaveInfoTab[u8i]; 
		
		FlashAddRead(pstInfo->u32AddStart, 
						pstInfo->u32AddEnd, 
						FlashAreaGetMaxSaveLen(pstInfo->u16UnitSaveLen), 
						pstInfo->pstFlashSaveAdd); 
	}

}
/*****************************************************************************
-Fuction		: SystemLogSaveInFlash
-Description	: SystemLogSaveInFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void SystemLogSaveInFlash(u8 *i_pucBuf, u16 i_wLen)
{
	u8 *pucBuf=i_pucBuf;
	u16 wLen;
	static u8 ucSaveInFlashBuf[LOG_SAVE_IN_FLASH_MAX_LEN-4]={0};//留四个余量防止错误
	u8 ucSaveInFlashLen=0;	
	u16 wYear=0;
	u8   ucStrTmpLen=0;
	datetime_t tDataTime={0};
	g_tTime.GetDateTime(&tDataTime);
	wYear=tDataTime.date.year+2000;
	ucStrTmpLen = snprintf( (s8 *)ucSaveInFlashBuf,21, "%04d-%02d-%02d %02d:%02d:%02d ",
						wYear,
						tDataTime.date.month,
						tDataTime.date.day,
						tDataTime.time.hour,				
						tDataTime.time.minute,
						tDataTime.time.second);
	ucSaveInFlashLen=(sizeof(ucSaveInFlashBuf)-ucStrTmpLen-1);
	if(i_wLen>ucSaveInFlashLen)
	{
		memcpy(ucSaveInFlashBuf+ucStrTmpLen,pucBuf,ucSaveInFlashLen);
		ucSaveInFlashBuf[ucSaveInFlashLen+ucStrTmpLen]='\0';
		wLen=strnlen(ucSaveInFlashBuf,sizeof(ucSaveInFlashBuf));
		OSSchedLock(); 
		FlashAreaSave(FLASH_ADDR_SYSTEM_RECORD_START, 
			FLASH_ADDR_SYSTEM_RECORD_END, 
			LOG_SAVE_IN_FLASH_MAX_LEN, 
			&g_tSystemLogAddr, 
			ucSaveInFlashBuf,
			wLen); 
		OSSchedUnlock(); 
		i_wLen=i_wLen-ucSaveInFlashLen;
		if(i_wLen>ucSaveInFlashLen)//保留剩余长度，最多两次也就是39+39
		{
			memcpy(ucSaveInFlashBuf+ucStrTmpLen,pucBuf+ucSaveInFlashLen,ucSaveInFlashLen);
			ucSaveInFlashBuf[ucSaveInFlashLen+ucStrTmpLen]='\0';
			wLen=strnlen(ucSaveInFlashBuf,sizeof(ucSaveInFlashBuf));
			OSSchedLock(); 
			FlashAreaSave(FLASH_ADDR_SYSTEM_RECORD_START, 
				FLASH_ADDR_SYSTEM_RECORD_END, 
				LOG_SAVE_IN_FLASH_MAX_LEN, 
				&g_tSystemLogAddr, 
				ucSaveInFlashBuf,
				wLen); 
			OSSchedUnlock(); 
		}
		else
		{
			memcpy(ucSaveInFlashBuf+ucStrTmpLen,pucBuf+ucSaveInFlashLen,i_wLen);
			ucSaveInFlashBuf[i_wLen+ucStrTmpLen]='\0';
			wLen=strnlen(ucSaveInFlashBuf,sizeof(ucSaveInFlashBuf));
			OSSchedLock(); 
			FlashAreaSave(FLASH_ADDR_SYSTEM_RECORD_START, 
				FLASH_ADDR_SYSTEM_RECORD_END, 
				LOG_SAVE_IN_FLASH_MAX_LEN, 
				&g_tSystemLogAddr, 
				ucSaveInFlashBuf,
				wLen); 
			OSSchedUnlock(); 
		}
	}
	else
	{
		memcpy(ucSaveInFlashBuf+ucStrTmpLen,pucBuf,i_wLen);
		ucSaveInFlashBuf[i_wLen+ucStrTmpLen]='\0';
		wLen=strnlen(ucSaveInFlashBuf,sizeof(ucSaveInFlashBuf));
		OSSchedLock(); 
		FlashAreaSave(FLASH_ADDR_SYSTEM_RECORD_START, 
			FLASH_ADDR_SYSTEM_RECORD_END, 
			LOG_SAVE_IN_FLASH_MAX_LEN, 
			&g_tSystemLogAddr, 
			ucSaveInFlashBuf,
			wLen); 
		OSSchedUnlock(); 
	}
}
/*****************************************************************************
-Fuction		: SystemLogReadFromFlash
-Description	: SystemLogReadFromFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
u8 SystemLogReadFromFlash(u8 i_ucMaxReadNum,u8 *o_pucBuf,u16 *o_pwLen)
{
	u8 ret=FALSE;
	u8 c=0;
	u8 ucCount=0;
	u8 *LocationInfoBuf=o_pucBuf;
	u32 dwReadAddress=0;
	u32 dwReadAddressStart=0;
	FlashSaveAddSt tLocationInfoFlashAddr={0};
	u16 wReadFromFlashDataLen=0;
	u16 wReadAllDataLen=0;
	s8 cDataStr;
	u16 wLen;
	
	OSSchedLock();
	tLocationInfoFlashAddr=g_tSystemLogAddr;
	OSSchedUnlock(); 
	if((NULL==o_pucBuf)||(NULL==o_pwLen)||
	    (tLocationInfoFlashAddr.u32Rd==tLocationInfoFlashAddr.u32Wr))
	{
		ret=FALSE;
	}
	else
	{
		OSSchedLock(); 		
		if(IS_TRUE_BIT(FlashAddAlign(&g_tSystemLogAddr.u32Rd, LOG_SAVE_IN_FLASH_MAX_LEN))
		&& IS_TRUE_BIT(FlashAddAlign(&g_tSystemLogAddr.u32Wr, LOG_SAVE_IN_FLASH_MAX_LEN))) 
		{;
		}
		else 
		{
			DebugPrintf(ERR"SystemLog add err\r\n");
		}
		if(g_tSystemLogAddr.u32Rd< FLASH_ADDR_SYSTEM_RECORD_START)
		{
			g_tSystemLogAddr.u32Rd = FLASH_ADDR_SYSTEM_RECORD_START;
		}
		else if(g_tSystemLogAddr.u32Rd >= FLASH_ADDR_SYSTEM_RECORD_END)
		{
			g_tSystemLogAddr.u32Rd = FLASH_ADDR_SYSTEM_RECORD_START;
		}
		else
		{
		}
		dwReadAddress=g_tSystemLogAddr.u32Rd;
		dwReadAddressStart=dwReadAddress;
		tLocationInfoFlashAddr=g_tSystemLogAddr;
		OSSchedUnlock(); 
		for(c=0;c<i_ucMaxReadNum;c++)
		{
			tLocationInfoFlashAddr.u32Rd=dwReadAddress;
			wReadFromFlashDataLen=0;
			FlashAreaRead(FLASH_ADDR_SYSTEM_RECORD_START,
							FLASH_ADDR_SYSTEM_RECORD_END,
							LOG_SAVE_IN_FLASH_MAX_LEN, 
							tLocationInfoFlashAddr, 
							LocationInfoBuf,
							&wReadFromFlashDataLen); 
			if(0==wReadFromFlashDataLen)
			{			
			}
			else
			{
				LocationInfoBuf+=wReadFromFlashDataLen;
				wReadAllDataLen+=wReadFromFlashDataLen;
				ucCount++;
			}
			dwReadAddress+=LOG_SAVE_IN_FLASH_MAX_LEN;
			if(dwReadAddress<FLASH_ADDR_SYSTEM_RECORD_END)
			{
			}
			else
			{
				dwReadAddress=FLASH_ADDR_SYSTEM_RECORD_START;
			}
			if(dwReadAddress!=tLocationInfoFlashAddr.u32Wr)
			{
			}
			else
			{
				break;
			}
		}
		*o_pwLen=wReadAllDataLen;
		OSSchedLock(); 
		if(g_tSystemLogAddr.u32Rd!= dwReadAddressStart) //写覆盖时可能修改R_ADDR,已经使数据无效
		{; 
		}
		else 
		{			
			FlashAreaInvailidSet(FLASH_ADDR_SYSTEM_RECORD_START, 
								FLASH_ADDR_SYSTEM_RECORD_END, 
								LOG_SAVE_IN_FLASH_MAX_LEN, 
								dwReadAddressStart, 
								dwReadAddress); 
			g_tSystemLogAddr.u32Rd= dwReadAddress; 
		}		
		OSSchedUnlock();
		if(0== wReadAllDataLen)
		{
			ret=FALSE;
		}
		else
		{
			ret=TRUE;
		}
	}
	return ret;
}

/*****************************************************************************
-Fuction		: LocationInfoSaveInFlash
-Description	: LocationInfoSaveInFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void LocationInfoSaveInFlash(u8 *i_pucBuf, u16 i_wLen)
{
	u8 *pucBuf=i_pucBuf;
	s8 cDataStr;
	u16 wLen=i_wLen;
	OSSchedLock(); 
	FlashAreaSave(FLASH_ADDR_POST_START, 
		FLASH_ADDR_POST_END, 
		TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN, 
		&g_tReportAddr, 
		i_pucBuf,
		i_wLen); 
	OSSchedUnlock(); 
	/*while(IS_TRUE_BIT(wLen)) {
	cDataStr = HexToAscii( (*pucBuf) >> 4 );
	DebugDataSend((u8 *)(&cDataStr),1);
	cDataStr = HexToAscii( (*pucBuf) & 0xF);
	DebugDataSend((u8 *)(&cDataStr),1);
	wLen--;
	pucBuf++;
	}
	DebugPrintf("\r\n");*/
}
/*****************************************************************************
-Fuction		: LocationInfoReadFromFlash
-Description	: LocationInfoReadFromFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
u8 LocationInfoReadFromFlash(u8 i_ucMaxReadNum,u8 *o_pucBuf,u16 *o_pwLen,u8 *o_ReadAllDataLenBuf,u8 *o_pucReadLocationInfoNum)
{
	u8 ret=FALSE;
	u8 c=0;
	u8 ucCount=0;
	u8 *LocationInfoBuf=o_pucBuf;
	u8 ReadAllDataLenBuf[TRACKER_PROTOCOL_HISTORY_LOCATION_INFO_PACK_NUM]={0};
	u32 dwReadAddress=0;
	u32 dwReadAddressStart=0;
	FlashSaveAddSt tLocationInfoFlashAddr={0};
	u16 wReadFromFlashDataLen=0;	
	u16 wReadFromFlashDataCheckLen=0;
	u16 wReadAllDataLen=0;
	u8 *pucBuf=g_LocationInfoReadFromFlashBuf;
	s8 cDataStr;
	u16 wLen;
	
	OSSchedLock();
	tLocationInfoFlashAddr=g_tReportAddr;
	OSSchedUnlock(); 
	if((NULL==o_pucBuf)||(NULL==o_pwLen)||(NULL==o_pucReadLocationInfoNum)||(NULL==o_ReadAllDataLenBuf)||
	    (tLocationInfoFlashAddr.u32Rd==tLocationInfoFlashAddr.u32Wr))
	{
		ret=FALSE;
	}
	else
	{
		OSSchedLock(); 		
		if(IS_TRUE_BIT(FlashAddAlign(&g_tReportAddr.u32Rd, TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN))
		&& IS_TRUE_BIT(FlashAddAlign(&g_tReportAddr.u32Wr, TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN))) 
		{;
		}
		else 
		{
			DebugPrintf(ERR"History add err\r\n");
		}
		if(g_tReportAddr.u32Rd< FLASH_ADDR_POST_START)
		{
			g_tReportAddr.u32Rd = FLASH_ADDR_POST_START;
		}
		else if(g_tReportAddr.u32Rd >= FLASH_ADDR_POST_END)
		{
			g_tReportAddr.u32Rd = FLASH_ADDR_POST_START;
		}
		else
		{
		}
		dwReadAddress=g_tReportAddr.u32Rd;
		dwReadAddressStart=dwReadAddress;
		tLocationInfoFlashAddr=g_tReportAddr;
		OSSchedUnlock(); 
		for(c=0;c<i_ucMaxReadNum;c++)
		{
			tLocationInfoFlashAddr.u32Rd=dwReadAddress;
			memset(g_LocationInfoReadFromFlashBuf,0,sizeof(g_LocationInfoReadFromFlashBuf));
			wReadFromFlashDataLen=0;
			FlashAreaRead(FLASH_ADDR_POST_START,
							FLASH_ADDR_POST_END,
							TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN, 
							tLocationInfoFlashAddr, 
							g_LocationInfoReadFromFlashBuf,
							&wReadFromFlashDataLen); 
			if(0==wReadFromFlashDataLen)
			{			
			}
			else
			{
				if(wReadFromFlashDataLen<=37)//信息最小长度38
				{	
					DebugPrintf(ERR"ReadFlashDataErrLen:%d\r\n",wReadFromFlashDataLen);
				}
				else
				{
					wReadFromFlashDataCheckLen=30+1+1+1;//根据协议来的数据长度
					if(1==g_LocationInfoReadFromFlashBuf[31])//如果有WIFI信息
					{
						wReadFromFlashDataCheckLen+=g_LocationInfoReadFromFlashBuf[32];
						wReadFromFlashDataCheckLen+=2;
						wReadFromFlashDataCheckLen+=g_LocationInfoReadFromFlashBuf[wReadFromFlashDataCheckLen-1];
					}
					else//基站信息
					{
						wReadFromFlashDataCheckLen+=g_LocationInfoReadFromFlashBuf[32];
					}
					wReadFromFlashDataCheckLen+=4;
				}
				if(wReadFromFlashDataLen!=wReadFromFlashDataCheckLen)
				{
					DebugPrintf(ERR"ReadFlashDataErr:Len%d,CheckLen:%d\r\n",wReadFromFlashDataLen,wReadFromFlashDataCheckLen);
				}
				else
				{
					DebugPrintf(ERR"ReadFromFlashDataLen:%d\r\n",wReadFromFlashDataLen);//防止打印过多
					memcpy(LocationInfoBuf,g_LocationInfoReadFromFlashBuf,wReadFromFlashDataLen);	
					LocationInfoBuf+=wReadFromFlashDataLen;
					wReadAllDataLen+=wReadFromFlashDataLen;
					ReadAllDataLenBuf[ucCount]=wReadFromFlashDataLen;
					ucCount++;
				}
				
				/*wLen=wReadFromFlashDataLen;
				while(IS_TRUE_BIT(wLen)) {
				cDataStr = HexToAscii( (*pucBuf) >> 4 );
				DebugDataSend((u8 *)(&cDataStr),1);
				cDataStr = HexToAscii( (*pucBuf) & 0xF);
				DebugDataSend((u8 *)(&cDataStr),1);
				wLen--;
				pucBuf++;
				}
				DebugPrintf("\r\n");*/
			}
			dwReadAddress+=TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN;
			if(dwReadAddress<FLASH_ADDR_POST_END)
			{
			}
			else
			{
				dwReadAddress=FLASH_ADDR_POST_START;
			}
			if(dwReadAddress!=tLocationInfoFlashAddr.u32Wr)
			{
			}
			else
			{
				break;
			}
		}
		*o_pucReadLocationInfoNum=ucCount;
		*o_pwLen=wReadAllDataLen;
		memcpy(o_ReadAllDataLenBuf,ReadAllDataLenBuf,ucCount);
		OSSchedLock(); 
		if(g_tReportAddr.u32Rd!= dwReadAddressStart) //写覆盖时可能修改R_ADDR,已经使数据无效
		{; 
		}
		else 
		{			
			FlashAreaInvailidSet(FLASH_ADDR_POST_START, 
								FLASH_ADDR_POST_END, 
								TRACKER_PROTOCOL_LOCATION_INFO_SAVE_MAX_LEN, 
								dwReadAddressStart, 
								dwReadAddress); 
			g_tReportAddr.u32Rd= dwReadAddress; 
		}		
		OSSchedUnlock();
		if(0== wReadAllDataLen)
		{
			ret=FALSE;
		}
		else
		{
			ret=TRUE;
		}
	}
	return ret;
}
/*****************************************************************************
-Fuction		: DeleteAllLocationInfoInFlash
-Description	: DeleteAllLocationInfoInFlash
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void DeleteAllLocationInfoInFlash()
{
	OSSchedLock(); 
	FlashAreaReset(FLASH_ADDR_POST_START,
					FLASH_ADDR_POST_END,
					&g_tReportAddr);
	FlashAreaReset(FLASH_ADDR_SYSTEM_RECORD_START,
					FLASH_ADDR_SYSTEM_RECORD_END,
					&g_tSystemLogAddr);
	OSSchedUnlock(); 
	g_tGPS.GpsClearBackupLocationInfo();
}
/*****************************************************************************
-Fuction		: FlashRomReadyForSystemUpdate
-Description	: FlashRomReadyForSystemUpdate
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void FlashRomReadyForSystemUpdate(void)
{
	u32 flash_addr=FLASH_ADDR_UPDATE_CODE;
	SPI_FLASH_SectorErase(FLASH_ADDR_UPDATE_MARK);
	while(flash_addr < FLASH_ADDR_UPDATE_END)
	{
		SPI_FLASH_SectorErase(flash_addr);
		flash_addr += FLASH_SECTOR_SIZE;
		IWDG_Feed();
	}
}
/*****************************************************************************
-Fuction		: FlashSetMarkForSystemUpdate
-Description	: FlashSetMarkForSystemUpdate
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19    V1.0.0		Yu Weifeng 	Created
******************************************************************************/
void FlashSetMarkForSystemUpdate()
{
	u8 update_flag = FLASH_MARK_FOR_UPDATE_FINISH;
	
	SPI_FLASH_SectorErase(FLASH_ADDR_UPDATE_MARK);
	FLASH_WriteDatas(FLASH_ADDR_UPDATE_MARK, &update_flag, 1);
	FLASH_ReadDatas(FLASH_ADDR_UPDATE_MARK, &update_flag, 1);
	if(update_flag != FLASH_MARK_FOR_UPDATE_FINISH)
	{
		DebugPrintf(ERR"Update finish err!\r\n");
	}
	else
	{
	}
}
/*****************************************************************************
-Fuction		: FlashSetMarkForSystemUpdate
-Description	: FlashSetMarkForSystemUpdate
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19	V1.0.0		Yu Weifeng	Created
******************************************************************************/
void FlashSetMarkForSelectFormalAPP()
{
	u8 update_flag = FLASH_MARK_FOR_SELECT_FORMAL_APP;
	
	SPI_FLASH_SectorErase(FLASH_ADDR_SELECT_FORMAL_APP_MARK);
	FLASH_WriteDatas(FLASH_ADDR_SELECT_FORMAL_APP_MARK, &update_flag, 1);
	FLASH_ReadDatas(FLASH_ADDR_SELECT_FORMAL_APP_MARK, &update_flag, 1);
	if(update_flag != FLASH_MARK_FOR_SELECT_FORMAL_APP)
	{
		DebugPrintf(EMERG"SetMarkForSelectFormalAPP err!\r\n");
	}
	else
	{
	}
}
/*****************************************************************************
-Fuction		: FlashSetMarkForSystemUpdate
-Description	: FlashSetMarkForSystemUpdate
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		Author			Modification
* -----------------------------------------------
* 2016/10/19	V1.0.0		Yu Weifeng	Created
******************************************************************************/
void FlashClearMarkForSelectFormalAPP()
{
	u8 update_flag = 0;
	
	SPI_FLASH_SectorErase(FLASH_ADDR_SELECT_FORMAL_APP_MARK);
	FLASH_WriteDatas(FLASH_ADDR_SELECT_FORMAL_APP_MARK, &update_flag, 1);
	FLASH_ReadDatas(FLASH_ADDR_SELECT_FORMAL_APP_MARK, &update_flag, 1);
	if(update_flag != 0)
	{
		DebugPrintf(EMERG"ClearMarkForSelectFormalAPP err!\r\n");
	}
	else
	{
	}
}




