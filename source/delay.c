/*
 * delay.c
 *
 *  Created on: 02-May-2022
 *      Author: prvbh
 */


/*
 * Description : hard-spin delay loop
 * Parameters : delay value in milliseconds
 * Returns : none
 * */
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

void Delay(uint32_t val_in)
{
	volatile uint32_t temp;
	for(temp=val_in*10000; temp>0; temp--){
		;
	}
}
