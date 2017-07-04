#include"app_gsm_includes.h"

/***************************************************************
			�绰����ͨ��״̬?
				��,����TRUE,
				��,��ΧFALSE, 
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
			�绰���ڵȴ���ͨ״̬?
				��,����TRUE,
				��,��ΧFALSE, 
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
			�绰��������״̬?
				��,����TRUE,
				��,��ΧFALSE, 
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
			�绰�����Ⲧ�绰
				��,����TRUE,
				��,��ΧFALSE, 
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
	���ڽ��е�ͨ����ƽ̨ͨ������LCDͨ��
	ΪLCD����ʾͨ������
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
	��ȡ��ǰ�绰����
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
����:���չ
ʱ��:2015-5-14 09:44:54
������:	��ȡGSM �����ź�ǿ��
��������:	��ȡGSM �����ź�ǿ��
����:��
���:�����ź�ǿ��,(0 ~ 31)
ע��:��
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
����:���չ
ʱ��:2015-5-14 09:44:54
������:	��ȡGPRS Socket ���Ӳ���
��������:	��ȡGPRS Socket ���Ӳ���
����:Ҫ��ȡ��Socket Id, ��ö�ٶ���,�����������ָ��
���:ͨ��ָ��������Ӳ���,���ṹ�嶨��
ע��:��
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
����:���չ
ʱ��:2015-5-14 09:44:54
������:	��ȡ�绰����(�ֱ� or LCD)
��������:	��ȡ�绰����(�ֱ� or LCD)
����:��
���:�绰����,���ṹ�嶨��
ע��:��
*************************************************************/
PhoneAttriEn GsmGetPhoneAttr(void)
{
	return (gsm_oper_info.phone.attri); 
}

