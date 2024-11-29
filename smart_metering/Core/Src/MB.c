/*
 * MB.c
 *
 *  Created on: Jul 12, 2023
 *      Author: Admin
 */
#include "MB.h"
#include "Usart.h"
#include "main.h"
#include "stm32l4xx_hal_def.h"
#include "smart_metering.h"


/* MB1 CRC Polynominal */
#define c_MB1_CRC_POLYNOMIAL 0xA001

/* MB1 Function Code */
#define c_MB1_FC03 		3
#define c_MB1_FC04 		4
#define c_MB1_FC06 		6		                                 //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
#define c_MB1_FC08 		8
#define c_MB1_FC16 		16

/* MB1 Proprietory Function Code) */
#define c_MB1_FC66 		66
#define c_MB1_FC72 		72

/* USART1 Inbox and Outbox Length */
#define c_MB1_INBOX_LENGTH 265								 //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
#define c_MB1_OUTBOX_LENGTH 265								 //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R

//extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef USART_2_HANDLE;
extern UART_HandleTypeDef USART_3_HANDLE;

MB1_Split MB1_U16bit;
//MB1_Address MB1_SlaveID;
MB1_ModuleInfo ModuleInfo;

unsigned short MB1_DelayNo = 5000;



unsigned short gl_MB1_slave_ID = 1;

unsigned short *l_MB1_InputRegisters;
unsigned short *l_MB1_HoldingRegisters;

unsigned short MB1_InputRegisters[c_MB1_FC04_Length];
unsigned short MB1_HoldingRegisters[c_MB1_FC03_Length];

unsigned char MB1_inbox[C_MB1_INBOX_LENGTH ];			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
uint8_t MB1_outbox[C_MB1_OUTBOX_LENGTH]; 			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//


unsigned short l_MB1_Rx_index;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
unsigned short l_MB1_Tx_index;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//


unsigned short l_MB1_Tx_Count;			 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//

unsigned short l_MB1_TxDelayCount = 0;	 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//
unsigned short l_MB1_RxDelayCount = 0;	 //EDITED BY GAGAN GOWDA G R DATE 12/02/2022 GLOBAL VARIABLE TO LOCAL VARIABLE//

unsigned short gl_MB1_FC06_Flag = 0;					 //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R

unsigned short gl_MB1_ReceivedByteCount;
unsigned short gl_MB1_ReceiveFrameComplete_Flag;

unsigned short gl_MB1_Address;
unsigned short gl_MB1_Length;
unsigned short gl_MB1_FC16_Flag = 0;

unsigned short gl_Control_Command = 0;
unsigned short gl_Control_Input = 0;
unsigned short gl_MB1_FC72_Flag = 0;

void MB1_ConfigureUSART(void);
void GetUSART2RxCount(unsigned short * p_Rxcount,unsigned short * p_CompltFlag);
void MB1_delay(void);
void MB1_TransmitQuery(unsigned short);
void MB1_ReceiveQueryProcess(void);
unsigned short MB1_CRC(unsigned short,unsigned char*);

void MB1_ReceiveQuery_F03(unsigned char *,unsigned char *,unsigned short *);					//Read Holding Registers
void MB1_ReceiveQuery_F04(unsigned char *,unsigned char *,unsigned short *);					//Read Input Registers
void MB1_ReceiveQuery_F06(unsigned char *,unsigned char *,unsigned short *);					//Single Write Holding Registers		//DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
void MB1_ReceiveQuery_F16(unsigned char *,unsigned char *,unsigned short *);					//Multiple Write Holding Registers

void MB1_ReceiveQuery_F08(unsigned char *,unsigned char *);										//Loop Back Test
void MB1_ReceiveQuery_F08_DC0(unsigned char *,unsigned char *);									//Loop Back Test

void MB1_ReceiveQuery_F66(unsigned char *,unsigned char *);										//Read Module Information (Proprietory Function Code)
void MB1_ReceiveQuery_F72(unsigned char *,unsigned char *);	        							//Rack Control Inputs (Proprietory Function Code)

void MB1_ExceptionCode(unsigned char *,unsigned char *,unsigned short);

