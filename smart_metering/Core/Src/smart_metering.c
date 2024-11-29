/*
 * smart_metering.c
 *
 *  Created on: Apr 1, 2024
 *      Author: Revanth
 */

#include "MB.h"
#include "main.h"
#include "smart_metering.h"
#include "call_back_functions.h"
#include "vmm.h"

#define c_SM_F03 0x3
#define c_SM_F04 0x4
#define c_SM_F06 0x6
#define c_SM_F16 0x10	//(0x10 = 16)

#define Slave_ID 0x01
#define Tx_MB_address 0x000f
#define Tx_MB_length 0x000a

BP35C5_Split array_U16bit;
merge_Hex array_U8bit;

void data_array(void);
void BP35C5_process(void);

void BP35C5_Send_Response(void);
void BP35C5_Receive_Query(void);
void BP35C5_Valid_Response(void);
void BP35C5_NoResponse(void);

void BP35C5_SendResponse_process(uint16_t address,uint16_t length);
void BP35C5_ReceiveFrame_process(void);
void BP35C5_ResponseProcess(void);
void BP35C5_NoResponse_process(void);

void BP35C5_tx_Response_F03(void);
void BP35C5_ReceiveDataFrame_F03(void);

void BP35C5_tx_Response_F04(void);
void BP35C5_ReceiveDataFrame_F04(void);
void BP35C5_tx_Response(void);

void BP35C5_tx_Response_F06(void);
void BP35C5_ReceiveDataFrame_F06(void);

void BP35C5_tx_Response_F16(void);
void BP35C5_ReceiveDataFrame_F16(void);

unsigned int ASCII_2_hex(unsigned int len);
void hex_2_ASCII(unsigned int len);

uint8_t BP35C5_SendResponse_Flag = 0;
uint8_t BP35C5_RecieveQuery_Flag = 0;
uint8_t BP35C5_Valid_Response_Flag = 0;
uint8_t BP35C5_NoResponseFlag = 0;
//uint8_t Invalid_tcpr_data_flag = 0;
uint8_t valid_tcpr_data_flag = 0;

uint16_t input_register[1024];
uint16_t holding_register[1024];
uint8_t coded_hex[150];
uint8_t decoded_ASCII[500];

uint8_t gl_BP35C5_TaskCode = 0;
unsigned char BP35C5_Outbox[500];
unsigned int tx_index;
unsigned int Rx_index;

char end[1] = {"\r"};
char Tx_ASCII_string[200];
char Rx_ASCII_string[200];  								//hexadecimal string
char Tx_frame[19] = "tcps 2001:db8::01 ";
char tcp_rx[17] = " tcpr <2001:db8::";

uint16_t Rx_MB_Address;  								//Rx Query address
uint16_t Rx_MB_length;  								//Rx Query length

void data_array(void)
{
	unsigned int x;
	for(x = 0; x < 1024; x++)
	{
		input_register[x] = x;
		holding_register[x] = x;
	}
}


void BP35C5_process(void)
{
	BP35C5_Send_Response();
//	BP35C5_Receive_Query();
//	BP35C5_Valid_Response();
//	BP35C5_NoResponse();
}

void BP35C5_Send_Response(void)
{
	if(BP35C5_SendResponse_Flag == 1)
	{
		BP35C5_SendResponse_Flag = 0;
		BP35C5_SendResponse_process(Tx_MB_address,Tx_MB_length);
	}
}

void BP35C5_SendResponse_process(uint16_t address,uint16_t length)
{
	unsigned int fl_index,fl_offset = 12;

	array_U8bit.byte.Data = Slave_ID;					//copy 8bit slave ID
	coded_hex[0] = array_U8bit.byte.bit.MSB;
	coded_hex[1] = array_U8bit.byte.bit.LSB;

	array_U8bit.byte.Data = c_SM_F04;					//copy 8bit function code
	coded_hex[2] = array_U8bit.byte.bit.MSB;
	coded_hex[3] = array_U8bit.byte.bit.LSB;

	array_U16bit.byte.Data = address;				//copy 16bit Address
	coded_hex[4] = array_U16bit.byte.bit.MSB2;
	coded_hex[5] = array_U16bit.byte.bit.MSB1;
	coded_hex[6] = array_U16bit.byte.bit.LSB2;
	coded_hex[7] = array_U16bit.byte.bit.LSB1;

	array_U16bit.byte.Data = length;				//copy 16bit length
	coded_hex[8] = array_U16bit.byte.bit.MSB2;
	coded_hex[9] = array_U16bit.byte.bit.MSB1;
	coded_hex[10] = array_U16bit.byte.bit.LSB2;
	coded_hex[11] = array_U16bit.byte.bit.LSB1;

	for(fl_index = address; fl_index < (address+length); fl_index++)
	{
		array_U16bit.byte.Data = input_register[fl_index];				//convert 16bit data to 4 bit data
		coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB2;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB1;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB2;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB1;
	}
	hex_2_ASCII(fl_offset);
	tx_index = fl_offset;
	BP35C5_tx_Response_F04();
}

