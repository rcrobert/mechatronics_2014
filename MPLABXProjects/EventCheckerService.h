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

#ifndef EVENTCHECKERSERVICE_H
#define EVENTCHECKERSERVICE_H


/*******************************************************************************
 * PUBLIC #INCLUDES                                                            *
 ******************************************************************************/

#include "BotConfig.h"
#include "ES_Configure.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/

// Possibly change to a real function wrapper to be type safe
#define PostToMainHSM(x) (PostTopHSM(x))

/*******************************************************************************
 * PUBLIC VARIABLES
 ******************************************************************************/



/*******************************************************************************
 * PUBLIC TYPEDEFS                                                             *
 ******************************************************************************/

typedef union {
    struct {
        unsigned char type : 8;
        unsigned char event : 8;
    } bits;
    uint16_t val;
} EventStorage;

/*
#define LIST_OF_EVENT_STATES(STATE) \
        STATE(NOT_READY_TO_READ)    \
        STATE(READY_TO_READ)        \
        STATE(LEDS_OFF)             \
        STATE(LEDS_ON)

#define ENUM_FORM(STATE) STATE, //Enums are reprinted verbatim and comma'd
typedef enum {
    LIST_OF_EVENT_STATES(ENUM_FORM)
} EventCheckerState_t ;
 */

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
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
uint8_t InitEventCheckerService(uint8_t Priority);

/**
 * @Function PostEventCheckerService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be posted to queue
 * @return TRUE or FALSE
 * @brief This function is a wrapper to the queue posting function, and its name
 *        will be used inside ES_Configure to point to which queue events should
 *        be posted to. Remember to rename to something appropriate.
 *        Returns TRUE if successful, FALSE otherwise
 * @author J. Edward Carryer, 2011.10.23 19:25 */
uint8_t PostEventCheckerService(ES_Event ThisEvent);

/**
 * @Function RunEventCheckerService(ES_Event ThisEvent)
 * @param ThisEvent - the event (type and param) to be responded.
 * @return Event - return event (type and param), in general should be ES_NO_EVENT
 * @brief This function is where you implement the whole of the service,
 *        as this is called any time a new event is passed to the event queue. 
 * @note Remember to rename to something appropriate.
 *       Returns ES_NO_EVENT if the event have been "consumed." 
 * @author J. Edward Carryer, 2011.10.23 19:25 */
ES_Event RunEventCheckerService(ES_Event ThisEvent);



#endif /* EVENTCHECKERSERVICE_H */
