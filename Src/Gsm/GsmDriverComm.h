#ifndef _GSM_DRIVER_H_
#define _GSM_DRIVER_H_

#include"GsmDriverTypeDef.h"

#include "GsmDriverPort.h"

/*************************************************
ģ��UCOS IIͨ��FIFO����,��Ҫ��������ʹ��
***************************************************/
typedef struct
{
    void         **FifoStart;            /* Pointer to start of queue data                              */
    void         **FifoEnd;              /* Pointer to end   of queue data                              */
    void         **FifoIn;               /* Pointer to where next message will be inserted  in   the Q  */
    void         **FifoOut;              /* Pointer to where next message will be extracted from the Q  */
    GsmTpU16         FifoSize;             /* Size of queue (maximum number of entries)                   */
    GsmTpU16         FifoEntries;          /* Current number of entries in the queue                      */
}GsmFifoSt; 
/*****************************************************
                   ͨ�ü�FIFO

ע��,FIFO������ֻ����һ��������ָ��
	��ָ�����ָ���κ���ҪFIFO�����
	����
                   
*****************************************************/
void GsmFifoCreat(GsmFifoSt *pstFifoDat, 
					void **p2FifoBufStartAdd, 
					GsmTpU16 u16Num); 
GsmTpBool GsmFifoPost(GsmFifoSt *pstFifoDat, void *pmsg); 
GsmTpBool GsmFifoRead(GsmFifoSt *pstFifoDat, void **p2msg); 
GsmTpS8 GsmAsciiToHex( GsmTpU8 isrc); 
GsmTpBool GsmMemIsEqu(const GsmTpU8 *pcu8Mem1, const GsmTpU8 *pcu8Mem2, GsmTpU16 u16Len); 
#define GsmM_Ascii2u8(s8Ascii) 	((s8Ascii) - '0') 
GsmTpS8 GsmHexToAscii(GsmTpU8 hex); 
GsmTpS16 GsmAscii_2_integer(GsmTpS8  *O_data, GsmTpS8  *N_data, GsmTpS16 len);
GsmTpS16 GsmHex_2_Ascii(GsmTpS8  *data, GsmTpS8  *buffer, GsmTpS16 len); 
GsmTpU8 GsmMsbU32To4U8(GsmTpU8 *pu8Dst, GsmTpU32 u32Dat); 


#endif

