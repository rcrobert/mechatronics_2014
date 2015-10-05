/*
 * File:   BotConfig.h
 * Author: rcrobert
 *
 * Reference header file for all the project pin assignments and values to be
 * tweaked for controlling the bot
 *
 * Created on December 1, 2014, 2:05 PM
 */

/* NOTES
 *
 */

/* How to set up ES_Configure.h
 * - EventCheckerService posts up to 4 events per call increase queue sizes
 * - EventCheckerService.h needs a macro for the name of the main HSM to post to
 */

#ifndef BOTCONFIG_H
#define	BOTCONFIG_H

#include <IO_Ports.h>
#include <AD.h>
#include <pwm.h>

// Timers
#define MOTOR_TURN_CR_45 (650)
#define MOTOR_TURN_CR_90 (1000)

#define MOTOR_TURN_EX_45 (210)
#define MOTOR_TURN_EX_90 (350)
#define MOTOR_TURN_EX_180 (660)

#define STALL_TIME_IN_MS (400)

// Timers for Exit
#define TIME_EXIT_OUTSIDE (3250)
#define TIME_EXIT_BACKUP (600)
#define TIME_EXIT_STRAIGHTEN (750)
#define TIME_EXIT_ALIGN (200)
#define TIME_EXIT_FRUSTRATION (1000)

// Timers for Search
#define TIME_SEARCH_PRE_LEAVE (1000)
#define TIME_SEARCH_OBSTACLE (500)
#define TIME_SEARCH_TOCENTER (2400)
#define TIME_SEARCH_HALL (2000)
#define TIME_SEARCH_BACKUP (300)
#define TIME_SEARCH_ENTER (1750)

// Timers for Approach
#define TIME_APPROACH_DRIVE (3000)
#define TIME_APPROACH_LIFT (3600)
#define TIME_APPROACH_BACKUP (800)

// Timers for Return
#define TIME_RETURN_CROWN_BACKUP (160)
#define TIME_RETURN_RECOVERY (1000)
#define TIME_RETURN_MINIBACK (60)

// Timers for Ram Sub HSM
#define TIME_RAM_WAIT (80)
#define TIME_RAM_STRAIGHTEN (600)
#define TIME_RAM_ALIGN (200)
#define TIME_RAM_TAPE (500)
#define TIME_RAM_EVADE (400)
#define TIME_RAM_BACKUP (500)

#define FRUSTRATION_TIMEOUT (4800)

#define TAPE_FRUSTRATION_TIMER (2000)

// Motor settings
#define MOTOR_RATIO (1.04)   // multiplier for left motor

#define MOTOR_SPEED_CRAWL (200)
#define MOTOR_SPEED_MEDIUM (300)
#define MOTOR_SPEED_EXPLORE (400)
#define MOTOR_SPEED_ALIGN (600)

// PWM configs
#define PWM_BOT_FREQUENCY (3000)

// Sensor defines
#define BUMP_LEFT 0x10
#define BUMP_RIGHT 0x40
#define BUMP_CENTER 0x20
#define BUMP_LIMIT 0x01
#define BUMP_CROWN 0x00 // NOT USED

#define TAPE_CENTER 0x10
#define TAPE_FRONT_LEFT 0x80
#define TAPE_FRONT_RIGHT 0x04
#define TAPE_BACK_LEFT 0x20
#define TAPE_BACK_RIGHT 0x08
#define TAPE_FAR_LEFT 0x40
#define TAPE_FAR_RIGHT 0x02

#define BEACON_FRONT 0x08
#define BEACON_RIGHT 0x02
#define BEACON_BACK 0x04
#define BEACON_LEFT 0x01

// Sensor thresholds
#define THRESHOLD_BEACON_HIGH 200
#define THRESHOLD_BEACON_LOW 150
#define THRESHOLD_TAPE_HIGH 250
#define THRESHOLD_TAPE_LOW 200

// Sensor configs
#define SENSORS_POLLING_DELAY (3)    // minimum delay in ms to settle

#define NUM_LIGHT_SENSORS (4)
#define NUM_TAPE_SENSORS (8)
#define NUM_BUMP_SENSORS (7)

#define MAX_MUX_SEL (0x08)

// Mux pins
#define MUX_PINS_PORT PORTZ         // any port works, all IO is digital here
#define MUX_PINS_BIT0 PIN11
#define MUX_PINS_BIT1 PIN9
#define MUX_PINS_BIT2 PIN7
#define MUX_PINS_OR (MUX_PINS_BIT0 | MUX_PINS_BIT1 | MUX_PINS_BIT2)

// Sensor pins
#define SENSOR_PINS_PORT        PORTW       // Port with analog, V also works
#define SENSOR_PINS_BUMP        PIN5        // digital
#define SENSOR_PINS_BEACON      AD_PORTW4   // analog
#define SENSOR_PINS_TAPE        AD_PORTW3   // analog
#define SENSOR_PINS_TRACK	PIN8        // digital
#define SENSOR_PINS_LEDS        PIN7        // digital out

// Motor pins
#define MOTOR_PINS_PORT         PORTY           // Port with 3 PWMs
#define MOTOR_PINS_LEFT_EN      PWM_PORTY10     // PWM
#define MOTOR_PINS_LEFT_DIR     PIN8            // digital
#define MOTOR_PINS_RIGHT_EN     PWM_PORTY12     // PWM
#define MOTOR_PINS_RIGHT_DIR    PIN11           // digital
#define MOTOR_PINS_LIFT_EN      PIN4            // digital
#define MOTOR_PINS_LIFT_DIR     PIN6            // digital

char Bot_Init(void);

#endif	/* BOTCONFIG_H */
