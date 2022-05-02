/*
 * mma8451q.c
 *
 *  Created on: 23-Apr-2022
 *      Author: prvbh
 */


#include "i2c.h"
#include "mma8451q.h"
#include "MKL25Z4.h"
#include "delay.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
//necessary for arc tangent calculation
#include <math.h>


void init_mma8451q()
{
	//use active mode of accelerometer, 14-bit sample size and 800Hz output data rate
	i2c_write_byte(MMA8451Q_I2C_ADDRESS, REG_CTRL1, 0x01);
}

int read_full_xyz()
{
	int i = 0;
	uint8_t data[6];
	int16_t temp[3];

	int16_t roll;
	int pitch=0, yaw=0;

	i2c_start();
	i2c_read_setup(MMA8451Q_I2C_ADDRESS, REG_XHI);

	//read 5 bytes in repeated start mode
	for(i=0;i<5;i++){
		data[i] = i2c_repeated_read(0);
	}

	//read last byte
	data[i] = i2c_repeated_read(1);

	for(i=0;i<3;i++){
		temp[i] = (int16_t)((data[2*i] << 8) | data[2*i+1]);
	}

	pitch = temp[1]/4;
	yaw = temp[2]/4;

	roll = atan2(pitch, yaw)*180/PI_FLOAT;

	return roll;
}

