/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
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

#include <stdint.h>
#include <stdio.h>
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	//Clock Control Register
	uint32_t* pClockControlReg = (uint32_t*) (0x40023830);

	//GPIO-A Mode register
	uint32_t* pPortModeReg = (uint32_t*) (0x40020000);

	//GPIO-A Data out register
	uint32_t* pDataOutReg = (uint32_t*) (0x40020014);

	//GPIO-A Data in register
	uint32_t* pDataInReg = (uint32_t*) (0x40020010);

	uint32_t pinStatus;

	//enable clock for GPIO - A
	*pClockControlReg |= ( 1 << 0 );

	//Set Port Mode of GPIO-A Pin5 to [0,1] at 10,11
	//Reset values at 10,11 to [0,0]
	*pPortModeReg &= ~( 3 << 10 );
	//Set value of 10,11 to [0,1]
	*pPortModeReg |= ( 1 << 10 );
	//Reset Values at 0,1 to [0,0]
	*pPortModeReg &= ~( 3 << 0 );

	*pDataOutReg |= ( 1 << 5 );


	while(1)
	{
		// Set GPIO - A pin 5
		//*pDataOutReg |= ( 1 << 5 );
		//for(uint32_t i =0;i<300000;i++);

		// Reset GPIO - A pin 5
		//*pDataOutReg &= ~( 1 << 5 );
		//for(uint32_t i =0;i<300000;i++);

		pinStatus = (uint32_t) (*pDataInReg & (0x1));
		if( pinStatus )
		{
			*pDataOutReg |= ( 1 << 5 );
		}
		else
		{
			*pDataOutReg &= ~( 1 << 5 );
		}
	}
}