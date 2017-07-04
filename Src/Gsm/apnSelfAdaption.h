#ifndef _APN_SELF_ADAPTION_H_
#define _APN_SELF_ADAPTION_H_

#include "Apns.h"

#define IMSI_LEN						(16)

u8 SearchApn(s8 *imsi,apns *apn);
void app_sysSetImsi(s8 *buff,u16 Size);
u8 app_sysGetApn(u8 *i_pstrApn,apns *apn);

#endif

