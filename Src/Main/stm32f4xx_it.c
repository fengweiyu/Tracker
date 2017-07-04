/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

#include "main.h"
#include "ucos_ii.h"
#include "Config.h"
#include "app_GpsManage.h"
#include "app_PowerChargeCheck.h"
#include "app_Gsm.h"
#include "app_WifiSupport.h"
#include "app_TimeManage.h"
#include "Bsp.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

/******************************************************************************/
/*            Cortex-M4 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
* @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
  	DebugPrintf(EMERG"HardFault_Handler err!>>SystemResetNow>>>>\r\n");
  	NVIC_SystemReset();
  }
  /* USER CODE BEGIN HardFault_IRQn 1 */

  /* USER CODE END HardFault_IRQn 1 */
}

/**
* @brief This function handles Memory management fault.
*/
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
	  DebugPrintf(EMERG"MemManage_Handler err!\r\n");
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {  	
	DebugPrintf(EMERG"BusFault_Handler err!\r\n");
  }
  /* USER CODE BEGIN BusFault_IRQn 1 */

  /* USER CODE END BusFault_IRQn 1 */
}

/**
* @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {  	
	DebugPrintf(EMERG"UsageFault_Handler err!\r\n");
  }
  /* USER CODE BEGIN UsageFault_IRQn 1 */

  /* USER CODE END UsageFault_IRQn 1 */
}

/**
* @brief This function handles System service call via SWI instruction.
*/
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
* @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
* @brief This function handles Pendable request for system service.
*/
//void PendSV_Handler(void)
//{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
//}

/**
* @brief This function handles System tick timer.
*/
//void SysTick_Handler(void)
//{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
 // HAL_IncTick();
//  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
//}
/*****************************************************************************
-Fuction		: SysTick_Handler
-Description	: SysTick_Handler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void SysTick_Handler(void)
{
    OS_CPU_SR  cpu_sr;

    HAL_IncTick();
    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntEnter();
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */

}

/*****************************************************************************
-Fuction		: USART1_IRQHandler
-Description	: USART1_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/27	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void USART1_IRQHandler(void)  
{
	g_tGPS.GpsRecvData();
}

/*****************************************************************************
-Fuction		: USART2_IRQHandler
-Description	: USART2_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/02	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void USART2_IRQHandler(void)  
{
	g_tGsm.GsmRecvData();
}

/*****************************************************************************
-Fuction		: USART6_IRQHandler
-Description	: USART6_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/09/02	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void USART6_IRQHandler(void)  
{
	g_tWifi.WifiRecvData();
}

/*****************************************************************************
-Fuction		: RTC_Alarm_IRQHandler
-Description	: RTC_Alarm_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RTC_Alarm_IRQHandler(void)
{
	OSIntEnter();
	g_tTime.AlarmHandle();
	OSIntExit();
}
/*****************************************************************************
-Fuction		: RTC_Alarm_IRQHandler
-Description	: RTC_Alarm_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/10/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void RTC_WKUP_IRQHandler(void)
{
	OSIntEnter();
	g_tTime.WakeUpHandle();
	OSIntExit();
}

/*****************************************************************************
-Fuction		: EXTI4_IRQHandler
-Description	: EXTI4_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI0_IRQHandler(void)
{
	OSIntEnter();

	//OSTaskResume(CHECK_TRACKER_STATE_TASK_PRIORITY);
	//OSSemPost(ptOsSemUsedForCheckedGpsPPS);
	//HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);//调用中断处理公用函数

	OSIntExit();

}

/*****************************************************************************
-Fuction		: EXTI4_IRQHandler
-Description	: EXTI4_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI1_IRQHandler(void)
{
	OSIntEnter();

	//OSTaskResume(CHECK_TRACKER_STATE_TASK_PRIORITY);//不使用
	//OSSemPost(ptOsSemUsedForCheckedGsmState);
	//HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);//调用中断处理公用函数

	OSIntExit();

}

/*****************************************************************************
-Fuction		: EXTI2_IRQHandler
-Description	: EXTI2_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI2_IRQHandler(void)
{
	OSIntEnter();
	
	OSTaskResume(CHECK_TRACKER_STATE_TASK_PRIORITY);
	OSSemPost(ptOsSemUsedForCheckedDropState);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);//调用中断处理公用函数
	
	OSIntExit();
}
/*****************************************************************************
-Fuction		: EXTI3_IRQHandler
-Description	: EXTI3_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI3_IRQHandler(void)
{
	OSIntEnter();

	OSTaskResume(CHECK_TRACKER_STATE_TASK_PRIORITY);
	OSSemPost(ptOsSemUsedForCheckedCutState);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);//调用中断处理公用函数
	
	OSIntExit();
}

/*****************************************************************************
-Fuction		: EXTI4_IRQHandler
-Description	: EXTI4_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI4_IRQHandler(void)
{
	OSIntEnter();

	//OSTaskResume(BATTERY_CHARGE_TASK_PRIORITY);//充电状态使用USB VBUS判断
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);//调用中断处理公用函数
	
	OSIntExit();
}

/*****************************************************************************
-Fuction		: EXTI4_IRQHandler
-Description	: EXTI4_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
	OSIntEnter();

	OSTaskResume(BATTERY_CHARGE_TASK_PRIORITY);
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);//调用中断处理公用函数
	
	OSIntExit();
}

/*****************************************************************************
-Fuction		: EXTI4_IRQHandler
-Description	: EXTI4_IRQHandler
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	OSIntEnter();

	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		OSSemPost(ptOsSemUsedForCheckedGsmState);
	 	 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
	}
	else{
	}
	if(__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET)
	{
		//OSSemPost(ptOsSemUsedForCheckedGpsPPS);
	 	 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
	}
	else{
	}
	OSTaskResume(CHECK_TRACKER_STATE_TASK_PRIORITY);
	
	OSIntExit();
}
/*****************************************************************************
-Fuction		: USE_USB_OTG_FS
-Description	: USE_USB_OTG_FS
-Input			: 
-Output 		: 
-Return 		: 
* Modify Date	  Version		 Author 		  Modification
* -----------------------------------------------
* 2016/08/09	   V1.0.0		 Yu Weifeng 	 Created
******************************************************************************/

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB On The Go FS global interrupt.
*/
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */
  OSIntEnter();
  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */
  OSIntExit();

  /* USER CODE END OTG_FS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
