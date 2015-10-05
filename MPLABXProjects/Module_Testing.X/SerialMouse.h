/* 
 * File:   SerialMouse.h
 * Author: rcrobert
 *
 * Software module for interfacing through a PS/2 connector to a mouse. This
 * module handles all read/write functionality for working with a PS/2 mouse.
 * The data packets sent by the mouse will be buffered to allow the caller to
 * read the mouse asynchronously.
 *
 * NOTE: Currently supports only 1 mouse input.
 * NOTE: Uses Timer4 for its interrupts
 *
 * Created on November 16, 2014, 4:25 PM
 */

#ifndef SERIALMOUSE_H
#define	SERIALMOUSE_H

#include <IO_Ports.h>
#include "ChangeNotification.h"

/*******************************************************************************
 * PUBLIC #DEFINES                                                             *
 ******************************************************************************/
// Definitions for pins available for use with the mouse
#define SM_CLKPIN1 (1<<0)

#define SM_PORT PORTV
#define SM_DPIN1

/*******************************************************************************
 * PUBLIC TYPES                                                                *
 ******************************************************************************/
typedef struct {
    int dx;
    int dy;
} Coordinate;

/*******************************************************************************
 * PUBLIC FUNCTION PROTOTYPES                                                  *
 ******************************************************************************/

/**
 * @Function SerialMouse_Init(void)
 * @param none
 * @return none
 * @brief Initializes SerialMouse module
 * @author Reese Robertson, 2014.11.16
 */
void SerialMouse_Init(void);

char SerialMouse_AddPins(CN clk, uint16_t data);

char SerialMouse_Start(void);

char SerialMouse_Write(uint8_t data);

Coordinate SerialMouse_Read(void);


#endif	/* SERIALMOUSE_H */

