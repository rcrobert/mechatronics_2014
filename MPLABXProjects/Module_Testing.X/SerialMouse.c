/*
 * File:   SerialMouse.h
 * Author: rcrobert
 *
 * Software module for interfacing through a PS/2 connector to a mouse.
 *
 * Created on November 16, 2014, 4:25 PM
 */

#include <xc.h>
#include <BOARD.h>
#include <peripheral/timer.h>

#include "SerialMouse.h"

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
#define F_PB (BOARD_GetPBClock())
#define TIMER_FREQUENCY (1000)

/*******************************************************************************
 * PRIVATE VARIABLES                                                           *
 ******************************************************************************/
typedef struct {
    int8_t port;
    uint16_t pin;
} PIN;

CN cn = {0};
PIN clkPin = {0, 0};
PIN dPin = {0, 0};

typedef enum {
    READY,
    READING,
    WRITING
}TopState;

TopState topState;

typedef enum {
    INT_BEGIN_WRITE,
    INT_TIME,
    INT_RISING,
    INT_FALLING
} InterruptType;

unsigned char isInitialized = 0;

Coordinate dTotal = {0, 0};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES                                                 *           *
 ******************************************************************************/

void PullPinLow(PIN pin);

void ReleasePin(PIN pin);

unsigned char ReadPin(PIN pin);

void RisingEdgeHandler(void);

void FallingEdgeHandler(void);

char SerialMouse_Run(InterruptType type);

/*******************************************************************************
 * PUBLIC FUNCTIONS                                                            *
 ******************************************************************************/

/**
 * @Function SerialMouse_Init(void)
 * @param none
 * @return none
 * @brief Initializes SerialMouse module
 * @author Reese Robertson, 2014.11.16
 */
void SerialMouse_Init(void)
{
    if (isInitialized) {
	return;
    }

    // Set state machine state, prep to send
    topState = WRITING;

    isInitialized = 1;
}

char SerialMouse_AddPins(CN clk, uint16_t data)
{
    // use a lookup table to convert from CN to port/pin
    // for now, hardcode to CN4
    cn = clk;

    clkPin.port = PORTV;
    clkPin.pin = PIN3;

    dPin.port = SM_PORT;
    dPin.pin = data;

    return SUCCESS;
}

char SerialMouse_Start(void)
{
    // Initialize Timer4 for interrupts, period is 100us, NOT ENABLED
    OpenTimer4(T4_ON | T4_SOURCE_INT | T4_PS_1_1, F_PB / TIMER_FREQUENCY);
    INTSetVectorPriority(INT_TIMER_4_VECTOR, 3);
    INTSetVectorSubPriority(INT_TIMER_4_VECTOR, 3);

    CN_AttachInterrupt(cn, FallingEdgeHandler, FALLING);
    CN_AttachInterrupt(cn, RisingEdgeHandler, RISING);
    
    SerialMouse_Run(INT_BEGIN_WRITE);

    return SUCCESS;
}

char SerialMouse_Write(uint8_t data)
{
    // Incomplete DO NOT USE
    return ERROR;
}

