/*
************************************************************************************************************************
* Copyright (c) 2005,航天无线通信技术开发（深圳）有限公司
* 
* MPIP-618  外部FLASH  数据操作

* ZhongChuanFeng.
*
************************************************************************************************************************
*/
/******************** (C) COPYRIGHT 2007 STMicroelectronics ***************************
* File Name          	: spi_flash.c
* Author             	: MCD Application Team
* Date First Issued	: 02/05/2007
* Description        	: This file provides a set of functions needed to manage the
*                      	  communication between SPI peripheral and SPI M25P64 FLASH.
***********************************************************************************
* History:
* 02/15/2011: V0.4  ZhongChuanFeng.
* 05/21/2007: V0.3
* 04/02/2007: V0.2
* 02/05/2007: V0.1
***********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
***********************************************************************************/
#include "CBasicTools.h"
#include "Config.h"
#include "stm32f4xx_hal.h"
#include "spi_flash.h"
#include "ucos_ii.h"
#include "stm32f4xx_hal_spi.h"
SPI_HandleTypeDef SPI1_Handler;  //SPI句柄
static u8 SPI_FLASH_SendByte(u8 byte);

//以下是SPI模块的初始化代码，配置成主机模式 						  
//SPI口初始化
//这里针是对SPI5的初始化
void SPI1_Init(void)
{
    SPI1_Handler.Instance=SPI1;                         //SP5
    SPI1_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI1_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI1_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI1_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //串行同步时钟的空闲状态为高电平
    SPI1_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI1_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI1_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_4;//定义波特率预分频的值:波特率预分频值为42/4=10.5M
    SPI1_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI1_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI1_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI1_Handler.Init.CRCPolynomial=10;                  //CRC值计算的多项式
    HAL_SPI_Init(&SPI1_Handler);//初始化
    
    __HAL_SPI_ENABLE(&SPI1_Handler);                    //使能SPI1
	
    SPI_FLASH_SendByte(0Xff);                           //启动传输
}

//SPI5底层驱动，时钟使能，引脚配置
//此函数会被HAL_SPI_Init()调用
//hspi:SPI句柄
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOA_CLK_ENABLE();       //使能GPIOA时钟
    __HAL_RCC_SPI1_CLK_ENABLE();        //使能SPI1时钟
    
    //PA5,6,7
    GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;              //复用推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;                  //上拉
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;             //快速            
    GPIO_Initure.Alternate=GPIO_AF5_SPI1;           //复用为SPI5
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
}

//SPI速度设置函数
//SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
//fAPB1时钟一般为45Mhz：401为最大42
void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
    __HAL_SPI_DISABLE(&SPI1_Handler);            //关闭SPI
    SPI1_Handler.Instance->CR1&=0XFFC7;          //位3-5清零，用来设置波特率
    SPI1_Handler.Instance->CR1|=SPI_BaudRatePrescaler;//设置SPI速度
    __HAL_SPI_ENABLE(&SPI1_Handler);             //使能SPI
    
}



/**********************************************************************************
* Function Name  	: SPI_FLASH_Init
* Description    	: Initializes the peripherals used by the SPI FLASH driver.
* Input          		: None
* Output         	: None
* Return         	: None
**********************************************************************************/
void SPI_FLASH_Init(void)
{
#if 0
  	SPI_InitTypeDef  SPI_InitStructure;
  	GPIO_InitTypeDef GPIO_InitStructure;
   
  	/* Enable FLASH_SPI and GPIOA clocks */
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_FLASH_SPI | RCC_APB2Periph_GPIOA, ENABLE);
  
  	/* Configure FLASH_SPI pins: NSS, SCK, MISO and MOSI */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Configure PA.4 as Output push-pull, used as Flash Chip select */
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//GPIO_Pin_13;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	//GPIO_Init(GPIOC, &GPIO_InitStructure);
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

  	/* Deselect the FLASH: Chip Select high */
 	SPI_FLASH_CS_HIGH();

  	/* FLASH_SPI configuration */ 
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  	SPI_InitStructure.SPI_CRCPolynomial = 7;
  	SPI_Init(FLASH_SPI, &SPI_InitStructure);
  
  	/* Enable FLASH_SPI  */
  	SPI_Cmd(FLASH_SPI, ENABLE);  
