/*
 * i2c.h
 *
 *  Created on: 23-Apr-2022
 *      Author: prvbh
 */

#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

//Define all the required masks and macros for I2C operation

#define I2C_START 		I2C0->C1 |= I2C_C1_MST_MASK;
#define I2C_STOP 		I2C0->C1 &= ~I2C_C1_MST_MASK;
#define I2C_REPEATED_START 	I2C0->C1 |= I2C_C1_RSTA_MASK;

#define I2C_TRANSMIT	I2C0->C1 |= I2C_C1_TX_MASK;
#define I2C_RECEIVE		I2C0->C1 &= ~I2C_C1_TX_MASK;
#define I2C_WAIT		while((I2C0->S & I2C_S_IICIF_MASK)==0){} \
							I2C0->S |= I2C_S_IICIF_MASK;

#define ACK				I2C0->C1 &= ~I2C_C1_TXAK_MASK
#define NACK 			I2C0->C1 |= I2C_C1_TXAK_MASK

#define LOCK_DETECT_MAX 200


//encode input address in 7-bit mode with read flag
#define I2C_READ_ADDRESS(input_address) ((uint8_t)((input_address << 1) | 1))

//encode input address in 7-bit mode with write flag
#define I2C_WRITE_ADDRESS(input_address) ((uint8_t)((input_address << 1) | 0))




void i2c_init();


void i2c_write_byte(uint8_t, uint8_t, uint8_t);


uint8_t i2c_read_byte(uint8_t, uint8_t);

void i2c_start();

void i2c_busy();

void i2c_wait();

void i2c_read_setup(uint8_t, uint8_t);

uint8_t i2c_repeated_read(uint8_t);


#endif /* I2C_H_ */
