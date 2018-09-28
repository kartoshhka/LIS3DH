/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
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
#include "stm32l4xx_hal.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

/* USER CODE BEGIN 0 */
#include "sensors.h"
uint32_t Timing;
uint32_t Period = 1020;//100000;
uint8_t Fire = 1;
uint8_t AutoOff;
uint8_t Length = 48;
uint8_t Order;
uint16_t Blanking = 72;

uint16_t Sonar_Front;
uint16_t Sonar_Front_Prev;
uint16_t Sonar_Front_First;
uint16_t Distance_Bw_Fronts;
uint16_t Num_Of_Corr_Fronts;

uint8_t Disable_EXTI_IR_for_SNSRs;

uint8_t num_of_sensor;

void GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case GPIO_PIN_5:
			num_of_sensor |= 1<<0;
		break;
		case GPIO_PIN_6:
			num_of_sensor |= 1<<1;
		break;
		case GPIO_PIN_7:
			num_of_sensor |= 1<<2;
		break;
		case GPIO_PIN_8:
			num_of_sensor |= 1<<3;
		break;
	}
	/*Code for Timing*/
	Sonar_Front_Prev = Sonar_Front;
	Sonar_Front = Timing;
	Distance_Bw_Fronts = Sonar_Front - Sonar_Front_Prev;
	if ((Distance_Bw_Fronts > 10) && (Signal_Count < 20)) //probably new pack of impulses or noise
	{
		Signal_Count = 0; //reset if noise
		Sonar_Front_First = Timing;
		Signal_Count++;
	}
	else
		Signal_Count++;
	
	if (Signal_Count > 20)
	{
		Num_Of_Corr_Fronts = Signal_Count;
		Sonar_Reading = Sonar_Front_First;
		Sonar_Get();
	}
	
	/*Code for TIM7 - add FALLING*/
	/**/

}
/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim7;
extern TIM_HandleTypeDef htim16;

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
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
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
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
  /* USER CODE BEGIN MemoryManagement_IRQn 1 */

  /* USER CODE END MemoryManagement_IRQn 1 */
}

/**
* @brief This function handles Prefetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
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
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
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
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles EXTI line4 interrupt.
*/
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */

  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  /* USER CODE BEGIN EXTI4_IRQn 1 */

  /* USER CODE END EXTI4_IRQn 1 */
}

/**
* @brief This function handles EXTI line[9:5] interrupts.
*/
void EXTI9_5_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI9_5_IRQn 0 */
	if (EXTI->PR1 & EXTI_PR1_PIF5)
	{
		EXTI->PR1 |= EXTI_PR1_PIF5;
		GPIO_EXTI_Callback(GPIO_PIN_5);
	}
	if (EXTI->PR1 & EXTI_PR1_PIF6)
	{
		EXTI->PR1 |= EXTI_PR1_PIF6;
		GPIO_EXTI_Callback(GPIO_PIN_6);
	}
	if (EXTI->PR1 & EXTI_PR1_PIF7)
	{
		EXTI->PR1 |= EXTI_PR1_PIF7;
		GPIO_EXTI_Callback(GPIO_PIN_7);
	}
	if (EXTI->PR1 & EXTI_PR1_PIF8)
	{
		EXTI->PR1 |= EXTI_PR1_PIF8;
		GPIO_EXTI_Callback(GPIO_PIN_8);
	}
	else
		num_of_sensor=0;
	
  /* USER CODE END EXTI9_5_IRQn 0 */

  /* USER CODE BEGIN EXTI9_5_IRQn 1 */

  /* USER CODE END EXTI9_5_IRQn 1 */
}

/**
* @brief This function handles TIM1 update interrupt and TIM16 global interrupt.
*/
void TIM1_UP_TIM16_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 0 */
	TIM16->SR &=~TIM_SR_UIF;
	uint8_t i = 0;
	if (Fire && Timing == 0) {
		switch (Order) {
			case 0:
				TIM2->CCR1 = 1080/2;//1180/2;
			break;
			case 1:
				TIM2->CCR2 = 1080/2;//1180/2;
			break;
			case 2:
				TIM2->CCR3 = 1080/2;//1180/2;
			break;
			case 3:
				TIM2->CCR4 = 1080/2;//1180/2;
			break;
		}
		if (AutoOff) Fire = 0;
	}
	if (Timing > Length) {
		TIM2->CCR1 = 0;
		TIM2->CCR2 = 0;
		TIM2->CCR3 = 0;
		TIM2->CCR4 = 0;
		Order++;
		if (Order == 4) Order = 0;
	}
	if (Timing > Length+Blanking)
	{
		EXTI->IMR1 |= (EXTI_IMR1_IM5 |  EXTI_IMR1_IM6 |  EXTI_IMR1_IM7 | EXTI_IMR1_IM8);
		if (Disable_EXTI_IR_for_SNSRs)
		{
			if ((1<<0) & Disable_EXTI_IR_for_SNSRs)
			{
				EXTI->IMR1 &=~ EXTI_IMR1_IM5;
			}
			if ((1<<1) & Disable_EXTI_IR_for_SNSRs)
			{
				EXTI->IMR1 &=~ EXTI_IMR1_IM6;
			}
			if ((1<<2) & Disable_EXTI_IR_for_SNSRs)
			{
				EXTI->IMR1 &=~ EXTI_IMR1_IM7;
			}
			if ((1<<3) & Disable_EXTI_IR_for_SNSRs)
			{
				EXTI->IMR1 &=~ EXTI_IMR1_IM8;
			}
		}
	}
	Timing++;
	if (Timing == Period) 
	{
		Timing = 0;
		EXTI->IMR1 &=~ (EXTI_IMR1_IM5 |  EXTI_IMR1_IM6 |  EXTI_IMR1_IM7 | EXTI_IMR1_IM8);
		Signal_Count = 0;
		Sonar_Front_First = 0;
		Sonar_Front_Prev = 0;
		Sonar_Front = 0;
	}
  /* USER CODE END TIM1_UP_TIM16_IRQn 0 */

  /* USER CODE BEGIN TIM1_UP_TIM16_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM16_IRQn 1 */
}

/**
* @brief This function handles TIM7 global interrupt.
*/
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
	TIM7->SR &=~TIM_SR_UIF;
  /* USER CODE END TIM7_IRQn 0 */
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
