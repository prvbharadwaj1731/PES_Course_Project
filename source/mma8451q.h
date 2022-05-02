/*
 * mma8451q.h
 *
 *  Created on: 23-Apr-2022
 *      Author: prvbh
 */

#ifndef MMA8451Q_H_
#define MMA8451Q_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MMA8451Q_I2C_ADDRESS		0x3A


//Addresses of registers onboard MMA8451Q
#define REG_XHI 0x01
#define REG_XLO 0x02
#define REG_YHI	0x03
#define REG_YLO	0x04
#define REG_ZHI	0x05
#define REG_ZLO	0x06

#define REG_WHOAMI 0x0D
#define REG_CTRL1  0x2A
#define REG_CTRL4  0x2D
#define REG_OFF_X  0x2F
#define REG_OFF_Z  0x31


#define WHOAMI	   0x1A

#define COUNTS_PER_G (4096.0)
#define PI_FLOAT	 (3.14159265)

#define abs(x) ((x) < 0 ? (-x) : (x))

//extern float roll, pitch;
//extern int16_t acc_X, acc_Y, acc_Z;

void init_mma8451q();

int read_full_xyz();

void mma8451q_test();

void mma8451q_calibrate(int, bool);

#endif /* MMA8451Q_H_ */
