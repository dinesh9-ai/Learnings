/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include <stdarg.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRC_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void printmsg(char *format,...);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char msg1[] = "Hello for UART\r\n";
char msg2[] = "Hello for Application\r\n";

uint8_t supported_commands[] = {
                               BL_GET_VER ,
                               BL_GET_HELP,
                               BL_GET_CID,
							   BL_GET_RDP_Status,
							   BL_GOTO_ADDR,
							   BL_CID_FLASH_ERASE,
                               BL_MEM_WRITE,
							   BL_READ_SECTOR_STATUS} ;

#define rcv_rx_len 200
#define C_UART &huart1
#define D_UART &huart2



uint8_t bl_rx_buffer[rcv_rx_len];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CRC_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	if( HAL_GPIO_ReadPin(B1_GPIO_Port,B1_Pin) == GPIO_PIN_RESET)
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)msg1,sizeof(msg1),HAL_MAX_DELAY);
		bootloader_read_uart_data();

	}
	else
	{
		HAL_UART_Transmit(&huart1,(uint8_t*)msg2,sizeof(msg2),HAL_MAX_DELAY);
		bootloader_jump_to_user_app();

	}

}

void printmsg(char *format,...)
{
	char str[80];
	va_list args;
	va_start(args,format);
	vsprintf(str,format,args);
	HAL_UART_Transmit(D_UART,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);
	va_end(args);
}

void bootloader_read_uart_data(void)
{
	uint8_t rcv_len =0;

	while(1)
	{
		memset(bl_rx_buffer,0,200);
		HAL_UART_Receive(C_UART,bl_rx_buffer,1,HAL_MAX_DELAY);
		rcv_len = bl_rx_buffer[0];
		HAL_UART_Receive(C_UART,&bl_rx_buffer[1],rcv_len,HAL_MAX_DELAY);
		switch(bl_rx_buffer[1])
		{
			case BL_GET_VER:
				bootloader_handle_getver_cmd(bl_rx_buffer);
				break;

			case BL_GET_HELP:
				bootloader_handle_gethelp_cmd(bl_rx_buffer);
				break;

			case BL_GET_CID:
				bootloader_handle_getcid_cmd(bl_rx_buffer);
				break;

			case BL_GET_RDP_Status:
				bootloader_handle_getrdp_cmd(bl_rx_buffer);
				break;

			case BL_GOTO_ADDR:
				bootloader_handle_go_cmd(bl_rx_buffer);
				break;

			case BL_CID_FLASH_ERASE:
				bootloader_handle_flash_erase_cmd(bl_rx_buffer);
				break;

			case BL_MEM_WRITE:
				bootloader_handle_mem_write_cmd(bl_rx_buffer);
				break;

			case BL_ENDIS_RW_PROTECT:
				bootloader_handle_endis_rw_protect(bl_rx_buffer);
				break;

			case BL_MEM_READ:
				bootloader_handle_mem_read(bl_rx_buffer);
				break;

			case BL_READ_SECTOR_STATUS:
				bootloader_handle_read_sector_status(bl_rx_buffer);
				break;

			case BL_OTP_READ:
				bootloader_handle_read_otp(bl_rx_buffer);
				break;
		}

	}
}

void bootloader_jump_to_user_app(void)
{
	void (*app_reset_handler)(void);

	uint32_t msp_value = *(volatile uint32_t *) FLASH_SECTOR2_BASE_ADDRESS;

	__set_MSP(msp_value);

	uint32_t reset_handler_address = *(volatile uint32_t *) (FLASH_SECTOR2_BASE_ADDRESS + 4);

	app_reset_handler = (void*) reset_handler_address;

	app_reset_handler();
}


/******** **********/
void bootloader_handle_getver_cmd(uint8_t *b1_rx_buffer)
{
	 uint8_t bl_version;

	// 1) verify the checksum
	  printmsg("BL_DEBUG_MSG:bootloader_handle_getver_cmd\r\n");

	 //Total length of the command packet
	  uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

	  //extract the CRC32 sent by the Host
	  uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

	if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
	{
		printmsg("BL_DEBUG_MSG:checksum success !!\r\n");
		// checksum is correct..
		bootloader_send_ack(bl_rx_buffer[0],1);
		bl_version=get_bootloader_version();
		printmsg("BL_DEBUG_MSG:BL_VER : %d %#x\r\n",bl_version,bl_version);
		bootloader_uart_write_data(&bl_version,1);

	}else
	{
		printmsg("BL_DEBUG_MSG:checksum fail !!\r\n");
		//checksum is wrong send nack
		bootloader_send_nack();
	}
}

/******** **********/
void bootloader_handle_gethelp_cmd(uint8_t *pBuffer)
{
	 	 printmsg("BL_DEBUG_MSG:bootloader_handle_gethelp_cmd\n");

		//Total length of the command packet
		uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

		//extract the CRC32 sent by the Host
		uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

		if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
		{
	        printmsg("BL_DEBUG_MSG:checksum success !!\n");
	        bootloader_send_ack(pBuffer[0],sizeof(supported_commands));
	        bootloader_uart_write_data(supported_commands,sizeof(supported_commands) );

		}else
		{
	        printmsg("BL_DEBUG_MSG:checksum fail !!\n");
	        bootloader_send_nack();
		}
}

