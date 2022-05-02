/*
 * sine_test.c
 *
 *  Created on: 17-Apr-2022
 *      Author: prvbh
 */


#include <stdio.h>
#include "MKL25Z4.h"
#include "sine_generator.h"
#include "fsl_debug_console.h"
#include "math.h" //for using inbuilt sine function to test



/*
 * Description : calculates error between computed integer-math sine and inbuilt math library instance of the same. Computes max error and sum of squares error
 * Parameters : none
 * Returns : none
 * */
void test_sine()
{
	double calculated_sine, expected_sine, error, ss_error, max_error;
	ss_error = 0;
	max_error = 0;


	for(int i=-TWO_PI;i<=TWO_PI;i++)
	{
		expected_sine = sin((double)i/TRIG_SCALE_FACTOR) * TRIG_SCALE_FACTOR;
		calculated_sine = sine_generate(i);
		error = calculated_sine - expected_sine;
		if(error<0)
			error = -error; //use all positive values for comparison

		if(error>max_error)
			max_error = error;

		ss_error += error*error;
	}

	printf("max error = %d  sum of squares error = %d\n", (int)max_error, (int)ss_error);

}


