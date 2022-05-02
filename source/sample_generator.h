/*
 * sample_generator.h
 *
 *  Created on: 18-Apr-2022
 *      Author: prvbh
 */

#ifndef SAMPLE_GENERATOR_H_
#define SAMPLE_GENERATOR_H_

size_t tone_to_samples(int, uint16_t *, size_t);

void adc_analysis(uint16_t *, uint32_t);

#endif /* SAMPLE_GENERATOR_H_ */
