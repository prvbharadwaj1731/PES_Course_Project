/*
 * sine_generator.c
 *
 *  Created on: 11-Apr-2022
 *      Author: prvbh
 *
 *  Description : implementing a integer-based sine function generator using lookup table approach and linear interpolation
 */


#include "MKL25Z4.h"
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include "sine_generator.h"
#include "assert.h"

#define LOOKUP_TABLE_SIZE 32
#define LOOKUP_TABLE_STEP_SIZE (HALF_PI/LOOKUP_TABLE_SIZE)

static const int16_t sine_lookup_table[LOOKUP_TABLE_SIZE + 1] =
{
		0, 100, 200, 299, 397, 495, 591, 686, 780, 871, 960, 1047, 1132, 1214,
		1292, 1368, 1440, 1509, 1575, 1636, 1694, 1747, 1797, 1842, 1882, 1918,
		1949, 1976, 1998, 2015, 2027, 2035, 2037
};


/* Description:
 *
 * Linear interpolation is used to calculate the y-axis value based previous known values
 * for a given set of known points (x1, y1) and (x2, y2), calculating a third point (x,y) is given by
 * (y-y1)/(x-x1) = (y2-y1)/(x2-x1)
 * This equation is solved for y to get the required sine value
 *
 * Parameters : int32_t x,x1,x2,y1,y2 - 2 sets of co-ordinates and variable of the value to be computed
 *
 * Returns : int32_t interpolated sine value
 * */

static int32_t interpolate(int32_t x, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
	int32_t result;
	result = ( (((x2-x1)/2) + ((x-x1) * (y2-y1))) / (x2-x1) + y1);
	return result;
}



/*
 * Description : Generates sine values using a lookup table with 32 entries. This fixes the resolution to around 3 degrees or 0.05 radians.
 * To compute values that do not match this resolution, we use linear interpolation.
 *
 * Parameters : int32_t angle input
 *
 * Returns : int32_t integer-math generated sine value within range of [0,2037]
 * */
int32_t sine_generate(int32_t in)
{
	int32_t index;

	//initially positive sine values
	int sign = 1;

	while(in < -PI)
		in += TWO_PI;

	while(in > PI)
		in -= TWO_PI;

	assert(-PI <= in && in <= PI);

	if(in < 0){
		in = -in;
		sign = -1;
	}

	//mirroring the range of [pi/2, pi] to [0, pi/2] removed from pi
	if(in > HALF_PI)
		in = PI - in;

	index = LOOKUP_TABLE_SIZE * in / HALF_PI;
	int32_t temp1 = index * LOOKUP_TABLE_STEP_SIZE;

	//if we are in the range of [0,pi/2]
	if(temp1 == in)
		return sign * sine_lookup_table[index];

	int32_t temp2 = (index+1) * LOOKUP_TABLE_STEP_SIZE;
	int32_t res = interpolate(in, temp1, sine_lookup_table[index], temp2, sine_lookup_table[index+1]);

	return sign*res;

}


