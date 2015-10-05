/*
 * File:   ChangeNotification.h
 * Author: majenko
 * Modified by: rcrobert
 *
 * Modified for use with the Uno32 shield used in CMPE-118/L without MPIDE
 * dependencies. Modifications include port from Cpp, type protection, and
 * documentation. Removed the CHANGE type since it slows down the ISR and can
 * be implemented in other ways. Increased the maximum operating frequency of
 * the module with optimizations to the ISR and module cleanliness.
 *
 * BUG: Module is limited by the CN delay of ~17us and a ~40us execution time
 * for the ISR. Generally, this gives an upper limit of 6kHz on a signal with
 * RISING and FALLING interrupts. Realistically, a lower limit is more likely
 * because of the caller's interrupt handlers.
 *
 * NOTE: AttachInterrupt() resets the current and previous values of that pin
 * even if an interrupt exists, may miss pending interrupts.
 *
 * Modified 2014.11.16
 */

#ifndef _CHANGENOTIFICATION_H
#define _CHANGENOTIFICATION_H

#include <stdint.h>

#ifndef ERROR
#define ERROR ((int8_t) -1)
#endif

#ifndef SUCCESS
#define SUCCESS ((int8_t) 1)
#endif

/*******************************************************************************
 * PUBLIC TYPES                                                 
 ******************************************************************************/

// Wrapper struct for holding CN pin value
typedef struct {
    unsigned char num;
} CN;

// Available CN pins for use with functions
#define CN_0 (CN){0}	    // UNAVAILABLE
#define CN_1 (CN){1}	    // UNAVAILABLE
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

// Enum defining signal interrupt types
typedef enum {
    RISING,
    FALLING
} CN_INTTYPE;

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function CN_AttachInterrupt(CN cn, void (*func)(), CN_INTTYPE type)
 * @param cn - One of the available CN pins
 * @param func - Pointer to interrupt handler for the associated type. The
 * handler will be called with the number (0 - 21) of the CN that triggered
 * it as an int.
 * @param type - The type of state change to attach the interrupt handler to
 * @return ERROR if cn is out of bounds, func is NULL, or a handler for given
 * type already exists
 * @brief Configures an interrupt for a specific type of edge detected on the
 * given CN pin. Sets the function to be called when the edge is detected.
 * @author rcrobert 2014.11.16
 */
char CN_AttachInterrupt(CN cn, void (*func)(int), CN_INTTYPE type);

/**
 * @Function CN_DetachInterrupt(CN cn, CN_INTTYPE type)
 * @param cn - CN pin to remove the interrupt from
 * @param type - Interrupt case to remove from the CN pin
 * @return ERROR if cn is out of bounds
 * @brief Removes the specified type of interrupt handler from the CN pin. If no
 * valid functions exist, CN pin is disabled as well
 * @author rcrobert 2014.11.16
 */
char CN_DetachInterrupt(CN cn, CN_INTTYPE type);

#endif
