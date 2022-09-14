#include <msp430.h>
#include "motor_controller.h"

/*
 * motor_controller.c
 *
 *  Created on: Mar 28, 2019
 *      Author: a00986311
 */

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
    // initialize timerA for the PWM generation
    TIMER_INIT
    ;
    // set timer mode for reset/set mode for positive PWM
    TIMER_MODE
    ;
    // use count of 100 for TA0CCR0 for 100kHz operating range of motors
    TA1CCR0 = 10;
    // start with TA0CCR1 value of 0 for 0 PWM so motors are stopped
    STP_MTR
    ;
    // initialize output pin for PWM, P1.2
    PWM_INIT
    ;

    // initialize motors to be in forward direction
    MOV_FWD
    ;

    // initialize direction for motor inputs as outputs on the msp
    M1_INIT
    ;
    M2_INIT
    ;
}

/*
 * Function: rampSpeed
 *  - ramps the motor speed up or down by 10% increments
 * Arguments:
 *  - int prevTA0: the previous TA0CCR1 value.
 *  - int newTA0: the new TA0CCR1 value that is going to be achieved after the ramping function
 *    is completed.
 *  - int *dCycle: a pointer to the duty cycle that is set after rampSpeed is done
 * Return:
 *  - none
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 * Date modified: N/A
 */
void rampSpeed(int prevTA1, int newTA1)
{
    // if the duty cycle is 0, ie the motors are stopped than change the duty cycle by ramping
    if (!(prevTA1))
    {
        // calculate the absolute value of the difference between the previous TA0 and the new TA0
        int difference = fabs(prevTA1 - newTA1);

        // while the previous TA0 value is not equal to the new TA0 value than stay in this loop
        // and keep incrementing or decrementing the TA0CCR1
        while (prevTA1 != newTA1)
        {
            // if the previous TA0CCR1 value is less than the new desired TA0CCR1 value
            if (prevTA1 < newTA1)
            {
                // increment the previous TA0 value by 1 (10% of TA0CCR0 value)
                prevTA1 += 1;
                // delay for 30ms to allow for "ramping"
                __delay_cycles((long int) RAMP_DELAY);
                // update the TA0CCR1 value with the new value for the prevTA0 that was calculated above
                TA1CCR1 = prevTA1;
                // calculate the new difference between the previous TA0CCR1 value and the new TA0CCR1 value
                // for the while loop
                difference = fabs(prevTA1 - newTA1);
            }
            // or if the previous TA0CCR1 value is greater than the new desired TA0CCR1 value
            else
            {
                // decrement the previous TA0CCR1 value by 1 (10% of TA0CCR0 value)
                prevTA1 -= 1;
                // delay for 30ms to allow for "ramping"
                __delay_cycles((long int) RAMP_DELAY);
                // update the TA0CCR1 value with the new value for the prevTA0 that was calculated above
                TA1CCR1 = prevTA1;
                // calculate the new difference between the previous TA0CCR1 value and
                // the new TA0CCR1 value for the while loop
                difference = fabs(prevTA1 - newTA1);
            }
        }
        //*dCycle = prevTA1;
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

    if (pixydata)
    {
        MOV_FWD
        ;
        direction = 0;
    }
    else if (pixydata == 1)
    {
        TRN_LFT
        ;
        direction = 1;
    }
    else if (pixydata == 2)
    {
        TRN_RHT
        ;
        direction = 2;
    }

    return direction;
}
