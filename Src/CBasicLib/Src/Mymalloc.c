/*
************************************************************************************************************************
* 								航天无线通信技术开发有限公司
------------------------------------------------------------------------------------------------------------------------
* 文件模块: 内存动态管理模块
* 文件功能: 内存分配管理
* 开发时间: 
* 软件设计: XiaoBao.Hu      Modified by ChuanFeng.Zhong
************************************************************************************************************************
*/
#include "CBasicTools.h"
#include "Mymalloc.h"
#include "Ucos_ii.h"
#pragma pack(1)
typedef unsigned char * (*get_ram_handle_t)(void);
typedef struct
{
	unsigned short ram_size;
	get_ram_handle_t handler; 
}get_ram_menu_t;
#pragma pack()

#define RAM_EMPTY_MARK  	0xEE
#define RAM_USED_MARK		0xAA
#define RAM_COLOR_EMPTY	0x55

#define MAX_RAM_ITEM   		(6)

#define RAM_SIZE_12       		(12 + 1)
#define RAM_SIZE_128    		(132 + 1)		//为GPS(130)缓存数据增加128内存大小
#define RAM_SIZE_256    		(256 + 1)
#define RAM_SIZE_512    		(528 + 1)
#define RAM_SIZE_1024   		(1256 + 1)
#define RAM_SIZE_2048   		(2400 + 1)		//必须大于GSM_IP_SEND_MAX_DAT_LEN
#define SIZE_12_NUM      		(50)
#define SIZE_128_NUM    		(20)
#define SIZE_256_NUM    		(8)
#define SIZE_512_NUM    		(3)
#define SIZE_1024_NUM   		(2)
#define SIZE_2048_NUM   		(3)
static unsigned char * get_ram_12(void);
static unsigned char * get_ram_128(void);
static unsigned char * get_ram_256(void);
static unsigned char * get_ram_512(void);
static unsigned char * get_ram_1024(void);
static unsigned char * get_ram_2048(void);

static unsigned char ram_size_12[SIZE_12_NUM][RAM_SIZE_12 +1];
static unsigned char ram_size_128[SIZE_128_NUM][RAM_SIZE_128 +1];
static unsigned char ram_size_256[SIZE_256_NUM][RAM_SIZE_256 +1];
static unsigned char ram_size_512[SIZE_512_NUM][RAM_SIZE_512 +1];
static unsigned char ram_size_1024[SIZE_1024_NUM][RAM_SIZE_1024 +1];
static unsigned char ram_size_2048[SIZE_2048_NUM][RAM_SIZE_2048 +1];

const get_ram_menu_t get_ram_menu[MAX_RAM_ITEM] =
{
	{RAM_SIZE_12, get_ram_12},
	{RAM_SIZE_128, get_ram_128},
	{RAM_SIZE_256, get_ram_256},
	{RAM_SIZE_512, get_ram_512},
	{RAM_SIZE_1024, get_ram_1024},
	{RAM_SIZE_2048, get_ram_2048}
};


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
typedef struct
{
	u8 *pcu8UnitRamStartAdd; 
	u8 u8UnitRamNum; 
	u16 u16UnitRamSize; 
} UnitRamSt; 

const UnitRamSt cstUnitRamTab[] = 
{
	{ram_size_12[0], SIZE_12_NUM, sizeof(ram_size_12[0])}, 
	{ram_size_128[0], SIZE_128_NUM, sizeof(ram_size_128[0])}, 
	{ram_size_256[0], SIZE_256_NUM, sizeof(ram_size_256[0])}, 
	{ram_size_512[0], SIZE_512_NUM, sizeof(ram_size_512[0])}, 
	{ram_size_1024[0], SIZE_1024_NUM, sizeof(ram_size_1024[0])}, 
	{ram_size_2048[0], SIZE_2048_NUM, sizeof(ram_size_2048[0])}, 
}; 

const u8 cu8UnitRamTabNum = sizeof(cstUnitRamTab) / sizeof(UnitRamSt); 
#define GET_ADD_IN_UNIT_SIZE_RAM(pu8UnitRamStartAdd, u16RamSize, u8Index) \
									((u8 *)(pu8UnitRamStartAdd + u16RamSize * u8Index))

void MAB_MallocInit(void)
{
	u8 u8i; 
	u8 u8j; 
	u8 *pu8Tmp;
	for(u8i = 0; u8i < cu8UnitRamTabNum; u8i++) {
		for(u8j = 0; u8j < cstUnitRamTab[u8i].u8UnitRamNum; u8j++) {
			pu8Tmp = GET_ADD_IN_UNIT_SIZE_RAM(cstUnitRamTab[u8i].pcu8UnitRamStartAdd, 
												 cstUnitRamTab[u8i].u16UnitRamSize, 
												 u8j); 
			pu8Tmp[0] = RAM_EMPTY_MARK; 
		}
	}		
}

