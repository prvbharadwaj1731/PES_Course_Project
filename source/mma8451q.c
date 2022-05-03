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
//#include <stddef.h>
//#include <stdbool.h>
//necessary for arc tangent calculation
#include <math.h>

#define MAX_ERROR 4

void init_mma8451q()
{
	//use active mode of accelerometer, 14-bit sample size and 800Hz output data rate
	i2c_write_byte(MMA8451Q_I2C_ADDRESS, REG_CTRL1, 0x01);
}


/*
 * Description : Returns roll-angle of the board in X-axis. This is done by reading the REG_XHI register onboard the accelerometer.
 * Parameters : none
 * Returns : int roll angle
 * */
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


/*
 * Description : MMA8451Q test function, which prompts user to place the board at 0 and 90 degrees respectively to calibrate the sensor.
 * 				 offset values are computed from this and stored onboard the sensor in offset registers.
 *
 * Parameters : none
 * Returns : none
 * */

void mma8451q_test()
{
	int roll_angle = 9999; //not initialized to 0 as this is the expected value when the board is held flat to ground
	int offset = 9999; //same concept as above
	printf("Please place the board on a flat surface, with the RGB LED facing upwards. Do not move the board till prompted.\r\n");

	roll_angle = abs(read_full_xyz());
	offset = roll_angle - 0;

	while(abs(offset) >= MAX_ERROR){
		printf(".");		//do nothing
		roll_angle = abs(read_full_xyz());
		offset = roll_angle - 0;
	}
	printf("\r\n");
	printf("Calibration Complete.\r\n");
	mma8451q_calibrate(offset, 1);
	Delay(500); //delay for 500 ms

	printf("Please place the board at a 90 degree angle, preferably flat against a wall or known vertical surface. Do not move the board till prompted.\r\n");

	roll_angle = abs(read_full_xyz());
	offset = roll_angle - 90;

	while(abs(offset) >= MAX_ERROR){
		printf(".");		//do nothing
		roll_angle = abs(read_full_xyz());
		offset = roll_angle - 90;
	}
	printf("\r\n");
	printf("Calibration Complete.\r\n");
	mma8451q_calibrate(offset, 0);
	Delay(500);
}


/*
 * Description : Writes precomputed offset values into their respective registers onboard the sensor. Refer to MMA8451Q reference manual for further information on this.
 * Parameters : int offset value, bool reg_offset, a switch to select between writing into X-axis or Z-axis register.
 * Returns : none
 * */
void mma8451q_calibrate(int offset, bool reg_offset)
{
	if(reg_offset)
		i2c_write_byte(MMA8451Q_I2C_ADDRESS, REG_OFF_X, offset); //set calibration value into offset X register
	else
		i2c_write_byte(MMA8451Q_I2C_ADDRESS, REG_OFF_Z, offset); //set calibration value into offset Z register
}
