/*
 * File:   EventChecker.h
 * Author: rcrobert
 *
 * Created on December 1, 2014, 1:01 AM
 */

#include "EventChecker.h"

#include <BOARD.h>
#include <timers.h>

/* If the muxs have a significant switching time ( >ns scale) then a timer with
 * an ISR needs to be implemented somehow
 *
 * Ideas:
 * - Volatile variables, event checkers simply return the relevant values from
 * the variables and behave like accessors. The ISR handles polling of the
 * sensors and updates their data structs accordingly.
 *
 * - Use the timers.h library somehow and make sure it doesnt interfere with its
 * events passed to the ES Framework
 *
 * - Read one sensor and increment the mux selector each time the checker is
 * called. Need to see how often this actually happens. It is most likely on the
 * ms scale so this should be completely fine.
 *
 */

/*******************************************************************************
 * PRIVATE #DEFINES
 ******************************************************************************/

// Uncomment to use a verbose debugger, recommend setting a large polling delay
//#define EVENTCHECKER_DEBUG

// A bitwise OR of all 3 mux selector pins, space saver
#define MUX_PINS_OR (MUX_PINS_BIT0 | MUX_PINS_BIT1 | MUX_PINS_BIT2)

#define MAX_MUX_SEL (0x05)


/*******************************************************************************
 * GLOBAL VARIABLE DEFINITIONS
 ******************************************************************************/

unsigned int distVals[] = {0, 0, 0, 0, 0, 0, 0, 0};

/*******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/

typedef enum InternalEvent_t {
    INT_NO_EVENT,
    INT_BUMPER_TRIPPED,
    INT_BUMPER_RELEASE,
    INT_BEACON_FOUND,
    INT_BEACON_LOST,
    INT_TAPE_FOUND,
    INT_TAPE_LOST
} InternalEvent;

// Used for tape sensor synchronous filtering
enum {
	LEDS_OFF,
	LEDS_ON
};

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/

static uint8_t muxCnt = 0x00;

static char tapeReadType = LEDS_OFF;
static int oldTapeVals[NUM_TAPE_SENSORS];
static int tapeValsOn[NUM_TAPE_SENSORS];
static int tapeValsOff[NUM_TAPE_SENSORS];

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/

static InternalEvent CheckBumps(int index);
static InternalEvent CheckBeacon(int index);
static InternalEvent CheckTape(int index);
static char UpdateTape(int index);
static char SwitchTapeState(void);
static char IncrementMux(void);

#ifdef EVENTCHECKER_DEBUG
static void PrintEvent(EventStorage event);
#endif

// TO BE IMPLEMENTED
InternalEvent CheckDistance(int index);

/*******************************************************************************
 * PUBLIC FUNCTION DEFINITIONS
 ******************************************************************************/

