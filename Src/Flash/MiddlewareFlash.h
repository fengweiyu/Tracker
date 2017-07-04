#ifndef __MIDDLEWARE_FLASH_H__
#define __MIDDLEWARE_FLASH_H__




#ifndef NULL
#define	NULL	0
#endif





typedef struct
{
	unsigned int u32Wr; 
	unsigned int u32Rd; 
}FlashSaveAddSt; 

/**************************************************************************
  获取库版本号
  返回整数的版本号
eg.版本号为1.0.2 返回102
**************************************************************************/
unsigned short FlashMidwareLibVersion(); 

/***************************************************************************
李宏展
2014-7-3 13:52:20
函数作用,FLASH 滚动存储地址读写指针获取
输入:
	u32AddStart: FLASH存储区开始地址,结束地址
	u32AddEnd : FLASH存储区结束地址
	u16SaveMaxLen : 存储数据大小(该大小必须为2的整数次方)
				存储的有效数据最长必须小于该数值4字节.
				可以由函数FlashAreaGetMaxSaveLen来获取
	pstFlaAdd:读出的地址存放位置

输出: 1.执行结果,成功返货真,失败返货假
		2.获取到的读写指针存放在传入的指针中
调用示例:
		FlashAddRead(FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_START, 
				FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_END, 
				FlashAreaGetMaxSaveLen(sizeof(RecordDriverLogSt)), 
				&stFlaAddLogDriverEvent); 

		FlashAddRead(FLASH_ADDR_POST_START, 
					FLASH_ADDR_POST_END, 
					POST_MAX_LEN, 
					(FlashSaveAddSt *)&report_addr); 

*****************************************************************************/

unsigned char FlashAddRead(const unsigned int cu32AddStart, 
				const unsigned int cu32AddEnd, 
				const unsigned short cu16SaveMaxLen, 
				FlashSaveAddSt *pstFlaAdd); 


/***************************************************************************
李宏展
2014-7-3 13:53:46
函数作用:由有效数据长度来获得FLASH存储单位大小
				因FLASH滚动存储需要对齐
				有效数据存储时需要对齐到2的整数次方大小
				且需额外的4字节简单校验和长度

				该函数主要配合后续的数据存储封装
				可以由数据结构直接计算规范的存储空间大小
				
输入:有效数据长度
输出:该有效数据长度对应的FLASH存储单位大小
***************************************************************************/
unsigned short FlashAreaGetMaxSaveLen(unsigned short u16Len); 

/*清除数据*/
void FlashAreaReset(unsigned int u32AddStart, 
				unsigned int u32AddEnd, 
				FlashSaveAddSt *pstFlaAdd); 


void FlashAreaSave(unsigned int u32AddStart, 
					unsigned int u32AddEnd, 
					unsigned short u16SaveMaxLen, 
					FlashSaveAddSt *pstFlaAdd, 
					unsigned char *pu8SrcDat, 
					unsigned short u16Len); 

/****************************************************************************
要使一段FLASH区域中的数据设置为
无效需要FlashAreaInvailidSet配合
*******************************************************************************/
void FlashAreaRead(unsigned int u32AddStart, 
					unsigned int u32AddEnd, 
					unsigned short u16SaveMaxLen, 
					FlashSaveAddSt stFlaAdd, 
					unsigned char *pu8DstDat, 
					unsigned short *pu16Len); 

/***********************************************************************
使一段FLASH区域中的数据设置为无效,和FlashAreaRead配合
注意,如果区段非常大的话,可能要单独加喂狗
****注意,使用前一定要注意要设置无效的区域是否
	地址还有效(覆盖存储时,可能修改读指针),防止丢
	有用数据
*************************************************************************/
void FlashAreaInvailidSet(const unsigned int cu32AddStart, 
							const unsigned int cu32AddEnd, 
							const unsigned short cu16SaveMaxLen, 
							const unsigned int cu32InvailidAddS,
							const unsigned int cu32InvailidAddE); 


/**************************************
FLASH地址错误处理
向最近的上一个地址对齐
***************************************/
unsigned char FlashAddAlign(unsigned int *pu32Add, unsigned short u16Len); 

#endif

