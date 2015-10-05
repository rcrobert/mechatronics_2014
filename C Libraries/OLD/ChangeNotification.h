/*
 * File:   ChangeNotification.h
 * Author: majenko
 * Modified by: rcrobert
 *
 * Modified for use with the Uno32 shield used in CMPE-118/L without MPIDE
 * dependencies. Modifications include port from Cpp, type protection, and
 * documentation.
 *
 * Currently increasing potential signal frequency.
 *
 * NOTE: Currently does not work on square waves of f > 3kHz.
 *
 * NOTE: AttachInterrupt() resets the current and previous values of that pin
 * even if an interrupt exists, may miss pending interrupts.
 * 
 * NOTE: Adding a handler to a CN pin that already has one of that type will
 * simply overwrite the existing function pointer without letting the caller
 * know.
 *
 * Modified 2014.11.16
 */

#ifndef _CHANGENOTIFICATION_H
#define _CHANGENOTIFICATION_H

//#include <WProgram.h> //probably MPIDE library, dependencies handled

/*******************************************************************************
 * PUBLIC TYPES                                                 
 ******************************************************************************/

// Wrapper struct for holding CN pin value; carryover, may remove later
typedef struct {
    unsigned char num;
} CN;

// Available CN pins for use with functions
#define CN_0 (CN){0}	    // UNAVAILABLE
#define CN_1 (CN){1}	    // W07
#define CN_2 (CN){2}	    // X04
#define CN_3 (CN){3}	    // UNAVAILABLE, USED FOR BAT Vout
#define CN_4 (CN){4}	    // V03
#define CN_5 (CN){5}	    // V04
#define CN_6 (CN){6}	    // V05
#define CN_7 (CN){7}	    // V06
#define CN_8 (CN){8}	    // X05 or X07/9
#define CN_9 (CN){9}	    // UNAVAILABLE
#define CN_10 (CN){10}	    // X09/7
#define CN_11 (CN){11}	    // UNAVAILABLE
#define CN_12 (CN){12}	    // W07
#define CN_13 (CN){13}	    // X11
#define CN_14 (CN){14}	    // Y05
#define CN_15 (CN){15}	    // X12
#define CN_16 (CN){16}	    // X10
#define CN_17 (CN){17}	    // X06
#define CN_18 (CN){18}	    // X03
#define CN_19 (CN){19}	    // UNAVAILABLE
#define CN_20 (CN){20}	    // UNAVAILABLE
#define CN_21 (CN){21}	    // UNAVAILABLE

#define NUM_CN 22

// Enum defining CN interrupt options
typedef enum {
    RISING = 0x04,
    FALLING = 0x02,
    CHANGE = 0x01
} CN_INTTYPE;

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function CN_AttachInterrupt(CN cn, void (*func)(), CN_INTTYPE type)
 * @param cn - One of the available CN pins
 * @param func - Point to interrupt handler for the type of interrupt thrown
 * @param type - The type of state change to attach the interrupt handler to
 * @return none
 * @brief Configures an interrupt for a specific type of edge detected on the
 * given CN pin. Sets the function to be called when the edge is detected.
 * @author rcrobert 2014.11.16
 */
void CN_AttachInterrupt(CN, void (*)(), CN_INTTYPE);

/**
 * @Function CN_DetachInterrupt(CN cn, CN_INTTYPE type)
 * @param cn - CN pin to remove the interrupt from
 * @param type - Interrupt case to remove from the CN pin
 * @return none
 * @brief Removes the specified type of interrupt handler from the CN pin. If no
 * valid functions exist, CN pin is disabled as well
 * @author rcrobert 2014.11.16
 */
void CN_DetachInterrupt(CN, CN_INTTYPE);

#endif