/******** **********/
void bootloader_handle_getcid_cmd(uint8_t *pBuffer)
{
		uint16_t bl_cid_num = 0;
		printmsg("BL_DEBUG_MSG:bootloader_handle_getcid_cmd\n");

	    //Total length of the command packet
		uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

		//extract the CRC32 sent by the Host
		uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

		if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
		{
	        printmsg("BL_DEBUG_MSG:checksum success !!\n");
	        bootloader_send_ack(pBuffer[0],2);
	        bl_cid_num = get_mcu_chip_id();
	        printmsg("BL_DEBUG_MSG:MCU id : %d %#x !!\n",bl_cid_num, bl_cid_num);
	        bootloader_uart_write_data((uint8_t *)&bl_cid_num,2);

		}else
		{
	        printmsg("BL_DEBUG_MSG:checksum fail !!\n");
	        bootloader_send_nack();
		}
}

/******** **********/
void bootloader_handle_getrdp_cmd(uint8_t *b1_rx_buffer)
{
		uint8_t rdp_level = 0x00;
	    printmsg("BL_DEBUG_MSG:bootloader_handle_getrdp_cmd\n");

	    //Total length of the command packet
		uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

		//extract the CRC32 sent by the Host
		uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

		if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
		{
	        printmsg("BL_DEBUG_MSG:checksum success !!\n");
	        bootloader_send_ack(pBuffer[0],1);
	        rdp_level = get_flash_rdp_level();
	        printmsg("BL_DEBUG_MSG:RDP level: %d %#x\n",rdp_level,rdp_level);
	        bootloader_uart_write_data(&rdp_level,1);

		}else
		{
	        printmsg("BL_DEBUG_MSG:checksum fail !!\n");
	        bootloader_send_nack();
		}
}

/******** **********/
void bootloader_handle_go_cmd(uint8_t *b1_rx_buffer)
{
		uint32_t go_address=0;
	    uint8_t addr_valid = ADDR_VALID;
	    uint8_t addr_invalid = ADDR_INVALID;

	    printmsg("BL_DEBUG_MSG:bootloader_handle_go_cmd\n");

	    //Total length of the command packet
		uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

		//extract the CRC32 sent by the Host
		uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

		if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
		{
	        printmsg("BL_DEBUG_MSG:checksum success !!\n");

	        bootloader_send_ack(pBuffer[0],1);

	        //extract the go address
	        go_address = *((uint32_t *)&pBuffer[2] );
	        printmsg("BL_DEBUG_MSG:GO addr: %#x\n",go_address);

	        if( verify_address(go_address) == ADDR_VALID )
	        {
	            //tell host that address is fine
	            bootloader_uart_write_data(&addr_valid,1);

	            /*jump to "go" address.
	            we dont care what is being done there.
	            host must ensure that valid code is present over there
	            Its not the duty of bootloader. so just trust and jump */

	            /* Not doing the below line will result in hardfault exception for ARM cortex M */
	            //watch : https://www.youtube.com/watch?v=VX_12SjnNhY

	            go_address+=1; //make T bit =1

	            void (*lets_jump)(void) = (void *)go_address;

	            printmsg("BL_DEBUG_MSG: jumping to go address! \n");

	            lets_jump();

			}else
			{
	            printmsg("BL_DEBUG_MSG:GO addr invalid ! \n");
	            //tell host that address is invalid
	            bootloader_uart_write_data(&addr_invalid,1);
			}

		}else
		{
	        printmsg("BL_DEBUG_MSG:checksum fail !!\n");
	        bootloader_send_nack();
		}
}

/******** **********/
void bootloader_handle_flash_erase_cmd(uint8_t *b1_rx_buffer)
{
		uint8_t erase_status = 0x00;
	    printmsg("BL_DEBUG_MSG:bootloader_handle_flash_erase_cmd\n");

	    //Total length of the command packet
		uint32_t command_packet_len = bl_rx_buffer[0]+1 ;

		//extract the CRC32 sent by the Host
		uint32_t host_crc = *((uint32_t * ) (bl_rx_buffer+command_packet_len - 4) ) ;

		if (! bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc))
		{
	        printmsg("BL_DEBUG_MSG:checksum success !!\n");
	        bootloader_send_ack(pBuffer[0],1);
	        printmsg("BL_DEBUG_MSG:initial_sector : %d  no_ofsectors: %d\n",pBuffer[2],pBuffer[3]);

	        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,1);
	        erase_status = execute_flash_erase(pBuffer[2] , pBuffer[3]);
	        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,0);

	        printmsg("BL_DEBUG_MSG: flash erase status: %#x\n",erase_status);

	        bootloader_uart_write_data(&erase_status,1);

		}else
		{
	        printmsg("BL_DEBUG_MSG:checksum fail !!\n");
	        bootloader_send_nack();
		}
}

