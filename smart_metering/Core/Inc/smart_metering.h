/*
 * smart_metering.h
 *
 *  Created on: Apr 1, 2024
 *      Author: Revanth
 */

#ifndef INC_SMART_METERING_H_
#define INC_SMART_METERING_H_

typedef struct
{
	union
	{
		uint16_t Data;
		struct
		{
			unsigned short LSB1				:4;
			unsigned short LSB2				:4;
			unsigned short MSB1				:4;
			unsigned short MSB2				:4;
		}bit;
	}byte;
}BP35C5_Split;

typedef struct
{
	union
	{
		uint8_t Data;
		struct
		{
			uint8_t LSB						:4;
			uint8_t MSB 					:4;
		}bit;
	}byte;
}merge_Hex;

void BP35C5_process(void);
void data_array(void);

extern uint16_t input_register[1024];

extern uint8_t BP35C5_RecieveQuery_Flag;
extern uint8_t BP35C5_SendResponse_Flag;

#endif /* INC_SMART_METERING_H_ */
