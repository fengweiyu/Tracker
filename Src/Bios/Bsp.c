/*****************************************************************************
* Copyright (C) 2014-2015 China Aerospace Telecommunications Ltd.  All rights reserved.
------------------------------------------------------------------------------
* File Module		: 	PT810 BSP.c
* Description		: 	PT810 Base Support Package
* Created			: 	2016.09.20.
* Author			: 	Yu Weifeng
* Function List 		: 	
* Last Modified 	: 	
* History			: 	
******************************************************************************/
#include "CBasictools.h"
#include "Config.h"
#include "Bsp.h"
#include "stm32f4xx_hal.h"
#include "app_UsbManage.h"
#include "app_GpsManage.h"
#include "app_Led.h"
#include "app_Motor.h"
#include "app_PowerStateCheck.h"
#include "app_PowerChargeCheck.h"
#include "ParameterManage.h"
#include "ProtocolManage.h"
#include "app_Gsm.h"
#include "app_WifiSupport.h"
#include "app_TimeManage.h"
#include "spi_flash.h"
#include "flash.h"
#include "app_LightDistanceSensor.h"
#include "app_CutStateCheck.h"
#include "Mymalloc.h"

static void CpuGpioIdelConfig(void);
void IWDG_Init();
void Error_Handler(void);
static void SystemStartInit();
static void SystemStartConfig();
static void ClockConfig(void);
static void SystemClockInit();
 	
 IWDG_HandleTypeDef IWDG_Handler; //�������Ź����

char *g_pcLogo="\r\n      ********          ****          ********    *************** **************  ****         "\
"\r\n    ***********        *****        ***********   *************** **************  ****         "\
"\r\n   *****    ****       ******      *****    ****       ****       ****            ****         "\
"\r\n   ****      ****     *******      ****      ****      ****       ****            ****         "\
"\r\n  ****        ****    ********     ****      ****      ****       ****            ****         "\
"\r\n  ***         ****    *** ****     ****                ****       ****            ****         "\
"\r\n ****                ****  ***     ******              ****       ****            ****         "\
"\r\n ****                ****  ****     *********          ****       *************   ****         "\
"\r\n ****                ***   ****       *********        ****       *************   ****         "\
"\r\n ****               ****    ****          *******      ****       ****            ****         "\
"\r\n ****               ************             ****      ****       ****            ****         "\
"\r\n  ***         **** *************  ****        ***      ****       ****            ****         "\
"\r\n  ****        **** ****      **** ****        ****     ****       ****            ****         "\
"\r\n   ****      ****  ***       ****  ****      ****      ****       ****            ****         "\
"\r\n   *****    ***** ****        **** *****    *****      ****       ****            ****         "\
"\r\n    ************  ****        ****  ************       ****       **************  ************ "\
"\r\n      ********   ****         *****   ********         ****       **************  ************ \r\n";