/******** **********/
void bootloader_handle_mem_write_cmd(uint8_t *b1_rx_buffer)
{

}

/******** **********/
void bootloader_handle_endis_rw_protect(uint8_t *b1_rx_buffer)
{

}

/******** **********/
void bootloader_handle_mem_read(uint8_t *b1_rx_buffer)
{

}

/******** **********/
void bootloader_handle_read_sector_status(uint8_t *b1_rx_buffer)
{

}

/******** **********/
void bootloader_handle_read_otp(uint8_t *b1_rx_buffer)
{

}

void bootloader_send_ack(uint8_t command_code, uint8_t follow_len)
{
	uint8_t ack_buf[2];
	ack_buf[0] = BL_ACK;
	ack_buf[1] = follow_len;

	HAL_UART_Transmit(C_UART,ack_buf,2,HAL_MAX_DELAY);
}

void bootloader_send_nack(void)
{
	uint8_t nack = BL_NACK;
	HAL_UART_Transmit(C_UART,&nack,1,HAL_MAX_DELAY);
}

uint8_t bootloader_verify_crc (uint8_t *pData, uint32_t len, uint32_t crc_host)
{
    uint32_t uwCRCValue=0xff;

    for (uint32_t i=0 ; i < len ; i++)
	{
        uint32_t i_data = pData[i];
        uwCRCValue = HAL_CRC_Accumulate(&hcrc, &i_data, 1);
	}

	 /* Reset CRC Calculation Unit */
  __HAL_CRC_DR_RESET(&hcrc);

	if( uwCRCValue == crc_host)
	{
		return VERIFY_CRC_SUCCESS;
	}

	return VERIFY_CRC_FAIL;
}

uint8_t get_bootloader_version(void)
{
  return (uint8_t)BL_VERSION;
}

uint8_t get_flash_rdp_level(void)
{

	uint8_t rdp_status=0;
#if 0
	FLASH_OBProgramInitTypeDef  ob_handle;
	HAL_FLASHEx_OBGetConfig(&ob_handle);
	rdp_status = (uint8_t)ob_handle.RDPLevel;
#else

	 volatile uint32_t *pOB_addr = (uint32_t*) 0x1FFFC000;
	 rdp_status =  (uint8_t)(*pOB_addr >> 8) ;
#endif

	return rdp_status;

}

void bootloader_uart_write_data(uint8_t *pBuffer,uint32_t len)
{
    /*you can replace the below ST's USART driver API call with your MCUs driver API call */
	HAL_UART_Transmit(C_UART,pBuffer,len,HAL_MAX_DELAY);

}

uint16_t get_mcu_chip_id(void)
{
/*
	The STM32F446xx MCUs integrate an MCU ID code. This ID identifies the ST MCU partnumber
	and the die revision. It is part of the DBG_MCU component and is mapped on the
	external PPB bus (see Section 33.16 on page 1304). This code is accessible using the
	JTAG debug pCat.2ort (4 to 5 pins) or the SW debug port (two pins) or by the user software.
	It is even accessible while the MCU is under system reset. */
	uint16_t cid;
	cid = (uint16_t)(DBGMCU->IDCODE) & 0x0FFF;
	return  cid;

}

uint8_t execute_flash_erase(uint8_t sector_number , uint8_t number_of_sector)
{
    //we have totally 8 sectors in STM32F446RE mcu .. sector[0 to 7]
	//number_of_sector has to be in the range of 0 to 7
	// if sector_number = 0xff , that means mass erase !
	//Code needs to modified if your MCU supports more flash sectors
	FLASH_EraseInitTypeDef flashErase_handle;
	uint32_t sectorError;
	HAL_StatusTypeDef status;


	if( number_of_sector > 8 )
		return INVALID_SECTOR;

	if( (sector_number == 0xff ) || (sector_number <= 7) )
	{
		if(sector_number == (uint8_t) 0xff)
		{
			flashErase_handle.TypeErase = FLASH_TYPEERASE_MASSERASE;
		}else
		{
		    /*Here we are just calculating how many sectors needs to erased */
			uint8_t remanining_sector = 8 - sector_number;
            if( number_of_sector > remanining_sector)
            {
            	number_of_sector = remanining_sector;
            }
			flashErase_handle.TypeErase = FLASH_TYPEERASE_SECTORS;
			flashErase_handle.Sector = sector_number; // this is the initial sector
			flashErase_handle.NbSectors = number_of_sector;
		}
		flashErase_handle.Banks = FLASH_BANK_1;

		/*Get access to touch the flash registers */
		HAL_FLASH_Unlock();
		flashErase_handle.VoltageRange = FLASH_VOLTAGE_RANGE_3;  // our mcu will work on this voltage range
		status = (uint8_t) HAL_FLASHEx_Erase(&flashErase_handle, &sectorError);
		HAL_FLASH_Lock();

		return status;
	}


	return INVALID_SECTOR;
}

/**
  * @briebootloader_jump_to_user_appf System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
