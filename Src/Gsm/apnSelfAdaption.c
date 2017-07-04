#include "apnSelfAdaption.h"
#include "Apns.h"
#include "ParameterManage.h"

static s8 sys_imsi[IMSI_LEN + 1];
u8 SearchApn(s8 *imsi,apns *apn)
{
	u8 boRetVal;
	u16 u16i = 0;

	M_ClrBit(boRetVal);
	for(u16i = 0;u16i < apn_num;u16i++)
	{
		if(IS_FALSE_BIT(memcmp(apn_table[u16i].mcc,imsi,strlen(apn_table[u16i].mcc))))
		{
			if(IS_FALSE_BIT(memcmp(apn_table[u16i].mnc,imsi + 3,strlen(apn_table[u16i].mnc))))
			{
				*apn =  apn_table[u16i];
				M_SetBit(boRetVal);
				break;
			}
			else
			{
			}
		}
		else
		{
			;
		}
	}

	return (boRetVal);
}

void app_sysSetImsi(s8 *buff,u16 Size)
{
	if((IS_NULL_P(buff)) || (Size < IMSI_LEN))
	{
		;
	}
	else
	{
		memset(&sys_imsi,0x00,IMSI_LEN + 1);
		memcpy(sys_imsi,buff,IMSI_LEN);
		
	}
}

u8 app_sysGetApn(u8 *i_pstrApn,apns *apn)
{
	u8 boRetVal;

	M_ClrBit(boRetVal);
	if(IS_FALSE_BIT(strnlen(i_pstrApn,APN_STR_LEN)))
	{
		if(IS_TRUE_BIT(SearchApn(sys_imsi,apn)))
		{
			M_SetBit(boRetVal);
		}
		else
		{
			M_ClrBit(boRetVal);
		}
	}
	else
	{
		M_ClrBit(boRetVal);
	}

	return (boRetVal);
}