void SetUSART2TxCount(unsigned short p_tx_length);
void MB1_SetUSART3TxEnable(unsigned short p_tx_length);

void (*USART_TxEnable)(unsigned short);

/* MB1 USART Selection */
//#define c_MB1_USART1_SELECT 1
#define c_MB1_USART2_SELECT 2
//#define c_MB1_USART3_SELECT 3

#if !defined (c_MB1_USART1_SELECT) && !defined (c_MB1_USART2_SELECT) && !defined (c_MB1_USART3_SELECT)
 	#error "Please select MB1 USART Port"
#endif

#if defined (c_MB1_USART1_SELECT) && defined (c_MB1_USART2_SELECT)
	#error "Please select MB1 USART Port one of three"
#endif

#if defined (c_MB1_USART1_SELECT) && defined (c_MB1_USART3_SELECT)
	#error "Please select MB1 USART Port one of three"
#endif

#if defined (c_MB1_USART2_SELECT) && defined (c_MB1_USART3_SELECT)
	#error "Please select MB1 USART Port one of three"
#endif



void MB1_Initialise(void)
{
	GPIO_PA1_Mode_Out_PP_80Mhz;

	 MB1_RS485_Tx;
	 MB1_RS485_Rx;

	/* MB1 USART Configuration */
	MB1_ConfigureUSART();

	/* MB1 Inputregister and HoldingRegister array allocation*/
	l_MB1_InputRegisters = MB1_GetAddress_InputReg();
	l_MB1_HoldingRegisters = MB1_GetAddress_HoldingReg();

}

/* MB1 USART Selection */
void MB1_ConfigureUSART(void)
{
	/* USART1 Configuration */
	#if(c_MB1_USART1_SELECT)
		USART1_Configuration(BaudRate_72Mhz_115200bps);					// USART Initialisation
		MB1_USART_TxEnable = &MB1_SetUSART1TxEnable;	 				// USART Transmitter Enable Function*/
	#endif

	/* USART2 Configuration */
	#if(c_MB1_USART2_SELECT)
		 //MX_USART2_UART_Init();
		Usart_Init(USART_2);				// USART Initialisation
		USART_TxEnable = &SetUSART2TxCount;	 	// USART Transmitter Enable Function	 				// USART Transmitter Enable Function*/
	#endif

	/* USART3 Configuration */
	#if(c_MB1_USART3_SELECT)
		Usart_Init(USART_3);					// USART Initialisation
		USART_TxEnable = &MB1_SetUSART3TxEnable;	 				// USART Transmitter Enable Function*/
	#endif
}

void MB1_ReadModuleInformation(void)
{
	ModuleInfo.byte.bit.Type   				= c_ModuleType;
	ModuleInfo.byte.bit.Slotid   			= gl_MB1_slave_ID ;//= MB1_SlaveID.byte.ID;
	ModuleInfo.byte.bit.HoldregLength   	= c_MB1_FC03_Length;
	ModuleInfo.byte.bit.InputregLength   	= c_MB1_FC04_Length;
	ModuleInfo.byte.bit.OutputcoilLength  	= c_MB1_FC02_Length;
	ModuleInfo.byte.bit.InputcoilLength   	= c_MB1_FC01_Length;
}

/* MB1 Select RS485 Line Tx/Rx */
void MB1_SelectRS485Line(unsigned short p_TxRxLine)
{
	switch(p_TxRxLine)
	{
		case c_RS485_Tx:
			MB1_RS485_Tx
		break;

		case c_RS485_Rx:
			MB1_RS485_Rx
		break;
	}
}

/* Localmodbus query Process */
void MB1_process(void)
{
	if(gl_MB1_ReceiveFrameComplete_Flag==1)
	{
		gl_MB1_ReceiveFrameComplete_Flag = 0;
		MB1_ReceiveQueryProcess( );
	}
}

