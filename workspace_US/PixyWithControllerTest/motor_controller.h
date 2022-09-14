/*
 * motor_controller.h
 *
 *  Created on: Mar 28, 2019
 *      Author: a00986311
 */

#ifndef MOTOR_CONTROLLER_H_
#define MOTOR_CONTROLLER_H_

// motor inputs M1 and M2 for the motor controller defined on P3.0 and P3.1
#define M1 BIT0
#define M2 BIT1

// initialize M1 and M2 as outputs
#define M1_INIT P3DIR |= M1;
#define M2_INIT P3DIR |= M2;

// define directions for motors
#define MOV_FWD P3OUT &= ~M1 & ~M2;
#define MOV_BCK P3OUT |= M1 | M2;
#define TRN_LFT P3OUT |= M1; P3OUT &= ~M2;
#define TRN_RHT P3OUT |= M2; P3OUT &= ~M1;
#define STP_MTR TA1CCR1 = 0;

// motor enable inputs EN1 and EN2 define on TA1CLK output pin P2.0
// initialize this pin
#define PWM_INIT P2DIR |= BIT0; P2SEL |= BIT0;

// initialize timerA1 for PWM generation
// SMCLK, no divider, up mode
#define TIMER_INIT TA1CTL = TASSEL_2 | ID_0 | MC_1;
// initialize timerA1 for reset/set mode for positive PWM
#define TIMER_MODE TA1CCTL1 = OUTMOD_7;

// define the delay between ramping up or down the TA0CCR1 values as 30ms
#define RAMP_DELAY 30000


/*
 * Function: motorControllerInit
 *  - initializes the motor controller and the PWM (timer A) to have outputs off and 0 PWM.
 * Arguments:
 *  - none
 * Return:
 *  - none
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 */
void motorControllerInit();


/*
 * Function: rampSpeed
 *  - ramps the motor speed up or down by 10% increments
 * Arguments:
 *  - int prevTA1: the previous TA1CCR1 value.
 *  - int newTA1: the new TA1CCR1 value that is going to be achieved after the ramping function
 *    is completed.
 * Return:
 *  - none
 * Author: Yousaf Nazari
 * Date created: March 28th, 2019
 */
void rampSpeed(int prevTA1, int newTA1);


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
unsigned char motorDirection(unsigned char pixydata);

#endif /* MOTOR_CONTROLLER_H_ */