/*****************************************************************************
-Fuction		: SysInit
-Description	: SysInit
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SysInit()
{
	HAL_Init();  
	SystemClockInit();
	IWDG_Init();
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);HAL������//����ϵͳ�ж����ȼ�����2
}

/*****************************************************************************
-Fuction		: BaseSupportInit
-Description	: BaseSupportInit
-Input			: 
-Output 		: 
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
 void BaseSupportInit()
{
	SystemStartInit();
	SystemStartConfig();
}
/*****************************************************************************
-Fuction		: SystemStartInit
-Description	: ģ������Ⱥ���ע���ϲ���ʹ�ã�����Ŀ����
ע�ᣬֻ����ע�����ʹ�ö����еĺ���
-Input			: 
-Output 		: 
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SystemStartInit()
{
	g_tUsb.Init();
	g_tGsm.Init();
	g_tGPS.Init();
	g_tWifi.Init();
	g_tTime.TimeInit();
	g_tLed.Init();
	g_tMotor.Init();
	g_tCutStateCheck.Init();
	g_tLightDistanceSensor.Init();
	SPI_FLASH_Init();
	Crc32ClkEnable();
	FlashBlockSaveAddressReadInit();
	g_tParameterOpr.RegisterPara();
	g_tPowerStateCheck.Init();
	//g_tPowerChargeCheck.//Init();//���״̬ʹ��USB VBUS�ж�
	g_tProtocol.Init();
}
/*****************************************************************************
-Fuction		: SystemStartConfig
-Description	: ��������
-Input			: 
-Output 		: 
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SystemStartConfig()
{
	//g_tUsb.//Config();//һ��ʼ�����ã�����USB������
	CpuGpioIdelConfig();
	FLASH_Check();//��������λ�һֱ��λ
	g_tGsm.Config();
	g_tGPS.Config();	//����Ҫ�õ�FLASHȥ��ȡ����flash����ʷ����
	g_tWifi.Config();
	g_tTime.TimeConfig();
	g_tLed.Config();
	g_tMotor.Config();	
	g_tCutStateCheck.Config();
	g_tParameterOpr.ParaInit();	
	g_tPowerStateCheck.Config();
	g_tLightDistanceSensor.Config();
	//g_tPowerChargeCheck.//Config();//���״̬ʹ��USB VBUS�ж�
}

/*****************************************************************************
-Fuction		: SystemClockInit
-Description	: SystemClockInit
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SystemClockInit()
{
	//RCC_ClocksTypeDef RCC_ClocksStatus;	
	ClockConfig();
  	/* SysTick interrupt each 100 Hz with HCLK equal to 48MHz */
	//���������-1,�˴����ü�
	SysTick_Config(HAL_RCC_GetHCLKFreq() / OS_TICKS_PER_SEC); 
}
/*****************************************************************************
-Fuction		: ClockConfig
-Description	: ClockConfig
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void ClockConfig(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 336;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
							  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
	Error_Handler();
	}

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}
/*****************************************************************************
-Fuction		: Error_Handler
-Description	: Error_Handler
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  	NVIC_SystemReset();
  }
  /* USER CODE END Error_Handler */ 
}
/*****************************************************************************
-Fuction		: SystemReset
-Description	: SystemReset
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SystemReset(u8 *i_pucDataBuf,u16 i_wDataLen,u8 *o_pucDataSendBuf,u16 *o_wSendLen)
{
	u8 ret=FALSE;	
	PT_ProtocolManage ptTrackerProtocolOpr;
	ret=g_tProtocol.GetProtocolOpr("TrackerProtocol",&ptTrackerProtocolOpr);
	ret&=ptTrackerProtocolOpr->ProtocolLogout(i_pucDataBuf,i_wDataLen,o_pucDataSendBuf,o_wSendLen);
	if(FALSE==ret)
	{
		DebugPrintf(ERR"ProtocolLogout err \r\n");
	}
	else
	{
	}
	OSTimeDlyHMSM(0,0,0,500);//��ӡ��Ҫ��ʱ��
	//DebugPrintf("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");	
	DebugPrintf(EMERG">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>System Reset>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	//DebugPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	OSTimeDlyHMSM(0,0,0,500);//��ӡ��Ҫ��ʱ��
	NVIC_SystemReset();	
}
/*****************************************************************************
-Fuction		: SystemEnterStopMode
-Description	: SystemEnterStopMode
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SystemEnterStopMode()
{
	__HAL_RCC_PWR_CLK_ENABLE(); 		//ʹ��PWRʱ��		
	//ֹͣģʽ����������һ���ⲿ�ж�(EXTI)���ѡ���ֹͣģʽ��ѡ��͹���ģʽ
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI); 	//
	SystemClockInit();//�˳�ֹͣģʽ������MCU��ʹ���ڲ�ʱ�ӣ�������������ʱ��
}
/*****************************************************************************
-Fuction		: SystemEnterSleepMode
-Description	: SystemEnterSleepMode
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SystemEnterSleepMode()
{
	__HAL_RCC_PWR_CLK_ENABLE(); 		//ʹ��PWRʱ��		
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI); 
}
/*****************************************************************************
-Fuction		: SystemEnterStandByMode
-Description	: SystemEnterStandByMode
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SystemEnterStandByMode()
{
	__HAL_RCC_AHB1_FORCE_RESET();		//��λ����IO�� 
	__HAL_RCC_PWR_CLK_ENABLE(); 		//ʹ��PWRʱ��		
	HAL_PWR_EnterSTANDBYMode();
}

/*****************************************************************************
-Fuction		: CpuGpioIdelConfig
-Description	: CpuGpioIdelConfig
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/11/25	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void CpuGpioIdelConfig(void)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           //����GPIOCʱ��
	__HAL_RCC_GPIOB_CLK_ENABLE();           //����GPIOCʱ��	
	__HAL_RCC_GPIOC_CLK_ENABLE();           //����GPIOCʱ��
	__HAL_RCC_GPIOD_CLK_ENABLE();           //����GPIOCʱ��

	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_15;          //P
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);                              //��ʼ�� 

	GPIO_Initure.Pin=GPIO_PIN_15;          //P
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	GPIO_Initure.Pull=GPIO_PULLUP;          //
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);                              //��ʼ�� 

	GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;          //PC
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);                              //��ʼ�� 

	GPIO_Initure.Pin=GPIO_PIN_2;           //PC
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //����
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);                              //��ʼ�� 	
}

/*****************************************************************************
-Fuction		: IWDG_Init
-Description	: IWDG_Init
//��ʼ���������Ź�
//prer:��Ƶ��:IWDG_PRESCALER_4~IWDG_PRESCALER_256
//rlr:�Զ���װ��ֵ,0~0XFFF.
//ʱ�����(���):Tout=((4*2^prer)*rlr)/32 (ms).
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void IWDG_Init()
{
	IWDG_Handler.Instance=IWDG;
	IWDG_Handler.Init.Prescaler=IWDG_PRESCALER_256;	//����IWDG��Ƶϵ����ǰ4������32��
	IWDG_Handler.Init.Reload=4095;//����Ҳ���ò���ȥ(4095+4095+4095+4095);		//��װ��ֵ32+32+32+32s��������Ҫ�ȴ�GSM������Ҫ����1����
   	if (HAL_IWDG_Init(&IWDG_Handler) != HAL_OK)
	{
		Error_Handler();
	}
}
/*****************************************************************************
-Fuction		: IWDG_Feed
-Description	: IWDG_Feed
//ι�������Ź�
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&IWDG_Handler); 	//ι��
}
/*****************************************************************************
-Fuction		: DebugStopPeripheral
-Description	: DebugStopPeripheral
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void DebugStopPeripheral()
{
	__HAL_DBGMCU_FREEZE_TIM2();
	__HAL_DBGMCU_FREEZE_TIM3();
	__HAL_DBGMCU_FREEZE_TIM4();
	__HAL_DBGMCU_FREEZE_TIM5();
	__HAL_DBGMCU_FREEZE_TIM6();
	__HAL_DBGMCU_FREEZE_TIM7();
	__HAL_DBGMCU_FREEZE_TIM12();
	__HAL_DBGMCU_FREEZE_TIM13();
	__HAL_DBGMCU_FREEZE_TIM14();
	__HAL_DBGMCU_FREEZE_RTC();
	__HAL_DBGMCU_FREEZE_WWDG();
	__HAL_DBGMCU_FREEZE_IWDG();
	__HAL_DBGMCU_FREEZE_I2C1_TIMEOUT();
	__HAL_DBGMCU_FREEZE_I2C2_TIMEOUT();
	__HAL_DBGMCU_FREEZE_I2C3_TIMEOUT();
	__HAL_DBGMCU_FREEZE_CAN1();
	__HAL_DBGMCU_FREEZE_CAN2();
	__HAL_DBGMCU_FREEZE_TIM1();
	__HAL_DBGMCU_FREEZE_TIM8();
	__HAL_DBGMCU_FREEZE_TIM9();
	__HAL_DBGMCU_FREEZE_TIM10();
	__HAL_DBGMCU_FREEZE_TIM11();
}

/*********************ϵͳ��������֧�ֺ���***********************/
/*****************************************************************************
-Fuction		: HAL_UART_RxCpltCallback
-Description	: HAL_UART_RxCpltCallback
ʹ�ÿ�Ľ��ջ��ƣ��ص���ʽ
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���1
	{
		//dev_gsm_DataRecv(Usart1RecvDataBuffer[0]);
	}
	else if(huart->Instance==USART2)//����Ǵ���1
	{
		GpsRecvDataDev(Usart2RecvDataBuffer[0]);
	}
	else if(huart->Instance==USART6)//����Ǵ���1
	{
		//wifiRecv(USART6RecvDataBuffer[0]);
	}
	else
	{
	}
}*/

