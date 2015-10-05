/* 
 * File:   TestHarness.c
 * Author: rcrobert
 *
 * Created on November 17, 2014, 12:19 AM
 */

#include <xc.h>
#include <AD.h>
#include <serial.h>
#include <BOARD.h>
#include <timers.h>
#include <IO_Ports.h>
#include "ChangeNotification.h"

/*******************************************************************************
 * GLOBAL DEFINES                                                  
 ******************************************************************************/
#ifndef FALSE
#define FALSE ((uint8_t) 0)
#endif

#ifndef TRUE
#define TRUE ((uint8_t) 1)
#endif

/*******************************************************************************
 * CONTROL DEFINITIONS
 ******************************************************************************/
/*
 * Only define one at a time. Define the one for the module that needs to be
 * tested
 */

//#define HELLO_WORLD
//#define CN_INT_TEST
#define ENCODER_TEST

/**
 * When all else fails
 */
#ifdef HELLO_WORLD

int main(void)
{
    BOARD_Init();
    SERIAL_Init();

    printf("\nHello world!");

    // Sit and spin
    while(1);
}

#endif

/**
 * Test harness for the ChangeNotification.h library
 * This will print out the edge type detected on the pin as well as the number
 * of the CN struct that detected the edge.
 */
#ifdef CN_INT_TEST

#include "ChangeNotification.h"

unsigned char risingFlag = FALSE;
unsigned char fallingFlag = FALSE;
int caller;

void RisingEdge(int cn)
{
    caller = cn;
    risingFlag = TRUE;
}

void FallingEdge(int cn)
{
    caller = cn;
    fallingFlag = TRUE;
}

int main(void)
{
    // Init
    BOARD_Init();
    SERIAL_Init();

    // Test attaching interrupts
    printf("\nAttaching interrupts.");

    CN_AttachInterrupt(CN_4, RisingEdge, RISING);
    CN_AttachInterrupt(CN_4, FallingEdge, FALLING);

    // Handle flags, print and reset
    while (1) {
	if (risingFlag) {
	    printf("\nRising Edge %d.", caller);

	    risingFlag = FALSE;
	}

	if (fallingFlag) {
	    printf("\nFalling Edge %d.", caller);

	    fallingFlag = FALSE;
	}
    }

    // Do not exit
    while (1);
}
#endif // CN_INT_TEST

/**
 * Test harness for the MotorEncoder.h library
 * This will initialized the motor encoder for Port X03. It takes serial input
 * c - Returns the current count of the encoder.
 * a - Attaches an interrupt to notify after 5 counts.
 * r - Clears the current count of the encoder.
 * o - Returns the overflow state of the encoder.
 * x - Deactivates the current encoder and ends the module.
 * h - Cancels the pending interrupt for the encoder.
 */
#ifdef ENCODER_TEST

#include "MotorEncoder.h"

void CallBack(void)
{
    printf("\nCallback Reached");
}

int main(void)
{
    char in;

    BOARD_Init();
    SERIAL_Init();

    // Initialize module
    if (Encoder_Init() == SUCCESS)
	printf("\nMotorEncoder initialized");
    else {
	printf("\nMotorEncoder failed to initialize");
	while(1);
    }

    // Add some pins
    printf("\nTesting encoder for pin X3 - ");
    if (Encoder_AddPins(ENCODER_PORTX3) == SUCCESS) {
	printf("Pins added");
    } else {
	printf("Failed to add pins");
	while(1);
    }

    while (1) {
	if (!IsReceiveEmpty()) {
	    in = GetChar();

	    switch (in) {

	    case 'c':
		printf("\nCount: %d", Encoder_GetCount(ENCODER_PORTX3));
		break;

	    case 'a':
		// Test attach
		if (Encoder_CountNum(ENCODER_PORTX3, 5, CallBack) == SUCCESS) {
		    printf("\nAttached handler");
		} else {
		    printf("\nFailed to attach handler");
		}
		break;

	    case 'r':
		// Clear
		if (Encoder_ClrCount(ENCODER_PORTX3) == SUCCESS) {
		    printf("\nCleared");
		} else {
		    printf("\nFailed to clear");
		}
		break;

	    case 'o':
		if (Encoder_IsOverflowed(ENCODER_PORTX3)) {
		    printf("\nEncoder is overflowed");
		} else {
		    printf("\nEncoder is not overflowed");
		}
		break;

	    case 'x':
		if (Encoder_RemovePins(ENCODER_PORTX3) == SUCCESS) {
		    printf("\nEncoder removed");
		} else {
		    printf("\nEncoder not removed");
		    while(1);
		}
		Encoder_End();
		break;

	    case 'h':
		if (Encoder_CancelCount(ENCODER_PORTX3) == SUCCESS) {
		    printf("\nEncoder interrupt cancelled");
		} else {
		    printf("\nEncoder failed to cancel");
		}

	    } // end switch

	}
    }

    // EoP
    while(1);
}

#endif