void MB1_ReceiveQueryProcess(void)
{
	unsigned short fl_crc1,fl_crc2,fl_no_of_bytes;

	if(MB1_inbox[0]==gl_MB1_slave_ID)
	{
		fl_no_of_bytes = gl_MB1_ReceivedByteCount - 2;
		fl_crc1 = MB1_CRC(fl_no_of_bytes,MB1_inbox);
		fl_crc2	= (MB1_inbox[fl_no_of_bytes + 1]<<8) | MB1_inbox[fl_no_of_bytes];

		if(fl_crc1==fl_crc2)
		{
			switch(MB1_inbox[1])
			{
				case c_MB1_FC03:
					MB1_ReceiveQuery_F03(MB1_inbox,MB1_outbox,l_MB1_HoldingRegisters);
					break;
				case c_MB1_FC04:
					MB1_ReceiveQuery_F04(MB1_inbox,MB1_outbox,l_MB1_InputRegisters);
					break;
				case c_MB1_FC06:
					MB1_ReceiveQuery_F06(MB1_inbox,MB1_outbox,l_MB1_HoldingRegisters);							//DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
					break;
				case c_MB1_FC16:
					MB1_ReceiveQuery_F16(MB1_inbox,MB1_outbox,l_MB1_HoldingRegisters);
					break;
				case c_MB1_FC08:
					MB1_ReceiveQuery_F08(MB1_inbox,MB1_outbox);
					break;
				case c_MB1_FC66:
					MB1_ReceiveQuery_F66(MB1_inbox,MB1_outbox);
					break;
				case c_MB1_FC72:
					MB1_ReceiveQuery_F72(MB1_inbox,MB1_outbox);
					break;
				default:
					MB1_ExceptionCode(MB1_inbox,MB1_outbox,1);
					break;
			}
		}
	}
}


/* Get Address from Input Register */
unsigned short * MB1_GetAddress_InputReg(void)
{
	return(MB1_InputRegisters);
}

/* Get Address from Holding Register */
unsigned short * MB1_GetAddress_HoldingReg(void)
{
	return(MB1_HoldingRegisters);
}

///* ReadHoldingRegisters(FunctionCode-03) */
void MB1_ReceiveQuery_F03(unsigned char * p_inbox,unsigned char * p_outbox,unsigned short * p_array)
{
	unsigned short fl_i,fl_offset=3,fl_address,fl_length;

	fl_address = (p_inbox[2] <<8 | p_inbox[3]);
	fl_length  = (p_inbox[4] <<8 | p_inbox[5]);

	if (fl_address >= c_MB1_FC03_Length)										//check if Illigal address
		MB1_ExceptionCode(p_inbox,p_outbox,2);									//Illigal address

	else if(fl_address + fl_length > c_MB1_FC03_Length)							// start address + no of ponts should be within the limits
		MB1_ExceptionCode(p_inbox,p_outbox,3);									//generate and transmit an exception response message and then just return to the main program and wait for any query to come

	else if(fl_length < 1 || fl_length > 128)
		MB1_ExceptionCode(p_inbox,p_outbox,3);

	else
	{
		p_outbox[0] = p_inbox[0];												//slave id
		p_outbox[1] = p_inbox[1];												//function code
		p_outbox[2] = 2 * fl_length;											//Byte Count

		if(fl_length==128)
		p_outbox[2] = 0xFF;

		for(fl_i= fl_address;fl_i<(fl_address + fl_length);fl_i++)
		{
			MB1_U16bit.byte.Data = p_array[fl_i];
			p_outbox[fl_offset++] = MB1_U16bit.byte.bit.MSB;
			p_outbox[fl_offset++] = MB1_U16bit.byte.bit.LSB;
		}

		MB1_U16bit.byte.Data = MB1_CRC(fl_offset,p_outbox);
		p_outbox[fl_offset++] = MB1_U16bit.byte.bit.LSB;
		p_outbox[fl_offset]   = MB1_U16bit.byte.bit.MSB;

		MB1_TransmitQuery(fl_offset);
	}
}