/*****************************************************************************
-Fuction		: DevOsRxQWrite
-Description	: DevOsRxQWrite
�˺������ͷŴ�����ڴ�ռ�
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
u8 DevOsRxQWrite(OS_EVENT *pstOsQ, myevent *event)
{
	u8 u8RetVal; 
	myevent *pstEveTmp; 
	u16 u16Len = ST_MYEVENT_SIZE + event->size; 
	M_SetBit(u8RetVal); 
	pstEveTmp = (myevent *)mymalloc(u16Len); 
	if((NULL == pstEveTmp) || (NULL == pstOsQ)) {
		if(NULL == pstEveTmp) {
			DebugPrintf(ERR"dev write malloc err1\r\n"); 
		}
		else {
			myfree((u8 *)pstEveTmp, u16Len); 
		}
		M_ClrBit(u8RetVal); 
	}
	else	{
		memset(pstEveTmp, 0, u16Len); 
		pstEveTmp->buff = (u8 *)pstEveTmp + ST_MYEVENT_SIZE;
		pstEveTmp->size = u16Len; 
		memcpy(pstEveTmp->buff, event->buff, event->size); 
		u8RetVal= OSQPost(pstOsQ, pstEveTmp);
		if(OS_ERR_NONE == u8RetVal) {
			M_SetBit(u8RetVal); 
		}
		else	{
			DebugPrintf(ERR"DevOsRxQWrite OsQ err=%d\r\n",u8RetVal);
			myfree((u8 *)pstEveTmp, u16Len); 
			M_ClrBit(u8RetVal); 
		}
	}					
	return(u8RetVal);
}
/*****************************************************************************
-Fuction		: DevOsRxQReadPend
-Description	: DevOsRxQReadPend
��������OS ERR���ñ����������񽫱�����ȴ�
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
u8 DevOsRxQReadPend(OS_EVENT *pstOsQ, myevent **event, u16 u16PendTimeMs)
{
	u8 u8Err; 
	u8 RetVal; 
	M_ClrBit(RetVal); 
	if((NULL == pstOsQ) || (NULL == event)) {
		M_ClrBit(RetVal); 
	}
	else {
		*event = (myevent *)OSQPend(pstOsQ, u16PendTimeMs / (1000 / OS_TICKS_PER_SEC), &u8Err); 
		OS_ERR_NONE == u8Err ? M_SetBit(RetVal) : M_ClrBit(RetVal); 

	}
	return RetVal; 
}
/*****************************************************************************
-Fuction		: DebugStopPeripheral
-Description	: DebugStopPeripheral
*����ucos iiϵͳ,�������ϵͳ��Ҫ�޸Ķ�Ӧ�ڲ����ú���
����:��̬��32λ�Ķ�ʱ���������
		����ñ������Զ���ʼ��,
		��Ҫ���г�ʼ��,��Ҫʹ��
		OSTimeGet(); ��ʼ��,�����ϵͳͬ�������Ķ�ʱ��
		���Բ��ó�ʼ��. 
���:�Դ��ϴ����о���������
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
u16 GetDurationSecTimer(u32 *pdwBackupTime)
{//�˺�������Ҫ��һ��֮��
	u16 wSecond=0; 
	u32 dwCurTime=0; 
	u32 dwDiffTime=0; 	
	u32 dwTmpPoint=0; 
	dwCurTime = OSTimeGet(); 
	if((0==*pdwBackupTime)|| (dwCurTime < *pdwBackupTime)) //�״����л򼫶��쳣,��ʼ��
	{; 
		*pdwBackupTime = dwCurTime; 
	}
	else 
	{; 
	}
	dwDiffTime = (dwCurTime - *pdwBackupTime);	
	if(dwDiffTime < OS_TICKS_PER_SEC) 
	{; 
	}
	else 
	{
		wSecond = dwDiffTime / OS_TICKS_PER_SEC; 
		dwTmpPoint = dwDiffTime % OS_TICKS_PER_SEC; //С������
		*pdwBackupTime = dwCurTime - dwTmpPoint; 
	}
	return wSecond;
}
/*****************************************************************************
-Fuction		: SetRtcAlarm
-Description	: SetRtcAlarm
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SetRtcAlarmTime(u16 i_wDataTime)
{
	u32 dwDataTime=0;
	datetime_t tDataTime={0};	
	g_tTime.GetDateTime(&tDataTime);
	dwDataTime=app_sys_dtime2mktime(tDataTime);
	dwDataTime+=i_wDataTime;
	tDataTime=app_sys_mktime2dtime(dwDataTime);
	g_tTime.AlarmConfig(tDataTime); 	
}
/*****************************************************************************
-Fuction		: UsbResetSystemFunction
-Description	: //���ţ�����USBС��1s��λ
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void UsbResetSystemFunction()
{
	u8 ucUsbState=0;
	g_tUsb.GetUsbState(&ucUsbState);//���ţ�����USBС��1s��λ
	if(USB_IS_CONNECT_STATE==ucUsbState)
	{
		OSTimeDlyHMSM(0,0,0,20);//����				
		g_tUsb.GetUsbState(&ucUsbState);				
		if(USB_IS_CONNECT_STATE==ucUsbState)
		{
			g_tLed.RedLedOn();	
			OSTimeDlyHMSM(0,0,1,20);
			g_tUsb.GetUsbState(&ucUsbState);//���ţ�����USBС��1s��λ
			if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
			{
				OSTimeDlyHMSM(0,0,0,20);//����				
				g_tUsb.GetUsbState(&ucUsbState);				
				if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
				{
					OSSemPost(ptOsSemUsedForSystemNeedReset);//��ֱ�ӵ��ø�λ����
				}					
				else
				{
				}
			}
			else
			{
			}
		}
		else
		{
		}
	}
	else
	{
		DebugPrintf(ERR"UsbResetUsbStateUnNormal\r\n");
	}
}
