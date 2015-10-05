/*
 * File: ReturnHSM.c
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
#include "ReturnHSM.h"
#include "SearchHSM.h"
#include "RamSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define LIST_OF_RETURN_STATES(STATE)    \
        STATE(Return_Init)              \
        STATE(Return_Leave_Room)        \
		STATE(Return_Ram_Leave_Room)	\
        STATE(Return_Backup)            \
        STATE(Return_Obstacle)          \
        STATE(Return_Turn_First_Wall)   \
        STATE(Return_Bump_First_Wall)   \
        STATE(Return_Face_Center)       \
        STATE(Return_Goto_Center)       \
        STATE(Return_Face_Hall)         \
        STATE(Return_Goto_Hall)         \
		STATE(Return_Ram_Hall)			\
        STATE(Return_Face_Door)         \
        STATE(Return_Enter_Castle)      \
		STATE(Return_Ram_Castle)		\
		STATE(Return_Backup_Throne)		\
        STATE(Return_Face_Throne)		\
        STATE(Return_Goto_Throne)       \
		STATE(Return_Place_Crown)		\
		STATE(Return_Wiggly)			\
		STATE(Return_Recovery)			\
        STATE(Return_Done_State)

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd

typedef enum {
	LIST_OF_RETURN_STATES(ENUM_FORM)
} ReturnState_t;

#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
	LIST_OF_RETURN_STATES(STRING_FORM)
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

static ReturnState_t CurrentState = Return_Init; // <- change name to match ENUM
static uint8_t MyPriority;


/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function InitReturnHSM(uint8_t Priority)
 * @param Priority - internal variable to track which event queue to use
 * @return TRUE or FALSE
 * @brief This will get called by the framework at the beginning of the code
 *        execution. It will post an ES_INIT event to the appropriate event
 *        queue, which will be handled inside RunReturnHSM function. Remember
 *        to rename this to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t InitReturnHSM(void)
{
	ES_Event returnEvent;

	CurrentState = Return_Init;
	returnEvent = RunReturnHSM(INIT_EVENT);
	if (returnEvent.EventType == ES_NO_EVENT) {
		return TRUE;
	}
	return FALSE;
}

/**
 * @Function RunReturnHSM(ES_Event ThisEvent)
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
ES_Event RunReturnHSM(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	ReturnState_t nextState;

	static ReturnState_t postBackupState = Return_Done_State;
	static ReturnState_t postObstacleState = Return_Done_State;

	static uint8_t wiggleDir = 0x00;

	EventStorage args;

	ES_Tattle(); // trace call stack

	switch (CurrentState) {
	case Return_Init:
		if (ThisEvent.EventType == ES_INIT) {
			// Init sub HSMs
			InitRamSubHSM();

			// Timers should not be running when we start the SM
			ES_Timer_StopTimer(RETURN_HSM_TIMER);
			ES_Timer_StopTimer(STALL_TIMER);
			ES_Timer_StopTimer(RAM_SUB_HSM_TIMER);

			// Reset counter
			postBackupState = Return_Done_State;
			postObstacleState = Return_Done_State;

			// now put the machine into the actual initial state
			CurrentState = Return_Leave_Room;
			makeTransition = FALSE;
			ThisEvent.EventType = ES_NO_EVENT;
		}
		break;

	case Return_Leave_Room:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(MOTOR_SPEED_CRAWL);
			break;

		case ES_EXIT:
			// Do nothing
			ES_Timer_StopTimer(EVADE_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == EVADE_TIMER) {
				Drive_Straight(MOTOR_SPEED_CRAWL);
			}
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// if center, done with state, back up and use ram to align to the wall
			// else veer away from obstacles
			if (args.bits.event & args.bits.type & BUMP_CENTER) {
				nextState = Return_Backup;
				postBackupState = Return_Ram_Leave_Room;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_LEFT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankRight(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_RIGHT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankLeft(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			// if we hit any tape here, reverse and ram to deal with things
			if (args.bits.type) {
				nextState = Return_Backup;
				postBackupState = Return_Ram_Leave_Room;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Ram_Leave_Room:
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
			nextState = Return_Backup;
			postBackupState = Return_Turn_First_Wall;
			makeTransition = TRUE;

			// Consume
			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Backup:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(-MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_SEARCH_BACKUP);
			break;

		case ES_EXIT:
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = postBackupState;
				makeTransition = TRUE;

				// Reset postBackupState for debug visibility
				postBackupState = Return_Done_State;
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;
		}
		break;

	case Return_Obstacle:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Allow 'caller' to set the initial behavior
			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_SEARCH_OBSTACLE);
			break;

		case ES_EXIT:
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				nextState = postObstacleState;
				makeTransition = TRUE;

				// Reset postObstacleState for debug visibility
				postObstacleState = Return_Done_State;

				ThisEvent.EventType = ES_NO_EVENT;
			}

		default:
			break;
		}
		break;

	case Return_Turn_First_Wall:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin turning CW
			Drive_TankRight(MOTOR_SPEED_EXPLORE);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_90);
			break;

		case ES_EXIT:
			Drive_Stop();

			ES_Timer_StopTimer(RETURN_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				nextState = Return_Bump_First_Wall;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default:
			break;
		}
		break;

	case Return_Bump_First_Wall:
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
			nextState = Return_Backup;
			postBackupState = Return_Face_Center;
			makeTransition = TRUE;

			// Consume
			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Face_Center:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin tank turning CCW
			Drive_TankLeft(MOTOR_SPEED_EXPLORE);

			// Init timer for 90deg turn
			ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_180);
			break;

		case ES_EXIT:
			// Stop motors
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Goto_Center;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Goto_Center:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin driving straight
			Drive_Straight(MOTOR_SPEED_MEDIUM);

			// Init timer to get to center, timeRemaining is set in the previous state
			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_SEARCH_TOCENTER);
			break;

		case ES_EXIT:
			ES_Timer_StopTimer(RETURN_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Face_Hall;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;

			if (args.bits.type & TAPE_FAR_RIGHT) {
				Drive_Left(MOTOR_SPEED_MEDIUM);

				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);
			} else if (args.bits.type & TAPE_FAR_LEFT) {
				Drive_Right(MOTOR_SPEED_MEDIUM);

				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Face_Hall:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Decide here which castle to return to
			if (SearchCount == 0) {
				Drive_TankRight(MOTOR_SPEED_EXPLORE);

				ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_90 + 35);
			} else if (SearchCount == 1) {
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
			} else if (SearchCount == 2) {
				Drive_TankLeft(MOTOR_SPEED_EXPLORE);

				ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_90);
			}
			break;

		case ES_EXIT:
			// Stop motors
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Goto_Hall;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Goto_Hall:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(MOTOR_SPEED_CRAWL);
			break;

		case ES_EXIT:
			// Do nothing
			ES_Timer_StopTimer(EVADE_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == EVADE_TIMER) {
				Drive_Straight(MOTOR_SPEED_CRAWL);
			}
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// if center, done with state, back up and use ram to align to the wall
			// else veer away from obstacles
			if (args.bits.event & args.bits.type & BUMP_CENTER) {
				nextState = Return_Backup;
				postBackupState = Return_Ram_Hall;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_LEFT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankRight(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_RIGHT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankLeft(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		case TAPE:
			args.val = ThisEvent.EventParam;


			if (args.bits.event & args.bits.type & TAPE_FAR_LEFT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_Right(MOTOR_SPEED_MEDIUM);
			} else if (args.bits.event & args.bits.type & TAPE_FAR_RIGHT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_Left(MOTOR_SPEED_MEDIUM);
			}
			break;

		default:
			break;
		}
		break;

	case Return_Ram_Hall:
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
			nextState = Return_Backup;
			postBackupState = Return_Face_Door;
			makeTransition = TRUE;

			// Consume
			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Face_Door:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Begin tank turning CW
			Drive_TankRight(MOTOR_SPEED_EXPLORE);

			// Init timer for 90deg turn
			ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_90 + 35);
			break;

		case ES_EXIT:
			// Stop
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);

				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Enter_Castle;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Enter_Castle:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(MOTOR_SPEED_CRAWL);
			break;

		case ES_EXIT:
			ES_Timer_StopTimer(EVADE_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == EVADE_TIMER) {
				Drive_Straight(MOTOR_SPEED_CRAWL);
			}
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// if center, done with state, back up and use ram to align to the wall
			// else veer away from obstacles
			if (args.bits.event & args.bits.type & BUMP_CENTER) {
				nextState = Return_Backup;
				postBackupState = Return_Ram_Castle;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_LEFT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankRight(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (args.bits.event & args.bits.type & BUMP_RIGHT) {
				ES_Timer_InitTimer(EVADE_TIMER, TIME_SEARCH_OBSTACLE);

				Drive_TankLeft(MOTOR_SPEED_CRAWL);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default:
			break;
		}
		break;

	case Return_Ram_Castle:
		// Block tape events
		if (ThisEvent.EventType == TAPE)
			ThisEvent.EventType = ES_NO_EVENT;

		ThisEvent = RunRamSubHSM(ThisEvent);

		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Do nothing;
			break;

		case ES_EXIT:
			// Do nothing
			InitRamSubHSM();
			break;

		case CHILD_DONE:
			nextState = Return_Backup_Throne;
			makeTransition = TRUE;

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default: // all unhandled events pass the event back up to the next level
			break;
		}
		break;

	case Return_Backup_Throne:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(-MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_RETURN_CROWN_BACKUP);
			break;

		case ES_EXIT:
			// Do nothing
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();

				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Face_Throne;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;
		}
		break;

	case Return_Face_Throne:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_TankRight(MOTOR_SPEED_EXPLORE);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, MOTOR_TURN_EX_90 + 25);
			break;

		case ES_EXIT:
			// Do nothing
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_Stop();

				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
			} else if (ThisEvent.EventParam == STALL_TIMER) {
				nextState = Return_Goto_Throne;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;
		}
		break;

	case Return_Goto_Throne:
		// Ignore tape events here
		if (ThisEvent.EventType == TAPE) {
			ThisEvent.EventType = ES_NO_EVENT;
		}

		ThisEvent = RunRamSubHSM(ThisEvent);

		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			// Do nothing
			break;

		case ES_EXIT:
			InitRamSubHSM();
			break;

		case CHILD_DONE:
			nextState = Return_Place_Crown;
			makeTransition = TRUE;

			ThisEvent.EventType = ES_NO_EVENT;
			break;

		default:
			break;
		}
		break;

	case Return_Place_Crown:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(-MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_RETURN_MINIBACK);
			break;

		case ES_EXIT:
			ES_Timer_StopTimer(RETURN_HSM_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				// Done correction, lower
				Drive_Stop();
				Drive_LiftDown();
			}
			break;

		case BUMPER:
			args.val = ThisEvent.EventParam;

			// Done lowering crown
			if (args.bits.event & args.bits.type & BUMP_LIMIT) {
				Drive_LiftStop();
				nextState = Return_Recovery;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;
		}
		break;

	case Return_Wiggly:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			ES_Timer_InitTimer(STALL_TIMER, 250);
			ES_Timer_InitTimer(RETURN_HSM_TIMER, 3000);
			break;

		case ES_EXIT:
			Drive_Stop();
			ES_Timer_StopTimer(STALL_TIMER);
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventType == RETURN_HSM_TIMER) {
				// next state
				nextState = Return_Recovery;
				makeTransition = TRUE;

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			} else if (ThisEvent.EventType == STALL_TIMER) {
				if (wiggleDir) {
					Drive_TankRight(MOTOR_SPEED_CRAWL);
				} else {
					Drive_TankLeft(MOTOR_SPEED_CRAWL);
				}

				wiggleDir = (wiggleDir) ? 0 : 1;

				ES_Timer_InitTimer(STALL_TIMER, 250);

				// Consume
				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default:
			break;
		}
		break;

	case Return_Recovery:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_Straight(-MOTOR_SPEED_CRAWL);

			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_RETURN_RECOVERY);
			break;

		case ES_EXIT:
			Drive_Stop();
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				nextState = Return_Done_State;
				makeTransition = TRUE;

				ThisEvent.EventType = ES_NO_EVENT;
			}
			break;

		default:
			break;
		}
		break;

	case Return_Done_State:
		switch (ThisEvent.EventType) {
		case ES_ENTRY:
			Drive_LiftUp();

			ES_Timer_InitTimer(RETURN_HSM_TIMER, TIME_APPROACH_LIFT);
			break;

		case ES_EXIT:
			// Do nothing
			break;

		case ES_TIMEOUT:
			if (ThisEvent.EventParam == RETURN_HSM_TIMER) {
				Drive_LiftStop();

				ThisEvent.EventType = CHILD_DONE;
			}
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
		RunReturnHSM(EXIT_EVENT);
		CurrentState = nextState;
		RunReturnHSM(ENTRY_EVENT);
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

#ifdef RETURNHSM_TEST // <-- change this name and define it in your MPLAB-X
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

#endif // RETURNHSM_TEST