///* ReadInputRegisters(FunctionCode-04) */
void MB1_ReceiveQuery_F04(unsigned char * p_inbox,unsigned char * p_outbox,unsigned short * p_array)
{
	unsigned short fl_i,fl_offset=3,fl_address,fl_length;

	fl_address = (p_inbox[2] <<8 | p_inbox[3]);
	fl_length  = (p_inbox[4] <<8 | p_inbox[5]);

	if (fl_address >= c_MB1_FC04_Length)									//check if Illegal address
		MB1_ExceptionCode(p_inbox,p_outbox,2);								//Illegal address

	else if(fl_address + fl_length > c_MB1_FC04_Length)						// start address + no of points should be within the limits
		MB1_ExceptionCode(p_inbox,p_outbox,3);								//generate and transmit an exception response message and then just return to the main program and wait for any query to come

	else if(fl_length < 1 || fl_length > 128)
		MB1_ExceptionCode(p_inbox,p_outbox,3);

	else
	{
		p_outbox[0] = p_inbox[0];											//slave id
		p_outbox[1] = p_inbox[1];											//function code
		p_outbox[2] = 2 * fl_length;										//Byte Count

		if(fl_length==128)
		p_outbox[2] = 0xFF;

		for(fl_i= fl_address;fl_i<(fl_address + fl_length);fl_i++)
		{
			MB1_U16bit.byte.Data = p_array[fl_i];
			p_outbox[fl_offset++] = MB1_U16bit.byte.bit.MSB;
			p_outbox[fl_offset++] = MB1_U16bit.byte.bit.LSB;
		}

		MB1_U16bit.byte.Data = MB1_CRC(fl_offset,p_outbox);
		p_outbox[fl_offset++] = MB1_U16bit.byte.bit.LSB;
		p_outbox[fl_offset]   = MB1_U16bit.byte.bit.MSB;

		MB1_TransmitQuery(fl_offset);
	}
}


/* SingleWriteHoldingRegisters(FunctionCode-06) */
void MB1_ReceiveQuery_F06(unsigned char * p_inbox,unsigned char * p_outbox,unsigned short * p_array) //DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
{
	unsigned short fl_address,fl_i;

	fl_address = (p_inbox[2] <<8 | p_inbox[3]);

	if (fl_address >= c_MB1_FC03_Length)													//check if Illegal address
		MB1_ExceptionCode(p_inbox,p_outbox,2);												//Illegal address

	else
	{

		p_array[fl_address] = ((p_inbox[4])<<8 | (p_inbox[5]));

		gl_MB1_Address 	= fl_address;
		gl_MB1_Length 	= 1;
		gl_MB1_FC06_Flag = 1;

//		p_outbox[0] = p_inbox[0];						 //slave id
//		p_outbox[1] = p_inbox[1];						 //function	id
//		p_outbox[2] = p_inbox[2];						 //Register High Order
//		p_outbox[3] = p_inbox[3];						 //Register low Order
//		p_outbox[4] = p_inbox[4];						 //preset data high
//		p_outbox[5] = p_inbox[5];						 //preset data low

//		MB1_U16bit.byte.Data = MB1_CRC(6,p_outbox);
//
//		p_outbox[6] = MB1_U16bit.byte.bit.LSB;
//		p_outbox[7] = MB1_U16bit.byte.bit.MSB;

		for(fl_i=0;fl_i<=7;fl_i++)
			p_outbox[fl_i] = p_inbox[fl_i];

 		MB1_TransmitQuery(7);														  	//DATE:16/02/2022  EDITED BY GAGAN GOWDA G R
	}

}

/* WriteHoldingRegisters(FunctionCode-16) */
void MB1_ReceiveQuery_F16(unsigned char * p_inbox,unsigned char * p_outbox,unsigned short * p_array)
{
	unsigned short fl_i,fl_j,fl_address,fl_length;

	fl_address = (p_inbox[2] <<8 | p_inbox[3]);
	fl_length  = (p_inbox[4] <<8 | p_inbox[5]);

	if (fl_address >= c_MB1_FC03_Length)													//check if Illegal address
		MB1_ExceptionCode(p_inbox,p_outbox,2);												//Illegal address

	else if(fl_address + fl_length > c_MB1_FC03_Length)										// start address + no of points should be within the limits
		MB1_ExceptionCode(p_inbox,p_outbox,3);												//generate and transmit an exception response message and then just return to the main program and wait for any query to come

	else if(fl_length < 1 || fl_length > 128)
		MB1_ExceptionCode(p_inbox,p_outbox,3);

	else
	{

		for(fl_i=fl_address,fl_j=7;fl_i<( fl_address + fl_length );fl_i++)
		{
			p_array[fl_i] = ((p_inbox[fl_j])<<8 | (p_inbox[fl_j+1]));
			fl_j=fl_j+2;
		}

		gl_MB1_Address 	= fl_address;
		gl_MB1_Length 	= fl_length;
		gl_MB1_FC16_Flag = 1;

		for(fl_i=0;fl_i<6;fl_i++)
			p_outbox[fl_i]=p_inbox[fl_i];

		MB1_U16bit.byte.Data = MB1_CRC(6,p_outbox);
		p_outbox[6] = MB1_U16bit.byte.bit.LSB;
		p_outbox[7] = MB1_U16bit.byte.bit.MSB;

		MB1_TransmitQuery(7);
	}
}

