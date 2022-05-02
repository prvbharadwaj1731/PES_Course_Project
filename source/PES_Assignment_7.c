/*
 * Copyright 2016-2022 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
/**
 * @author			Pranav Bharadwaj
 * @file    		PES_Assignment_7.c
 * @brief   		Application entry point.
 *
 * @Description		This project utilizes the on board accelerometer to extract roll-angle of the board in the x-axis, and change the frequency of the
 * 					musical note being played based on the angle. Valid roll angles are from 0-180 degrees. Valid musical notes are in the range of [C0, F7] - 19 notes
 * 					The following operations occur in this file:
 * 					1. Initialize the following - ADC, DAC, DMA, TPM0, TPM1, I2C0, MMA8451Q accelerometer.
 * 					2. The integer-math based sine generator is tested with expected values from the math.h library
 * 					3. ADC is calibrated using the routine provided in the KL25Z reference manual
 * 					4. Musical note test. All available notes are played in succession, with a delay of 500 ms between each note.
 * 					5. Accelerometer Calibration routine. The user is prompted to place the board in different orientations to compute offset values.
 * 					5. Main while routine, where the roll-angle of the board in x-axis is mapped to a different musical note in the ascending order. We come around this loop
 * 					every 500 ms, which is duration for which a given tone is played.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MKL25Z4.h"
#include "fsl_debug_console.h"
/* TODO: insert other include files here. */
#include "sine_generator.h"
#include "sine_test.h"
#include "sample_generator.h"
#include "DAC.h"
#include "ADC.h"
#include "delay.h"
#include "i2c.h"
#include "mma8451q.h"
/* TODO: insert other definitions and declarations here. */
#define NO_OF_SAMPLES 1024
#define ADC_SAMPLING_FREQ 96000
#define MAX_ROLL_ANGLE 180
//musical note frequencies

#define A4 440
#define D5 587
#define E5 659
#define A5 880

/*
 * @brief   Application entry point.
 */
int main(void) {

  	/* Init board hardware. */
	BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
#ifndef BOARD_INIT_DEBUG_CONSOLE_PERIPHERAL
    /* Init FSL debug console. */
    BOARD_InitDebugConsole();
#endif

int sample_count;
uint16_t output_buffer[NO_OF_SAMPLES], input_buffer[NO_OF_SAMPLES];; //define holding buffers for DAC and ADC output
//
/*
 * Notes used are: (tuned for an equal-tempered scale tuned at A4 = 440 Hz)
* 							C0,  E0, G0, B0, D1, F1, A1, C2, E2, G2, B2,  D3,  F3,  A3,  C4,  E4,  G4,  B4,  D5,  F5,  A5,  C6,   E6,   G6,   B6,   D7,   F7
 * */

int note_frequency_list[] = {82, 98, 123, 147,
							175, 220, 262, 330,
							392, 494, 587, 698,
							880, 1047, 1319, 1568,
							1976, 2349, 2794};

//initializations
DAC_Init();
ADC_Init();
DMA_Init();
TPM0_Init();
TPM1_Init(ADC_SAMPLING_FREQ);
i2c_init();
init_mma8451q();
printf("Initializing MMA8451Q Accelerometer. Please Wait.\r\n");
Delay(100);
printf("Accelerometer initialization completed.\r\n");


// Test Functions
printf("Sine function tests running to check for errors. Please wait.\r\n");
test_sine();
printf("Sine function tests complete.\r\n");

printf("Running ADC calibration routine. Please wait.\r\n");
ADC_calibrate();
printf("ADC calibration routine complete.\r\n");

//initializing again cause ADC config was changed for during calibration routine
ADC_Init();

printf("Running musical note frequency test. All available notes shall be played in sequence. Please wait.\r\n");

int note_frequency_list_size = sizeof(note_frequency_list)/sizeof(note_frequency_list[0]);
int roll_angle = 0, index = 0;
int resolution = (int)(MAX_ROLL_ANGLE/note_frequency_list_size);

for(int i=0;i<note_frequency_list_size;i++)
{
	sample_count = tone_to_samples(note_frequency_list[i], output_buffer, NO_OF_SAMPLES);
	DMA_start(sample_count, output_buffer);
	ADC_buffer(input_buffer, NO_OF_SAMPLES);
	adc_analysis(input_buffer, NO_OF_SAMPLES);
	Delay(500);
}

printf("Musical note frequency test complete.\r\n");

//Accelerometer calibration and test
mma8451q_test();



		while(1){
			roll_angle = abs(read_full_xyz());
			printf("Roll angle = %d degrees\r\n", roll_angle);

			index = (int)(roll_angle/resolution);

			printf("Frequency of musical note played : %d Hz\r\n", note_frequency_list[index]);
			sample_count = tone_to_samples(note_frequency_list[index], output_buffer, NO_OF_SAMPLES);
			DMA_start(sample_count, output_buffer);
			Delay(500);
		}
}
