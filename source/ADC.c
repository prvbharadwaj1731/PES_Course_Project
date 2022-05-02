/*
 * ADC.c
 *
 *  Created on: 18-Apr-2022
 *      Author: prvbh
 */

#include "MKL25Z4.h"
#include <stdio.h>
#include <stdint.h>
#include "DAC.h"

#define BEGIN_TPM1 TPM1->SC |= TPM_SC_CMOD(1);
#define STOP_TPM1 TPM1->SC &= ~TPM_SC_CMOD_MASK;
#define CALIBRATION_BIT 7

/*
 * Description : Use TPM1 and initialize it for ADC clocking
 * Parameters : int sample count to set TPM overflow
 * Returns : none
 * */
void TPM1_Init(int sample_count)
{
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;

	//Disable TPM before further configuration
	TPM1->SC = 0;

	//set mod values and clear count value
	TPM1->MOD = TPM_MOD_MOD(SYSCLOCK/sample_count);
	TPM1->CNT = 0;

	//Prescalar value = 1 (no prescaling) and use centre-aligned PWM with up-counting
	TPM1->SC = TPM_SC_PS(0) | TPM_SC_CPWMS(0);
}



void ADC_Init()
{
	//Clock gating
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

	/*Following config set
	 * 1. normal power mode
	 * 2. no prescaling/clock dividing
	 * 3. 16-bit mode
	 * 4. use bus clock
	 * 5. long sampling mode
	 * */

	ADC0->CFG1 = ADC_CFG1_ADLPC(0) | ADC_CFG1_ADIV(0) | ADC_CFG1_ADLSMP(0) | ADC_CFG1_MODE(3) | ADC_CFG1_ADICLK(0);

	//Set ADC0->CFG2 to 0
	ADC0->CFG2 = 0;

	//Disable DMA initially
	ADC0->SC2 = ADC_SC2_ADTRG(1) | ADC_SC2_ACFE(0) | ADC_SC2_DMAEN(0) | ADC_SC2_REFSEL(0);

	//Set input channel for ADC the same as DAC channel, to route signal through
	ADC0->SC1[0] = ADC_SC1_AIEN(0) | ADC_SC1_DIFF(0) | ADC_SC1_ADCH(23);

	//Enable ADC0 to use TPM1 overflow
	SIM->SOPT7 = SIM_SOPT7_ADC0ALTTRGEN(1) | SIM_SOPT7_ADC0TRGSEL(9);
}


// *********************** EXTRA CREDIT ****************************

/*
 * Description : Running the inbuilt ADC calibration routine, as referenced in the KL25Z reference manual, §28.4.6.
 * The routine involves setting the CAL bit in the SC3 register high, only after ensuring that the conversion trigger for the ADC is set to software trigger
 * This automatically starts the calibration routine, and sets the COCO bit high once it is complete. We then generate gain calibration offset values to be
 *  set in the Plus Gain (PG) and Minus Gain (MG) registers.
 *
 *  Parameters : none
 *  Returns : none
 * */
void ADC_calibrate()
{
	uint16_t offset_plus_side = 0, offset_minus_side = 0;

	//set conversion trigger is set to software
	ADC0->SC2 = ADC_SC2_ADTRG(0);

	//start calibration process
	ADC0->SC3 |= (1 << CALIBRATION_BIT);

	//wait till calibration procedure is complete
	while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK)){
		; //do nothing
	}

	//CALIBRATION FOR PLUS-SIDE GAIN VALUE
	offset_plus_side = ADC0->CLP0 + ADC0->CLP1 + ADC0->CLP2 + ADC0->CLP3 + ADC0->CLP4 + ADC0->CLPS;
	offset_plus_side = offset_plus_side >> 1; //divide by 2

	//calculating MSB position
	int msb_pos = sizeof(offset_plus_side) << 3;

	//setting MSB
	offset_plus_side |= (1 << (msb_pos - 1));

	//set this value in ADC plus gain calibration register PG
	ADC0->PG |= offset_plus_side;

	//CALIBRATION FOR MINUS-SIDE GAIN VALUE
	offset_minus_side = ADC0->CLM0 + ADC0->CLM1 + ADC0->CLM2 + ADC0->CLM3 + ADC0->CLM4 + ADC0->CLMS;
	offset_minus_side = offset_minus_side >> 1; //divide by 2

	//set MSB
	offset_minus_side |= (1 << (msb_pos - 1));

	//set this value in ADC minus gain calibration register MG
	ADC0->MG |= offset_minus_side;
}


/*
 * Description : starts ADC conversion and passes input buffer to store values
 *
 * Parameters : uint16_t* pointer to input buffer, uint16_t sample count
 *
 * Returns : none
 * */
void ADC_buffer(uint16_t *buffer, uint32_t sample_count)
{
	BEGIN_TPM1

	for(int i=0;i<sample_count;i++)
	{
		//wait for ADC process to finish, by monitoring the COCO mask bit
		while(!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
				;
		buffer[i] = ADC0->R[0];
	}

	STOP_TPM1;
}