void BP35C5_Receive_Query(void)
{
	if(BP35C5_RecieveQuery_Flag == 1)
	{
		BP35C5_RecieveQuery_Flag = 0;
		BP35C5_ReceiveFrame_process();
	}
}

void BP35C5_ReceiveFrame_process(void)
{
	unsigned short int fl_i,fl_invalid_flag = 0,fl_j = 20;
	if(rx_index >= 20)
	{
		for(fl_i = 1; fl_i <= 16; fl_i++)		//"tcpr <2001:db8::" string length is 15
		{
			if(BP35C5_Inbox[fl_i] != tcp_rx[fl_i])
			{
				fl_invalid_flag = 1;
			}
		}

		if(fl_invalid_flag == 0)
			valid_tcpr_data_flag = 1;
//		else
//			Invalid_tcpr_data_flag = 1;
	}

	rx_index = 0;
	if(valid_tcpr_data_flag == 1)
	{
		valid_tcpr_data_flag = 0;
		for(fl_i = 1; BP35C5_Inbox[fl_j] != end[0]; fl_i++,fl_j++)
		{
			Rx_ASCII_string[fl_i] = BP35C5_Inbox[fl_j];					//copying rx_data(ASCII)
		}
		Rx_index = ASCII_2_hex(fl_i);
		gl_BP35C5_TaskCode = decoded_ASCII[1];							//copying function code(Hex)

		switch(gl_BP35C5_TaskCode)
		{
			case c_SM_F03:			BP35C5_ReceiveDataFrame_F03();
				break;
			case c_SM_F04:			BP35C5_ReceiveDataFrame_F04();
				break;
			case c_SM_F06:			BP35C5_ReceiveDataFrame_F06();
				break;
			case c_SM_F16:			BP35C5_ReceiveDataFrame_F16();
				break;
		}
	}
}

void BP35C5_ReceiveDataFrame_F03(void)
{
	unsigned int fl_x,fl_index,fl_offset = 12,offset =0;

	Rx_MB_Address = (decoded_ASCII[2] <<8 | decoded_ASCII[3]);	//OR MSB and LSB Address
	Rx_MB_length = (decoded_ASCII[4] <<8 | decoded_ASCII[5]);	//OR MSB and LSB Address

	for(fl_x = 0; fl_x <= 11; fl_x++)
	{
		array_U8bit.byte.Data = decoded_ASCII[offset++];
		coded_hex[fl_x++] = array_U8bit.byte.bit.MSB;			//copy MSB Slave ID ,MSB Function code,MSB Address,MSB length
		coded_hex[fl_x] = array_U8bit.byte.bit.LSB;				//copy LSB Slave ID ,LSB Function code,LSB Address,LSB length
	}


	for(fl_index = Rx_MB_Address; fl_index < (Rx_MB_Address+Rx_MB_length); fl_index++)
	{
		array_U16bit.byte.Data = holding_register[fl_index];				//convert 16bit data to 4 bit data
		coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB2;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB1;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB2;
		coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB1;
	}
	hex_2_ASCII(fl_offset);
	tx_index = fl_offset;
	BP35C5_tx_Response_F03();
}

void BP35C5_tx_Response_F03()			//Read holding register
{
	unsigned int fl_index, fl_index2;
	for(fl_index = 0; fl_index < 18; fl_index++)
	{
		BP35C5_Outbox[fl_index] = Tx_frame[fl_index];				//copy TCP tx command
	}
	for(fl_index2 = 0; fl_index2 < tx_index; fl_index2++)
	{
		BP35C5_Outbox[fl_index++] = Tx_ASCII_string[fl_index2];		//copy tx data
	}
	BP35C5_Outbox[fl_index++] = 0xd;								//copy carriage return("\r")
	BP35C5_Outbox[fl_index++] = 0xa;								//copy line feed("\n")
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)BP35C5_Outbox, fl_index);
}

