#ifndef WIFI_API_H_
#define WIFI_API_H_

#include "CBasicTools.h"
#include "Dev_wifi.h"



typedef void (*pFunWifiScanHook)(u8 boScanOkFlg, WifiAccessPointSt *stWifiAccessPointInfo); 

u8 WifiApiScanApReq(pFunWifiScanHook pFun); 


#endif


