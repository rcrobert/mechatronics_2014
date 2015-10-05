/*
 * File: SensorTestFSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Flat State Machine to work with the Events and Services
 * Frameword (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 *Generally you will just be modifying the statenames and the run function
 *However make sure you do a find and replace to convert every instance of
 *  "Template" to your current state machine's name
 * History
 * When           Who     What/Why
 * -------------- ---     --------
 * 09/13/13 15:17 ghe      added tattletail functionality and recursive calls
 * 01/15/12 11:12 jec      revisions for Gen2 framework
 * 11/07/11 11:26 jec      made the queue static
 * 10/30/11 17:59 jec      fixed references to CurrentEvent in RunTemplateSM()
 * 10/23/11 18:20 jec      began conversion from SMTemplate.c (02/20/07 rev)
 */


/*******************************************************************************
 * MODULE #INCLUDE                                                             *
 ******************************************************************************/

#include "ES_Configure.h"
#include "ES_Framework.h"

#include "SensorTestFSM.h"
#include <BOARD.h>
#include <stdio.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/

#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_STATES(STRING_FORM)
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine.
 Example: char RunAway(uint_8 seconds);*/

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static SensorTestState_t CurrentState = SensorTest_InitState;
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitSensorTest(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunSensorTest function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitSensorTest(uint8_t Priority)
{
	MyPriority = Priority;

	// put us into the Initial PseudoState
	CurrentState = SensorTest_InitState;

	// post the initial transition event
	if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * @Function PostSensorTest(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostSensorTest(ES_Event ThisEvent)
{
	return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function QuerySensorTest(void)
 * @param none
 * @return Current state of the state machine
 * @brief This function is a wrapper to return the current state of the state
 *        machine. Return will match the ENUM above. Remember to rename to
 *        something appropriate, and also to rename the TemplateState_t to your
 *        correct variable name.
 * @author J. Edward Carryer, 2011.10.23 19:25 */
SensorTestState_t QuerySensorTest(void)
{
	return (CurrentState);
}

/**
 * @Function RunSensorTest(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the flat state machine,
 *        as this is called any time a new event is passed to the event queue. This
 *        function will be called recursively to implement the correct order for a
 *        state transition to be: exit current state -> enter next state using the
 *        ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed."
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunSensorTest(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	SensorTestState_t nextState;

	//ES_Tattle(); // trace call stack

	int i;
	EventStorage args;

	switch (CurrentState) {
	case SensorTest_InitState: // If current state is initial Psedudo State
		if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
		{
			// Transition actions before first state
			// EventChecker should be initialized by the framework

			// Go to first state
			nextState = SensorTest_OnlyState;
			makeTransition = TRUE;
			ThisEvent.EventType = ES_NO_EVENT;
		}
		break;

	case SensorTest_OnlyState:

		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Do nothing
			break;

		case ES_EXIT:
			// Should never be called
			printf("ERROR - Exiting SensorTest FSM\r\n");
			break;

		case BUMPER:
			args = (EventStorage) ThisEvent.EventParam;

			//	    printf("Testing data type:\r\n");
			//	    printf("bits: %d %d\r\n", args.bits.event, args.bits.type);
			//	    printf("val: %d\r\n", args.val);

			for (i = 0; i < NUM_BUMP_SENSORS; i++) {
				if (args.bits.event & (1 << i)) {
					//  Event occurred
					if (args.bits.type & (1 << i)) {
						// Bumper pressed
						printf("Bumper %d event: pressed\r\n", i);
					} else {
						// Bumped released
						//printf(" released\r\n");
					}
				}
			}

			ThisEvent.EventType = ES_NO_EVENT;

			break;

		case BEACON_FOUND:
			args.val = ThisEvent.EventParam;

			if (args.val == BEACON_FRONT)
				printf("Front beacon sensor event: FOUND\r\n");

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		case BEACON_LOST:
			args.val = ThisEvent.EventParam;

			if (args.val == BEACON_FRONT)
				printf("Front beacon sensor event: LOST\r\n");

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			for (i = 0; i < NUM_TAPE_SENSORS; i++) {
				if (args.bits.event & (1 << i)) {
					printf("Tape sensor %d event:", i);
					if (args.bits.type & (1 << i)) {
						// ON TAPE
						printf(" FOUND\r\n");
					} else {
						// OFF TAPE
						printf(" LOST\r\n");
					}
				}
			}

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		case TRACK_FOUND:
			printf("Track wire found\r\n");
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	default: // all unhandled states fall into here
		break;
	} // end switch on Current State
	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		//PrintLoc();
		// printf("current event at state transition: %s\r\n",EventNames[ThisEvent.EventType]);
		RunSensorTest(EXIT_EVENT);
		// PrintLoc();
		CurrentState = nextState;
		RunSensorTest(ENTRY_EVENT);
		// PrintLoc();
	}
	//ES_Tail(); // trace call stack end
	return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
/*Here's where you put the actual content of your functions.
Example: 
 * char RunAway(uint_8 seconds) {
 * Lots of code here
 * } */

/*******************************************************************************
 * TEST HARNESS                                                                *
 ******************************************************************************/
/* Define SensorTest_TEST to run this file as your main file (without the rest
 * of the framework)-useful for debugging */
#ifdef SENSORTESTFSM_TEST
#include <xc.h>
#include <serial.h>
#include "BotConfig.h"

void main(void)
{
	ES_Return_t ErrorType;
	BOARD_Init();
	// When doing testing, it is useful to annouce just which program
	// is running.

	printf("Starting the Flat State Machine Test Harness \r\n");
	printf("using the 2nd Generation Events & Services Framework\n\r");

	// Your hardware initialization function calls go here
	Bot_Init();
	Drive_Init();
	Drive_LiftStop();

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

#endif // SensorTest_TEST