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
    unsigned char previous;
    unsigned char current;
    void (*rising)();
    void (*falling)();
    void (*change)();
} CN_STATE;

typedef struct{
    unsigned char pin;
    volatile unsigned int *TRIS;
    volatile unsigned int *PORT;
} CN_SETTING;

typedef struct {
    int index;
    unsigned char handlers;
} CN_ITEM;

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/
CN_STATE cn_states[NUM_CN];

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

CN_ITEM cn_activePins[NUM_CN] = {
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 },
    { 0, 0x00 }
};

int numActive = 0;

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/
void AddHandler(CN cn, CN_INTTYPE type);
void RemoveHandler(CN cn, CN_INTTYPE type);
void CheckHasHandler(CN cn, CN_INTTYPE type);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/

void CN_AttachInterrupt(CN cn, void (*function)(), CN_INTTYPE type)
{
    int s;

    // Set function pointer for specified interrupt type
    switch(type) {

    case RISING:
	cn_states[cn.num].rising = function;
	break;

    case FALLING:
	cn_states[cn.num].falling = function;
	break;

    case CHANGE:
	cn_states[cn.num].change = function;
    }

    *(cn_settings[cn.num].TRIS) |= (1<<cn_settings[cn.num].pin);
    CNEN |= 1<<cn.num;
    s = *(cn_settings[cn.num].PORT) & (1<<cn_settings[cn.num].pin);

    // initialize current and previous
    // if s, set 1 else 0
    cn_states[cn.num].current = cn_states[cn.num].previous = s == 0 ? 0 : 1;
    
    IFS1bits.CNIF	=	0;
    IEC1bits.CNIE	=	1;
    IPC6bits.CNIP	=	_CN_IPL_IPC;
    IPC6bits.CNIS	=	_CN_SPL_IPC;
    CNCONbits.ON	=	1;
    CNCONbits.SIDL	=	0;
}

void CN_DetachInterrupt(CN cn, CN_INTTYPE type)
{
    switch(type) {

    case RISING:
	cn_states[cn.num].rising = NULL;
	break;

    case FALLING:
	cn_states[cn.num].falling = NULL;
	break;

    case CHANGE:
	cn_states[cn.num].change = NULL;
    }

    // if no valid functions, disable interrupts
    if((cn_states[cn.num].rising == NULL) &&
	(cn_states[cn.num].falling == NULL) &&
	(cn_states[cn.num].change == NULL))
	    CNEN &= ~(1<<cn.num);

    // TODO does not change pin I/O state when they are removed
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/
// cn_activePins[]
// numActive

void AddHandler(CN cn, CN_INTTYPE type){
    int i;
    
    // see if the CN pin is already active and does not have a handler
    for (i = 0; i < NUM_CN; i++) {
	if (cn_activePins[i].index == cn.num && !()) {
	    cn_activePins[i].handlers |= type;
	    break;
	} // else do nothing
    }
    
    // else add it to the array
    if (i == NUM_CN) {
	
    }
}

void RemoveHandler(CN cn, CN_INTTYPE type)
{
    
}

void CheckHasHandler(CN cn, CN_INTTYPE type)
{
    
}

void __ISR(_CHANGE_NOTICE_VECTOR, _CN_IPL_ISR) cn_isr()
{
    unsigned char i;

    // Update all CN pin values, call appropriate handlers
    for (i = 0; i < NUM_CN; i++) {
	// Read the CN pin, store in var s
	int s = *(cn_settings[i].PORT) & (1 << cn_settings[i].pin);

	unsigned char onoff = s == 0 ? 0 : 1;

	// If state changed
	if (cn_states[i].current != onoff) {
	    cn_states[i].previous = cn_states[i].current;
	    cn_states[i].current = onoff;

	    // if FALLING EDGE and service functions exist to handle
	    if (((cn_states[i].falling != NULL) || (cn_states[i].change != NULL))
		&& (cn_states[i].previous == 1) && (cn_states[i].current == 0)) {

		// call falling edge handler
		if (cn_states[i].falling != NULL)
		    cn_states[i].falling();

		// call edge handler
		if (cn_states[i].change != NULL)
		    cn_states[i].change();
	    }

	    // if RISING EDGE and service functions exist to handle
	    if (((cn_states[i].rising != NULL) || (cn_states[i].change != NULL))
		&& (cn_states[i].previous == 0) && (cn_states[i].current == 1)) {

		// call rising edge handler
		if (cn_states[i].rising != NULL)
		    cn_states[i].rising();

		// call edge handler
		if (cn_states[i].change != NULL)
		    cn_states[i].change();
	    }
	}
    }

    // Clear flag (?)
    IFS1bits.CNIF = 0;
}
