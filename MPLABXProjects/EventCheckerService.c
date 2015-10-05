/*
 * File: EventCheckerService.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Modified for use: rcrobert
 *
 * Uses template.
 *
 * Service for checking all sensors associated with the bots. Handles the state
 * machine for sensor settling times and posts events
 *
 * Created on 23/Oct/2011
 * Updated on 13/Nov/2013
 */

/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"
#include "EventCheckerService.h"
#include <stdio.h>

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

// Be careful with this, seems to lock up a lot since it is blocking
//#define EVENTCHECKERSERVICE_VERBOSE
#ifdef EVENTCHECKERSERVICE_VERBOSE
#include "serial.h"
#define dbprintf(...) while(!IsTransmitEmpty()); printf(__VA_ARGS__)
#else
#define dbprintf(...)
#endif

/*
#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_STATES(STRING_FORM)
};
 */

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                    *
 ******************************************************************************/

enum {
	READY_TO_READ,
	NOT_READY_TO_READ
};

enum {
	LEDS_OFF,
	LEDS_ON
};

static uint8_t MyPriority;

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitEventCheckerService(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunEventCheckerService function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitEventCheckerService(uint8_t Priority)
{
	ES_Event ThisEvent;

	MyPriority = Priority;



	// Set up first timer call
	ES_Timer_InitTimer(EVENT_CHECKER_TIMER, SENSORS_POLLING_DELAY);

	// post the initial transition event
	ThisEvent.EventType = ES_INIT;
	if (ES_PostToService(MyPriority, ThisEvent) == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * @Function PostEventCheckerService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostEventCheckerService(ES_Event ThisEvent)
{
	return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function RunEventCheckerService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunEventCheckerService(ES_Event ThisEvent)
{
	int i;
	static char CurrentState;
	static uint8_t muxCnt = 0x00;

	static uint8_t oldBumpState = 0x00; // init to no bumps, active low
	static uint8_t newBumpState = 0x00;
	uint8_t mask;

	static uint16_t oldBeaconVals[] = {1023, 1023, 1023, 1023}; // active low
	uint16_t newADVal;

	static uint8_t oldTrackState = 0;
	uint8_t newTrackState;
	static uint8_t newTrack[3];
	uint8_t trackSum;

	static char tapeReadType = LEDS_OFF;
	int newTapeVal;
	static int oldTapeVals[MAX_MUX_SEL];
	static int tapeValsOn[MAX_MUX_SEL];
	static int tapeValsOff[MAX_MUX_SEL];

	ES_Event PostEvent = NO_EVENT;
	EventStorage EventData;
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT; // assume no errors

	switch (CurrentState) {

	case NOT_READY_TO_READ:
		// Responds to ES_TIMEOUT events
		if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam ==
			EVENT_CHECKER_TIMER) {
			// Timer has expired

			// Set some ports as inputs
			//	    IO_PortsSetPortInputs(SENSOR_PINS_PORT, (SENSOR_PINS_DIST |
			//		    SENSOR_PINS_BEACON));

			//Set up the next timeout, SERVICE ONLY TRIGGERS ON EVENTS
			ES_Timer_InitTimer(EVENT_CHECKER_TIMER, SENSORS_POLLING_DELAY);

			// Change state
			CurrentState = READY_TO_READ;

			// Event consumed by default
		}
		break;


	case READY_TO_READ:
		// Responds to ES_TIMEOUT events
		if (ThisEvent.EventType == ES_TIMEOUT && ThisEvent.EventParam ==
			EVENT_CHECKER_TIMER) {
			// Timer has expired

			/*
			 * Read bump sensor and post events
			 * Bump sensors are inverted, active LOW
			 * This should handle setting them to HIGH while tripped
			 */
			if (muxCnt < NUM_BUMP_SENSORS) {
				// Used to avoid repetitive shift operations
				mask = 1 << muxCnt;

				// If non zero, write bit shifted equivalent into the temp state
				newBumpState = (IO_PortsReadPort(SENSOR_PINS_PORT) &
					SENSOR_PINS_BUMP) ? 0x00 : mask;

				// If new is different than old at the bit shifted mask value
				if ((newBumpState ^ oldBumpState) & mask) {
					// Update old value, flip corresponding bit
					oldBumpState ^= mask;

					// Configure the event to be posted
					// bumpStates shows the current state of all bumpers
					// eventStates shows which one was just updated
					PostEvent.EventType = BUMPER;
					EventData.bits.type = oldBumpState;
					EventData.bits.event = mask;
					PostEvent.EventParam = EventData.val;

					// Post it
					PostToMainHSM(PostEvent);
				}
			}

			/*
			 * Read beacon sensor and post events
			 */
			if (muxCnt < NUM_LIGHT_SENSORS) {
				newADVal = AD_ReadADPin(SENSOR_PINS_BEACON);

				// Hysteresis
				// Rising edge
				if ((newADVal > THRESHOLD_BEACON_HIGH) &&
					(oldBeaconVals[muxCnt] < THRESHOLD_BEACON_HIGH)) {
					// BEACON LOST, RISING EDGE
					PostEvent.EventType = BEACON_LOST;
					EventData.val = 1 << muxCnt;
					PostEvent.EventParam = EventData.val;

					// Post it
					dbprintf("Beacon read: %d\r\n", newADVal);
					PostToMainHSM(PostEvent);
				}
				// Falling edge
				else if ((newADVal < THRESHOLD_BEACON_LOW) &&
					(oldBeaconVals[muxCnt] > THRESHOLD_BEACON_LOW)) {
					// BEACON FOUND, FALLING EDGE
					PostEvent.EventType = BEACON_FOUND;
					EventData.val = 1 << muxCnt;
					PostEvent.EventParam = EventData.val;

					// Post it
					dbprintf("Beacon read: %d\r\n", newADVal);
					PostToMainHSM(PostEvent);
				}

				// Update old vals
				oldBeaconVals[muxCnt] = newADVal;
			}

			/*
			 * Read tape sensor, does not post events
			 */
			if (muxCnt < NUM_TAPE_SENSORS) {
				switch (tapeReadType) {
				case LEDS_OFF:
					tapeValsOff[muxCnt] = (int) AD_ReadADPin(SENSOR_PINS_TAPE);
					break;

				case LEDS_ON:
					tapeValsOn[muxCnt] = (int) AD_ReadADPin(SENSOR_PINS_TAPE);
					break;
				}
			}

			/*
			 * Read track wire and post events
			 */
			// Take 3 samples
			if ((muxCnt % 3) == 0) {
				newTrack[muxCnt / 3] = (IO_PortsReadPort(SENSOR_PINS_PORT) & SENSOR_PINS_TRACK) ?
										1 : 0;

				// On 3rd sample, take a best of 3
				if (muxCnt == 6) {
					trackSum = newTrack[0] + newTrack[1] + newTrack[2];

					newTrackState = (trackSum >= 2) ? 1 : 0;

					// if change in state
					if (newTrackState != oldTrackState) {
						if (newTrackState) {
							PostEvent.EventType = TRACK_FOUND;
						} else {
							PostEvent.EventType = TRACK_LOST;
						}

						oldTrackState = newTrackState;

						PostToMainHSM(PostEvent);
					}
				}
			}


			/*
			 * Increment mux, hardcoded for 3-bit mux
			 */;
			muxCnt = (muxCnt + 1) % MAX_MUX_SEL;

			for (i = 0; i < 3; i++) {
				mask = 1 << i;

				if (mask & muxCnt) { // that bit == 1
					// Set that bit, pins must be adjacent, BIT0 lowest pin
					// The pins are on consecutive odd pins, this may need to be adjusted
					IO_PortsClearPortBits(MUX_PINS_PORT, (MUX_PINS_BIT0 >> (2 * i)));
				} else { // that bit == 0
					// Clear that bit, make it viable for non-adjacent pins
					// The pins are on consecutive odd pins, this may need to be adjusted
					IO_PortsSetPortBits(MUX_PINS_PORT, (MUX_PINS_BIT0 >> (2 * i)));
				}
			}

			/*
			 * Switch tape sensor state when muxCnt rolls over, post events
			 */
			if (muxCnt == 0x00) {
				// Change state
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

					// Check for events
					EventData.bits.event = 0x00;
					EventData.bits.type = 0x00;

					//*
					for (i = 0; i < NUM_TAPE_SENSORS; i++) {
						newTapeVal = tapeValsOn[i] - tapeValsOff[i];

						// Hysteresis
						// Rising edge
						if ((newTapeVal > THRESHOLD_TAPE_HIGH) &&
							(oldTapeVals[i] < THRESHOLD_TAPE_HIGH)) {
							// LEAVING TAPE, RISING EDGE
							EventData.bits.event |= 0x01 << i; // Set event flag
							// Leave type flag clear
						}							// Falling edge
						else if ((newTapeVal < THRESHOLD_TAPE_LOW) &&
							(oldTapeVals[i] > THRESHOLD_TAPE_LOW)) {
							// ON TAPE, FALLING EDGE
							EventData.bits.event |= 0x01 << i; // Set event flag
							EventData.bits.type |= 0x01 << i; // Set type flag
						}

						// Update old vals
						oldTapeVals[i] = newTapeVal;
					}

					// Post it, if an event was detected
					if (EventData.bits.event != 0x00) {
						PostEvent.EventType = TAPE;
						PostEvent.EventParam = EventData.val;
						PostToMainHSM(PostEvent);
					}
					//*/
				}


			}

			/*
			 * Set up the next timeout, SERVICE ONLY TRIGGERS ON EVENTS
			 */
			ES_Timer_InitTimer(EVENT_CHECKER_TIMER, SENSORS_POLLING_DELAY);

			/*
			 * Change states, done reading, consume event
			 */
			CurrentState = NOT_READY_TO_READ;
		}
		break;


	}

	if (ThisEvent.EventType == ES_INIT) {
		// Do nothing
	}

	return ReturnEvent;
}

/*******************************************************************************
 * PRIVATE FUNCTIONs                                                           *
 ******************************************************************************/


/*******************************************************************************
 * TEST HARNESS                                                                *
 ******************************************************************************/

#ifdef EVENTCHECKERSERVICE_TEST
#include <xc.h>
#include <stdio.h>
#include <serial.h>
#include <BOARD.h>
#include "BotConfig.h"

void main(void)
{
	ES_Return_t ErrorType;
	BOARD_Init();
	SERIAL_Init();
	// When doing testing, it is useful to annouce just which program
	// is running.

	printf("Starting the Simple Service Test Harness \r\n");
	printf("using the 2nd Generation Events & Services Framework\n\r");

	// Your hardware initialization function calls go here

	// now initialize the Events and Services Framework and start it running
	ErrorType = ES_Initialize();
	if (ErrorType == Success) {

		ErrorType = ES_Run();

	}

	//
	//if we got to here, there was an error
	//

	switch (ErrorType) {
	case FailedPointer:
		printf("Failed on NULL pointer");
		break;
	case FailedInit:
		printf("Failed Initialization");
		break;
	default:
		printf("Other Failure");
		break;
	}

	while (1) {
		;
	}
}

#endif // EventCheckerService_TEST