#include <msp430.h> 

/**
 * main.c
 */
int CAP_VAL;
char CALC_RDY;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

    // setup P3.1 as an output
    P3DIR |= BIT0;

    // setup timerA0 that generates 10us trigger pulse
    // SMCLK, no divider, up mode
    TA0CTL = TASSEL_1 | ID_3 | MC_1 | TAIE;
    // select capture for both rising and falling edge, CCIS input, capture mode and synchronous
    // also enable the capture interrupt
    TA0CCTL1 |= CM_3 | CCIS_0 | CAP | SCS | CCIE;
    // disable timer interrupt flag
    TA0CTL &= ~TAIFG;
    // set for ~500Hz
    TA0CCR0 = 63;

    // setup the timer hardware pin as an input to capture the echo
    P1DIR &= ~BIT2;
    P1REN |= BIT2;
    P1SEL |= BIT2;

    // enable global interrupts
    _bis_SR_register(GIE);

    int distance_cm;

    while (1)
    {
        // if your ready to calculate distance
        if(CALC_RDY)
        {
            // calculate the distance in cm
            distance_cm = CAP_VAL/58;
            // reset calculation ready variable
            CALC_RDY = 0;
        }
    }
    return 0;
}

#pragma vector = TIMER0_A1_VECTOR
__interrupt void timer0A1Isr(void)
{

    switch (__even_in_range(TA0IV, 14))
    {
    case 0:
        break;
    case 2:
        // read the TA0CCR register
        CAP_VAL = TA0CCR1;
        // set variable that says your ready to calculate distance
        CALC_RDY = 1;
        break;
    case 4:
        break;
    case 6:
        break;
    case 8:
        break;
    case 10:
        break;
    case 12:
        break;
    case 14:
        // set P3.1 high
        P3OUT |= BIT0;
        // delay for 10us
        _delay_cycles(6);
        // set P3.1 low
        P3OUT &= ~BIT0;
        break;
    default:
        break;
    }

    // disable timer interrupt flag
    //TA0CTL &= ~TAIFG;
}
/*
#pragma vector = TIMER0_A0_VECTOR
__interrupt void timer0A0Isr(void)
{
    CAP_VAL = TA0CCR0;

    // disable timer interrupt flag
    TA0CTL &= ~TAIFG;

    switch (__even_in_range(TA0IV, 6))
    {
    case 0:
        break;
    case 2:
        // read the TA0CCR register
        CAP_VAL = TA0CCR0;
        break;
    case 4:
        // set P3.1 high
        P3OUT |= BIT0;
        // delay for 10us
        _delay_cycles(6);
        // set P3.1 low
        P3OUT &= ~BIT0;
        break;
    case 6:
        // set P3.1 high
        P3OUT |= BIT0;
        // delay for 10us
        _delay_cycles(6);
        // set P3.1 low
        P3OUT &= ~BIT0;
        break;
    default:
        break;
    }

}
*/



