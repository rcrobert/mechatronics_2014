/*
 * File: RamSubHSM.c
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
#include "SearchHSM.h"
#include "RamSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define LIST_OF_RAM_STATES(STATE) \
        STATE(Ram_Init)		\
        STATE(Ram_Drive)	\
		STATE(Ram_Straighten)	\
        STATE(Ram_Align_Left)	\
        STATE(Ram_Align_Right)	\
		STATE(Ram_Tape_Align_Left)	\
		STATE(Ram_Tape_Align_Right)	\
		STATE(Ram_Backup)	\
		STATE(Ram_Done)

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd

typedef enum {
	LIST_OF_RAM_STATES(ENUM_FORM)
} RamState_t;

#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_RAM_STATES(STRING_FORM)
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

static RamState_t CurrentState = Ram_Init; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitRamSubHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunRamSubHSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitRamSubHSM(void)
{
	ES_Event returnEvent;

	CurrentState = Ram_Init;
	returnEvent = RunRamSubHSM(INIT_EVENT);
	if (returnEvent.EventType == ES_NO_EVENT) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @Function RunRamSubHSM(ES_Event ThisEvent)
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
ES_Event RunRamSubHSM(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	RamState_t nextState;

	static uint8_t bounceCount = 0; // make sure we dont get stuck in align states

	EventStorage args;

	ES_Tattle(); // trace call stack

	switch (CurrentState) {
	case Ram_Init:
		if (ThisEvent.EventType == ES_INIT) {
			// Timer should not be running when we start the SM
			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);

			// Reset counter
			bounceCount = 0;

			// now put the machine into the actual initial state
			CurrentState = Ram_Drive;
			makeTransition = FALSE;
			ThisEvent.EventType = ES_NO_EVENT;
		}
		break;

	case Ram_Drive:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Start driving
			Drive_Straight(MOTOR_SPEED_CRAWL);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case BUMPER:
			// Check which bumper triggered
			args.val = ThisEvent.EventParam;

			// if center tripped
			if (args.bits.event & args.bits.type & BUMP_CENTER) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;

			}				// else if left tripped
			else if (args.bits.event & args.bits.type & BUMP_LEFT) {
				nextState = Ram_Align_Left;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}				// else if right tripped
			else if (args.bits.event & args.bits.type & BUMP_RIGHT) {
				nextState = Ram_Align_Right;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			// Check which tape sensor triggered
			args.val = ThisEvent.EventParam;

			// if left tape and not right
			if ((args.bits.type & TAPE_FRONT_LEFT) && (~args.bits.type & TAPE_FRONT_RIGHT)) {
				// Tape_Align_Left
				nextState = Ram_Tape_Align_Left;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			// if right tape and not left
			else if ((args.bits.type & TAPE_FRONT_RIGHT) && (~args.bits.type & TAPE_FRONT_LEFT)) {
				// Tape_Align_Right
				nextState = Ram_Tape_Align_Right;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			// if right and left
			else if ((args.bits.type & TAPE_FRONT_RIGHT) && (args.bits.type & TAPE_FRONT_LEFT)) {
				// Straighten
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Straighten:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(RAM_SUB_HSM_TIMER, TIME_RAM_STRAIGHTEN);
			break;

		case ES_EXIT:
			Drive_Stop();

			// Stop timer on exit
			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// if left bumper and NOT right bumper
			if ((args.bits.type & BUMP_LEFT) && (~args.bits.type & BUMP_RIGHT)) {
				nextState = Ram_Align_Left;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}				// else if right bumper and NOT left bumper
			else if ((args.bits.type & BUMP_RIGHT) && (~args.bits.type & BUMP_LEFT)) {
				nextState = Ram_Align_Right;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			// else if its right AND left or center it will time out and move to backup
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			// if left tape and not right
			if ((args.bits.type & TAPE_FRONT_LEFT) && (~args.bits.type & TAPE_FRONT_RIGHT)) {
				// Tape_Align_Left
				nextState = Ram_Done;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			// if right tape and not left
			else if ((args.bits.type & TAPE_FRONT_RIGHT) && (~args.bits.type & TAPE_FRONT_LEFT)) {
				// Tape_Align_Right
				nextState = Ram_Done;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			// if center or left AND right
			else if ((args.bits.type & TAPE_CENTER) || ( (args.bits.type & TAPE_FRONT_LEFT) &&
				(args.bits.type & TAPE_FRONT_RIGHT) )) {
				nextState = Ram_Done;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RAM_SUB_HSM_TIMER) {
				nextState = Ram_Done;
				makeTransition = TRUE;

				// Finished
				ThisEvent.EventType = CHILD_DONE;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Align_Left:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Right(-MOTOR_SPEED_MEDIUM);

			ES_Timer_InitTimer(RAM_SUB_HSM_TIMER, TIME_RAM_ALIGN);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();

			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RAM_SUB_HSM_TIMER) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Align_Right:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Left(-MOTOR_SPEED_MEDIUM);

			ES_Timer_InitTimer(RAM_SUB_HSM_TIMER, TIME_RAM_ALIGN);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();

			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RAM_SUB_HSM_TIMER) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Tape_Align_Left:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Left(MOTOR_SPEED_CRAWL);

			// Timeout limit
			ES_Timer_InitTimer(RAM_SUB_HSM_TIMER, TIME_RAM_TAPE);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();

			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RAM_SUB_HSM_TIMER) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			// if right tape, done turning
			if (args.bits.type & TAPE_FRONT_RIGHT) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Tape_Align_Right:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Right(MOTOR_SPEED_CRAWL);

			// Timeout limit
			ES_Timer_InitTimer(RAM_SUB_HSM_TIMER, TIME_RAM_TAPE);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();

			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RAM_SUB_HSM_TIMER) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			// if right tape, done turning
			if (args.bits.type & TAPE_FRONT_LEFT) {
				nextState = Ram_Straighten;
				makeTransition = TRUE;

				ThisEvent.EventType = CHILD_DONE;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	// UNUSED RIGHT NOW POSSIBLE BUG FIXING STATE
	case Ram_Backup:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			break;

		case ES_EXIT:
			break;

		case ES_TIMEOUT:
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Ram_Done:
		// Dummy state, ignore all events here and return complete
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			break;

		case ES_EXIT:
			break;

		default: // all unhandled events pass the event back up to the next level
			// Do nothing with events, we are finished
			ThisEvent.EventType = CHILD_DONE;
			break;
		}
		break;

	default: // all unhandled states fall into here
		break;
	} // end switch on Current State

	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		RunRamSubHSM(EXIT_EVENT); // <- rename to your own Run function
		CurrentState = nextState;
		RunRamSubHSM(ENTRY_EVENT); // <- rename to your own Run function
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

#ifdef RAMSUBHSM_TEST // <-- change this name and define it in your MPLAB-X
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

#endif // RAMSUBHSM_TEST
