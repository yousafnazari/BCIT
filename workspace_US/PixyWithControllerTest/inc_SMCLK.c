/*
 * inc_SMCLK.c
 *
 *  Created on: Mar 12, 2019
 *      Author: a01010358
 */

#include <msp430.h>
#include "inc_SMCLK.h"
/************************************************************************************
 * Function: smclkChange
 * - configures MSP430's CPU clock to 20MHz
 * argument:
 * Arguments: none.
 *
 * return: none
 * Author: Robert Trost and modified by Chris Siu
 * Date: 2013
 * Modified: 2015
 ************************************************************************************/

void smclkChange()
{

    // Increase Vcore setting to level3 to support fsystem=20MHz
    // NOTE: Change core voltage one level at a time..
    SetVcoreUp(0x01);
    SetVcoreUp(0x02);
    SetVcoreUp(0x03);

    P5SEL |= BIT4 + BIT5;                       // Port select XT1
    UCSCTL6 &= ~(XT1OFF);                     // XT1 On
    UCSCTL6 |= XCAP_3;                        // Internal load cap
    // Loop until XT1 fault flag is cleared
    do
    {
        UCSCTL7 &= ~XT1LFOFFG;                  // Clear XT1 fault flags
    }
    while (UCSCTL7 & XT1LFOFFG);               // Test XT1 fault flag

    // Initialize DCO to 25MHz
    __bis_SR_register(SCG0);                  // Disable the FLL control loop
    UCSCTL0 = 0x0000;                         // Set lowest possible DCOx, MODx
    UCSCTL1 = DCORSEL_7;                     // Select DCO range 50MHz operation
    UCSCTL2 = FLLD_0 + 611;                   // Set DCO Multiplier for 20MHz
                                              // (N + 1) * FLLRef = Fdco
                                              // (611 + 1) * 32768 = 20MHz
                                              // Set FLL Div = fDCOCLK/2
    __bic_SR_register(SCG0);                  // Enable the FLL control loop

    // Worst-case settling time for the DCO when the DCO range bits have been
    // changed is n x 32 x 32 x f_MCLK / f_FLL_reference. See UCS chapter in 5xx
    // UG for optimization.
    // 32 x 32 x 25 MHz / 32,768 Hz ~ 780k MCLK cycles for DCO to settle
    __delay_cycles(782000);

    // Loop until XT1,XT2 & DCO stabilizes - In this case only DCO has to stabilize
    do
    {
        UCSCTL7 &= ~(XT2OFFG + XT1LFOFFG + DCOFFG);
        // Clear XT2,XT1,DCO fault flags
        SFRIFG1 &= ~OFIFG;                      // Clear fault flags
    }
    while (SFRIFG1 & OFIFG);                   // Test oscillator fault flag
}
/************************************************************************************
 * Function: SetVcoreUp
 * - configures MSP430's PMM to a new Vcore level
 * argument:
 * Arguments: New level that needs to be reached (int)
 *
 * return: none
 * Author: Robert Trost and modified by Chris Siu
 * Date: 2013
 * Modified: 2015
 ************************************************************************************/
void SetVcoreUp(unsigned int level)
{
    // Open PMM registers for write
    PMMCTL0_H = PMMPW_H;
    // Set SVS/SVM high side new level
    SVSMHCTL = SVSHE + SVSHRVL0 * level + SVMHE + SVSMHRRL0 * level;
    // Set SVM low side to new level
    SVSMLCTL = SVSLE + SVMLE + SVSMLRRL0 * level;
    // Wait till SVM is settled
    while ((PMMIFG & SVSMLDLYIFG) == 0)
        ;
    // Clear already set flags
    PMMIFG &= ~(SVMLVLRIFG + SVMLIFG);
    // Set VCore to new level
    PMMCTL0_L = PMMCOREV0 * level;
    // Wait till new level reached
    if ((PMMIFG & SVMLIFG))
        while ((PMMIFG & SVMLVLRIFG) == 0)
            ;
    // Set SVS/SVM low side to new level
    SVSMLCTL = SVSLE + SVSLRVL0 * level + SVMLE + SVSMLRRL0 * level;
    // Lock PMM registers for write access
    PMMCTL0_H = 0x00;
}
