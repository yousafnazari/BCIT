/**********************************************************************************************
 * Authur: Mohammad Ali Sharifi / Yousaf Nazari
 *
 * Created: 2019-Feb-02
 *
 * main function for the SW driver for Autonomous Vision-aided Vehicle
 *
 * Modified: 2019-Mar-13
 */

#include <msp430.h> 
#include <pixy.h>
#include "inc_SMCLK.h"

char rxBuffer[BUFF_LEN];
char txBuffer[BUFF_LEN];
unsigned char response;
char rxCount = 0;
char txCount = 0;
char MaxCount = BUFF_LEN;

/**
 * main.c
 */
int main(void)
{

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    PIXY_DATA *pixyTemp;
    PIXY_DATA pixyData[3];

    int numObj;

    smclkChange();   // Increase SMCLK to 20 MHz

    /******************************Setup timerA0 *************************************/
    TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR; // Aclk, divide by 8, up mode and clear the the TAR
    //TA0EX0 = TAIDEX_7; // further devide by 8
    TA0CCR0 = 3276; // set the timer to request data 10 times per 1 sec
    TA0CCTL0 &= ~CCIFG;
    TA0CCTL0 |= CCIE;

    ucsiA0UartInit(); // Initialize UART A0 state machine

    UCA0IE |= UCRXIE; // enable interrupt for Receive Buffer A0

    /***************Configure P4 and P1 for LEDs**********************/

    P1OUT &= ~BIT0;
    P4OUT &= ~BIT7;

    P1DIR |= BIT0;
    P4DIR |= BIT7;

    __bis_SR_register(GIE);


    while (1)
    {
        if (response & BIT0)
        {
            numObj = pixyDataParse(rxBuffer, pixyData);

            if (numObj)
            {

                pixyTemp = nearestObject(pixyData, numObj); // determine which object is the nearest

                switch (inRange(pixyTemp))
                {
                case 0:   // On Target
                    P1OUT |= BIT0;
                    P4OUT |= BIT7;
                    break;
                case 1:   // Turn right
                    P1OUT |= BIT0;
                    P4OUT &= ~BIT7;
                    break;
                case 2:   // Turn left

                    P1OUT &= ~BIT0;
                    P4OUT |= BIT7;

                    break;
                default:
                    break;
                }

            }
            else if (!numObj) // No object detected
            {
                P1OUT &= ~BIT0;
                P4OUT &= ~BIT7;
            }

        }

    }

    return 0;
}
