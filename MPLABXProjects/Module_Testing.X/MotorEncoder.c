
#include "MotorEncoder.h"
#include "ChangeNotification.h"

#include <BOARD.h>
/*
 * Any advantage in hiding the EncoderData data structure? Should users be given
 * access to it? Or can we improve the reliability through functions since they
 * are dynamically altered by ISR?
 */

/*******************************************************************************
 * PRIVATE #DEFINES
 ******************************************************************************/

// uses X3 X4 X6 X10 X11 X12
// these are CN18 CN2 CN17 CN16 CN13 CN15
#define ENCODER_1_CN (CN){18}
#define ENCODER_2_CN (CN){2}
#define ENCODER_3_CN (CN){17}
#define ENCODER_4_CN (CN){16}
#define ENCODER_5_CN (CN){13}
#define ENCODER_6_CN (CN){15}

#define NO_TARGET (-1)

/*******************************************************************************
 * PRIVATE VARIABLES
 ******************************************************************************/
unsigned char ActiveEncoders = 0x00;
unsigned char ModInitialized = FALSE;

typedef struct {
    volatile int count;
    volatile int target;
    void (*handler)();
    unsigned char overflow;
} EncoderData;

EncoderData Encoders[NUM_ENCODERS];
CN EncoderCN[NUM_ENCODERS] = {
    ENCODER_1_CN,
    ENCODER_2_CN,
    ENCODER_3_CN,
    ENCODER_4_CN,
    ENCODER_5_CN,
    ENCODER_6_CN
};

/*******************************************************************************
 * PRIVATE FUNCTION PROTOTYPES
 ******************************************************************************/
// Handler function called by CN ISR
void EncoderHandler(int cn);

/*******************************************************************************
 * PUBLIC FUNCTIONS
 ******************************************************************************/
// init the module, initialize data structures
char Encoder_Init(void)
{
    if (ModInitialized)
	return ERROR;

    // No encoders active
    ActiveEncoders = 0x00;

    // Init array
    int i;
    for (i = 0; i < NUM_ENCODERS; i++) {
	Encoders[i].count = 0;
	Encoders[i].target = NO_TARGET;
	Encoders[i].handler = NULL;
	Encoders[i].overflow = FALSE;
    }

    // Mark intialized
    ModInitialized = TRUE;

    return SUCCESS;
}

// enables encoder x and starts it counting
char Encoder_AddPins(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }

    if (encoder >= NUM_ENCODERS) {
	// ERROR out of bounds
	return ERROR;
    }

    if (ActiveEncoders & (1 << encoder)) {
	// ERROR already added
	return ERROR;
    }

    // Add to active list
    ActiveEncoders |= (1 << encoder);

    // Attach handler
    CN_AttachInterrupt(EncoderCN[encoder], EncoderHandler, RISING);

    // Clear
    EncoderData *E = &(Encoders[encoder]);

    E->count = 0;
    E->target = NO_TARGET;
    E->handler = NULL;
    E->overflow = FALSE;

    return SUCCESS;
}

// interrupts after encoder x counts n times, runs func
char Encoder_CountNum(int encoder, int n, void (*func)())
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	// ERROR out of range
	return ERROR;
    }

    if ( !(ActiveEncoders & (1 << encoder)) ) {
	// ERROR not active yet, no handler exists
	return ERROR;
    }

    EncoderData *E = &(Encoders[encoder]);

    if (E->target != NO_TARGET) {
	// ERROR already pending count
	return ERROR;
    }

    // Attach handler
    E->handler = func;
    
    // Set target
    E->target = (n - 1);

    return SUCCESS;
}

// removes the pending count from CountNum()
char Encoder_CancelCount(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	// ERROR out of range
	return ERROR;
    }

    if ( !(ActiveEncoders & (1 << encoder))) {
	// ERROR not active yet
	return ERROR;
    }

    EncoderData *E = &(Encoders[encoder]);

    if (E->target == NO_TARGET) {
	// ERROR no pending count
	return ERROR;
    }

    // Remove target and handler
    E->target = NO_TARGET;
    E->handler = NULL;

    return SUCCESS;
}

// returns current count of encoder x (should this clear?)
int Encoder_GetCount(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	// ERROR out of range
	return ERROR;
    }

    if ( !(ActiveEncoders & (1 << encoder))) {
	// ERROR not active yet
	return ERROR;
    }

    return Encoders[encoder].count;
}

// resets the internal count
char Encoder_ClrCount(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	// ERROR out of range
	return ERROR;
    }

    if (!(ActiveEncoders & (1 << encoder))) {
	// ERROR not active
	return ERROR;
    }

    // Reset internal count
    Encoders[encoder].count = 0;

    return SUCCESS;
}

// true false check if overflow occurred
char Encoder_IsOverflowed(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	// ERROR out of range
	return ERROR;
    }

    if ( !(ActiveEncoders & (1 << encoder))) {
	// ERROR not active yet
	return ERROR;
    }

    if (Encoders[encoder].overflow) {
	Encoders[encoder].overflow = FALSE;
	return TRUE;
    } else {
	return FALSE;
    }
}

// removes encoder x and resets the pins to default input state
char Encoder_RemovePins(int encoder)
{
    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    if (encoder >= NUM_ENCODERS || encoder < 0) {
	//ERROR out of range
	return ERROR;
    }

    if ( !(ActiveEncoders & (1 << encoder)) ) {
	// ERROR not active
	return ERROR;
    }

    EncoderData *E = &(Encoders[encoder]);

    // Clear
    E->target = NO_TARGET;
    E->count = 0;
    E->handler = NULL;
    E->overflow = FALSE;

    // Remove handler
    CN_DetachInterrupt(EncoderCN[encoder], RISING);

    return SUCCESS;
}

// closes all encoders (may not be needed really)
char Encoder_End(void)
{
    int i;

    if (!ModInitialized) {
	// ERROR not initialized
	return ERROR;
    }
    
    // Detach all handlers
    for (i = 0; i < NUM_ENCODERS; i++) {
	CN_DetachInterrupt(EncoderCN[i], RISING);
	Encoders[i].count = 0;
	Encoders[i].target = NO_TARGET;
	Encoders[i].handler = NULL;
	Encoders[i].overflow = FALSE;
    }

    // Reset module
    ActiveEncoders = 0x00;
    ModInitialized = FALSE;

    return SUCCESS;
}

/*******************************************************************************
 * PRIVATE FUNCTIONS
 ******************************************************************************/
void EncoderHandler(int cn)
{
    int i;
    EncoderData *E;

    for (i = 0; i < NUM_ENCODERS; i++) {
	if (EncoderCN[i].num == cn) {
	    // i is the encoder that triggered, handle
	    E = &(Encoders[i]);
	    
	    // Check for overflow, increment if not
	    if (!E->overflow)
		++E->count;

	    // Check for resulting overflow
	    if (E->count < 0) {
		E->overflow = TRUE;
	    }

	    // Check if target reached, call handler and remove it after
	    if (E->target == 0) {
		if (E->handler) {
		    E->handler();
		    E->handler = NULL;
		}
		
		E->target = NO_TARGET;
	    } else if (E->target > 0) {
		--E->target;
	    }

	    // Only called with one encoder, break and exit
	    break;
	}
    }
}
