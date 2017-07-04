#ifndef _GSM_DRIVER_TYPE_H_
#define _GSM_DRIVER_TYPE_H_

#define GSM_PHONE_STR_LEN		20
typedef char GsmTpS8; 
typedef unsigned char GsmTpU8; 
typedef signed short GsmTpS16; 
typedef unsigned short GsmTpU16; 
typedef signed int	GsmTpS32; 
typedef unsigned int GsmTpU32; 
typedef unsigned char GsmTpBool; 
typedef enum {GsmTpFALSE = 0, GsmTpTRUE = !GsmTpFALSE, } Gsmbool;

#define IS_TRUE_BIT(X)	(0 != (X))
#define IS_FALSE_BIT(X)	(0 == (X))
#define M_SetBit(X)		((X) = 1)
#define M_ClrBit(X)		((X) = 0)
#define M_MIN(X,Y)			((X) < (Y) ? (X) : (Y))
#define M_MAX(X,Y)			((X) > (Y) ? (X) : (Y))
#define M_NOT_ZERO_DEC(X)	{if(0 != (X)) {(X)--;} else {/*No action*/}}
#define IS_NULL_P(X)		(NULL == (X))

/***************************************************
	返回长度的内存拷贝,
协助解决内存拷贝地址忘记更改问题
	Eg. pu8Dst += M_MEM_CPY(pu8Dst, src, size); 
****************************************************/
#define M_MEM_CPY(pu8Dst,pu8Src,Size)	memcpy(pu8Dst, pu8Src, Size) ? (Size) : (Size) 


#pragma pack(1)
typedef struct
{
	GsmTpU8 year;
	GsmTpU8 month;
	GsmTpU8 day;
}Gsmdate_t;
typedef struct
{
	GsmTpU8 hour;
	GsmTpU8 minute;
	GsmTpU8 second;
}Gsmtime_t;
typedef struct
{
	Gsmdate_t date;
	Gsmtime_t time;
}Gsmdatetime_t;
typedef struct  
{
	unsigned int size;
	unsigned char *buff;
}Gsmmyevent;


/*不可随意调换次序,和模块输出的Socket id 对应,
具体G610, 模块socket id - 1*/
typedef enum{
	IP_DAT_SRC_SCK_1, 
	IP_DAT_SRC_SCK_2, 
	IP_DAT_SRC_SCK_3, 
	IP_DAT_SRC_SCK_4, 
	IP_DAT_SRC_SCK_5, 

	IP_DAT_SRC_SCK_NUM, 
} IpDatSrcEn; 

typedef enum {
	IP_PRO_TYPE_TCP, 
	IP_PRO_TYPE_UDP, 
}IpProTypeEn; 

typedef struct
{
	IpDatSrcEn enSrc; 
	GsmTpU16 u16Len; 
	GsmTpU8 *pu8Dat; 
}IpDatSt; 
typedef struct
{
	GsmTpU8 status;
	GsmTpU8 len;
	GsmTpU8 num[GSM_PHONE_STR_LEN];
	Gsmdatetime_t datetime;
}sms_head_t;

#define SMS_ST_DEL 		0
#define SMS_ST_HRD 		1
#define SMS_ST_NRD 		3
#define SMS_ST_SEND 	4

typedef struct
{
	sms_head_t head;
	GsmTpU8 *sms_info;
}sms_info_t;
#define SMS_ATTRI_NULL 		0
#define SMS_ATTRI_LCD 		1
#define SMS_ATTRI_HAND 	2
typedef struct
{
	GsmTpU8 *pu8Dat; 
	GsmTpU16 u16Len; 
	IpDatSrcEn enIpDatSrcId; 
}GsmIpDatTxBufSt; 
#define IP_V4_STR_MAX_LEN				15
#define IP_V4_STR_BUF_LEN_MIN			((IP_V4_STR_MAX_LEN) + 1)	
#pragma pack(1)
typedef struct
{
	IpProTypeEn enIpProType; 
	GsmTpU16 u16Port; 
	GsmTpU32 u32IpV4Addr; 
}GsmAtUseSocketParaSt; 

typedef struct
{
	struct {
		GsmTpS8 s8Num[GSM_PHONE_STR_LEN+1]; 
		GsmTpU8 u8Len; 
	}stDstNum; 
	struct {
		GsmTpU8 *pu8Dat; 
		GsmTpU16 u16Len; 
	}stSmsDat; 

	void (*SmsTxEndHook)(GsmTpBool boTxOkFlg); 
}SmsTxParaSt; 

typedef enum {
	PHONE_ATTRI_NULL, 
	PHONE_ATTRI_LCD, 
	PHONE_ATTRI_HAND, 
	PHONE_ATTRI_PLTF, 
	PHONE_ATTRI_LISTEN, 
}PhoneAttriEn; 
enum GsmPhoneVolOprEn
{
	GSM_PHONE_VOL_UP, 
	GSM_PHONE_VOL_DW, 
}; 
struct GsmAudioRecordReqParaSt
{
	GsmTpU16 u16TimeS; 
	GsmTpU8 u8SampleFrequency; //和具体的模块有关系
	GsmTpU32 u32AudioDatSaveMaxSize; //存储区大小,字节
	void (*pFunHookForAudioStart)(void);//开始录音前处理(准备存储区等) 
	void (*pFunHookForAudioDatSave)(GsmTpU8 *pu8Dat, GsmTpU16 u16DatLen, GsmTpU32 u32Offset); //保存数据回调
	void (*pFunGetDatOkHook)(GsmTpU32 u32FileSize); //数据获取完毕回到
}; 
#pragma pack()
#define GPRS_TCP 0
#define GPRS_UDP 1

#endif