void BP35C5_ReceiveDataFrame_F04(void)
{
		unsigned int fl_x,fl_index,fl_offset = 12,offset =0;

		Rx_MB_Address = (decoded_ASCII[2] <<8 | decoded_ASCII[3]);	//OR MSB and LSB Address
		Rx_MB_length = (decoded_ASCII[4] <<8 | decoded_ASCII[5]);	//OR MSB and LSB Address

		for(fl_x = 0; fl_x <= 11; fl_x++)
		{
			array_U8bit.byte.Data = decoded_ASCII[offset++];
			coded_hex[fl_x++] = array_U8bit.byte.bit.MSB;			//copy MSB Slave ID ,MSB Function code,MSB Address,MSB length
			coded_hex[fl_x] = array_U8bit.byte.bit.LSB;				//copy LSB Slave ID ,LSB Function code,LSB Address,LSB length
		}


		for(fl_index = Rx_MB_Address; fl_index < (Rx_MB_Address+Rx_MB_length); fl_index++)
		{
			array_U16bit.byte.Data = input_register[fl_index];				//convert 16bit data to 4 bit data
			coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB2;
			coded_hex[fl_offset++] = array_U16bit.byte.bit.MSB1;
			coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB2;
			coded_hex[fl_offset++] = array_U16bit.byte.bit.LSB1;
		}
		hex_2_ASCII(fl_offset);
		tx_index = fl_offset;
		BP35C5_tx_Response_F04();
}

void BP35C5_tx_Response_F04(void)			//Read input register
{
	unsigned int fl_index, fl_index2;
	for(fl_index = 0; fl_index < 18; fl_index++)
	{
		BP35C5_Outbox[fl_index] = Tx_frame[fl_index];				//copy TCP tx command
	}
	for(fl_index2 = 0; fl_index2 < tx_index; fl_index2++)
	{
		BP35C5_Outbox[fl_index++] = Tx_ASCII_string[fl_index2];		//copy tx data
	}
	BP35C5_Outbox[fl_index++] = 0xd;								//copy carriage return("\r")
	BP35C5_Outbox[fl_index++] = 0xa;								//copy line feed("\n")
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)BP35C5_Outbox, fl_index);
}

void BP35C5_ReceiveDataFrame_F06(void)			//write single holding register
{
	unsigned int fl_x,fl_index,fl_offset = 6,offset =0;

	Rx_MB_Address = (decoded_ASCII[2] <<8 | decoded_ASCII[3]);	//OR MSB and LSB Address
	Rx_MB_length = (decoded_ASCII[4] <<8 | decoded_ASCII[5]);	//OR MSB and LSB Address

	if((Rx_MB_length == 1) && (Rx_index == 8))
	{
		for(fl_x = 0; fl_x <= 11; fl_x++)
		{
			array_U8bit.byte.Data = decoded_ASCII[offset++];
			coded_hex[fl_x++] = array_U8bit.byte.bit.MSB;			//copy MSB Slave ID ,MSB Function code,MSB Address,MSB length
			coded_hex[fl_x] = array_U8bit.byte.bit.LSB;				//copy LSB Slave ID ,LSB Function code,LSB Address,LSB length
		}

		for(fl_index = Rx_MB_Address; fl_index < (Rx_MB_Address+Rx_MB_length); fl_index++)
		{
			holding_register[fl_index] = (decoded_ASCII[fl_offset] << 8 | decoded_ASCII[fl_offset + 1]);	//copying data to holding register
		}
		hex_2_ASCII(fl_x);
		tx_index = fl_x;
		BP35C5_tx_Response_F06();
	}
}

void BP35C5_tx_Response_F06(void)
{
	unsigned int fl_index, fl_index2;
	for(fl_index = 0; fl_index < 18; fl_index++)
	{
		BP35C5_Outbox[fl_index] = Tx_frame[fl_index];				//copy TCP tx command
	}
	for(fl_index2 = 0; fl_index2 < tx_index; fl_index2++)
	{
		BP35C5_Outbox[fl_index++] = Tx_ASCII_string[fl_index2];		//copy tx data
	}
	BP35C5_Outbox[fl_index++] = 0xd;								//copy carriage return("\r")
	BP35C5_Outbox[fl_index++] = 0xa;								//copy line feed("\n")
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)BP35C5_Outbox, fl_index);
}

