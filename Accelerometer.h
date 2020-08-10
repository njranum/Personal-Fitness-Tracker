/*
 * Accelerometer.h
 *
 *  Created on: 11/05/2020
 *      Author: 64218
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_


typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

void
initAccl (void);

vector3_t getAcclData (void);





#endif /* ACCELEROMETER_H_ */
