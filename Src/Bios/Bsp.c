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
 	
 IWDG_HandleTypeDef IWDG_Handler; //独立看门狗句柄

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
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);HAL中设置//设置系统中断优先级分组2
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
-Description	: 模块操作等函数注册上才能使用，函数目的是
注册，只有先注册才能使用对象中的函数
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
	//g_tPowerChargeCheck.//Init();//充电状态使用USB VBUS判断
	g_tProtocol.Init();
}
/*****************************************************************************
-Fuction		: SystemStartConfig
-Description	: 启动功能
-Input			: 
-Output 		: 
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
static void SystemStartConfig()
{
	//g_tUsb.//Config();//一开始不配置，插上USB才配置
	CpuGpioIdelConfig();
	FLASH_Check();//如果不屏蔽会一直复位
	g_tGsm.Config();
	g_tGPS.Config();	//里面要用到FLASH去读取存在flash中历史数据
	g_tWifi.Config();
	g_tTime.TimeConfig();
	g_tLed.Config();
	g_tMotor.Config();	
	g_tCutStateCheck.Config();
	g_tParameterOpr.ParaInit();	
	g_tPowerStateCheck.Config();
	g_tLightDistanceSensor.Config();
	//g_tPowerChargeCheck.//Config();//充电状态使用USB VBUS判断
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
	//函数里面会-1,此处不用减
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
	OSTimeDlyHMSM(0,0,0,500);//打印需要的时间
	//DebugPrintf("\r\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");	
	DebugPrintf(EMERG">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>System Reset>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	//DebugPrintf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\r\n");
	OSTimeDlyHMSM(0,0,0,500);//打印需要的时间
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
	__HAL_RCC_PWR_CLK_ENABLE(); 		//使能PWR时钟		
	//停止模式可以由任意一个外部中断(EXTI)唤醒。在停止模式中选择低功耗模式
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI); 	//
	SystemClockInit();//退出停止模式，由于MCU会使用内部时钟，所以重新配置时钟
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
	__HAL_RCC_PWR_CLK_ENABLE(); 		//使能PWR时钟		
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
	__HAL_RCC_AHB1_FORCE_RESET();		//复位所有IO口 
	__HAL_RCC_PWR_CLK_ENABLE(); 		//使能PWR时钟		
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

	__HAL_RCC_GPIOA_CLK_ENABLE();           //开启GPIOC时钟
	__HAL_RCC_GPIOB_CLK_ENABLE();           //开启GPIOC时钟	
	__HAL_RCC_GPIOC_CLK_ENABLE();           //开启GPIOC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE();           //开启GPIOC时钟

	GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_15;          //P
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);                              //初始化 

	GPIO_Initure.Pin=GPIO_PIN_15;          //P
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLUP;          //
	HAL_GPIO_Init(GPIOB,&GPIO_Initure);                              //初始化 

	GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;          //PC
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOC,&GPIO_Initure);                              //初始化 

	GPIO_Initure.Pin=GPIO_PIN_2;           //PC
	GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
	GPIO_Initure.Pull=GPIO_PULLDOWN;          //
	HAL_GPIO_Init(GPIOD,&GPIO_Initure);                              //初始化 	
}

/*****************************************************************************
-Fuction		: IWDG_Init
-Description	: IWDG_Init
//初始化独立看门狗
//prer:分频数:IWDG_PRESCALER_4~IWDG_PRESCALER_256
//rlr:自动重装载值,0~0XFFF.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).
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
	IWDG_Handler.Init.Prescaler=IWDG_PRESCALER_256;	//设置IWDG分频系数以前4倍数，32秒
	IWDG_Handler.Init.Reload=4095;//多了也设置不进去(4095+4095+4095+4095);		//重装载值32+32+32+32s由于心跳要等待GSM网络需要至少1分钟
   	if (HAL_IWDG_Init(&IWDG_Handler) != HAL_OK)
	{
		Error_Handler();
	}
}
/*****************************************************************************
-Fuction		: IWDG_Feed
-Description	: IWDG_Feed
//喂独立看门狗
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void IWDG_Feed(void)
{   
    HAL_IWDG_Refresh(&IWDG_Handler); 	//喂狗
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

/*********************系统操作基本支持函数***********************/
/*****************************************************************************
-Fuction		: HAL_UART_RxCpltCallback
-Description	: HAL_UART_RxCpltCallback
使用库的接收机制，回调方式
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
/*void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{
		//dev_gsm_DataRecv(Usart1RecvDataBuffer[0]);
	}
	else if(huart->Instance==USART2)//如果是串口1
	{
		GpsRecvDataDev(Usart2RecvDataBuffer[0]);
	}
	else if(huart->Instance==USART6)//如果是串口1
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
此函数不释放传入的内存空间
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
返回类型OS ERR调用本函数的任务将被挂起等待
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
*依据ucos ii系统,如果更换系统需要修改对应内部引用函数
输入:静态的32位的定时器缓存变量
		如果该变量会自动初始化,
		如要自行初始化,需要使用
		OSTimeGet(); 初始化,如果和系统同步启动的定时器
		可以不用初始化. 
输出:自从上次运行经过的秒数
-Input			: none
-Output 		: none
-Return 		: int 0
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/20	  V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
u16 GetDurationSecTimer(u32 *pdwBackupTime)
{//此函数周期要在一秒之内
	u16 wSecond=0; 
	u32 dwCurTime=0; 
	u32 dwDiffTime=0; 	
	u32 dwTmpPoint=0; 
	dwCurTime = OSTimeGet(); 
	if((0==*pdwBackupTime)|| (dwCurTime < *pdwBackupTime)) //首次运行或极端异常,初始化
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
		dwTmpPoint = dwDiffTime % OS_TICKS_PER_SEC; //小数部分
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
-Description	: //后门，插上USB小于1s则复位
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
	g_tUsb.GetUsbState(&ucUsbState);//后门，插上USB小于1s则复位
	if(USB_IS_CONNECT_STATE==ucUsbState)
	{
		OSTimeDlyHMSM(0,0,0,20);//消抖				
		g_tUsb.GetUsbState(&ucUsbState);				
		if(USB_IS_CONNECT_STATE==ucUsbState)
		{
			g_tLed.RedLedOn();	
			OSTimeDlyHMSM(0,0,1,20);
			g_tUsb.GetUsbState(&ucUsbState);//后门，插上USB小于1s则复位
			if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
			{
				OSTimeDlyHMSM(0,0,0,20);//消抖				
				g_tUsb.GetUsbState(&ucUsbState);				
				if(USB_IS_NOT_CONNECT_STATE==ucUsbState)
				{
					OSSemPost(ptOsSemUsedForSystemNeedReset);//不直接调用复位函数
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
