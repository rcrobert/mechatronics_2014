/*
 * File: ApproachHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is for a substate machine. Make sure it has a unique name
 *
 * This is provided as an example and a good place to start.
 *
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
#include "BOARD.h"
#include "BotConfig.h"
#include "MotorDriver.h"
#include "ApproachHSM.h"
#include "RamSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define LIST_OF_APPROACH_STATES(STATE)  \
        STATE(Approach_Init)            \
        STATE(Approach_Backup)          \
        STATE(Approach_Lower_Arm)       \
        STATE(Approach_Drive)           \
        STATE(Approach_Check_Right)     \
        STATE(Approach_Check_Left)      \
        STATE(Approach_Lifting)         \
        STATE(Approach_Turn180)         \
        STATE(Approach_Align)           \
        STATE(Approach_Face_Out)        \
        STATE(Approach_Done_State)

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd

typedef enum {
	LIST_OF_APPROACH_STATES(ENUM_FORM)
} ApproachState_t;

#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_APPROACH_STATES(STRING_FORM)
};


/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *
 ******************************************************************************/
/* Prototypes for private functions for this machine. They should be functions
   relevant to the behavior of this state machine */

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/
/* You will need MyPriority and the state variable; you may need others as well.
 * The type of state variable should match that of enum in header file. */

static ApproachState_t CurrentState = Approach_Init; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitApproachHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunApproachHSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitApproachHSM(void)
{
	ES_Event returnEvent;

	CurrentState = Approach_Init;
	returnEvent = RunApproachHSM(INIT_EVENT);
	if (returnEvent.EventType == ES_NO_EVENT) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @Function RunApproachHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the heirarchical state
 *        machine, as this is called any time a new event is passed to the event
 *        queue. This function will be called recursively to implement the correct
 *        order for a state transition to be: exit current state -> enter next state
 *        using the ES_EXIT and ES_ENTRY events.
 * @note Remember to rename to something appropriate.
 *       The lower level state machines are run first, to see if the event is dealt
 *       with there rather than at the current level. ES_EXIT and ES_ENTRY events are
 *       not consumed as these need to pass pack to the higher level state machine.
 * @author J. Edward Carryer, 2011.10.23 19:25
 * @author Gabriel H Elkaim, 2011.10.23 19:25 */
ES_Event RunApproachHSM(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	ApproachState_t nextState;
	static ApproachState_t postBackupState;

	EventStorage args;

	ES_Tattle(); // trace call stack

	switch (CurrentState) {
	case Approach_Init:
		if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
		{
			// Init sub HSMs
			InitRamSubHSM();
			
			// Handle initialization of modules
			ES_Timer_StopTimer(APPROACH_HSM_TIMER);
			ES_Timer_StopTimer(STALL_TIMER);

			// Initialize sub HSMs
			InitRamSubHSM();

			// Move to real state
			CurrentState = Approach_Lower_Arm;
			makeTransition = FALSE;
			ThisEvent.EventType = ES_NO_EVENT;
			;
		}
		break;

	case Approach_Backup:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(-MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(APPROACH_HSM_TIMER, TIME_APPROACH_BACKUP);
			break;

		case ES_EXIT:
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == APPROACH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = postBackupState;
				makeTransition = TRUE;

				// Reset postBackupState for debug visibility
				postBackupState = Approach_Done_State;
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;
		}
		break;

	case Approach_Lower_Arm:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Start lift motor
				Drive_LiftDown();
				break;

			case ES_EXIT:
				// Stop lift motor
				Drive_LiftStop();
				break;

			case BUMPER:
				args.val = ThisEvent.EventParam;

				if (args.bits.event & args.bits.type & BUMP_LIMIT) {
					nextState = Approach_Drive;
					makeTransition = TRUE;

					// Consume evet
					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;
			}
		}
		break;

	case Approach_Drive:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Drive
			Drive_Straight(MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(APPROACH_HSM_TIMER, TIME_APPROACH_DRIVE);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == APPROACH_HSM_TIMER) {
				// Took to long, back off and retry
				// Implement this later

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// Check that it was the lifting arm bumper
			if ((args.bits.type & BUMP_CROWN) || (args.bits.type & BUMP_CENTER)) {
				// Found the throne
				nextState = Approach_Lifting;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Approach_Lifting:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Start lift motor raising
			Drive_LiftUp();

			// Set timer
			ES_Timer_InitTimer(APPROACH_HSM_TIMER, TIME_APPROACH_LIFT);
			break;

		case ES_EXIT:
			// Stop lift motor
			Drive_LiftStop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == APPROACH_HSM_TIMER) {
				nextState = Approach_Backup;
				postBackupState = Approach_Turn180;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Approach_Turn180:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin turning 180 CW
			Drive_TankLeft(MOTOR_SPEED_EXPLORE);

			ES_Timer_InitTimer(APPROACH_HSM_TIMER, MOTOR_TURN_EX_180);
			break;

		case ES_EXIT:
			// Do nothing
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == APPROACH_HSM_TIMER) {
				Drive_Stop();

				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Approach_Align;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default:
			break;
		}
		break;

	case Approach_Align:
		ThisEvent = RunRamSubHSM(ThisEvent);

		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Do nothing
			break;

		case ES_EXIT:
			// Clean up
			InitRamSubHSM();
			break;

		case CHILD_DONE:
			nextState = Approach_Backup;
			postBackupState = Approach_Face_Out;
			makeTransition = TRUE;

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default:
			break;
		}
		break;

	case Approach_Face_Out:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Turn 90deg CCW
			Drive_TankLeft(MOTOR_SPEED_EXPLORE);

			ES_Timer_InitTimer(APPROACH_HSM_TIMER, MOTOR_TURN_EX_90);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == APPROACH_HSM_TIMER) {
				Drive_Stop();

				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Approach_Done_State;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			break;

		default:
			break;
		}
		break;

	case Approach_Done_State:
		// Testing state
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Clean up state machine timers before leaving
				ES_Timer_StopTimer(APPROACH_HSM_TIMER);
				ES_Timer_StopTimer(STALL_TIMER);
			break;

		case ES_EXIT:
			// Do nothing
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
		RunApproachHSM(EXIT_EVENT);
		CurrentState = nextState;
		RunApproachHSM(ENTRY_EVENT);
	}

	ES_Tail(); // trace call stack end
	return ThisEvent;
}


/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/


/*******************************************************************************
 * TEST HARNESS                                                                *
 ******************************************************************************/

#ifdef APPROACHHSM_TEST // <-- change this name and define it in your MPLAB-X
//     project to run the test harness
#include <stdio.h>

void main(void)
{
	ES_Return_t ErrorType;
	BOARD_Init();
	// When doing testing, it is useful to annouce just which program
	// is running.

	printf("Starting the Ram HSM Test Harness \r\n");
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

#endif // APPROACHHSM_TEST