uint8_t CheckSensors(void)
{
    enum {
	READY_TO_READ,
	NOT_READY_TO_READ
    };

    // Function control variables
    static uint8_t canRead = READY_TO_READ;

    uint8_t retVal = FALSE;
    int i;

    InternalEvent check;

    // Variables for distance
    

    // If not expired, do nothing
    if (IsTimerExpired(SENSORS_TIMER) == TIMER_NOT_EXPIRED) {
	if (IsTimerActive(SENSORS_TIMER) == TIMER_NOT_ACTIVE) {
	    // Start it
	    InitTimer(SENSORS_TIMER, SENSORS_POLLING_DELAY);
	} else {
	    return retVal;
	}
    } // else it is expired

    // Timer has expired, reset and determine how to handle
    ClearTimerExpired(SENSORS_TIMER);

    switch (canRead) {
    case NOT_READY_TO_READ:

	#ifdef EVENTCHECKER_DEBUG
	printf("\nEntered NOT_READY_TO_READ");
	#endif

	// Set read ports as inputs, need to switch back to inputs
	IO_PortsSetPortInputs(SENSOR_PINS_PORT, (SENSOR_PINS_DIST |
						 SENSOR_PINS_BEACON));

	// Set timer delay
	if (IsTimerActive(SENSORS_TIMER) == TIMER_NOT_ACTIVE) {
	    InitTimer(SENSORS_TIMER, SENSORS_POLLING_DELAY);
	}

	// Change state
	canRead = READY_TO_READ;
	
	break;

    case READY_TO_READ:

	#ifdef EVENTCHECKER_DEBUG
	printf("\nEntered READY_TO_READ\nSelector: %d", muxCnt);
	#endif

	// Read from each sensor

	// Read bump sensor
	check = CheckBumps(muxCnt);

	if (check == INT_BUMPER_RELEASE) {
	    // Post event for this bumper

	    retVal = TRUE;
	    #ifdef EVENTCHECKER_DEBUG
	    printf("\nBumper %d released", muxCnt);
	    #endif
	} else if (check == INT_BUMPER_TRIPPED) {
	    // Post event for this bumper

	    retVal = TRUE;
	    #ifdef EVENTCHECKER_DEBUG
	    printf("\nBumper %d tripped", muxCnt);
	    #endif
	}

	// Read beacon sensor
	check = CheckBeacon(muxCnt);

	if (check == INT_BEACON_FOUND) {
	    // Post event for this beacon detector

	    retVal = TRUE;
	    #ifdef EVENTCHECKER_DEBUG
	    printf("\nBeacon %d found", muxCnt);
	    #endif
	} else if (check == INT_BEACON_LOST) {
	    // Post event for this beacon detector

	    retVal = TRUE;
	    #ifdef EVENTCHECKER_DEBUG
	    printf("\nBeacon %d lost", muxCnt);
	    #endif
	}


	// THRESHOLD VALUES AND NUMBER OF THRESHOLDS TBD
	// Read distance sensors
	if (muxCnt < NUM_LIGHT_SENSORS) {
	    // Update array with reading
	    // distVals[muxCnt] = AD_ReadADPin(SENSOR_PINS_DIST);

	    // Possibly set up thresholds to throw events
	    // Otherwise always throw events? Or the state machines can all poll
	    // the array as they see fit? Should probably simplify it down to
	    // use events
	}

	// Read tape sensor DO NOT HANDLE EVENTS HERE
	UpdateTape(muxCnt);

	// Increment mux
	IncrementMux();

	// Set outputs, write high to discharge circuits (only distance/beacon)
	IO_PortsSetPortOutputs(SENSOR_PINS_PORT, (SENSOR_PINS_DIST |
						  SENSOR_PINS_BEACON));
	IO_PortsSetPortBits(SENSOR_PINS_PORT, (SENSOR_PINS_DIST |
					       SENSOR_PINS_BEACON));

	// Switch tape sensor state once muxCnt rolls over to 0, check events
	if (muxCnt == 0x00) {
	    SwitchTapeState();

	    // Iterate and check for events
	    for (i = 0; i < NUM_TAPE_SENSORS; i++) {
		check = CheckTape(i);

		if (check == INT_TAPE_FOUND) {
		    // Post event for this tape sensor

		    retVal = TRUE;
		    #ifdef EVENTCHECKER_DEBUG
		    printf("\"Tape %d found", i);
		    #endif
		} else if (check == INT_TAPE_LOST) {
		    // Post event for this tape sensor

		    retVal = TRUE;
		    #ifdef EVENTCHECKER_DEBUG
		    printf("\nTape %d found", i);
		    #endif
		}
	    }
	}

	// Set timer delay
	if (IsTimerActive(SENSORS_TIMER) == TIMER_NOT_ACTIVE) {
	    InitTimer(SENSORS_TIMER, SENSORS_POLLING_DELAY);
	}

	// Change state
	canRead = NOT_READY_TO_READ;

	break;
    }

    return retVal;
}

/*******************************************************************************
 * PRIVATE FUNCTION DEFINITIONS
 ******************************************************************************/

static InternalEvent CheckBumps(int index)
{
    static uint8_t oldBumpState = 0x1F;	// set to initialize to all released
    uint8_t newBumpState;
    uint8_t mask;

    InternalEvent retVal = INT_NO_EVENT;
    
    if (index < NUM_BUMP_SENSORS) {
	mask = 1 << index;
	
	// Read in bumper state and compare with the old bump state
	newBumpState = (IO_PortsReadPort(SENSOR_PINS_PORT) &
			    SENSOR_PINS_BUMP) ? mask : 0x00;

	if ((newBumpState ^ oldBumpState) & mask) {
	    // State has changed, prep to return event
	    if (newBumpState) {
		retVal = INT_BUMPER_RELEASE;
	    } else {
		retVal = INT_BUMPER_TRIPPED;
	    }

	    // Update old value; flip corresponding bit
	    oldBumpState ^= mask;
	}
    }

    return retVal;
}

