/*
 * DAC.h
 *
 *  Created on: 11-Apr-2022
 *      Author: prvbh
 */

#ifndef DAC_H_
#define DAC_H_

#define SYSCLOCK 48000000 //use 48 MHz clock

void DAC_Init();

void DMA_Init();

void TPM0_Init();

void DMA_start(uint32_t, uint16_t*);

#endif /* DAC_H_ */
