/*
 * DAC.c
 *
 *  Created on: 11-Apr-2022
 *      Author: prvbh
 */
#include "MKL25Z4.h"
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "DAC.h"

#define DAC0_OUT_PIN 30
#define TPM0_SOURCE 54
#define SAMPLE_COUNT 1024

//globals for buffer
static uint16_t buffer_g[SAMPLE_COUNT];
static uint32_t count_g;

void DAC_Init()
{
	//Clock gating
	SIM->SCGC6 |= SIM_SCGC6_DAC0_MASK; //for DAC
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; //for DAC output on Port E pin

	PORTE->PCR[DAC0_OUT_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[DAC0_OUT_PIN] |= PORT_PCR_MUX(3);

	//We are not using onboard DAC buffer, so disable it

	DAC0->C1 = 0;
	DAC0->C2 = 0;

	//Use VDDA(Analog power voltage)(DACREF_2) as reference voltage and enable DAC
	DAC0->C0 = DAC_C0_DACEN_MASK | DAC_C0_DACRFS_MASK;
}

void DMA_Init()
{
	//Clock gating
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;

	//disable DMA before any further configuration
	DMAMUX0->CHCFG[0] = 0;

	/*Set the following configuration:
	 * 1. enable interrupt
	 * 2. increment source address once interrupted
	 * 3. source address size : 16 bits
	 * 4. destination address size : 16 bits
	 * 5. Enable cycle stealing mode
	 * 6. Allow peripherals to initiate DMA transfer
	 * 5. enable DMA0
	 * */

	DMA0->DMA[0].DCR = DMA_DCR_EINT_MASK | DMA_DCR_SINC_MASK | DMA_DCR_SSIZE(2) | DMA_DCR_DSIZE(2) | DMA_DCR_ERQ_MASK | DMA_DCR_CS_MASK;

	//NVIC config
	NVIC_SetPriority(DMA0_IRQn, 2);
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);

	//Use TPM0 overflow as the periodic trigger for DMA0 transfer
	DMAMUX0->CHCFG[0] = DMAMUX_CHCFG_SOURCE(TPM0_SOURCE);
}

void TPM0_Init()
{
	//Clock gating
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	//Set input clock frequency to 48 MHz
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;

	//Config
	TPM0->SC = 0;
	TPM0->MOD = TPM_MOD_MOD(SYSCLOCK/(SYSCLOCK/1000));
	TPM0->CNT = 0;

	/*The following config was set:
	 * 1. Enable DMA transfer on overflow
	 * 2. Centre-aligned PWM mode and up counting mode
	 * 3. Counter increments on every clock pulse
	 * 4. No prescaling needed
	 * */
	TPM0->SC = TPM_SC_PS(0) | TPM_SC_CPWMS(0) | TPM_SC_CMOD(1) | TPM_SC_DMA_MASK;
}

void DMA_start(uint32_t count, uint16_t *freq)
{
	//Disable TPM before reconfiguring
	TPM0->SC &= ~TPM_SC_CMOD_MASK;

	count_g = count;

	//copy data into temporary buffer global
	memcpy(buffer_g, freq, count<<1);

	//Config change
	TPM0->SC |= TPM_SC_CMOD(1);

	//Set source and destination registers with the correct addresses
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) buffer_g);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));

	//set number of bytes to transfer into the BCH register
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(count_g<<1);
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;

}

void DMA0_IRQHandler(void)
{
	//Clear done flag
	DMA0->DMA[0].DSR_BCR |= DMA_DSR_BCR_DONE_MASK;

	//set source and destination addresses
	DMA0->DMA[0].SAR = DMA_SAR_SAR((uint32_t) buffer_g);
	DMA0->DMA[0].DAR = DMA_DAR_DAR((uint32_t) (&(DAC0->DAT[0])));

	//set number of bytes to transfer into BCH register
	DMA0->DMA[0].DSR_BCR = DMA_DSR_BCR_BCR(count_g<<1);
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
}
