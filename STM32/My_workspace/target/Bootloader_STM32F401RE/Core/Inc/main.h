/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdint.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#define BL_GET_VER				0x51
#define BL_GET_HELP				0x52
#define BL_GET_CID				0x53
#define BL_GET_RDP_Status		0x54
#define BL_GOTO_ADDR			0x55
#define BL_CID_FLASH_ERASE		0x56
#define BL_MEM_WRITE			0x57
#define BL_ENDIS_RW_PROTECT		0x58
#define	BL_MEM_READ				0x59
#define BL_READ_SECTOR_STATUS	0x5A
#define BL_OTP_READ				0x5B

#define BL_ACK	0x7F
#define	BL_NACK	0xA5

#define VERIFY_CRC_FAIL    1
#define VERIFY_CRC_SUCCESS 0

#define BL_VERSION 0x10

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void bootloader_handle_getver_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_gethelp_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_getcid_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_getrdp_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_go_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_flash_erase_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_mem_write_cmd(uint8_t *b1_rx_buffer);
void bootloader_handle_endis_rw_protect(uint8_t *b1_rx_buffer);
void bootloader_handle_mem_read(uint8_t *b1_rx_buffer);
void bootloader_handle_read_sector_status(uint8_t *b1_rx_buffer);
void bootloader_handle_read_otp(uint8_t *b1_rx_buffer);

void bootloader_send_ack(uint8_t command_code, uint8_t follow_len);
void bootloader_send_nack(void);

uint8_t bootloader_verify_crc (uint8_t *pData, uint32_t len, uint32_t crc_host);
uint8_t get_bootloader_version(void);
void bootloader_uart_write_data(uint8_t *pBuffer,uint32_t len);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

#define FLASH_SECTOR2_BASE_ADDRESS 0x8008000U
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
void bootloader_read_uart_data(void);
void bootloader_jump_to_user_app(void);
uint8_t bootloader_verify_crc (uint8_t *pData, uint32_t len, uint32_t crc_host);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