/* Loop Back Test(FunctionCode-08) */
void MB1_ReceiveQuery_F08(unsigned char * p_inbox,unsigned char * p_outbox)
{
	unsigned short fl_DiagnosticCode;

	fl_DiagnosticCode = (p_inbox[2] <<8 | p_inbox[3]);

	switch(fl_DiagnosticCode)
	{
		case 0:
			MB1_ReceiveQuery_F08_DC0(p_inbox,p_outbox);
			break;
		default:
			MB1_ExceptionCode(p_inbox,p_outbox,1);
			break;
	}
}

/* Loop Back Test(Diagnostic Code - 00) */
void MB1_ReceiveQuery_F08_DC0(unsigned char * p_inbox,unsigned char * p_outbox)
{
	unsigned short fl_i;

	for(fl_i = 0;fl_i<8;fl_i++)
		p_outbox[fl_i] = p_inbox[fl_i];

	MB1_TransmitQuery(7);
}

/* Exception Response */
void MB1_ExceptionCode(unsigned char * p_inbox,unsigned char * p_outbox,unsigned short p_exception_code)
{
	p_outbox[0] = p_inbox[0];									//slave address
	p_outbox[1] = 0x80 + p_inbox[1];							//msb of function code with msbit as high
	p_outbox[2] = p_exception_code;								//Exception code

	MB1_U16bit.byte.Data = MB1_CRC(3,p_outbox);
	p_outbox[3] = MB1_U16bit.byte.bit.LSB;
	p_outbox[4] = MB1_U16bit.byte.bit.MSB;

	MB1_TransmitQuery(4);
}

/* Read Module Information (Proprietory Function Code-66)*/
void MB1_ReceiveQuery_F66(unsigned char * p_inbox,unsigned char * p_outbox)
{
	unsigned int fl_CRC,fl_address,fl_length;

	fl_address = (p_inbox[2] <<8 | p_inbox[3]);
	fl_length  = (p_inbox[4] <<8 | p_inbox[5]);

	if (fl_address >= c_MB1_FC66_Length)										//check if Illigal address
		MB1_ExceptionCode(p_inbox,p_outbox,2);									//Illigal address

	else if(fl_address + fl_length > c_MB1_FC66_Length)							// start address + no of ponts should be within the limits
		MB1_ExceptionCode(p_inbox,p_outbox,3);									//generate and transmit an exception response message and then just return to the main program and wait for any query to come

	else if(fl_length != c_MB1_FC66_Length)
		MB1_ExceptionCode(p_inbox,p_outbox,3);

	else
	{
		p_outbox[0] = p_inbox[0];												//slave id
		p_outbox[1] = p_inbox[1];												//function code
		p_outbox[2] = 2 * fl_length;											//Byte Count
		p_outbox[3] = ModuleInfo.byte.bit.Slotid;
		p_outbox[4] = ModuleInfo.byte.bit.Type;
 		p_outbox[5] = (0xFF00 & ModuleInfo.byte.bit.HoldregLength)>>8;
		p_outbox[6] = (0x00FF & ModuleInfo.byte.bit.HoldregLength);
		p_outbox[7] = (0xFF00 & ModuleInfo.byte.bit.InputregLength)>>8;
		p_outbox[8] = (0x00FF & ModuleInfo.byte.bit.InputregLength);
		p_outbox[9] = (0xFF00 & ModuleInfo.byte.bit.OutputcoilLength)>>8;
		p_outbox[10]= (0x00FF & ModuleInfo.byte.bit.OutputcoilLength);
		p_outbox[11]= (0xFF00 & ModuleInfo.byte.bit.InputcoilLength)>>8;
		p_outbox[12]= (0x00FF & ModuleInfo.byte.bit.InputcoilLength);

		fl_CRC = MB1_CRC(13,p_outbox);

		p_outbox[13]  = (0x00FF & fl_CRC);
		p_outbox[14] = (0xFF00 & fl_CRC)>>8;

		MB1_TransmitQuery(14);
	}
}

