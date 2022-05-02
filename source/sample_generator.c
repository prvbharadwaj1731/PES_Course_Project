/*
 * sample_generator.c
 *
 *  Created on: 18-Apr-2022
 *      Author: prvbh
 */


#include <stdio.h>
#include <stdint.h>
#include "assert.h"
#include "fsl_debug_console.h"
#include "sine_generator.h"
#include "autocorrelate.h"
#include "DAC.h"

#define DAC_FREQ 48000 //48 KHz
#define ADC_SAMPLING_FREQ 96000 //96 KHz

/*
 * Description : Computes sine wave of required frequency and sample size (in this case, 1 second worth samples - 1024) and fills a buffer with it.
 * Parameters : int in_freq, uint16_t *buffer - buffer that DMA uses to transfer data to DAC data register, size_t size
 * Returns : size_t number of samples computed and filled in the buffer
 * */
size_t tone_to_samples(int in_freq, uint16_t *buffer, size_t size)
{
	int cycle_count = size/ ((DAC_FREQ + (in_freq/2)) / in_freq);

	//Computes number of samples to be generated at the given frequency and DAC resolution
	int sample_count = ((DAC_FREQ + (in_freq/2)) / in_freq) * cycle_count;

	//generate sine values from integer-math sine function and fill output buffer with it.
	for(int i=0;i<sample_count;i++)
	{
		buffer[i] = sine_generate(i*TWO_PI/((DAC_FREQ + (in_freq/2)) / in_freq)) + (TRIG_SCALE_FACTOR-1);
	}

	printf("Generated %d samples at %d Hz, computed period = %d, observed period = %d\r\n",
			sample_count, in_freq, ((DAC_FREQ + (in_freq/2)) / in_freq), autocorrelate_detect_period(buffer, sample_count, 0)
			);

	return sample_count;
}


void adc_analysis(uint16_t *buffer, uint32_t count)
{
	uint32_t max = 0, sum = 0, min = 0;

	//iterate through input buffer and computed max and min values from sampled sine wave.
	for(int i=0;i<count;i++)
	{
		if(buffer[i] > max)
			max = buffer[i];

		if(buffer[i] < min)
			min = buffer[i];

		sum += buffer[i];
	}

	//used to compute period and frequency
	int temp = autocorrelate_detect_period(buffer, count, 1);

	printf("ADC Analysis of DAC generated sine function\r\n");
	printf("min = %d  max = %d  avg = %d  period = %d  frequency = %d Hz \r\n"
			,min, max, sum/count, temp, ADC_SAMPLING_FREQ/temp
			);
}
