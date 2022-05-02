/*
 * i2c.c
 *
 *  Created on: 23-Apr-2022
 *      Author: prvbh
 */

#include <stdio.h>
#include <stddef.h>
#include "i2c.h"
#include "MKL25Z4.h"

//globals for I2C lock
int lock_detect_g = 0;
int i2c_lock_g = 0;


/*
 * Description : Initializes I2C0 peripheral to operate at a baud rate of 400k
 * Parameters : none
 * Returns : none
 * */

void i2c_init(void)
{
	//Clock gating to I2C0 peripheral and port E pins for communicating with accelerometer
	SIM->SCGC4 |= SIM_SCGC4_I2C0_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

	//set port E pins for I2C function (open drain)
	PORTE->PCR[24] |= PORT_PCR_MUX(5);
	PORTE->PCR[25] |= PORT_PCR_MUX(5);

	/*
	 * Use baud rate of 400K. Relation between baud rate and bus frequency is given below:
	 *
	 * 		baud rate = bus frequency / (scl divider factor * multipler);
	 *
	 * Our bus frequency is 24 MHz. Thus, bus_freq/baud_rate = 60, which necessitates a SCL divider factor of 56
	 * */

	I2C0->F = (I2C_F_ICR(0x10) | I2C_F_MULT(0));

	//Enable I2C0 peripheral
	I2C0->C1 |= (I2C_C1_IICEN_MASK);

	//Use high drive mode
	I2C0->C2 |= (I2C_C2_HDRS_MASK);
}


/*
 * Description : transmits 1 byte of data across I2C0 to given register address
 * Parameters : uint8_t device address, register address and data to be transmitted
 * Returns : none
 * */
void i2c_write_byte(uint8_t device_address, uint8_t register_address, uint8_t data)
{
	I2C_TRANSMIT; //set to transmit mode
	I2C_START; //sends start bit in master mode

	I2C0->D = device_address;
	I2C_WAIT;

	I2C0->D = register_address;
	I2C_WAIT;

	I2C0->D = data;
	I2C_WAIT;

	I2C_STOP;
}


/*
 * Description : Reads 1 byte of data off the I2C0 peripheral at given register address
 * Parameters : uint8_t device address, register address
 * Returns : uint8_t data
 * */

uint8_t i2c_read_byte(uint8_t device_address, uint8_t register_address)
{
	uint8_t rx_data;

	//set to transmit mode initially to send read command
	I2C_TRANSMIT;
	I2C_START;
	I2C0->D = device_address;

	I2C_WAIT;

	//Send register address across the SDA line
	I2C0->D = register_address;

	I2C_WAIT;

	I2C_REPEATED_START;
	I2C0->D = (device_address | 0x1);

	I2C_WAIT;

	//set to receive mode to read data from SDA line
	I2C_RECEIVE;
	NACK; //this only sets NACK, and not send it

	//Perform a dummy read
	rx_data = I2C0->D;

	I2C_WAIT;

	I2C_STOP;
	rx_data = I2C0->D;

	return rx_data;
}


/*
 * Description : Performs repeated read from given register location
 * Parameters : uint8_t last_read_flag
 * Returns : uint8_t received data
 * */

uint8_t i2c_repeated_read(uint8_t last_read_flag)
{
	uint8_t rx_data;
	lock_detect_g = 0;

	if(last_read_flag){
		NACK; //send NACK after reading
	}else{
		ACK; //send ACK after read
	}

	rx_data = I2C0->D;
	I2C_WAIT;

	if(last_read_flag){
		I2C_STOP;
	}

	rx_data = I2C0->D;

	return rx_data;
}

/*
 * Description : Reads setup register of a slave device operating at given address
 * Parameters : uint8_t device address, register address
 * Returns : none
 * */

void i2c_read_setup(uint8_t device_address, uint8_t register_address)
{
	I2C0->D = device_address;
	I2C_WAIT;

	I2C0->D = register_address;
	I2C_WAIT;

	I2C_REPEATED_START;

	I2C0->D = (device_address | 0x1); //send address in 7-bit mode
	I2C_WAIT;

	I2C_RECEIVE; //switch back to receive mode
}

/*
 * Description : macro function to set I2C0 in transmit mode and send out start bit
 * Parameters : none
 * Returns : none
 * */

void i2c_start()
{
	I2C_TRANSMIT;
	I2C_START;
}

/*
 * Description : Macro function to send out start bit across I2C0 and set peripheral to busy mode
 * Parameters : none
 * Returns : none
 * */

void i2c_busy()
{

	lock_detect_g = 0;
	//send start signal
	I2C0->C1 &= ~I2C_C1_IICEN_MASK; //disable peripheral before config
	I2C_TRANSMIT;
	I2C_START;

	I2C0->C1 |= I2C_C1_IICEN_MASK; //enable I2C0

	//clear SDA after sending start
	I2C_START;
	I2C_TRANSMIT //set to transmit mode

	I2C0->D = 0xFF;

	//I2C_WAIT;

	while((I2C0->S & I2C_S_IICIF_MASK) == 0U){
	}
	I2C0->S |= I2C_S_IICIF_MASK;

	I2C0->S |= I2C_S_ARBL_MASK;

	//disable I2C0 peripheral and send start
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C_TRANSMIT;
	I2C_START;

	//enable it again
	I2C0->C1 |= I2C_C1_IICEN_MASK;

	//disable I2C0 and send stop
	I2C0->C1 &= ~I2C_C1_IICEN_MASK;
	I2C_START;
	I2C_STOP;
	I2C_RECEIVE;

	I2C0->C1 |= I2C_C1_IICEN_MASK;

	//Enable interrupt status bits
	I2C0->S |= I2C_S_IICIF_MASK;
	I2C0->S |= I2C_S_ARBL_MASK;
	lock_detect_g = 0;
	i2c_lock_g = 1;

}

/*
 * Description : Macro function to wait on I2C0 return data
 * Parameters : none
 * Returns : none
 * */

void i2c_wait()
{
	lock_detect_g = 0;
	while(((I2C0->S & I2C_S_IICIF_MASK) == 0) & (lock_detect_g < LOCK_DETECT_MAX)){
		lock_detect_g++;
	}
	if(lock_detect_g >= LOCK_DETECT_MAX)
		i2c_busy();

	I2C0->S |= I2C_S_IICIF_MASK; //sets interrupt flag for next event
}

