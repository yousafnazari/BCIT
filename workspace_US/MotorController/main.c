#include <msp430.h> 
#include "motor_controller.h"

/**
 * main.c
 */

unsigned char CONTROL;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	motorControllerInit();
	// setup pins for enabling and motor outputs
	//MOTOR_INIT;
	/*
	// first motor outputs
	P3DIR |= EN1 | M1;
	// second motor outputs
	P3DIR |= EN2 | M2;
    */

    // setup output for timerA1
	//PWM_INIT;
    //P1DIR |= BIT2;      // setup P1.2 as output for PWM signal
    //P1SEL |= BIT2;      // setup peripheral


    // configure TIMERA for up mode for PWM
	//TIMER_INIT;
    //TA0CTL = TASSEL_2 | ID_0 | MC_1; // SMCLK, no divider, up mode
    //TA1CCR0 = 10;           // use count of 20 for CCRO for 20kHz operating range of motors
    //TA1CCR1 = 1;            // start with 0 PWM
    //TIMER_MODE;             // RESET/SET mode for positive PWM
    //TA0CCTL1 = OUTMOD_7;    // RESET/SET mode for positive PWM

    int i;

	while(1)
	{
	    // set forward direction
	    //MOV_FWD;

	    // set on time for PWM duty cycle
	    for(i = 1; i <= 10; i++)
	    {
	        TA1CCR1 = i;
	        __delay_cycles((long int)1048000);
	        __delay_cycles((long int)1048000);
	        __delay_cycles((long int)1048000);
	    }
	}
	return 0;
}


