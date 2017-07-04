#include"app_gsm_includes.h"

/***************************************************************
			电话处于通话状态?
				是,返回TRUE,
				否,范围FALSE, 
****************************************************************/
GsmTpBool GsmApiPhoneStIsSpking(void)
{
	GsmTpBool boRetVal; 
	if(PHONE_STATUS_SPKG == gsm_oper_info.phone.status) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	return (boRetVal); 
}

/***************************************************************
			电话处于等待接通状态?
				是,返回TRUE,
				否,范围FALSE, 
****************************************************************/
GsmTpBool GsmApiPhoneStIsLink(void)
{
	GsmTpBool boRetVal; 
	if(PHONE_STATUS_LINK == gsm_oper_info.phone.status) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	return (boRetVal); 
}
/***************************************************************
			电话处于响铃状态?
				是,返回TRUE,
				否,范围FALSE, 
****************************************************************/
GsmTpBool GsmApiPhoneStIsRing(void)
{
	GsmTpBool boRetVal; 
	if(PHONE_STATUS_RING == gsm_oper_info.phone.status) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	return (boRetVal); 
}


/***************************************************************
			电话属于外拨电话
				是,返回TRUE,
				否,范围FALSE, 
****************************************************************/
GsmTpBool GsmApiPhoneIsDailOut(void)
{
	GsmTpBool boRetVal; 
	if(IS_TRUE_BIT(gsm_oper_info.flgs.phone_dial)) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	return (boRetVal); 
}

/*************************************************************
	正在进行的通话是平台通话或者LCD通话
	为LCD屏显示通话所做
*************************************************************/
GsmTpBool GsmApiPhoneIsPltOrLcd(void)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	if((PHONE_ATTRI_PLTF == gsm_oper_info.phone.attri)
	|| (PHONE_ATTRI_LCD == gsm_oper_info.phone.attri)) {
		M_SetBit(boRetVal); 
	}
	else {
		M_ClrBit(boRetVal); 
	}
	return (boRetVal); 
}

/*************************************************************
	获取当前电话号码
*************************************************************/
GsmTpBool GsmApiPhoneGetCurNum(GsmTpU8 *pu8Dat, GsmTpU8 *pu8Len)
{
	GsmTpU8 u8Len;  
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	if(IS_NULL_P(pu8Dat) 
	|| IS_NULL_P(pu8Len)) {
		M_ClrBit(boRetVal); 
	}
	else {
		u8Len = strlen((GsmTpS8 *)gsm_oper_info.phone.num) ; 
		u8Len = M_MIN(u8Len, sizeof(gsm_oper_info.phone.num)); 
		GsmDriverEnterCritical(); 
		memcpy(pu8Dat, gsm_oper_info.phone.num, u8Len); 
		GsmDriverExitCriticical(); 
		*pu8Len = u8Len; 
		M_SetBit(boRetVal); 
	}
	return (boRetVal); 
}

/*************************************************************
作者:李宏展
时间:2015-5-14 09:44:54
函数名:	获取GSM 网络信号强度
函数功能:	获取GSM 网络信号强度
输入:无
输出:网络信号强度,(0 ~ 31)
注意:无
*************************************************************/
GsmTpU8 GsmGetCsq(void)
{
	if(IS_FALSE_BIT(GsmPhoneIsIdle()))
	{
	}
	else
	{
		ATLib_gsm_CSQ1();
	}
	return (gsm_oper_info.Csq_val); 	
}
/*************************************************************
作者:李宏展
时间:2015-5-14 09:44:54
函数名:	获取GPRS Socket 链接参数
函数功能:	获取GPRS Socket 链接参数
输入:要获取的Socket Id, 见枚举定义,输出数据载体指针
输出:通过指针输出链接参数,见结构体定义
注意:无
*************************************************************/
void GsmGetSocketLinkPara(IpDatSrcEn enSckId, GsmAtUseSocketParaSt *pstDstPara)
{
	GsmTpU8 u8Id; 
	if(enSckId < IP_DAT_SRC_SCK_NUM) {
		u8Id = (GsmTpU8)enSckId; 
	}
	else {
		u8Id = (GsmTpU8)IP_DAT_SRC_SCK_1; 
	}
	memcpy(pstDstPara, &sstGsmAtUseSocketPara[u8Id], sizeof(GsmAtUseSocketParaSt)); 
}
/*************************************************************
作者:李宏展
时间:2015-5-14 09:44:54
函数名:	获取电话属性(手柄 or LCD)
函数功能:	获取电话属性(手柄 or LCD)
输入:无
输出:电话属性,见结构体定义
注意:无
*************************************************************/
PhoneAttriEn GsmGetPhoneAttr(void)
{
	return (gsm_oper_info.phone.attri); 
}