Coordinate SerialMouse_Read(void)
{
    return dTotal;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS                                                           *
 ******************************************************************************/
void PullPinLow(PIN pin)
{
    // Pull the line low
    IO_PortsSetPortOutputs(pin.port, pin.pin);
    IO_PortsClearPortBits(pin.port, pin.pin);
}

void ReleasePin(PIN pin)
{
    // Release the line, high impedance with pull-up
    IO_PortsSetPortBits(pin.port, pin.pin);
    IO_PortsSetPortInputs(pin.port, pin.pin);
}

unsigned char ReadPin(PIN pin)
{
    return (IO_PortsReadPort(pin.port) & pin.pin) ? 0x01 : 0x00;
}

void RisingEdgeHandler(void)
{
    SerialMouse_Run(INT_RISING);
}

void FallingEdgeHandler(void)
{
    SerialMouse_Run(INT_FALLING);
}

char SerialMouse_Run(InterruptType type)
{
    enum {
	INIT,
	CLK_LO,
	START_BIT,
	WAIT_LO,
	WAIT_HI,
	WAIT_ACK
    };

    static unsigned char intState = INIT;
    static int dCnt = 0;

    const unsigned char CMD_INIT = 0xF4;
    const unsigned char CMD_INIT_PAR = 0x0;

    switch (intState) {
    case INIT:
	//DEBUG
	//printf("\nINIT");
	
	// Pull the clock line low
	PullPinLow(clkPin);

	// Configure timer for 100us wait for holding low clk signal
	INTClearFlag(INT_T4);
	INTEnable(INT_T4, INT_ENABLED);

	// if 1, change states
	topState = WRITING;
	intState = START_BIT;
	break;

    case START_BIT:
	// DEBUG
	/*
	printf("\nSTART_BIT");
	if (type == INT_FALLING)
	    printf(" - FALLING");
	else if (type == INT_RISING)
	    printf(" - RISING");
	 //*/

	if (type == INT_TIME) {

	    // Time is up, bring data lo
	    PullPinLow(dPin);

	    // Release clk line
	    ReleasePin(clkPin);

	    // Now wait for clock low to begin sending data
	    // Device will read on rising edge, set data when clk goes lo
	    // Send 8bit data + parity + stop
	    dCnt = 0;

	    // change states
	    intState = WAIT_LO;
	}
	break;

    case WAIT_LO:
	// DEBUG
	/*
	printf("\nWAIT_LO");
	if (type == INT_FALLING)
	    printf(" - FALLING");
	else if (type == INT_RISING)
	    printf(" - RISING");
	 //*/

	if (type == INT_FALLING) {

	    // set/reset dPin for appropriate data
	    if (dCnt == 0) {
		// send start bit
		PullPinLow(dPin);

		++dCnt;

		// change states
		//intState = WAIT_HI;
	    }
	    else if (dCnt < 9) {
		if (CMD_INIT & ( 1 << (dCnt - 1) )) {
		    ReleasePin(dPin);
		} else {
		    PullPinLow(dPin);
		}

		++dCnt;

		// change states
		//intState = WAIT_HI;

	    } else if (dCnt == 9) {
		// hardcoded parity for 0xF4 command
		PullPinLow(dPin);

		++dCnt;

		// change states
		//intState = WAIT_HI;
	    } else { // dCnt == 10
		// send stop bit
		ReleasePin(dPin);

		// DEBUG
		// change states
		//intState = WAIT_LO;
		dCnt = 0;
	    }

	    // DEBUG
	    //printf("\nCount: %d", dCnt);
	}
	break;

    case WAIT_HI:
	// DEBUG
	/*
	printf("\nWAIT_HI");
	if (type == INT_FALLING)
	    printf(" - FALLING");
	else if (type == INT_RISING)
	    printf(" - RISING");
	 //*/

	if (type == INT_RISING) {
	    // change states
	    intState = WAIT_LO;
	}
	break;

    case WAIT_ACK:	
	if (type == INT_RISING) {
	    if (ReadPin(dPin)) {
		// ACK received, ready to begin reading, clock stays released
		topState = READING;
	    } else {
		// ACK failed, removed interrupts and hold clk line low
		CN_DetachInterrupt(cn, FALLING);
		CN_DetachInterrupt(cn, RISING);

		PullPinLow(clkPin);
	    }
	}
	break;
    }

    return SUCCESS;
}

void __ISR(_TIMER_4_VECTOR, ipl3) Timer4IntHandler(void) {
    //INTClearFlag(INT_T4);

    // Disable timer and interrupt until next use, possibly replace with one-off?
    INTEnable(INT_T4, INT_DISABLED);
    CloseTimer4();

    // 100us is up, handle it
    SerialMouse_Run(INT_TIME);
}
