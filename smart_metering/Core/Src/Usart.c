/*
 * Usart.c
 *
 *  Created on: Jul 13, 2023
 *      Author: Admin
 */


#include "Usart.h"
#include "main.h"
#include "MB.h"
#include "stm32l4xx_it.h"
#include "stm32l4xx_hal_def.h"

UART_HandleTypeDef USART_2_HANDLE;
UART_HandleTypeDef USART_3_HANDLE;

extern void Usart_Init(USART_T usart)
{
	if (usart == USART_2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();

		USART_2_HANDLE.Instance = USART2;
		USART_2_HANDLE.Init.BaudRate = 115200;
		USART_2_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
		USART_2_HANDLE.Init.StopBits = UART_STOPBITS_1;
		USART_2_HANDLE.Init.Parity = UART_PARITY_NONE;
		USART_2_HANDLE.Init.Mode = UART_MODE_TX_RX;
		USART_2_HANDLE.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		USART_2_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
		USART_2_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		USART_2_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		  if (HAL_RS485Ex_Init(&USART_2_HANDLE, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
		  {
			Error_Handler();
		  }
		  NVIC_SetPriority(USART2_IRQn, 0);  // Set the priority of the USART2 interrupt
		   NVIC_EnableIRQ(USART2_IRQn);       // Enable the USART2 interrupt in the NVIC

		  /* USER CODE BEGIN USART2_Init 2 */

		  HAL_UART_Receive_IT(&USART_2_HANDLE, &Rx_Data[0], 1);
	}

	if (usart == USART_3)
	{
		USART_3_HANDLE.Instance = USART2;
		USART_3_HANDLE.Init.BaudRate = 115200;
		USART_3_HANDLE.Init.WordLength = UART_WORDLENGTH_8B;
		USART_3_HANDLE.Init.StopBits = UART_STOPBITS_1;
		USART_3_HANDLE.Init.Parity = UART_PARITY_NONE;
		USART_3_HANDLE.Init.Mode = UART_MODE_TX_RX;
		USART_3_HANDLE.Init.HwFlowCtl = UART_HWCONTROL_NONE;
		USART_3_HANDLE.Init.OverSampling = UART_OVERSAMPLING_16;
		USART_3_HANDLE.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
		USART_3_HANDLE.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
		  if (HAL_RS485Ex_Init(&USART_3_HANDLE, UART_DE_POLARITY_HIGH, 0, 0) != HAL_OK)
		  {
			Error_Handler();
		  }
		  NVIC_SetPriority(USART3_IRQn, 0);  // Set the priority of the USART2 interrupt
		  NVIC_EnableIRQ(USART3_IRQn);       // Enable the USART2 interrupt in the NVIC
		  /* USER CODE BEGIN USART2_Init 2 */

		  HAL_UART_Receive_IT(&USART_3_HANDLE, &Rx_Data[0], 1);
	}
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&USART_2_HANDLE);  // Call the HAL UART interrupt handler for USART2
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&USART_3_HANDLE);  // Call the HAL UART interrupt handler for USART2
}

