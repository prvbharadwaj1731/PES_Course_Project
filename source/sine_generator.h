/*
 * sine_generator.h
 *
 *  Created on: 11-Apr-2022
 *      Author: prvbh
 */

#ifndef SINE_GENERATOR_H_
#define SINE_GENERATOR_H_

#define PI 6399 //After Scaling factor is applied
#define HALF_PI 3200
#define TWO_PI 12799
#define TRIG_SCALE_FACTOR 2037 //scaling factor used in conjunction with DAC conversion equatino


int32_t sine_generate(int32_t);

#endif /* SINE_GENERATOR_H_ */
