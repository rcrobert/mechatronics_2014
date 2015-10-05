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
#include "TopHSM.h"
#include "ExitHSM.h"
#include "SearchHSM.h"
#include "ApproachHSM.h"
#include "ReturnHSM.h"
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
	LIST_OF_TOP_STATES(STRING_FORM)
};

/*******************************************************************************
 * GLOBAL VARIABLES							       *
 ******************************************************************************/


/*******************************************************************************
 * PRIVATE MODULE VARIABLES                                                            *
 ******************************************************************************/

static TopState_t CurrentState = Top_Init;
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
uint8_t InitTopHSM(uint8_t Priority)
{
	MyPriority = Priority;
	// put us into the Initial PseudoState
	CurrentState = Top_Init;
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
uint8_t PostTopHSM(ES_Event ThisEvent)
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
TopState_t QueryTopHSM(void)
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
ES_Event RunTopHSM(ES_Event ThisEvent)
{
	uint8_t makeTransition = FALSE; // use to flag transition
	TopState_t nextState;

	ES_Tattle(); // trace call stack

	switch (CurrentState) {
	case Top_Init: // If current state is initial Pseudo State
		if (ThisEvent.EventType == ES_INIT)// only respond to ES_Init
		{
			// Initialize sub HSMs
			InitExitHSM();
			InitSearchHSM();
			InitApproachHSM();
			InitReturnHSM();

			// Move to real state
			nextState = Top_Exit;
			makeTransition = TRUE;
			ThisEvent.EventType = ES_NO_EVENT;
			;
		}
		break;

	case Top_Exit:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			ThisEvent = RunExitHSM(ThisEvent);

			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				break;

			case ES_EXIT:
				break;

			case CHILD_DONE:
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == STALL_TIMER) {
					nextState = Top_Search;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default:
				break;
			}
		}
		break;

	case Top_Search:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			ThisEvent = RunSearchHSM(ThisEvent);

			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				break;

			case ES_EXIT:
				break;

			case CHILD_DONE:
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == STALL_TIMER) {
					nextState = Top_Approach;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default:
				break;
			}
		}
		break;

	case Top_Approach:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			ThisEvent = RunApproachHSM(ThisEvent);
			
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				break;

			case ES_EXIT:
				break;

			case CHILD_DONE:
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == STALL_TIMER) {
					nextState = Top_Return;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default:
				break;
			}
		}
		break;

	case Top_Return:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			ThisEvent = RunReturnHSM(ThisEvent);
			
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				// Do nothing
				break;

			case ES_EXIT:
				break;

			case CHILD_DONE:
				ES_Timer_InitTimer(STALL_TIMER, STALL_TIME_IN_MS);
				break;

			case ES_TIMEOUT:
				if (ThisEvent.EventParam == STALL_TIMER) {
					nextState = Top_Party;
					makeTransition = TRUE;

					ThisEvent.EventType = ES_NO_EVENT;
				}
				break;

			default:
				break;
			}
		}
		break;

	case Top_Party:
		if (ThisEvent.EventType != ES_NO_EVENT) {
			switch (ThisEvent.EventType) {
			case ES_ENTRY:
				break;

			case ES_EXIT:
				break;

			case CHILD_DONE:
				// All done
				
				ThisEvent.EventType = ES_NO_EVENT;
				break;

			default:
				break;
			}
		}
		break;

	default: // all unhandled states fall into here
		break;
	} // end switch on Current State

	if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
		// recursively call the current state with an exit event
		RunTopHSM(EXIT_EVENT);
		CurrentState = nextState;
		RunTopHSM(ENTRY_EVENT);
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
#ifdef TOPHSM_TEST

#include <stdio.h>

void main(void)
{
	ES_Return_t ErrorType;
	BOARD_Init();
	// When doing testing, it is useful to annouce just which program
	// is running.

	printf("Starting the Hierarchical State Machine Test Harness \r\n");
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

#endif // TOPHSM_TEST
