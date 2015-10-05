/*
 * File: SensorTest.h
 * Author: J. Edward Carryer
 * Modified: Gabriel H Elkaim
 *
 * Template file to set up a Flat State Machine to work with the Events and Services
 * Framework (ES_Framework) on the Uno32 for the CMPE-118/L class. Note that this file
 * will need to be modified to fit your exact needs, and most of the names will have
 * to be changed to match your code.
 *
 * This is provided as an example and a good place to start.
 *
 * Created on 23/Oct/2011
 * Updated on 16/Sep/2013
 * Modified by MaxL on 8/11/2014
 */

#ifndef SENSORTEST_H  // <- This should be changed to your own guard on both
#define SENSORTEST_H  //    of these lines


/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "BotConfig.h"
#include "ES_Configure.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/


/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/

// typedefs for the states
// State definitions for use with the query function

//list your states as a macro'd list of other functionlike macros
//Give them unique names!
#define LIST_OF_STATES(STATE) \
        STATE(SensorTest_InitState)     \
        STATE(SensorTest_OnlyState)     \

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd
typedef enum {
    LIST_OF_STATES(ENUM_FORM)
} SensorTestState_t ;


/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
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
uint8_t InitSensorTest(uint8_t Priority);

/**
 * @Function PostSensorTest(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostSensorTest(ES_Event ThisEvent);

/**
 * @Function QuerySensorTest(void)
 * @param none
 * @return Current state of the state machine
 * @brief This function is a wrapper to return the current state of the state
 *        machine. Return will match the ENUM above. Remember to rename to
 *        something appropriate, and also to rename the TemplateState_t to your
 *        correct variable name.
 * @author J. Edward Carryer, 2011.10.23 19:25 */
SensorTestState_t QuerySensorTest(void);

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
ES_Event RunSensorTest(ES_Event ThisEvent);

#endif /* SENSORTEST_H */

