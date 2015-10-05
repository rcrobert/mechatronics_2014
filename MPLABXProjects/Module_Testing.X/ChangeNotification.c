/*
 * File:   ChangeNotification.c
 * Author: majenko
 * Modified by: rcrobert
 *
 * Modified 2014.11.16
 */

#include <plib.h>
#include <p32xxxx.h>
#include "ChangeNotification.h"

#include <IO_Ports.h>

/*******************************************************************************
 * PRIVATE DEFINES
 ******************************************************************************/
// Timer configuration
#define _CN_IPL_ISR	ipl4
#define _CN_IPL_IPC	4
#define _CN_SPL_IPC	0

/*******************************************************************************
 * PRIVATE TYPES
 ******************************************************************************/
typedef struct {
    unsigned char isActive;
    unsigned char previous;
    unsigned char current;
    void (*rising)(int);
    void (*falling)(int);
} CN_STATE;

typedef struct{
    unsigned char pin;
    volatile unsigned int *TRIS;
    volatile unsigned int *PORT;
} CN_SETTING;

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/
CN_STATE cn_states[NUM_CN] = {
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL},
    {0x00, 0x00, 0x00, NULL, NULL}
};

const CN_SETTING cn_settings[NUM_CN] = {
	{ 14, &TRISC, &PORTC }, // CN0  - RC14
	{ 13, &TRISC, &PORTC }, // CN1  - RC13
	{  0, &TRISB, &PORTB }, // CN2  - RB0
	{  1, &TRISB, &PORTB }, // CN3  - RB1
	{  2, &TRISB, &PORTB }, // CN4  - RB2
	{  3, &TRISB, &PORTB }, // CN5  - RB3
	{  4, &TRISB, &PORTB }, // CN6  - RB4
	{  5, &TRISB, &PORTB }, // CN7  - RB5
	{  6, &TRISG, &PORTG }, // CN8  - RG6
	{  7, &TRISG, &PORTG }, // CN9  - RG7
	{  8, &TRISG, &PORTG }, // CN10 - RG8
	{  9, &TRISG, &PORTG }, // CN11 - RG9
	{ 15, &TRISB, &PORTB }, // CN12 - RB15
	{  4, &TRISD, &PORTD }, // CN13 - RD4
	{  5, &TRISD, &PORTD }, // CN14 - RD5
	{  6, &TRISD, &PORTD }, // CN15 - RD6
	{  7, &TRISD, &PORTD }, // CN16 - RD7
	{  4, &TRISF, &PORTF }, // CN17 - RF4
	{  5, &TRISF, &PORTF }, // CN18 - RF5
	{ 13, &TRISD, &PORTD }, // CN19 - RD13
	{ 14, &TRISD, &PORTD }, // CN20 - RD14
	{ 15, &TRISD, &PORTD }  // CN21 - RD15
};

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
char CN_AttachInterrupt(CN cn, void (*function)(int), CN_INTTYPE type)
{
    int s;

    // check bounds
    if (cn.num < 0 || cn.num > NUM_CN)
	return ERROR;

    // check that a non-NULL handler is passed
    if (function == NULL)
	return ERROR;

    // if not already marked active, mark active
    cn_states[cn.num].isActive |= 0x01;

    // Set function pointer for specified interrupt type, check for existing
    switch(type) {

    case RISING:
	if (cn_states[cn.num].rising) {
	    return ERROR;
	} else {
	    cn_states[cn.num].rising = function;
	}
	break;

    case FALLING:
	if (cn_states[cn.num].falling) {
	    return ERROR;
	} else {
	    cn_states[cn.num].falling = function;
	}
    }

    // set as input
    *(cn_settings[cn.num].TRIS) |= (1<<cn_settings[cn.num].pin);

    // enable in CNEN reg
    CNEN |= 1<<cn.num;

    // read current value
    s = *(cn_settings[cn.num].PORT) & (1<<cn_settings[cn.num].pin);

    // initialize current and previous
    // if s, set 1 else 0
    cn_states[cn.num].current = cn_states[cn.num].previous = s == 0 ? 0 : 1;

    // clear int flag, enable int, and assign priorities
    // enable CN control reg, disable stop idle
    IFS1bits.CNIF	=	0;
    IEC1bits.CNIE	=	1;
    IPC6bits.CNIP	=	_CN_IPL_IPC;
    IPC6bits.CNIS	=	_CN_SPL_IPC;
    CNCONbits.ON	=	1;
    CNCONbits.SIDL	=	0;

    return SUCCESS;
}

/* Future:
 * Disable the CN functions and interrupts if all interrupts are detached?
 */
char CN_DetachInterrupt(CN cn, CN_INTTYPE type)
{
    // check bounds
    if (cn.num < 0 || cn.num > NUM_CN)
	return ERROR;

    switch(type) {

    case RISING:
	cn_states[cn.num].rising = NULL;
	break;

    case FALLING:
	cn_states[cn.num].falling = NULL;

    }

    // if no valid functions, disable interrupts for this pin
    if((cn_states[cn.num].rising == NULL) &&
	(cn_states[cn.num].falling == NULL)) {
	// disable CN for this pin
	CNEN &= ~(1<<cn.num);

	// mark inactive
	cn_states[cn.num].isActive = 0x00;
    }

    return SUCCESS;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/
void __ISR(_CHANGE_NOTICE_VECTOR, _CN_IPL_ISR) cn_isr()
{
    unsigned char i;
    unsigned int onoff;

    // Update all ACTIVE CN pin values, call appropriate handlers
    for (i = 0; i < NUM_CN; i++) {
	
	// check that its active first, save some processing
	if (cn_states[i].isActive) {

	    // Read the CN pin, store in var s
	    onoff = (*(cn_settings[i].PORT) & (1 << cn_settings[i].pin)) ? 0x01 : 0x00;

	    // If state changed
	    if (cn_states[i].current != onoff) {
		
		// update state memory
		cn_states[i].previous = cn_states[i].current;
		cn_states[i].current = onoff;

		// if FALLING EDGE and service functions exist to handle
		if (cn_states[i].previous == 0x01) {

		    // call falling edge handler
		    if (cn_states[i].falling != NULL)
			cn_states[i].falling(i);
		    
		} else {

		    // call rising edge handler
		    if (cn_states[i].rising != NULL)
			cn_states[i].rising(i);
		}
	    }
	}
    }

    // Clear flag
    IFS1bits.CNIF = 0;
}
