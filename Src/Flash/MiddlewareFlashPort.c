#include"MiddlewareFlash.h"
#include "CBasicTools.h"
#include "Config.h"
/***********************************************************************
			User define start:
*************************************************************************/
void MidDebugString(char *str)
{
        DebugPrintf(CRIT"%s",str); 
}

void MidDebugPrintfHex2Ascii(unsigned char *pData, unsigned short size)
{
  // debugPrintfHex2Ascii(u8 *str, u16 size);
}

void MidwareFlaWdgRst() 
{
   // WDT_EXT_RESET; 
    IWDG_Feed(); // ι��ָ�� 
}

/************************************************************************
	Ӳ�����ȡFLASH���ݽӿ�
*************************************************************************/
void MidFLASH_ReadDatas(unsigned int addr, unsigned char *buff, unsigned short Len) {
	FLASH_ReadDatas(addr, buff, Len); 
}
/************************************************************************
	Ӳ����д��FLASH���ݽӿ�
*************************************************************************/
void MidwareFLASH_WriteDatas(unsigned int addr, unsigned char *buff, unsigned short Len) {
	FLASH_WriteDatas(addr, buff, Len); 
}
/************************************************************************
	Ӳ�������FLASH���ݽӿ�
*************************************************************************/
void MidwareSPI_FLASH_SectorErase(unsigned int addr) {
	SPI_FLASH_SectorErase(addr); 
}