unsigned char * mymalloc(unsigned int Size)
{
	unsigned char *Ram_Ptr;
	unsigned char i;
	OS_CPU_SR	cpu_sr; 
	for(i = 0, Ram_Ptr = NULL; i < MAX_RAM_ITEM; i ++)
	{
		if(get_ram_menu[i].ram_size >= Size)
		{
			OS_ENTER_CRITICAL();
			Ram_Ptr = get_ram_menu[i].handler();
			OS_EXIT_CRITICAL();
			
			if(NULL != Ram_Ptr) return(Ram_Ptr);
		}
	}
	return(NULL);
}


u8 IsVaildAdd(u8 *pu8Add)
{
	u8 boRetVal; 
	u8 u8i; 
	u8 u8j; 
	u8 *pu8Tmp;
	M_ClrBit(boRetVal); 
	for(u8i = 0; u8i < cu8UnitRamTabNum; u8i++) {
		for(u8j = 0; u8j < cstUnitRamTab[u8i].u8UnitRamNum; u8j++) {
			pu8Tmp = GET_ADD_IN_UNIT_SIZE_RAM(cstUnitRamTab[u8i].pcu8UnitRamStartAdd, 
												 cstUnitRamTab[u8i].u16UnitRamSize, 
												 u8j); 
			if(pu8Tmp != pu8Add) {; 
			}
			else {
				M_SetBit(boRetVal); 
				break; 
			}
		}
	}		

	return boRetVal; 
}

void myfree(unsigned char *buff, unsigned int Size)
{
	OS_CPU_SR	cpu_sr; 
	if(NULL == buff)
	{
		return; 
	}
	else
	{; }
	
	memset(buff, RAM_COLOR_EMPTY, Size);//此操作一定要在释放内存之前,或者取消此操作
#if LHZTEST//释放内存错误检查
{
	u16 u16i;
	u8 *pu8Tmp; 
	for(u16i = 0; u16i < Size; u16i++) {
		pu8Tmp = buff + u16i; 
		if(IsVaildAdd(pu8Tmp)) {; 
			DebugPrintf(ERR"Err:mem free Err\r\n"); 
		}
		else {
		}
	}	
}


	u8 *pu8Buf; 
	pu8Buf = (u8 *)(buff - 1); 
	if(IsVaildAdd(pu8Buf)) {; 
	}
	else {
		DebugPrintf(ERR"Err:mem free Err\r\n"); 
	}
	
#endif
	
	OS_ENTER_CRITICAL();
	*((u8 *)buff - 1) = RAM_EMPTY_MARK; 
	OS_EXIT_CRITICAL();

}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
static unsigned char * get_ram_12(void)
{
	unsigned char i;
	
	for(i = 0; i < SIZE_12_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_12[i][0])
		{
			ram_size_12[i][0] = RAM_USED_MARK;
			return(ram_size_12[i]+1);
		}
	}
	return(NULL);
}

static unsigned char * get_ram_128(void)
{
	unsigned char i;

	for(i = 0; i < SIZE_128_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_128[i][0])
		{
			ram_size_128[i][0] = RAM_USED_MARK;
			return(ram_size_128[i]+1);
		}
	}
	return(NULL);
}

static unsigned char * get_ram_256(void)
{
	unsigned char i;

	for(i = 0; i < SIZE_256_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_256[i][0])
		{
			ram_size_256[i][0] = RAM_USED_MARK;
			return(ram_size_256[i]+1);
		}
	}
	return(NULL);
}

static unsigned char * get_ram_512(void)
{
	unsigned char i;

	for(i = 0; i < SIZE_512_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_512[i][0])
		{
			ram_size_512[i][0] = RAM_USED_MARK;
			return(ram_size_512[i]+1);
		}
	}
	return(NULL);
}

static unsigned char * get_ram_1024(void)
{
	unsigned char i;
	
	for(i = 0; i < SIZE_1024_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_1024[i][0])
		{
			ram_size_1024[i][0] = RAM_USED_MARK;
			return(ram_size_1024[i]+1);
		}
	}
	return(NULL);
}

static unsigned char * get_ram_2048(void)
{
	unsigned char i;
	
	for(i = 0; i < SIZE_2048_NUM; i ++)
	{
		if(RAM_EMPTY_MARK == ram_size_2048[i][0])
		{
			ram_size_2048[i][0] = RAM_USED_MARK;
			return(ram_size_2048[i]+1);
		}
	}
	return(NULL);
}


/*
-------------------------------------------------------------------------------------------------
-函数功能:	
-参数说明:	
-------------------------------------------------------------------------------------------------
*/
u8 MAB_ram12used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_12_NUM; i ++)
	{
		if(ram_size_12[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

u8 MAB_ram128used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_128_NUM; i ++)
	{
		if(ram_size_128[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

u8 MAB_ram256used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_256_NUM; i ++)
	{
		if(ram_size_256[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

u8 MAB_ram512used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_512_NUM; i ++)
	{
		if(ram_size_512[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

u8 MAB_ram1024used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_1024_NUM; i ++)
	{
		if(ram_size_1024[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

u8 MAB_ram2048used(void)
{
	u8 i, used_num;

	for(i = 0, used_num = 0; i < SIZE_2048_NUM; i ++)
	{
		if(ram_size_2048[i][0] != RAM_EMPTY_MARK)
		{
			used_num++;
		}
	}
	return(used_num);
}

void DebugRamContent(void)
{
}


