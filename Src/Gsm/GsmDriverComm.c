#include "app_gsm_includes.h"

/*****************************************************
                   ͨ�ü�FIFO

ע��,FIFO������ֻ����һ��������ָ��
	��ָ�����ָ���κ���ҪFIFO�����
	����
!!!!!!!ע��:1.��FIFOֻ�ܴ���ָ��,
                        2.p2FifoBufStartAdd������ָ������,
                        	��������ָ�뵥λ(ÿ��Ԫ�������ܹ�����
                        	һ��ָ��,����,32λ������,16λ����Ͳ��ܹ�
                        	����ָ��,������Ԫ������ÿ�����Ա���һ��ָ��)
*****************************************************/
void GsmFifoCreat(GsmFifoSt *pstFifoDat, 
					void **p2FifoBufStartAdd, 
					GsmTpU16 u16Num)
{
	GsmDriverEnterCritical();
	pstFifoDat->FifoStart           = p2FifoBufStartAdd;               /*      Initialize the queue                 */
	pstFifoDat->FifoEnd             = &p2FifoBufStartAdd[u16Num];
	pstFifoDat->FifoIn              = p2FifoBufStartAdd;
	pstFifoDat->FifoOut             = p2FifoBufStartAdd;
	pstFifoDat->FifoSize            = u16Num;
	pstFifoDat->FifoEntries         = 0u;
	GsmDriverExitCriticical();
		
}

GsmTpBool GsmFifoPost(GsmFifoSt *pstFifoDat, void *pmsg)
{
	GsmTpBool boRetVal; 
	M_ClrBit(boRetVal); 
	if (pstFifoDat == NULL) {                     /* Validate 'pevent'                            */
		GsmDebugString("fifo post err\r\n"); 
		M_ClrBit(boRetVal); 
	}
	else {
		GsmDriverEnterCritical(); 
		if (pstFifoDat->FifoEntries >= pstFifoDat->FifoSize) {               /* Make sure queue is not full                  */
			M_ClrBit(boRetVal); 
		}
		else {
			*pstFifoDat->FifoIn++ = pmsg;                               /* Insert message into queue                    */
			pstFifoDat->FifoEntries++;                                  /* Update the nbr of entries in the queue       */
			if (pstFifoDat->FifoIn == pstFifoDat->FifoEnd) {                     /* Wrap IN ptr if we are at end of queue        */
			    pstFifoDat->FifoIn = pstFifoDat->FifoStart;
			}
			M_SetBit(boRetVal); 
		}
		GsmDriverExitCriticical(); 
	}

	return (boRetVal);
}

GsmTpBool GsmFifoRead(GsmFifoSt *pstFifoDat, void **p2msg)
{
	GsmTpBool boRetVal; 
	
	M_ClrBit(boRetVal); 
	if (pstFifoDat == NULL) {                     /* Validate 'pevent'                            */
		GsmDebugString("fifo post err\r\n"); 
		M_ClrBit(boRetVal); 
	}
	else {
	    GsmDriverEnterCritical();
	    if (pstFifoDat->FifoEntries > 0u) {                   /* See if any messages in the queue                   */
	        *p2msg = *pstFifoDat->FifoOut++;                    /* Yes, extract oldest message from the queue         */
	        pstFifoDat->FifoEntries--;                        /* Update the number of entries in the queue          */
	        if (pstFifoDat->FifoOut == pstFifoDat->FifoEnd) {          /* Wrap OUT pointer if we are at the end of the queue */
	            pstFifoDat->FifoOut = pstFifoDat->FifoStart;
	        }
	       M_SetBit(boRetVal);
	    } else {
	        *p2msg  = (void *)0;                       /* Queue is empty                                     */
		M_ClrBit(boRetVal); 
	    }
	    GsmDriverExitCriticical();
	}

	return (boRetVal); 
}

/*
*************************************************************************************************************************
* ����������ת��1�������ַ�Ϊ����, �Ƿ�Ϊ'0~9, a~f, A~F',  ���жϸ��ַ��Ƿ�Ϊ�����ַ�
* ���������isrc����
* �����������
* ���ز�����ת���������-1: ����, ���򷵻�ת���������
*************************************************************************************************************************
*/
GsmTpS8 GsmAsciiToHex( GsmTpU8 isrc)
{
    	GsmTpS8 itemp = 0;

    	itemp = isrc;
	if((itemp > ('0' - 1)) && (itemp < ('9' + 1)))
    	{
        	itemp -= '0';
    	}
    	else if((itemp > ('A' - 1)) && (itemp < ('F' + 1)))
    	{
        	itemp -= 'A';
        	itemp += 0x0a;
    	}
    	else if((itemp > ('a' - 1)) && (itemp < ('f' + 1)))
    	{
        	itemp -= 'a';
        	itemp += 0x0a;
    	}
   	else
    	{
        	itemp = (GsmTpS8)-1;
    	}
    	return itemp;
}


