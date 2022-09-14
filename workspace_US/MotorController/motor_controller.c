#include <msp430.h>
#include "motor_controller.h"

/*
 * motor_controller.c
 *
 *  Created on: Mar 28, 2019
 *      Author: a00986311
 */

extern unsigned char CONTROL;

/*
 * Function: motorControllerInit
 *  - initializes the motor controller and the PWM (timer A) to have outputs off and 0 PWM
 * Arguments:
 *  - none
 * Return:
 *  - none
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 * Date modified: N/A
 */
void motorControllerInit()
{
    // initialize global CONTROL variable to be stopped, forward direction and to be slow
    CONTROL_INIT;

    // initialize timerA for the PWM generation
    TIMER_INIT;
    // set timer mode for reset/set mode for positive PWM
    TIMER_MODE;
    // use count of 100 for TA0CCR0 for 100kHz operating range of motors
    TA1CCR0 = 10;
    // start with TA0CCR1 value of 0 for 0 PWM so motors are stopped
    TA1CCR1 = 0;
    // initialize output pin for PWM, P1.2
    PWM_INIT;

    // initialize motors to be in forward direction
    MOV_FWD;

    // initialize direction for motor inputs as outputs on the msp
    MOTOR_INIT;
}

/*
 * Function: rampSpeed
 *  - ramps the motor speed up or down by 10% increments
 * Arguments:
 *  - char prevTA0: the previous TA0CCR1 value.
 *  - char newTA0: the new TA0CCR1 value that is going to be achieved after the ramping function
 *    is completed.
 * Return:
 *  - none
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 * Date modified: N/A
 */
void rampSpeed(char prevTA0, char newTA0)
{
    // calculate the absolute value of the difference between the previous TA0 and the new TA0
    char difference = fabs(prevTA0-newTA0);

    // while the previous TA0 value is not equal to the new TA0 value than stay in this loop
    // and keep incrementing or decrementing the TA0CCR1
    while (prevTA0 != newTA0 )
    {
        // if the previous TA0CCR1 value is less than the new desired TA0CCR1 value
        if (prevTA0 < newTA0)
        {
            // increment the previous TA0 value by 1 (10% of TA0CCR0 value)
            prevTA0 += 1;
            // delay for 30ms to allow for "ramping"
            __delay_cycles((long int) RAMP_DELAY);
            // update the TA0CCR1 value with the new value for the prevTA0 that was calculated above
            TA0CCR1 = prevTA0;
            // calculate the new difference between the previous TA0CCR1 value and the new TA0CCR1 value
            // for the while loop
            difference = fabs(prevTA0-newTA0);
        }
        // or if the previous TA0CCR1 value is greater than the new desired TA0CCR1 value
        else
        {
            // decrement the previous TA0CCR1 value by 1 (10% of TA0CCR0 value)
            prevTA0 -= 1;
            // delay for 30ms to allow for "ramping"
            __delay_cycles((long int) RAMP_DELAY);
            // update the TA0CCR1 value with the new value for the prevTA0 that was calculated above
            TA0CCR1 = prevTA0;
            // calculate the new difference between the previous TA0CCR1 value and
            // the new TA0CCR1 value for the while loop
            difference = fabs(prevTA0-newTA0);
        }
    }
}

/*
 * Function: motorDirection
 *  - changes direction of motors based on input from pixy
 * Arguments:
 *  - none
 * Return:
 *  - direction: the direction the motors are in after this function is finished;
 *    direction = 0, motors are in the forward direction,
 *    direction = 1, motors are turning left,
 *    direction = 2, motors are turning right
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 * Date modified: N/A
 */
unsigned char motorDirection(unsigned char pixydata)
{
    unsigned char direction;

    if(pixydata)
    {
        MOV_FWD;
        direction = 0;
    }
    else if (pixydata == 1)
    {
        TRN_LFT;
        direction = 1;
    }
    else if (pixydata == 2)
    {
        TRN_RHT;
        direction = 2;
    }

    return direction;
}
