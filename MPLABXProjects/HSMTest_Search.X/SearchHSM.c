/*
 * File: SearchHSM.c
 * Author: J. Edward Carryer
 * Modified: Gabriel Elkaim and Soja-Marie Morgens
 *
 * Template file to set up a Heirarchical State Machine to work with the Events and
 * Services Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that
 * this file will need to be modified to fit your exact needs, and most of the names
 * will have to be changed to match your code.
 *
 * There is another template file for the SubHSM's that is slightly differet, and
 * should be used for all of the subordinate state machines (flat or heirarchical)
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
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
//Include any defines you need to do
/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/


#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_SEARCH_STATES(STRING_FORM)
};

/*******************************************************************************
 * GLOBAL VARIABLES							       *
 ******************************************************************************/

int SearchCount = 0;

/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/

static SearchState_t CurrentState = Search_Init;
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitSearchHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunSearchFSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitSearchHSM(uint8_t Priority)
{
	MyPriority = Priority;
	// put us into the Initial PseudoState
	CurrentState = Search_Init;
	// post the initial transition event
	if (ES_PostToService(MyPriority, INIT_EVENT) == TRUE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
 * @Function PostSearchHSM(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostSearchHSM(ES_Event ThisEvent)
{
	return ES_PostToService(MyPriority, ThisEvent);
}

/**
 * @Function QuerySearchHSM(void)
 * @param none
 * @return Current state of the state machine
 * @brief This function is a wrapper to return the current state of the state
 *        machine. Return will match the ENUM above. Remember to rename to
 *        something appropriate, and also to rename the SearchState_t to your
 *        correct variable name.
 * @author J. Edward Carryer, 2011.10.23 19:25 */
SearchState_t QuerySearchHSM(void)
{
	return (CurrentState);
}

/**
 * @Function RunSearchHSM(ES_Event ThisEvent)
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
ES_Event RunSearchHSM(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	SearchState_t nextState;

	// Default states set up for error visibility
	static SearchState_t postBackupState = Search_Done_State;
	static SearchState_t postObstacleState = Search_Done_State;
	static uint32_t startTime = 0;
	static uint32_t timeRemaining = 0;

	EventStorage args;

	ES_Tattle(); // trace call stack

	switch (CurrentState) {
	case Search_Init: // If current state is initial Pseudo State
		if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
		{
			// Init vars
			postBackupState = Search_Done_State;
			postObstacleState = Search_Done_State;
			timeRemaining = 0;

			// Handle initialization of modules
			// These are handled in main() instead

			// Initialize sub HSMs
			InitRamSubHSM();

			// Move to real state
			nextState = Search_Leave_Room;
			makeTransition = TRUE;
			ThisEvent.EventType = ES_NO_EVENT;
			;
		}
		break;

	case Search_Leave_Room:
		ThisEvent = RunRamSubHSM(ThisEvent);

		if (ThisEvent.EventType != ES_NO_EVENT) { // An event is still active
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Do nothing
				break;

			case ES_EXIT:
				// Reset sub HSM
				InitRamSubHSM();
				break;

			case CHILD_DONE:
				// Done here
				nextState = Search_Backup;
				postBackupState = Search_Turn_First_Wall;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
				break;

			default: // all unhandled events pass the event back up to the next level
				break;
			}
		}
		break;

	case Search_Backup:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				Drive_Straight(-MOTOR_SPEED_CRAWL);

				ES_Timer_InitTimer(SEARCH_HSM_TIMER, TIME_SEARCH_BACKUP);
				break;

			case ES_EXIT:
				Drive_Stop();
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
					Drive_Stop();
					ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

					ThisEvent.EventType = ES_NO_EVENT;
				} else if (ThisEvent.EventParam == STALL_TIMER) {
					nextState = postBackupState;
					makeTransition = TRUE;

					// Reset postBackupState for debug visibility
					postBackupState = Search_Done_State;
					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;
			}
		}
		break;

	case Search_Obstacle:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Allow 'caller' to set the initial behavior
				ES_Timer_InitTimer(SEARCH_HSM_TIMER, TIME_SEARCH_OBSTACLE);
				timeRemaining = timeRemaining - 400;
				break;

			case ES_EXIT:
				Drive_Stop();
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
					nextState = postObstacleState;
					makeTransition = TRUE;

					// Reset postObstacleState for debug visibility
					postObstacleState = Search_Done_State;

					ThisEvent.EventType = ES_NO_EVENT;
				}

			default:
				break;
			}
		}
		break;

	case Search_Turn_First_Wall:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Begin turning CW
				Drive_TankRight(MOTOR_SPEED_EXPLORE);

				ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_EX_90);
				break;

			case ES_EXIT:
				Drive_Stop();

				ES_Timer_StopTimer(SEARCH_HSM_TIMER);
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
					nextState = Search_Bump_First_Wall;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default:
				break;
			}
		}
		break;

	case Search_Bump_First_Wall:
		ThisEvent = RunRamSubHSM(ThisEvent);

		if (ThisEvent.EventType != ES_NO_EVENT) { // An event is still active
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Do nothing
				break;

			case ES_EXIT:
				// Reset sub HSM
				InitRamSubHSM();
				break;

			case CHILD_DONE:
				// Done here
				nextState = Search_Backup;
				postBackupState = Search_Face_Center;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
				break;

			default: // all unhandled events pass the event back up to the next level
				break;
			}
		}
		break;

	case Search_Face_Center:
		if (ThisEvent.EventType != ES_NO_EVENT) { // An event is active
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Begin tank turning CCW
				Drive_TankLeft(MOTOR_SPEED_EXPLORE);

				// Init timer for 90deg turn
				ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_EX_180);
				break;

			case ES_EXIT:
				// Stop motors
				Drive_Stop();
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
					Drive_Stop();
					ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

					ThisEvent.EventType = ES_NO_EVENT;
				} else if (ThisEvent.EventParam == STALL_TIMER) {
					timeRemaining = TIME_SEARCH_TOCENTER;
					nextState = Search_Goto_Center;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default: // all unhandled events pass the event back up to the next level
				break;
			}
		}
		break;

	case Search_Goto_Center: // example of a state without a sub-statemachine
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin driving straight
			Drive_Straight(MOTOR_SPEED_MEDIUM);

			// Init timer to get to center, timeRemaining is set in the previous state
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, timeRemaining);

			// Track starting time
			startTime = ES_Timer_GetTime();
			break;

		case ES_EXIT:
			ES_Timer_StopTimer(SEARCH_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Face_Hall;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

			// Obstacle Handling
		case BUMPER:
			args.val = ThisEvent.EventParam;

			// if left bumper, move to obstacle state and begin turning right
			if (args.bits.type & BUMP_LEFT) {
				postObstacleState = Search_Goto_Center;
				nextState = Search_Obstacle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;

				timeRemaining = TIME_SEARCH_TOCENTER - (ES_Timer_GetTime() - startTime);

				// Begin turning away
				Drive_Right(MOTOR_SPEED_MEDIUM);
			}				// if right bumper
			else if (args.bits.type & BUMP_RIGHT) {
				postObstacleState = Search_Goto_Center;
				nextState = Search_Obstacle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;

				timeRemaining = TIME_SEARCH_TOCENTER - (ES_Timer_GetTime() - startTime);

				// Begin turning away
				Drive_Left(MOTOR_SPEED_MEDIUM);
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			// if left bumper, move to obstacle state and begin turning right
			if (args.bits.type & TAPE_FAR_LEFT) {
				postObstacleState = Search_Goto_Center;
				nextState = Search_Obstacle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;

				// Calculate time left to the middle
				timeRemaining = TIME_SEARCH_TOCENTER - (ES_Timer_GetTime() - startTime);

				// Begin turning away
				Drive_Right(MOTOR_SPEED_MEDIUM);
			}				// if right bumper
			else if (args.bits.type & TAPE_FAR_RIGHT) {
				postObstacleState = Search_Goto_Center;
				nextState = Search_Obstacle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;

				// Calculate time left to the middle
				timeRemaining = TIME_SEARCH_TOCENTER - (ES_Timer_GetTime() - startTime);

				// Begin turning away
				Drive_Left(MOTOR_SPEED_MEDIUM);
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Face_Hall:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin tank turning CCW
			Drive_TankLeft(MOTOR_SPEED_EXPLORE);

			// Init timer for 90deg turn
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_EX_90);
			break;

		case ES_EXIT:
			// Stop motors
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Goto_Hall;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Goto_Hall:
		ThisEvent = RunRamSubHSM(ThisEvent);

		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Do nothing
			break;

		case ES_EXIT:
			// Reset sub HSM
			InitRamSubHSM();
			break;

		case CHILD_DONE:
			// Done here
			nextState = Search_Backup;
			postBackupState = Search_Face_Door;
			makeTransition = TRUE;

			// Consume
			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Face_Door:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin tank turning CW
			Drive_TankRight(MOTOR_SPEED_EXPLORE);

			// Init timer for 90deg turn
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_EX_90);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Enter_Castle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Enter_Castle:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin driving
			Drive_Straight(MOTOR_SPEED_MEDIUM);

			// Init timer to enter
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, TIME_SEARCH_ENTER);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Check_Beacon;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Check_Beacon:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin tank turning CW
			Drive_TankRight(MOTOR_SPEED_CRAWL);

			// Init timer to enter
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_CR_90);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();

			ES_Timer_StopTimer(SEARCH_HSM_TIMER);
			break;

		case BEACON_FOUND:
			// Check that it was the front beacon
			args.val = ThisEvent.EventParam;

			if (args.val & BEACON_FRONT) {
				// Cancel timer
				ES_Timer_StopTimer(SEARCH_HSM_TIMER);
				Drive_Stop();

				// Done here
				nextState = Search_Done_State;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case ES_TIMEOUT:if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Turn_Around;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Search_Turn_Around:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Increment global count
			++SearchCount;

			// Begin tank turning CW
			Drive_TankRight(MOTOR_SPEED_EXPLORE);

			// Init timer for 90deg turn
			ES_Timer_InitTimer(SEARCH_HSM_TIMER, MOTOR_TURN_EX_90);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == SEARCH_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Search_Leave_Room;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

		// TEMPORARY FOR TESTING
	case Search_Done_State:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Stop
			Drive_Stop();
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
		RunSearchHSM(EXIT_EVENT);
		CurrentState = nextState;
		RunSearchHSM(ENTRY_EVENT);
	}

	ES_Tail(); // trace call stack end
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
/* Define SEARCHFSM_TEST to run this file as your main file (without the rest
 * of the framework)-useful for debugging */
#ifdef SEARCHHSM_TEST

#include <stdio.h>

void main(void)
{
	ES_Return_t ErrorType;
	BOARD_Init();
	// When doing testing, it is useful to annouce just which program
	// is running.

	printf("Starting the Search HSM Test Harness \r\n");
	printf("using the 2nd Generation Events & Services Framework\n\r");

	// Your hardware initialization function calls go here
	Bot_Init();
	Drive_Init();

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

#endif // SEARCHHSM_TEST
