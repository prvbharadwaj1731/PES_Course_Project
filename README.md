# PES_Course_Project
Final submission in fulfillment of coursework for Principles of Embedded Software, ECEN 5813, Spring 2022. This project builds a musical note generator where the frequency of the note is mapped to the roll-angle of the board in x-axis.

## Project Description and Requirements

This project aims to use the on-board MMA8451Q 3-axis accelerometer in conjunction with the DAC peripheral to generate different musical notes depending on the roll-angle of the board in the X-axis. Only the DEBUG target is to be considered for this project. 

## Repository contents

1. i2c.c & i2c.h - contains all code to intialize I2C0 peripheral at a baud rate of 100k. Further, it also  contains code to allow I2C read and write by accepting device address, register address and input data as the parameters. 
2. mma8451q.c & mma8451q.h - contains all code to initialize MMA8451Q 3-axis accelerometer in its active mode and use 14-bit mode at an output data rate (ODR) of 800 Hz.Further, it contains code that reads the acceleration data from the sensor and converts it into usable roll-angle in degrees with a range of [0-180]. 
3. DAC.c & DAC.h - contains all code to initialize DAC0 peripheral to generate the required musical notes. The DAC sampling frequency is 48 KHz and is driven by DMA interrupt. It  also contains code to initialize the DMA peripheral to transfer data from sine lookup table present in flash to the DAC buffer.
4. ADC.c & ADC.h - contains all code to initialize the ADC0 peripheral with a sampling frequency of 96 KHz in 16-bit mode. It also contains code to initialize TPM1, which is used by the ADC code to time sampling windows and ADC calibration routine. 
5. sine_generator.c & sine_generator.h - contains code to generate required sine values using integer-math. The lookup table is precomputed and stored in memory. This also handles linear interpolation for values that do not match the exact lookup table values. 
6. sample_generator.c & sample_generator.h - contains code to generate musical notes at required frequency. Fills up the buffer accessed by the DMA peripheral with the correct samples. 
7. sine_test.c & sine_test.h - contains code to automatically test the accuracy of the integer-math based sine function. Iterates over all values between -Pi to Pi. Returns maximum error and sum of squares error. 
8. delay.c & delay.h - contains code to generate delays using hard-spin loops. Input is provided in number of milliseconds of delay required.
9. PES_Assignment_7.c - main file that calls all initialization functions and runs the main while loop. Reused PES Assignment-7 MCUXpresso project for this. 

## Project Comments and Observation

Musical notes were heard using a small buzzer connected to DAC0 output pin (PTE30). However, musical notes with frequency below 80 Hz were inaudible using this buzzer. Hence, notes with frequency greater than 80 Hz was considered (starting with C0). Similarly, musical notes with frequency above 3 KHz had a sudden decrease in volume due to the buzzer's poor frequency response. Hence notes with frequency greater than 3 KHz were not used. The last musical note considered was F7. The transition between different frequencies produces a noticeable delay, which is realized with a stair-step change in frequency in the demo. This is due to the limitations in the performance of the onboard DAC and the code producing the samples filling the DAC buffer in time. 

## Demo video 
[Click here for Demo video](https://www.youtube.com/watch?v=qMJ4uztnX1U)

## References and Citations
1. Embedded Systems Fundamentals with ARM Cortex-M Based Microcontrollers: Embedded Systems Fundamentals by Alexander G Dean.
2. Alexander G Dean Github repository with supporting material for the book mentioned above: [Click here](https://github.com/alexander-g-dean/ESF)
3. List of musical notes in western music with their corresponding frequencies: [Click here](https://pages.mtu.edu/~suits/notefreqs.html)
4. NXP MMA8451Q reference manual and calibration document : [Click here](https://www.nxp.com/docs/en/data-sheet/MMA8451Q.pdf)



