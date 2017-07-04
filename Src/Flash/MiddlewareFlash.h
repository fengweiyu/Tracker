#ifndef __MIDDLEWARE_FLASH_H__
#define __MIDDLEWARE_FLASH_H__




#ifndef NULL
#define	NULL	0
#endif





typedef struct
{
	unsigned int u32Wr; 
	unsigned int u32Rd; 
}FlashSaveAddSt; 

/**************************************************************************
  ��ȡ��汾��
  ���������İ汾��
eg.�汾��Ϊ1.0.2 ����102
**************************************************************************/
unsigned short FlashMidwareLibVersion(); 

/***************************************************************************
���չ
2014-7-3 13:52:20
��������,FLASH �����洢��ַ��дָ���ȡ
����:
	u32AddStart: FLASH�洢����ʼ��ַ,������ַ
	u32AddEnd : FLASH�洢��������ַ
	u16SaveMaxLen : �洢���ݴ�С(�ô�С����Ϊ2�������η�)
				�洢����Ч���������С�ڸ���ֵ4�ֽ�.
				�����ɺ���FlashAreaGetMaxSaveLen����ȡ
	pstFlaAdd:�����ĵ�ַ���λ��

���: 1.ִ�н��,�ɹ�������,ʧ�ܷ�����
		2.��ȡ���Ķ�дָ�����ڴ����ָ����
����ʾ��:
		FlashAddRead(FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_START, 
				FLASH_ADDR_DRIVER_LOGIN_LOGOUT_LOG_END, 
				FlashAreaGetMaxSaveLen(sizeof(RecordDriverLogSt)), 
				&stFlaAddLogDriverEvent); 

		FlashAddRead(FLASH_ADDR_POST_START, 
					FLASH_ADDR_POST_END, 
					POST_MAX_LEN, 
					(FlashSaveAddSt *)&report_addr); 

*****************************************************************************/

unsigned char FlashAddRead(const unsigned int cu32AddStart, 
				const unsigned int cu32AddEnd, 
				const unsigned short cu16SaveMaxLen, 
				FlashSaveAddSt *pstFlaAdd); 


/***************************************************************************
���չ
2014-7-3 13:53:46
��������:����Ч���ݳ��������FLASH�洢��λ��С
				��FLASH�����洢��Ҫ����
				��Ч���ݴ洢ʱ��Ҫ���뵽2�������η���С
				��������4�ֽڼ�У��ͳ���

				�ú�����Ҫ��Ϻ��������ݴ洢��װ
				���������ݽṹֱ�Ӽ���淶�Ĵ洢�ռ��С
				
����:��Ч���ݳ���
���:����Ч���ݳ��ȶ�Ӧ��FLASH�洢��λ��С
***************************************************************************/
unsigned short FlashAreaGetMaxSaveLen(unsigned short u16Len); 

/*�������*/
void FlashAreaReset(unsigned int u32AddStart, 
				unsigned int u32AddEnd, 
				FlashSaveAddSt *pstFlaAdd); 


void FlashAreaSave(unsigned int u32AddStart, 
					unsigned int u32AddEnd, 
					unsigned short u16SaveMaxLen, 
					FlashSaveAddSt *pstFlaAdd, 
					unsigned char *pu8SrcDat, 
					unsigned short u16Len); 

/****************************************************************************
Ҫʹһ��FLASH�����е���������Ϊ
��Ч��ҪFlashAreaInvailidSet���
*******************************************************************************/
void FlashAreaRead(unsigned int u32AddStart, 
					unsigned int u32AddEnd, 
					unsigned short u16SaveMaxLen, 
					FlashSaveAddSt stFlaAdd, 
					unsigned char *pu8DstDat, 
					unsigned short *pu16Len); 

/***********************************************************************
ʹһ��FLASH�����е���������Ϊ��Ч,��FlashAreaRead���
ע��,������ηǳ���Ļ�,����Ҫ������ι��
****ע��,ʹ��ǰһ��Ҫע��Ҫ������Ч�������Ƿ�
	��ַ����Ч(���Ǵ洢ʱ,�����޸Ķ�ָ��),��ֹ��
	��������
*************************************************************************/
void FlashAreaInvailidSet(const unsigned int cu32AddStart, 
							const unsigned int cu32AddEnd, 
							const unsigned short cu16SaveMaxLen, 
							const unsigned int cu32InvailidAddS,
							const unsigned int cu32InvailidAddE); 


/**************************************
FLASH��ַ������
���������һ����ַ����
***************************************/
unsigned char FlashAddAlign(unsigned int *pu32Add, unsigned short u16Len); 

#endif

