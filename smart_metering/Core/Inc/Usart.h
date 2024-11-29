/*
 * Usart.h
 *
 *  Created on: Jul 13, 2023
 *      Author: Admin
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "main.h"

typedef enum
{
	USART_1,
	USART_2,
	USART_3
} USART_T;

extern void Usart_Init(USART_T usart);

#endif /* INC_USART_H_ */