/* Rack Control Inputs (Proprietory Function Code-72)*/
void MB1_ReceiveQuery_F72(unsigned char * p_inbox,unsigned char * p_outbox)
{
	unsigned short fl_CRC;

	p_outbox[0] = p_inbox[0];						//slave id
	p_outbox[1] = p_inbox[1];					 	//function code
	p_outbox[2] = p_inbox[2];
	p_outbox[3] = p_inbox[3];

	gl_Control_Command	= p_inbox[2];
	gl_Control_Input	= p_inbox[3];

	gl_MB1_FC72_Flag = 1;

	fl_CRC = MB1_CRC(4,p_outbox);

	p_outbox[4]  = (0x00FF & fl_CRC);
	p_outbox[5] = (0xFF00 & fl_CRC)>>8;

	MB1_TransmitQuery(5);
}

void MB1_TransmitQuery(unsigned short p_Txlength)
{
	MB1_SelectRS485Line(c_RS485_Tx);
	MB1_delay();
	USART_TxEnable(p_Txlength);
}

/* Cyclic Redundancy Check */
unsigned short MB1_CRC(unsigned short p_no_of_bytes, unsigned char * p_array)
{
	unsigned short fl_i,fl_j,fl_result=0xffff;

	for(fl_j=0;fl_j<p_no_of_bytes;fl_j++)
	{
		fl_result ^= p_array[fl_j];
		for(fl_i=0;fl_i<8;fl_i++)
		{
			if(fl_result & 0x0001)
			{
				fl_result=fl_result>>1;
				fl_result ^=c_MB1_CRC_POLYNOMIAL;
			}
			else
			fl_result=fl_result>>1;
		}
	}
	return fl_result;
}

void MB1_delay(void)
{
	unsigned short fl_i;
	for(fl_i=0;fl_i<MB1_DelayNo;fl_i++)
		__ASM("nop");
}


/* Set Usart2 Transmiter Counts */												//EDITED BY GAGAN GOWDA G R DATE 12/02/2022//
void SetUSART2TxCount(unsigned short p_tx_length)
{
	if(p_tx_length<C_MB1_OUTBOX_LENGTH)									   //EDITED BY GAGAN GOWDA G R DATE 12/02/2022//
	{
		l_MB1_Tx_index = 0;
		l_MB1_Tx_Count = p_tx_length;
		HAL_UART_Transmit_IT(&USART_2_HANDLE, MB1_outbox, p_tx_length+1);
	}
}

void MB1_SetUSART3TxEnable(unsigned short p_tx_length)
{
	if(p_tx_length<c_MB1_OUTBOX_LENGTH)										//EDITED BY GAGAN GOWDA G R DATE 12/02/2022//
	{
		l_MB1_Tx_index = 0;
		l_MB1_Tx_Count = p_tx_length;
		HAL_UART_Transmit_IT(&USART_3_HANDLE, MB1_outbox, p_tx_length+1);
	}
}

void GetUSART2RxCount(unsigned short * p_Rxcount,unsigned short * p_CompltFlag)
{
	if(l_MB1_Rx_index >= 3 && l_MB1_Rx_index < C_MB1_INBOX_LENGTH)
	{
		*p_CompltFlag = 1;
		*p_Rxcount = l_MB1_Rx_index;
	}
	else
	{
	 	*p_CompltFlag = 0;
		*p_Rxcount = 0;
		HAL_UART_Receive_IT(&USART_3_HANDLE, &Rx_Data[0], 1);
	}
	l_MB1_Rx_index = 0;
}
