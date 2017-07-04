/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : spi_flash.h
* Author             : MCD Application Team
* Date First Issued  : 02/05/2007
* Description        : Header for spi_flash.c file.
********************************************************************************
* History:
* 05/21/2007: V0.3
* 04/02/2007: V0.2
* 02/05/2007: V0.1
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "CBasicTools.h"

#define FLASH_SPI	SPI1

#define W25Q64FV

#define SPI_FLASH_PageSize 256

/* Private typedef -----------------------------------------------------------*/
#ifdef W25Q64FV
#define WRITE      	0x02  /* Write to Memory instruction */

#define EWSR 		0x50  /* Enable-Write-Status-Register */
#define WRSR       	0x01  /* Write Status Register instruction */

#define WREN       	0x06  /* Write enable instruction */
#define WRDIS      	0x04  /* Write diable instruction */

#define READ       		0x03  /* Read from Memory instruction */
#define HIGHT_READ 	0x0B  /* Read from Memory instruction */
#define RDSR       		0x05  /* Read Status Register-1 instruction  */
#define RDSR2		0x35	/*Read Status Register-2 instruction  */
#define RDID       		0x9F  /* Read identification */

#define SE4K       		0x20  /* Sector Erase instruction(4K) */
#define SE32K      	0x52  /* Sector Erase instruction(32K) */
#define SE64K      	0xD8  /* Sector Erase instruction(64K) */
#define CHIP_ERASE 	0xC7  /* Chip Erase instruction */	 

#define WIP_Flag   	0x01  /* Write In Progress (WIP) flag */
#else
#define WRITE      	0x02  /* Write to Memory instruction */
#define AAI        		0xAD  /* AAI word program instruction */

#define EWSR 		0x50  /* Enable-Write-Status-Register */
#define WRSR       	0x01  /* Write Status Register instruction */

#define WREN       	0x06  /* Write enable instruction */
#define WRDIS      	0x04  /* Write diable instruction */

#define READ       		0x03  /* Read from Memory instruction */
#define HIGHT_READ 	0x0B  /* Read from Memory instruction */
#define RDSR       		0x05  /* Read Status Register instruction  */
#define RDID       		0x9F  /* Read identification */

#define SE4K       		0x20  /* Sector Erase instruction(4K) */
#define SE32K      	0x20  /* Sector Erase instruction(32K) */
#define SE64K      	0x20  /* Sector Erase instruction(64K) */
#define CHIP_ERASE 	0xC7  /* Chip Erase instruction */	 

#define WIP_Flag   	0x01  /* Write In Progress (WIP) flag */
#endif

#define Dummy_Byte 0xA5

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if 0
#define GPIO_SPI		GPIOA
#define SPI_SCK		GPIO_Pin_5
#define SPI_SDO		GPIO_Pin_7
#define SPI_SDI		GPIO_Pin_6

#define PIN_NUM_SPI_FLASH_CS_CTL	4
#define SPI_FLASH_CS_CTL	PAout(PIN_NUM_SPI_FLASH_CS_CTL)
/* Select SPI FLASH: ChipSelect pin low  */
#define SPI_FLASH_CS_LOW() 		M_ClrBit(SPI_FLASH_CS_CTL)//GPIO_ResetBits(GPIOA, GPIO_Pin_4)	/*GPIO_ResetBits(GPIOC, GPIO_Pin_13)*/

/* Deselect SPI FLASH: ChipSelect pin high */
#define SPI_FLASH_CS_HIGH() 	M_SetBit(SPI_FLASH_CS_CTL)//GPIO_SetBits(GPIOA, GPIO_Pin_4)		/*GPIO_SetBits(GPIOC, GPIO_Pin_13)*/
#else

#define PIN_NUM_SPI_FLASH_CS_CTL	7
/* Select SPI FLASH: ChipSelect pin low  */
#define SPI_FLASH_CS_LOW()    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET); ////SPI FLASH选中


/* Deselect SPI FLASH: ChipSelect pin high */
#define SPI_FLASH_CS_HIGH() 	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);	////SPI FLASH不选中


#endif

//#define SPI_DLY()	TimeDly(0)


void SPI_FLASH_Init(void);
u8 SPI_FLASH_ReadStatus(void);
void SPI_FLASH_WriteStatus(u8 status_val);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_ByteWrite(u8 *pBuffer, u32 WriteAddr);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
#ifndef W25Q64FV
void SPI_FLASH_AAIWrite(u8 *pBuffer, u32 addr, u16 NumByteToWrite, u8 hbyte);
#endif
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_FLASH_SoDisable(void); 
void SPI_FLASH_WriteDisable(void); 

void SPI_FLASH_Init(void);

#endif /* __SPI_FLASH_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
