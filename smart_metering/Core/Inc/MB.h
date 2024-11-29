/*
 * MB.h
 *
 *  Created on: Jul 12, 2023
 *      Author: Admin
 */

#ifndef INC_MB_H_
#define INC_MB_H_

#define C_MB1_INBOX_LENGTH 265								 //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
#define C_MB1_OUTBOX_LENGTH 265

#define c_MB1_FC66_Length 5									//Read Module Information

/* MB1 RS485 Line Selection */
#define c_RS485_Tx 		1
#define c_RS485_Rx 		2

/* MB1 Length*/
#define c_MB1_FC01_Length 0									//Input Coils
#define c_MB1_FC02_Length 0									//Output Coils
#define c_MB1_FC03_Length 128								//Read Holding Registers
#define c_MB1_FC04_Length 1024								//Read Input Registers

#define GPIO_PA1_Mode_Out_PP_80Mhz				               RCC->APB2ENR |= 0x00000001; GPIOA->OSPEEDR = (GPIOA->OSPEEDR & 0xFFFFFF0F)|0x000000C0;
#define MB1_RS485_Tx                                           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
#define MB1_RS485_Rx                                           HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

/* MB1 USART Macro Definition*/
#define c_USART1  	1											     //DATE:21/02/2022  EDITED BY GAGAN GOWDA G R
#define c_USART2 	2											 	 //DATE:21/02/2022  EDITED BY GAGAN GOWDA G R
#define c_USART3 	3											 	 //DATE:21/02/2022  EDITED BY GAGAN GOWDA G R

/* Slave ID */
#define c_SLOT_ADDRESS 	(GPIOE->IDR>>12) & 0x000F;

/* Module Information */
#define c_ModuleType	1                                   // New Module
#define c_SlotNo		1                                   // Fixed to Slot No

typedef struct
{
	union
	{
		unsigned short Data;
		struct
		{
			unsigned short LSB				:8;
			unsigned short MSB				:8;
		}bit;

	}byte;
}MB1_Split;

typedef struct
{
	union
	{
		unsigned short array[5];
		struct
		{
			unsigned int Type 				:8 ;
			unsigned int Slotid	   			:8 ;
			unsigned int HoldregLength		:16 ;
			unsigned int InputregLength		:16 ;
			unsigned int OutputcoilLength	:16 ;
			unsigned int InputcoilLength	:16 ;
		}bit;
	}byte;
}MB1_ModuleInfo;
extern MB1_ModuleInfo ModuleInfo;

extern unsigned short MB1_InputRegisters[];
extern unsigned short MB1_HoldingRegisters[];

extern unsigned char MB1_inbox[C_MB1_INBOX_LENGTH ];			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
extern unsigned char MB1_outbox[C_MB1_OUTBOX_LENGTH]; 			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//


extern unsigned short l_MB1_Rx_index;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
extern unsigned short l_MB1_Tx_index;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//


extern unsigned short l_MB1_Tx_Count;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//

extern unsigned short l_MB1_TxDelayCount;	 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
extern unsigned short l_MB1_RxDelayCount;	 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//

extern unsigned short gl_MB1_ReceivedByteCount;
extern unsigned short gl_MB1_ReceiveFrameComplete_Flag;

unsigned short * MB1_GetAddress_InputReg(void);
unsigned short * MB1_GetAddress_HoldingReg(void);

void GetUSART2RxCount(unsigned short * p_Rxcount,unsigned short * p_CompltFlag);
void MB1_SelectRS485Line(unsigned short p_TxRxLine);
void MB1_process(void);
void MB1_Initialise(void);


#endif /* INC_MB_H_ */
