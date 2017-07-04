#ifndef _APP_GSM_ANALYSE_EXPAND_H_
#define _APP_GSM_ANALYSE_EXPAND_H_

extern const GsmTpS8 cs8GsmRxIpDatTcpStr[]; 
extern const GsmTpU8 cu8GsmRxIpDatTcpStrLen; 
extern const GsmTpS8 cs8GsmRxIpDatUdpStr[]; 

void GsmAtAnalyseExpandProc(GsmTpU8 *pu8Dat, GsmTpU16 u16DatLen); 



#endif

