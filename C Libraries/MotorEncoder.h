/* 
 * File:   MotorEncoder.h
 * Author: rcrobert
 *
 * Created on November 18, 2014, 4:58 PM
 */

#ifndef MOTORENCODER_H
#define	MOTORENCODER_H

/*******************************************************************************
 * PUBLIC #DEFINES
 ******************************************************************************/

// List of available pins for the motor encoders, uses CMPE-118/L Uno stack pins
#define ENCODER_PORTX3 (0)
#define ENCODER_PORTX4 (1)
#define ENCODER_PORTX6 (2)
#define ENCODER_PORTX10 (3)
#define ENCODER_PORTX11 (4)
#define ENCODER_PORTX12 (5)

#define NUM_ENCODERS 6

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES
 ******************************************************************************/

/**
 * @Function Encoder_Init(void)
 * @return Returns FAILURE if the module is already initialized, else SUCCESS
 * @brief Begins the encoder module. Initializes all data structures
 */
char Encoder_Init(void);

/**
 * @Function Encoder_AddPins(int encoder)
 * @param encoder - The encoder to activate
 * @return ERROR if the encoder specified is out of range or already active
 * @brief Activates an encoder to have it begin counting. Also handles the
 * initialization for the specific encoder
 */
char Encoder_AddPins(int encoder);

/**
 * @Function Encoder_CountNum(int encoder, int n, void (*func)()
 * @param encoder - The encoder to count on
 * @param n - The number of ticks to count
 * @param func - The function called after n ticks are counted
 * @return ERROR if the encoder specified is either out of range or not active.
 * Also errors out if a count is already pending
 * @brief Counts n ticks on the specified encoder and calls func() when it is
 * done within the ISR. Keep func short. Intended to be used with ES_Framework
 * for generating events
 */
char Encoder_CountNum(int encoder, int n, void (*func)());

/**
 * @Function Encoder_Cancel(int encoder)
 * @param encoder - The encoder to remove a pending count from
 * @return ERROR if the encoder specified is out of range or nothing is pending
 * @brief Cancels a pending interrupt set up by CountNum()
 */
char Encoder_CancelCount(int encoder);

/**
 * @Function Encoder_GetCount(int encoder)
 * @param encoder - The encoder to check the count from
 * @return Returns the current count value as an integer. ERROR if the encoder
 * specified is out of range or not active
 * @brief Accessor function for the internal count value
 */
int Encoder_GetCount(int encoder);

/**
 * @Function Encoder_ClrCount(int encoder)
 * @param encoder - The encoder to reset the count for
 * @return ERROR if the encoder specified is out of range or not active
 * @brief Reset function for the internal count value
 */
char Encoder_ClrCount(int encoder);

/**
 * @Function Encoder_IsOverflowed(int encoder)
 * @param encoder - The encoder to check for overflow
 * @return TRUE or FALSE for the encoder's overflow status. ERROR if the encoder
 * specified is out of range or not active, make sure to compare ERROR returns
 * against TRUE returns to differentiate
 * @brief Checks to see if the encoder has overflowed its integer count limit.
 * Resets the overflow state to FALSE after being read
 */
char Encoder_IsOverflowed(int encoder);

/**
 * @Function Encoder_RemovePins(int encoder)
 * @param encoder - The encoder to remove and deactivate
 * @return ERROR if the encoder specified is out of range or not active
 * @brief Removes the interrupt handler for the encoder and resets it to default
 */
char Encoder_RemovePins(int encoder);

/**
 * @Function Encoder_End()
 * @return SUCCESS
 * @brief Removes the interrupt handlers for all encoders and resets all to
 * default state. Sets module to uninitialized
 */
char Encoder_End(void);

#endif	/* MOTORENCODER_H */

