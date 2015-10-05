
#include <BOARD.h>
#include "MotorDriver.h"


// (?) Can burn out H bridge if going from forward to reverse instantly
// (x) Solved with power resistors
// Solved, H bridge can handle 2.5A per channel

static int ConvertDC(int speed)
{
	int x;
	unsigned int batRead;
	float inVolt;
	float mult;

	batRead = AD_ReadADPin(BAT_VOLTAGE);

	inVolt = ((float) batRead) / 1023.0 * 33.0;
	mult = 9.90 / inVolt;

	// Cap it at 1000
	return (int) (((speed * mult) > 1000.0) ? 1000.0 : (speed * mult));
}

static char Left_MtrSpeed(int speed)
{
	// Check argument range
	if (speed > MAX_PWM || speed < ((-1) * MAX_PWM)) {
		return ERROR;
	}

	// Modify speed for ratio
	speed = (int) ((float) speed * MOTOR_RATIO);

	// Check direction
	if (speed < 0) {
		// Reverse
		speed = speed * (-1);

		// Clear direction
		IO_PortsClearPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LEFT_DIR);
	} else {
		// Forward

		// Set direction
		IO_PortsSetPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LEFT_DIR);
	}

	// Set PWM
	speed = ConvertDC(speed);
	PWM_SetDutyCycle(MOTOR_PINS_LEFT_EN, speed);

	return SUCCESS;
}

static char Right_MtrSpeed(int speed)
{
	// Check argument range
	if (speed > MAX_PWM || speed < ((-1) * MAX_PWM)) {
		return ERROR;
	}

	// Check direction
	if (speed < 0) {
		// Reverse
		speed = speed * (-1);

		// Clear direction
		IO_PortsClearPortBits(MOTOR_PINS_PORT, MOTOR_PINS_RIGHT_DIR);
	} else {
		// Forward

		// Set direction
		IO_PortsSetPortBits(MOTOR_PINS_PORT, MOTOR_PINS_RIGHT_DIR);
	}

	// Set PWM
	speed = ConvertDC(speed);
	PWM_SetDutyCycle(MOTOR_PINS_RIGHT_EN, speed);

	return SUCCESS;
}

// Lower level functions

void Drive_Init(void)
{
	// Configure PWM components
	PWM_Init();
	PWM_SetFrequency(PWM_BOT_FREQUENCY);

	// Add motor pins
	PWM_AddPins(MOTOR_PINS_LEFT_EN | MOTOR_PINS_RIGHT_EN | MOTOR_PINS_LIFT_EN);

	// Configure direction pins
	IO_PortsSetPortOutputs(MOTOR_PINS_PORT, MOTOR_PINS_RIGHT_DIR | MOTOR_PINS_LEFT_DIR |
		MOTOR_PINS_LIFT_DIR);
}

char Drive_Straight(int speed)
{
	// Arguments checked in helper functions

	// Should motors be halted first?
	if (Left_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	if (Right_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

char Drive_Stop(void)
{
	return (Drive_Straight(0));
}

char Drive_Left(int speed)
{
	// Currently just moves one motor twice as fast, test more to find the right
	// gradient, maybe accept gradient as an argument
	if (Right_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	if (Left_MtrSpeed((speed / 2)) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

char Drive_Right(int speed)
{
	// Currently just moves one motor twice as fast, test more
	if (Left_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	if (Right_MtrSpeed((speed / 2)) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

char Drive_TankLeft(int speed)
{
	// Currently just moves one motor opposite, test more
	if (Right_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	if (Left_MtrSpeed(speed * (-1)) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

char Drive_TankRight(int speed)
{
	// Currently just moves one motor opposite, test more
	if (Left_MtrSpeed(speed) == ERROR) {
		return ERROR;
	}

	if (Right_MtrSpeed(speed * (-1)) == ERROR) {
		return ERROR;
	}

	return SUCCESS;
}

char Drive_LiftUp(void)
{
	// Set direction to raise it
	IO_PortsSetPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LIFT_DIR | MOTOR_PINS_LIFT_EN);

	return SUCCESS;
}

char Drive_LiftDown(void)
{
	// Set direction to lower it
	IO_PortsClearPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LIFT_DIR);

	IO_PortsSetPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LIFT_EN);

	return SUCCESS;
}

char Drive_LiftStop(void)
{
	IO_PortsClearPortBits(MOTOR_PINS_PORT, MOTOR_PINS_LIFT_EN);

	return SUCCESS;
}

// Helper functions

