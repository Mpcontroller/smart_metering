/*
 * call_back_function.c
 *
 *  Created on: Apr 18, 2024
 *      Author: Admin
 */
#include "MB.h"
#include "Usart.h"
#include "main.h"
#include "vmm.h"
#include "stm32l4xx_hal_def.h"
#include "smart_metering.h"
#include "call_back_functions.h"

extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef USART_2_HANDLE;
extern UART_HandleTypeDef USART_3_HANDLE;

unsigned char BP35C5_Inbox[200];
unsigned short rx_index = 0;

unsigned short l_EOF_Rx_Flag=0;
unsigned short l_EOF_Tx_Flag;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		BP35C5_Inbox[rx_index++] = Inbox[0];
		HAL_UART_Receive_IT(&huart1, (uint8_t *)Inbox, 1);
		l_EOF_Rx_Flag = 1;
		MX_TIM17_Init();
	}
	else if (huart == &USART_2_HANDLE)
	{
		if(l_MB1_Rx_index > C_MB1_INBOX_LENGTH)				 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022//
		l_MB1_Rx_index = 0;
		MB1_inbox[l_MB1_Rx_index] = USART_2_HANDLE.Instance->RDR;  // Store received byte in buffer
		l_MB1_Rx_index++;
		HAL_UART_Receive_IT(&USART_2_HANDLE, &Rx_Data[0], 1);
		l_EOF_Rx_Flag=1;
		MX_TIM16_Init();
	}
	else if(huart == &USART_3_HANDLE)
    {
  		if(l_MB1_Rx_index > C_MB1_INBOX_LENGTH)				 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022//
  		l_MB1_Rx_index = 0;
  		MB1_inbox[l_MB1_Rx_index] = USART_3_HANDLE.Instance->RDR;  // Store received byte in buffer
  		l_MB1_Rx_index++;
  		HAL_UART_Receive_IT(&USART_3_HANDLE, &Rx_Data[0], 1);
  		l_EOF_Rx_Flag=1;
  		MX_TIM16_Init();
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart == &huart1)
	{
		HAL_UART_Receive_IT(&huart1, (uint8_t *)Inbox, 1);
	}
	else if (huart == &USART_2_HANDLE)
	{
		// USART2->TDR=MB1_outbox[l_MB1_Tx_index];	//transmit a byte;
		 //if(l_MB1_Tx_index++>=l_MB1_Tx_Count)
		 //{
		 	 l_MB1_Tx_index	= 0;
			 l_EOF_Tx_Flag 	= 1;
			// MX_TIM16_Init();
			 HAL_UART_Receive_IT(&USART_2_HANDLE, &Rx_Data[0], 1);
	 }
	 else if(huart == &USART_3_HANDLE)
	 {
		USART2->TDR=MB1_outbox[l_MB1_Tx_index];	//transmit a byte;
		if(l_MB1_Tx_index++>=l_MB1_Tx_Count)
		{
		 	 l_MB1_Tx_index	= 0;
			 l_EOF_Tx_Flag 	= 1;
			 MX_TIM16_Init();
			 HAL_UART_Receive_IT(&USART_3_HANDLE, &Rx_Data[0], 1);
		}
	 }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM16)
    {
    	if(l_EOF_Rx_Flag==1)
    	{
    		l_EOF_Rx_Flag=0;
    		GetUSART2RxCount(&gl_MB1_ReceivedByteCount,&gl_MB1_ReceiveFrameComplete_Flag);
    	}

    	if(l_EOF_Tx_Flag==1)
    	{
    		l_EOF_Tx_Flag=0;
    		MB1_SelectRS485Line(c_RS485_Rx);
    	}

		HAL_TIM_Base_Stop_IT(&htim16);
    }

    else if (htim->Instance == TIM17)
	{
    	if(l_EOF_Rx_Flag==1)
		{
			l_EOF_Rx_Flag=0;
			BP35C5_RecieveQuery_Flag = 1;
		}
	}

    else if(htim->Instance == TIM7)
    {
    	MX_TIM7_Init();
    	BP35C5_SendResponse_Flag = 1;
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	gl_ADC_samplingConv_Cmplt_flag = 1;
}