void BP35C5_ReceiveDataFrame_F16(void)			//write multiple holding register
{
	unsigned int fl_x,fl_index,offset =0;
	unsigned int fl_offset = 6;

	Rx_MB_Address = (decoded_ASCII[2] <<8 | decoded_ASCII[3]);	//OR MSB and LSB Address
	Rx_MB_length = (decoded_ASCII[4] <<8 | decoded_ASCII[5]);	//OR MSB and LSB Address

	if(Rx_index == (Rx_MB_length*2) +6 )
	{
		for(fl_x = 0; fl_x <= 11; fl_x++)
		{
			array_U8bit.byte.Data = decoded_ASCII[offset++];
			coded_hex[fl_x++] = array_U8bit.byte.bit.MSB;			//copy MSB Slave ID ,MSB Function code,MSB Address,MSB length
			coded_hex[fl_x] = array_U8bit.byte.bit.LSB;				//copy LSB Slave ID ,LSB Function code,LSB Address,LSB length
		}

		for(fl_index = Rx_MB_Address; fl_index < (Rx_MB_Address+Rx_MB_length); fl_index++)
		{
			holding_register[fl_index] = (decoded_ASCII[fl_offset++] << 8 | decoded_ASCII[fl_offset++]);	//copying data to holding register
		}
		hex_2_ASCII(fl_x);
		tx_index = fl_x;
		BP35C5_tx_Response_F16();
	}
}

void BP35C5_tx_Response_F16(void)
{
	unsigned int fl_index, fl_index2;
	for(fl_index = 0; fl_index < 18; fl_index++)
	{
		BP35C5_Outbox[fl_index] = Tx_frame[fl_index];				//copy TCP tx command
	}
	for(fl_index2 = 0; fl_index2 < tx_index; fl_index2++)
	{
		BP35C5_Outbox[fl_index++] = Tx_ASCII_string[fl_index2];		//copy tx data
	}
	BP35C5_Outbox[fl_index++] = 0xd;								//copy carriage return("\r")
	BP35C5_Outbox[fl_index++] = 0xa;								//copy line feed("\n")
	HAL_UART_Transmit_IT(&huart1, (uint8_t *)BP35C5_Outbox, fl_index);
}

void BP35C5_Valid_Response(void)
{
	if(BP35C5_Valid_Response_Flag == 1)
	{
		BP35C5_Valid_Response_Flag = 0;
		BP35C5_ResponseProcess();
	}
}

void BP35C5_ResponseProcess(void)
{
	switch(gl_BP35C5_TaskCode)
	{
		case c_SM_F03:			BP35C5_tx_Response();
			break;
		case c_SM_F04:			BP35C5_tx_Response();
			break;
		case c_SM_F06:			BP35C5_tx_Response();
			break;
		case c_SM_F16:			BP35C5_tx_Response();
			break;
	}
}

void BP35C5_tx_Response(void)
{
	__ASM("nop");
}

void BP35C5_NoResponse(void)
{
	if(BP35C5_NoResponseFlag == 1)
	{
		BP35C5_NoResponseFlag = 0;
		BP35C5_NoResponse_process();
	}
}

void BP35C5_NoResponse_process(void)
{
	__ASM("nop");
}

unsigned int ASCII_2_hex(unsigned int len)
{
	unsigned int i, t, hn, ln;

	for (t = 0, i = 0; i < len; i += 2, ++t)
	{
		if(Rx_ASCII_string[i] > '9')
			hn = (Rx_ASCII_string[i] | 32) - 'a' + 10 ;
		else
			hn = Rx_ASCII_string[i] - '0';

		if(Rx_ASCII_string[i + 1] > '9')
			ln = (Rx_ASCII_string[i + 1] | 32) - 'a' + 10;
		else
			ln = Rx_ASCII_string[i + 1] - '0';

		decoded_ASCII[t] = (hn << 4) | ln;

	}
	return t;
}

void hex_2_ASCII(unsigned int len)
{
	unsigned int index;
	for(index = 0; index < len; index++)
	{
		if(coded_hex[index] <= 0x9)
		{
			Tx_ASCII_string[index] = coded_hex[index] + '0';
		}
		else if((coded_hex[index] >= 0xa) && (coded_hex[index] <= 0xf))
		{
			Tx_ASCII_string[index] = coded_hex[index] + 'W';
		}
	}
}