GsmTpBool GsmMemIsEqu(const GsmTpU8 *pcu8Mem1, const GsmTpU8 *pcu8Mem2, GsmTpU16 u16Len)
{
	GsmTpBool boRetVal = GsmTpTRUE; 
	GsmTpU16 u16i; 
	
	if((NULL == pcu8Mem1) || (NULL == pcu8Mem2))
	{
		M_ClrBit(boRetVal); 
	}
	else
	{
		for(u16i = 0; u16i < u16Len; u16i++)
		{
			if(*(pcu8Mem1 + u16i) != *(pcu8Mem2 + u16i))
			{
				M_ClrBit(boRetVal); 
				return boRetVal; 
			}
			else
			{; 
			}
		}
		M_SetBit(boRetVal); 
	}	
	
	return boRetVal; 
}

/*
 ******************************************************************************
 * ����������ת��1�������ַ�Ϊ�����Ƿ�Ϊ'0~9, a~f, A~F',���жϸ��ַ��Ƿ�Ϊ����
 *           �ַ�
 * ���������isrc����
 * �����������
 * ���ز�����ת���������
 *           -1: ����
 * �ⲿ���ã���
 * ע�������
 ******************************************************************************
 */
GsmTpS8 GsmHexToAscii(GsmTpU8 hex)
{
    const GsmTpS8 Byte2Hex[] = "0123456789ABCDEF";

	return Byte2Hex[hex];
}
GsmTpS16 GsmAscii_2_integer(GsmTpS8  *O_data, GsmTpS8  *N_data, GsmTpS16 len)
{
	int i,j,tmp_len;
	GsmTpS8  tmpData;
	GsmTpS8  *O_buf = O_data;
	GsmTpS8  *N_buf = N_data;
	
	for(i = 0; i < len; i++)
	{
		if((O_buf[i] >= '0') && (O_buf[i] <= '9'))
		{
			tmpData = O_buf[i] - '0';
		}
		else if((O_buf[i] >= 'A') && (O_buf[i] <= 'F')) //A....F
		{
			tmpData = O_buf[i] - 0x37;
		}
		else if((O_buf[i] >= 'a') && (O_buf[i] <= 'f')) //a....f
		{
			tmpData = O_buf[i] - 0x57;
		}
		else
		{
			return -1;
		}
		O_buf[i] = tmpData;
	}
	
	for(tmp_len = 0,j = 0; j < i; j+=2)
	{
		N_buf[tmp_len++] = (O_buf[j]*10) + O_buf[j+1];
	}
	
	return tmp_len;
}
/*
 ******************************************************************************
 * ����������HEX �� ASCII��ת������
 * ���������data: ת�����ݵ����ָ��
 *		   buffer: ת�����������ָ��
 *			 len : ��Ҫת���ĳ���
 * ���ز�����ת�������ݳ���
 ******************************************************************************
 */
GsmTpS16 GsmHex_2_Ascii(GsmTpS8  *data, GsmTpS8  *buffer, GsmTpS16 len)
{
//	const GsmTpS8 ascTable[17] = {"0123456789ABCDEF"};
	GsmTpS8  *tmp_p = buffer;
	int i, pos;
	
	pos = 0;
	for(i = 0; i < len; i++)
	{
		tmp_p[pos++] = GsmHexToAscii(data[i] >> 4);
		tmp_p[pos++] = GsmHexToAscii(data[i] & 0x0f);
	}
	tmp_p[pos++] = '\0';
	
	return (pos);
}
GsmTpU8 GsmMsbU32To4U8(GsmTpU8 *pu8Dst, GsmTpU32 u32Dat)
{
	GsmTpU8 *pu8Temp; 
	pu8Temp = pu8Dst; 
	
	*pu8Temp++ = (GsmTpU8)(u32Dat >> 24); 
	*pu8Temp++ = (GsmTpU8)(u32Dat >> 16); 
	*pu8Temp++ = (GsmTpU8)(u32Dat >> 8); 
	*pu8Temp++ = (GsmTpU8)(u32Dat); 
	
	return 4; 
}



