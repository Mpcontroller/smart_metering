/*
 * vmm.c
 *
 *  Created on: May 9, 2024
 *      Author: Revanth
 */

/* Includes ------------------------------------------------------------------*/
#include "vmm.h"
#include "main.h"
#include "smart_metering.h"
#include "call_back_functions.h"
#include <math.h>
#include "float.h"
#include <arm_neon.h>
#include "arm_math.h"


//#define pi 3.14285714
//#define DAC_sampling_length 1024
//#define res_8b 256
//#define res_12b 4096  //Resolution 12-Bit
#define FFT_SIZE 4096


arm_rfft_fast_instance_f32 fft_instance;
arm_status fft_status;


//uint32_t sine_val[DAC_sampling_length];

unsigned short gl_ADC_samplingConv_Cmplt_flag = 0;
uint16_t ADC_Data[ADC_sampling_length];
signed int AC_average[ADC_sampling_length];

float DC_offset, final_VRMS;
float Vbitvalue = 0.0008056640625;			// 3.3/4096

__attribute__   ((section(".Buffer"), used))  float32_t fft_input_buffer[FFT_SIZE];		// Buffer for FFT input data
__attribute__   ((section(".Buffer"), used))  float32_t fft_output_buffer[FFT_SIZE];		// Buffer for FFT output data
float32_t  magnitude;
float FFT_VALUE;
float FFT_INPUT;
float  FFT[FFT_SIZE/2];


void vmm_process(void);
//void get_sineval(void);  //DAC calculation
float VMM_RemoveDC(void);
float VMM_rms(void);
void perform_fft(void);
void FFT_initialization(void);


void vmm_process(void)
{
	if(gl_ADC_samplingConv_Cmplt_flag == 1)
	{
		gl_ADC_samplingConv_Cmplt_flag = 0;
		DC_offset = VMM_RemoveDC();
		final_VRMS = VMM_rms();
	}
}

//void get_sineval(void)
//{
//	for(int i = 0; i < DAC_sampling_length; i++)
//	{
//		sine_val[i] = ((sin(i * 2 * pi / DAC_sampling_length) +1)) * res_12b / 2;
//	}
//}

/* Remove DC Component */
float VMM_RemoveDC(void)
{
	unsigned short fl_i;
	uint32_t DC_avg;
	uint32_t fl_dc_sum = 0;
	float final_DC = 0;

	for(fl_i = 0; fl_i < ADC_sampling_length; fl_i++)
	{
		fl_dc_sum = fl_dc_sum + ADC_Data[fl_i];
	}

	DC_avg = (fl_dc_sum/ADC_sampling_length);
	final_DC = ((DC_avg * 3.3)/4096);

	for(fl_i = 0; fl_i < ADC_sampling_length; fl_i++)
	{
		AC_average[fl_i] =  DC_avg - ADC_Data[fl_i];
	}
	return final_DC;
}

/* calculate RMS */
float VMM_rms(void)
{
	unsigned short fl_j;
	float total_ADC_value = 0;
	float Avg_voltage;
	float fl_temp,fl_rms;

	for(fl_j = 0; fl_j < ADC_sampling_length; fl_j++)
	{
		fl_temp = AC_average[fl_j] * Vbitvalue;
		fft_input_buffer[fl_j] = fl_temp;
		total_ADC_value = (fl_temp * fl_temp) + total_ADC_value;
	}

	Avg_voltage = total_ADC_value/ADC_sampling_length;
	fl_rms = sqrt(Avg_voltage);

	perform_fft();

	return fl_rms;
}

void FFT_initialization(void)
{
	fft_status = arm_rfft_fast_init_f32(&fft_instance, FFT_SIZE);

	if (fft_status != ARM_MATH_SUCCESS)
	{
		// FFT initialization failed, handle the error
		while (1)
		{
			// Error handling...
		}
	}
}

// Perform the FFT
void perform_fft(void)
{
	arm_rfft_fast_f32(&fft_instance, fft_input_buffer, fft_output_buffer, 0);

  // Visualize the results (you can customize this part as needed)
  for (int i = 0; i < FFT_SIZE / 2; i++)
  {
	float32_t real = fft_output_buffer[i * 2];
	float32_t imag = fft_output_buffer[i * 2 + 1];
	magnitude = sqrtf(real * real + imag * imag);
	FFT[i] = magnitude;
  }

  for(int fl_m=0; fl_m < FFT_SIZE; fl_m++)
  {
  	FFT_VALUE = FFT[fl_m];
  	FFT_INPUT = FFT_VALUE * 10;
//  MB1_InputRegisters[fl_m] = FFT_INPUT;				//LSB
  }
}