// Currently assumes high values imply beacon
static InternalEvent CheckBeacon(int index)
{
    static unsigned int oldADVals[] = {0, 0, 0, 0}; // init undetected all
    unsigned int newADVal;

    InternalEvent retVal = INT_NO_EVENT;

    // Out of bounds
    if (index >= NUM_LIGHT_SENSORS) {
	return retVal;
    }

    newADVal = AD_ReadADPin(SENSOR_PINS_BEACON);

    // Rising edge
    if ((newADVal > THRESHOLD_BEACON_HIGH) && (oldADVals[index] <
						THRESHOLD_BEACON_HIGH)) {
	retVal = INT_BEACON_FOUND;
    }

    // Falling edge
    else if ((newADVal < THRESHOLD_BEACON_LOW) && (oldADVals[index] >
						    THRESHOLD_BEACON_LOW)) {
	retVal = INT_BEACON_LOST;
    }
    // else no event

    oldADVals[index] = newADVal;

    return retVal;
}

// Checks array index for thresholds based on previous val
// Must be called iteratively?
// Check polarity on the tape sensors, adjust events accordingly
static InternalEvent CheckTape(int index)
{
    int newTapeVal;

    InternalEvent retVal = INT_NO_EVENT;

    // Out of bounds
    if (index >= NUM_TAPE_SENSORS) {
	return retVal;
    }

    newTapeVal = tapeValsOn[index] - tapeValsOff[index];

    // Rising edge
    if ((newTapeVal > THRESHOLD_TAPE_HIGH) && (oldTapeVals[index] <
						THRESHOLD_TAPE_HIGH)) {
	retVal = INT_TAPE_FOUND;
    }

    // Falling edge
    else if ((newTapeVal < THRESHOLD_TAPE_LOW) && (oldTapeVals[index] >
						    THRESHOLD_TAPE_LOW)) {
	retVal = INT_TAPE_LOST;
    }
    // else no event

    oldTapeVals[index] = newTapeVal;

    return retVal;
}

// wrapper for reading the tape sensors, handles which arrays are written to
static char UpdateTape(int index)
{
    // Out of bounds
    if (index >= NUM_TAPE_SENSORS) {
	return ERROR;
    }

    switch (tapeReadType) {
    case LEDS_OFF:
	// Read values
	tapeValsOff[index] = AD_ReadADPin(SENSOR_PINS_TAPE);
	break;

    case LEDS_ON:
	// Read values
	tapeValsOn[index] = AD_ReadADPin(SENSOR_PINS_TAPE);
	break;
    }

    return SUCCESS;
}

// handle switching between LEDs on and LEDs off states
static char SwitchTapeState(void)
{
    switch (tapeReadType) {
    case LEDS_OFF:
	// Turn on LEDs
	IO_PortsSetPortBits(SENSOR_PINS_PORT, SENSOR_PINS_LEDS);

	// Change states
	tapeReadType = LEDS_ON;
	break;

    case LEDS_ON:
	// Turn off LEDs
	IO_PortsClearPortBits(SENSOR_PINS_PORT, SENSOR_PINS_LEDS);

	// Change states
	tapeReadType = LEDS_OFF;
	break;
    }

    return SUCCESS;
}

static char IncrementMux(void)
{
    muxCnt = (muxCnt + 1) % MAX_MUX_SEL;
    IO_PortsSetPortBits(MUX_PINS_PORT, MUX_PINS_OR);
    IO_PortsClearPortBits( MUX_PINS_PORT, (0x0000 ^ (~MUX_PINS_OR)) );

    return SUCCESS;
}

#ifdef EVENTCHECKER_DEBUG
// Test the union type from the header, print out some of them etc
static void PrintEvent(EventStorage event)
{
    
}
#endif
