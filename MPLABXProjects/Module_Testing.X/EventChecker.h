/* 
 * File:   EventChecker.h
 * Author: rcrobert
 *
 * Created on December 1, 2014, 1:01 AM
 */

#ifndef EVENTCHECKER_H
#define	EVENTCHECKER_H

#include <inttypes.h>

#include "BotConfig.h"
#include "ES_Configure.h"

/*******************************************************************************
 * PUBLIC #DEFINES
 ******************************************************************************/

#define SENSORS_TIMER 10              // timers.h number used by the module
#define SENSORS_POLLING_DELAY (1000)   // minimum delay in ms for values to settle

// Number of each sensor, internally controls the limits on the mux iterations
// Limit of 8 each
#define NUM_LIGHT_SENSORS (4)
#define NUM_TAPE_SENSORS (5)
#define NUM_BUMP_SENSORS (5)

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/

extern unsigned int distVals[];

/*******************************************************************************
 * PUBLIC TYPES
 ******************************************************************************/

// Define union used for passing event parameters
typedef union EventStorage_t {
    struct {
        uint8_t bumpStates : 5;     // one bit per sensor, current tripped state
        uint8_t eventStates : 5;    // one bit per sensor, if event occurred
    } bits;
    uint16_t val;
} EventStorage;

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

uint8_t CheckSensors(void);

#endif	/* EVENTCHECKER_H */