#endif
	
	GPIO_InitTypeDef GPIO_Initure;
 	u32 W25QXX_TYPE;   
	u8 temp;
   	 __HAL_RCC_GPIOA_CLK_ENABLE();           //使能GPIOA时钟
    
    //PA4
   	 GPIO_Initure.Pin=GPIO_PIN_4;            //PA4
    	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    	GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    	GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速         
    	HAL_GPIO_Init(GPIOA,&GPIO_Initure);     //初始化
    	
	SPI_FLASH_CS_HIGH();
	SPI1_Init();							//初始化SPI
	//SPI1_SetSpeed(SPI_BAUDRATEPRESCALER_2); //设置为45/2M时钟,高速模式//不需要不时地设置速度
	W25QXX_TYPE=SPI_FLASH_ReadID();	        //读取FLASH ID.
	if(W25QXX_TYPE==0x00EF4017)                //SPI FLASH为FLASH_ID_W25Q64FV_SPI  IF里面的语句加不加都不影响
	{
	    temp=SPI_FLASH_SendByte(0x15);              //读取状态寄存器3，判断地址模式
	    if((temp&0X01)==0)			        //如果不是4字节地址模式,则进入4字节地址模式
	    {
		SPI_FLASH_CS_LOW();
		SPI_FLASH_SendByte(0xB7);//发送进入4字节地址模式指令   
		SPI_FLASH_CS_HIGH();
	     }
	     else
	     {
	     }
	}
	else
	{
	}
	SPI_FLASH_CS_LOW();
	SPI_FLASH_WriteDisable(); 
	SPI_FLASH_SoDisable(); 
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_SendByte
* Description    	: Sends a byte through the SPI interface and return the byte 
*                  		  received from the SPI bus.
* Input          		: byte : byte to send.
* Output         	: None
* Return         	: The value of the received byte.
*******************************************************************************/
static u8 SPI_FLASH_SendByte(u8 byte)
{
	u8 Rxdata=0;
	u8 ret;
	u32 dwBackupTime,dwCurrentTime=0;
	dwBackupTime=dwCurrentTime=OSTimeGet();
	while((dwCurrentTime-dwBackupTime)<(10*OS_TICKS_PER_SEC))
	{
		dwCurrentTime=OSTimeGet();
		if((__HAL_SPI_GET_FLAG(&SPI1_Handler, SPI_FLAG_TXE))||(__HAL_SPI_GET_FLAG(&SPI1_Handler, SPI_FLAG_RXNE)))
		{
			ret=HAL_SPI_TransmitReceive(&SPI1_Handler,&byte,&Rxdata,1, 50);
			if(HAL_BUSY==ret)
			{
				DebugPrintf("SpiTransmiRecvBusy:%d,State:%d\r\n",ret,SPI1_Handler.State);
				OSTimeDly(50);
			}
			else
			{
				if(HAL_OK!=ret)
				{
					DebugPrintf("SpiTransmiRecvErr:%d,State:%d\r\n",ret,SPI1_Handler.State);
				}
				else
				{
				}
				break;
			}
		}
		else
		{
		}
	}
	if((dwCurrentTime-dwBackupTime)<(10*OS_TICKS_PER_SEC))
	{
	}
	else
	{
		DebugPrintf(ERR"SpiTransmiRecvTimeOut:%d,State:%d\r\n",ret,SPI1_Handler.State);
	}
	return Rxdata;   
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_WaitForWriteEnd
* Description    	: Polls the status of the Write In Progress (WIP) flag in the  FLASH's status
				  register  and  loop  until write  opertaion has completed.  
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
static void SPI_FLASH_WaitForWriteEnd(void)
{
	u8 FLASH_Status = 0;

	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	
	/* Send "Read Status Register" instruction */
	SPI_FLASH_SendByte(RDSR);
  
	/* Loop as long as the memory is busy with a write cycle */
	do
	{
		/* Send a dummy byte to generate the clock needed by the FLASH 
		and put the value of the status register in FLASH_Status variable */
		FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);

	}while((FLASH_Status & WIP_Flag) == SET); /* Write in progress */

	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_WriteEnable
* Description    	: Enables the write access to the FLASH.
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
static void SPI_FLASH_WriteEnable(void)
{
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(WREN);
  	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name	: SPI_FLASH_WriteDisable
* Description    	: Disable the write access to the FLASH.
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
void SPI_FLASH_WriteDisable(void)
{
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(WRDIS);
	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_SoEnable
* Description    	: Enable So pin to indicate the device AAI status.
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
static void SPI_FLASH_SoEnable(void)
{
#ifdef W25Q64FV
#else
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(0x70);
	SPI_FLASH_CS_HIGH();
#endif
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_SoDisable
* Description    	: Disable So pin to indicate the device AAI status.
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
void SPI_FLASH_SoDisable(void)
{
#ifdef W25Q64FV
#else
	SPI_FLASH_CS_LOW();
	SPI_FLASH_SendByte(0x80);
	SPI_FLASH_CS_HIGH();
#endif
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_PollSO
* Description    	: Waiting for AAI end.
* Input          		: None
* Output         	: None
* Return         	: None
*******************************************************************************/
static void SPI_FLASH_Poll_SO()
{
#ifdef W25Q64FV
#else
	u8 temp=0;
	
	SPI_FLASH_CS_LOW();
    	while(temp == 0x00) 		/* waste time until not busy. */
    	{
    		temp = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6);
	}
	SPI_FLASH_CS_HIGH();
#endif	
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_Read_Status.
* Description    	: Read Spi flash status register.
* Input          		: None
* Output         	: None
* Return         	: The value of the status register.
*******************************************************************************/
u8 SPI_FLASH_ReadStatus(void)
{
	u8 status_val;
	
	SPI_FLASH_CS_LOW();
//	SPI_FLASH_SendByte(0x05);
	SPI_FLASH_SendByte(RDSR);
	status_val = SPI_FLASH_SendByte(Dummy_Byte);
	SPI_FLASH_CS_HIGH();
	return(status_val);
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_WriteStatus
* Description    	: Write Spi flash tatus register
* Input          		: The write status value.
* Output         	: None
* Return         	: None
*******************************************************************************/
void SPI_FLASH_WriteStatus(u8 status_val)
{
	SPI_FLASH_CS_LOW();
//	SPI_FLASH_SendByte(0x50);
	SPI_FLASH_SendByte(EWSR);
	SPI_FLASH_CS_HIGH();
	
	SPI_FLASH_CS_LOW();
//	SPI_FLASH_SendByte(0x01);
	SPI_FLASH_SendByte(WRSR);
    	SPI_FLASH_SendByte(status_val);
	SPI_FLASH_CS_HIGH();
	
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  	: SPI_FLASH_SectorErase
* Description    	: Erases the specified FLASH sector.
* Input          		: SectorAddr: address of the sector to erase.
* Output         	: None
* Return         	: None
*******************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
	OSSchedLock();

	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();
	
	/* Sector Erase */ 
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	
	/* Send Sector Erase instruction */
	SPI_FLASH_SendByte(SE4K);
  
	/* Send SectorAddr high nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
	/* Send SectorAddr medium nibble address byte */
	SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
	/* Send SectorAddr low nibble address byte */
	SPI_FLASH_SendByte(SectorAddr & 0xFF);
  
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
	
	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();

	OSSchedUnlock();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BulkErase
* Description    : Erases the entire FLASH.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BulkErase(void)
{
	/* Send write enable instruction */
	SPI_FLASH_WriteEnable();

	/* Bulk Erase */ 
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	
	/* Send Bulk Erase instruction  */
	SPI_FLASH_SendByte(CHIP_ERASE);
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
               
	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_ByteWrite(u8 *pBuffer, u32 WriteAddr)
{
	/* Enable the write access to the FLASH */
	SPI_FLASH_WriteEnable();
  
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	
	/* Send "Write to Memory " instruction */
	SPI_FLASH_SendByte(WRITE);
  
	/* Send WriteAddr high nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
	/* Send WriteAddr medium nibble address byte to write to */
	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);  
	/* Send WriteAddr low nibble address byte to write to */
	SPI_FLASH_SendByte(WriteAddr & 0xFF);
  
	/* Send the current byte */
	SPI_FLASH_SendByte(*pBuffer);
  
	/* Deselect the FLASH: Chip Select high */
	SPI_FLASH_CS_HIGH();
  
  	SPI_FLASH_WriteDisable();
  
	/* Wait the end of Flash writing */
	SPI_FLASH_WaitForWriteEnd();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_PageWrite
* Description    : Writes more than one byte to the FLASH with a single WRITE
*                  cycle(Page WRITE sequence). The number of byte can't exceed
*                  the FLASH page size.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  	/* Enable the write access to the FLASH */
  	SPI_FLASH_WriteEnable();
  
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
	
  	/* Send "Write to Memory " instruction */
  	SPI_FLASH_SendByte(WRITE);
	
  	/* Send WriteAddr high nibble address byte to write to */
  	SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  	/* Send WriteAddr medium nibble address byte to write to */
  	SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);  
  	/* Send WriteAddr low nibble address byte to write to */
  	SPI_FLASH_SendByte(WriteAddr & 0xFF);
  
  	/* while there is data to be written on the FLASH */
  	while(NumByteToWrite--) 
  	{
    		/* Send the current byte */
    		SPI_FLASH_SendByte(*pBuffer);
    		/* Point on the next byte to be written */
    		pBuffer++; 
  	}
  
  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();
  
  	/* Wait the end of Flash writing */
  	SPI_FLASH_WaitForWriteEnd();
}

#ifndef W25Q64FV
/*******************************************************************************
* Function Name  : SPI_FLASH_AAI program.
* Description    : AAI program.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH,
*                    must be equal or less than "SPI_FLASH_PageSize" value. 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_AAIWrite(u8 *pBuffer, u32 addr, u16 NumByteToWrite, u8 hbyte)
{
  	u8 head_byte_flag = FALSE;
  	u8 tail_byte_flag = FALSE;

	if((NULL == pBuffer) ||(0 == NumByteToWrite))
	{
		return;
	}
	if(addr %2)
  	{
  		if(0 == (NumByteToWrite % 2))
		{
			NumByteToWrite ++;
			tail_byte_flag = TRUE;
		}
		
		addr --;
  		head_byte_flag = TRUE;
  	}
	else
	{
		if(NumByteToWrite % 2)
		{
			NumByteToWrite ++;
			tail_byte_flag = TRUE;
		}
	}

	/* Enable the write access to the FLASH */
  	SPI_FLASH_WriteEnable();
	
	SPI_FLASH_SoEnable();
	
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
	
  	/* Send "Write to Memory " instruction */
  	SPI_FLASH_SendByte(0xAD);
	
  	/* Send WriteAddr high nibble address byte to write to */
  	SPI_FLASH_SendByte((addr & 0xFF0000) >> 16);
  	/* Send WriteAddr medium nibble address byte to write to */
  	SPI_FLASH_SendByte((addr & 0xFF00) >> 8);  
  	/* Send WriteAddr low nibble address byte to write to */
  	SPI_FLASH_SendByte(addr & 0xFF);

	if(TRUE == head_byte_flag)
	{
		NumByteToWrite --;
		SPI_FLASH_SendByte(hbyte);
		SPI_FLASH_SendByte(*pBuffer++);
	}
	
  	while(NumByteToWrite > 0) 
  	{
		if((NumByteToWrite == 2) && (tail_byte_flag == TRUE))
	  	{
	  		SPI_FLASH_SendByte(pBuffer[0]);
		  	SPI_FLASH_SendByte(0xFF);
	  	}
	  	else
	  	{
	  		SPI_FLASH_SendByte(pBuffer[0]);
		  	SPI_FLASH_SendByte(pBuffer[1]);
	  	}
		
		NumByteToWrite -= 2;
		pBuffer += 2;
		
		SPI_FLASH_CS_HIGH();
		
#ifdef W25Q64FV
		SPI_FLASH_WaitForWriteEnd(); //@@@
#else		
		SPI_FLASH_Poll_SO();
#endif		
		if(NumByteToWrite == 0) break;

		SPI_FLASH_CS_LOW();
		SPI_FLASH_SendByte(0xAD);
  	}

	SPI_FLASH_WriteDisable();
	
	SPI_FLASH_SoDisable();
	
  	/* Wait the end of Flash writing */
  	SPI_FLASH_WaitForWriteEnd();
}
#endif

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferWrite
* Description    : Writes block of data to the FLASH. In this function, the
*                  number of WRITE cycles are reduced, using Page WRITE sequence.
* Input          : - pBuffer : pointer to the buffer  containing the data to be 
*                    written to the FLASH.
*                  - WriteAddr : FLASH's internal address to write to.
*                  - NumByteToWrite : number of bytes to write to the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  	u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

  	Addr = WriteAddr % SPI_FLASH_PageSize;
  	count = SPI_FLASH_PageSize - Addr;
  	NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
  	NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
  
  	if(Addr == 0) /* WriteAddr is SPI_FLASH_PageSize aligned  */
  	{
    		if(NumOfPage == 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    		{
      			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
    		}
    		else /* NumByteToWrite > SPI_FLASH_PageSize */ 
    		{
      			while(NumOfPage--)
      			{
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        			WriteAddr +=  SPI_FLASH_PageSize;
        			pBuffer += SPI_FLASH_PageSize;  
      			}    
     
      			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
   		}
  	}
  	else /* WriteAddr is not SPI_FLASH_PageSize aligned  */
  	{
    		if(NumOfPage== 0) /* NumByteToWrite < SPI_FLASH_PageSize */
    		{
      			if(NumOfSingle > count) /* (NumByteToWrite + WriteAddr) > SPI_FLASH_PageSize */
      			{
        			temp = NumOfSingle - count;
      
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
        			WriteAddr +=  count;
        			pBuffer += count; 
        
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      			}
      			else
      			{
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      			}
    		}
    		else /* NumByteToWrite > SPI_FLASH_PageSize */
    		{
      			NumByteToWrite -= count;
     			 NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;
      			NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;
      
      			SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
      			WriteAddr +=  count;
      			pBuffer += count;  
     
      			while(NumOfPage--)
      			{
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
        			WriteAddr +=  SPI_FLASH_PageSize;
        			pBuffer += SPI_FLASH_PageSize;
      			}
      
      			if(NumOfSingle != 0)
      			{
        			SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      			}
    		}
  	}
}

/*******************************************************************************
* Function Name  : SPI_FLASH_BufferRead
* Description    : Reads a block of data from the FLASH.
* Input          : - pBuffer : pointer to the buffer that receives the data read 
*                    from the FLASH.
*                  - ReadAddr : FLASH's internal address to read from.
*                  - NumByteToRead : number of bytes to read from the FLASH.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  	u8 tmp_data;
  
  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
	
  	/* Send "Read from Memory " instruction */
  	SPI_FLASH_SendByte(READ);
  	//SPI_FLASH_SendByte(HIGHT_READ);	/* Added by zcf. 2011.09.02.*/
  
  	/* Send ReadAddr high nibble address byte to read from */
  	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  	/* Send ReadAddr medium nibble address byte to read from */
  	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  	/* Send ReadAddr low nibble address byte to read from */
  	SPI_FLASH_SendByte(ReadAddr & 0xFF);

	//SPI_FLASH_SendByte(Dummy_Byte); 	/* Added by zcf. 2011.09.02.*/

  	while(NumByteToRead--) /* while there is data to be read */
  	{
   		tmp_data = SPI_FLASH_SendByte(Dummy_Byte);
    		*pBuffer++ = tmp_data;
  	}
  
  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();
}

/*******************************************************************************
* Function Name  : SPI_FLASH_ReadID
* Description    : Reads FLASH identification.
* Input          : None
* Output         : None
* Return         : FLASH identification
*******************************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  	u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  	/* Select the FLASH: Chip Select low */
  	SPI_FLASH_CS_LOW();
	
  	/* Send "RDID " instruction */
//  	SPI_FLASH_SendByte(0x9F);
  	SPI_FLASH_SendByte(RDID);

  	/* Read a byte from the FLASH */
  	Temp0 = SPI_FLASH_SendByte(Dummy_Byte);

  	/* Read a byte from the FLASH */
  	Temp1 = SPI_FLASH_SendByte(Dummy_Byte);

  	/* Read a byte from the FLASH */
  	Temp2 = SPI_FLASH_SendByte(Dummy_Byte);

  	/* Deselect the FLASH: Chip Select high */
  	SPI_FLASH_CS_HIGH();
         
  	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;

  	return Temp;
}

/*******************************************************************************
* Function Name  : SPI_FLASH_StartReadSequence
* Description    : Initiates a read data byte (READ) sequence from the Flash.
*                  This is done by driving the /CS line low to select the device,
*                  then the READ instruction is transmitted followed by 3 bytes
*                  address. This function exit and keep the /CS line low, so the
*                  Flash still being selected. With this technique the whole
*                  content of the Flash is read with a single READ instruction.
* Input          : - ReadAddr : FLASH's internal address to read from.
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
	/* Select the FLASH: Chip Select low */
	SPI_FLASH_CS_LOW();
	
	/* Send "Read from Memory " instruction */
	SPI_FLASH_SendByte(READ);

	/* Send the 24-bit address of the address to read from -----------------------*/  
	/* Send ReadAddr high nibble address byte */
	SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
	/* Send ReadAddr medium nibble address byte */
	SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
	/* Send ReadAddr low nibble address byte */
	SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

