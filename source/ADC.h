/*
 * ADC.h
 *
 *  Created on: 18-Apr-2022
 *      Author: prvbh
 */

#ifndef ADC_H_
#define ADC_H_


void ADC_Init();

void ADC_calibrate();

void TPM1_Init(int);

void ADC_buffer(uint16_t*, uint32_t);

#endif /* ADC_H_ */
