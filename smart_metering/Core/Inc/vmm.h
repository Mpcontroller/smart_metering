/*
 * vmm.h
 *
 *  Created on: May 9, 2024
 *      Author: Admin
 */

#ifndef INC_VMM_H_
#define INC_VMM_H_

#include <machine/_default_types.h>
#include <stdint.h>

#define ADC_sampling_length 4096

void vmm_process(void);
//void get_sineval(void);
void FFT_initialization(void);

extern unsigned short gl_ADC_samplingConv_Cmplt_flag;
extern uint16_t ADC_Data[];

//extern uint32_t sine_val[];

#endif /* INC_VMM_H_ */
