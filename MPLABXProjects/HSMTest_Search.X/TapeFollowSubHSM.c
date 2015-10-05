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
#include "TapeFollowSubHSM.h"

/*******************************************************************************
 * MODULE #DEFINES                                                             *
 ******************************************************************************/
#define LIST_OF_TAPE_STATES(STATE)	\
	STATE(Tape_Init)		\
        STATE(Tape_Searching)		\
	STATE(Tape_Reversing)		\
	STATE(Tape_Turning)		\
        STATE(Tape_Follow_Front)	\
        STATE(Tape_Follow_Back)		\
        STATE(Tape_Done_State)

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd
typedef enum {
    LIST_OF_TAPE_STATES(ENUM_FORM)
} TapeState_t;

#define STRING_FORM(STATE) #STATE, //Strings are stringified and comma'd
static const char *StateNames[] = {
    LIST_OF_TAPE_STATES(STRING_FORM)
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

static TapeState_t CurrentState = Tape_Init;   // <- change name to match ENUM
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
uint8_t InitTapeSubHSM(void)
{
     ES_Event returnEvent;

    CurrentState = Tape_Init;
    returnEvent = RunTapeSubHSM(INIT_EVENT);
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
ES_Event RunTapeSubHSM(ES_Event ThisEvent)
{
    uint8_t makeTransition = FALSE; // use to flag transition
    TapeState_t nextState;

    EventStorage args;

    ES_Tattle(); // trace call stack

    switch (CurrentState) {
    case Tape_Init:
        if (ThisEvent.EventType == ES_INIT)
        {
	    // Make sure timer isnt going to be running
	    ES_Timer_StopTimer(TAPE_SUB_HSM_TIMER);

            // now put the machine into the actual initial state
            CurrentState = Ram_Begin;
            makeTransition = FALSE;
            ThisEvent.EventType = ES_NO_EVENT;
        }
        break;

    case Tape_Searching:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
		// Drive forward
		Drive_Forward(MOTOR_SPEED_CRAWL);

		// Set up frustration timer
		ES_Timer_InitTimer(TAPE_SUB_HSM_TIMER, TAPE_FRUSTRATION_TIMER);
                break;

            case ES_EXIT:
		// Stop
		Drive_Stop();

		// Stop timer
		ES_Timer_StopTimer(TAPE_SUB_HSM_TIMER);
                break;

	    case ES_TIMEOUT:
		if (ThisEvent.EventParam == TAPE_SUB_HSM_TIMER) {
		    nextState = Tape_Reversing;
		    makeTransition = TRUE;

		    // Consume
		    ThisEvent.EventType = ES_NO_EVENT;
		}
		break;

	    case TAPE:
		args.val = ThisEvent.EventParam;

		// Cancel timer for now, tape has been seen
		ES_Timer_StopTimer(TAPE_SUB_HSM_TIMER);

		if (args.bits.event & args.bits.type & TAPE_CENTER) {
		    // If center connects, begin to follow the tape
		    nextState = Tape_Follow_Front;
		    makeTransition = TRUE;
		} else if ((args.bits.event & args.bits.type & TAPE_FRONT_LEFT) &&
			    (~args.bits.type & TAPE_FRONT_RIGHT)) {
		    // If front left entered tape and right is off tape
		    Drive_Left(MOTOR_SPEED_CRAWL);
		} else if ((args.bits.event & args.bits.type & TAPE_FRONT_RIGHT) &&
			    (~args.bits.type & TAPE_FRONT_LEFT)) {
		    // If front right entered tape and left is off tape
		    Drive_Right(MOTOR_SPEED_CRAWL);
		} else if (~args.bits.type & (TAPE_FRONT_RIGHT | TAPE_FRONT_LEFT)) {
		    // If front right and left are both off tape
		    Drive_Forward(MOTOR_SPEED_CRAWL);
		}
		break;

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    case Tape_Reversing:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
		// Start reversing
		Drive_Forward(-MOTOR_SPEED_CRAWL);

		// Set timer
		ES_Timer_InitTimer(TAPE_SUB_HSM_TIMER, 1000);
                break;

            case ES_EXIT:
		// Stop
		Drive_Stop();
                break;

	    case ES_TIMEOUT:
		if (ThisEvent.EventParam == TAPE_SUB_HSM_TIMER) {
		    nextState = Tape_Turning;
		    makeTransition = TRUE;

		    // Consume
		    ThisEvent.EventType = ES_NO_EVENT;
		}
		break;

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    case Tape_Turning:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
		// Start turning CW 45
		Drive_TankRight(MOTOR_SPEED_EXPLORE);

		// Set timer
		ES_Timer_InitTimer(TAPE_SUB_HSM_TIMER, MOTOR_TURN_EX_45);
                break;

            case ES_EXIT:
		// Stop
		Drive_Stop();
                break;

	    case ES_TIMEOUT:
		if (ThisEvent.EventParam == TAPE_SUB_HSM_TIMER) {
		    nextState = Tape_Searching;
		    makeTransition = TRUE;

		    // Consume
		    ThisEvent.EventType = ES_NO_EVENT;
		}

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    case Tape_Follow_Front:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
		// Start driving straight
		Drive_Forward(MOTOR_SPEED_CRAWL);
                break;

            case ES_EXIT:
		// Do nothing
                break;

	    case TAPE:
		args.val = ThisEvent.EventParam;

		if (args.bits.event & args.bits.type & TAPE_FRONT_LEFT) {
		    // If front left on tape
		    Drive_Left(MOTOR_SPEED_CRAWL);
		} else if (args.bits.event & args.bits.type & TAPE_FRONT_RIGHT) {
		    // If front right on tape
		    Drive_Right(MOTOR_SPEED_CRAWL);
		} else if (args.bits.event & ~args.bits.type &
			(TAPE_FRONT_LEFT | TAPE_FRONT_RIGHT)) {
		    // If front left off tape
		    Drive_Forward(MOTOR_SPEED_CRAWL);
		}
		break;

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    case Tape_Follow_Back:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
                break;

            case ES_EXIT:
                break;

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    case Tape_Done_State:
        if (ThisEvent.EventType != ES_NO_EVENT) {
            switch (ThisEvent.EventType) {
            case ES_ENTRY:
                break;

            case ES_EXIT:
                break;

            default: // all unhandled events pass the event back up to the next level
                break;
            }
        }
        break;

    default: // all unhandled states fall into here
        break;
    } // end switch on Current State

    if (makeTransition == TRUE) { // making a state transition, send EXIT and ENTRY
        // recursively call the current state with an exit event
        RunTapeSubHSM(EXIT_EVENT);
        CurrentState = nextState;
        RunTapeSubHSM(ENTRY_EVENT);
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

#ifdef TAPESUBHSM_TEST // <-- change this name and define it in your MPLAB-X
                        //     project to run the test harness
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
