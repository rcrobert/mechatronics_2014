/* 
 * File:   MotorDriver.h
 * Author: rcrobert
 *
 * Created on November 29, 2014, 8:02 AM
 */

/* Notes
 * Do we have all motor driving functions in the same library? Or do we separate
 * them? Build one library off of the basic motor driving functions and the more
 * advanced interface on top of this?
 *
 * Need to calculate motor ratio
 * Need to implement voltage checking, maybe only in high level functions
 *
 * Voltage from battery
 * through 10:1 divider, 0-1023
 * reads 0-33V
 * vbat = in / 1023.0 * 33.0
 */

#ifndef MOTORDRIVER_H
#define	MOTORDRIVER_H

#include "BotConfig.h"

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

// Init
void Drive_Init(void);

// Range of -1000 to 1000
char Drive_Straight(int speed);
char Drive_Stop(void);

// Gradual turns
// Currently drives one motor at speed and one at speed/2
char Drive_Left(int speed);
char Drive_Right(int speed);

// Turn in place, no point sending negative speeds
char Drive_TankLeft(int speed);
char Drive_TankRight(int speed);

// Drive lift motor
char Drive_LiftUp(void);
char Drive_LiftDown(void);
char Drive_LiftStop(void);

#endif	/* MOTORDRIVER_H */

