
#include "BotConfig.h"

char Bot_Init(void)
{
    // Initialize modules
    AD_Init();
    PWM_Init();


    // Configure digital pins
    IO_PortsSetPortOutputs(MUX_PINS_PORT, MUX_PINS_OR);
    IO_PortsSetPortOutputs(MOTOR_PINS_PORT, MOTOR_PINS_LEFT_DIR |
			    MOTOR_PINS_RIGHT_DIR | MOTOR_PINS_LIFT_DIR | MOTOR_PINS_LIFT_EN);
    IO_PortsSetPortOutputs(SENSOR_PINS_PORT, SENSOR_PINS_LEDS);
    IO_PortsSetPortInputs(SENSOR_PINS_PORT, SENSOR_PINS_BUMP | SENSOR_PINS_TRACK);

	// Init lift motor to off
	IO_PortsClearPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LIFT_EN);

    // Configure PWM pins
//    PWM_SetFrequency(PWM_BOT_FREQUENCY);
//    PWM_AddPins(MOTOR_PINS_LEFT_EN | MOTOR_PINS_RIGHT_EN);

    // Configure AD pins
    AD_AddPins(SENSOR_PINS_BEACON | SENSOR_PINS_TAPE);

    return SUCCESS;
}
