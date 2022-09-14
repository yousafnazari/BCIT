/*
 * pixy.h
 *
 *  Created on: Mar 26, 2019
 *      Author: Mohammad ali Sharifi
 */

#ifndef PIXY_H_
#define PIXY_H_

#include <msp430.h>
#include <stdio.h>

#define SYNC_1_S 0xAE
#define SYNC_2_S 0xC1
#define SYNC_1_R 0xAF
#define SYNC_2_R 0xC1
#define     _UART_A1PSEL    P4SEL |= PM_UCA1TXD | PM_UCA1RXD // use macros for short expressions.
#define     TXD_A1          BIT4            //Transmit Data on P4.4
#define     RXD_A1          BIT5            //Receive Data on P4.5
#define     TXD_A0          BIT3            // Transmit data P3.3
#define     RXD_A0          BIT4            // receive data P3.4
#define     BUFF_LEN        50           // length of receive buffer

typedef struct PIXY_DATA
{
    char sig, payLoad, index, age;
    int  x, y, width, height, angle;
}
PIXY_DATA;

extern char txBuffer[BUFF_LEN];
extern char rxBuffer[BUFF_LEN];
extern unsigned char response;
extern char rxCount;
extern char txCount;
extern char MaxCount;




/************************************************************************************
 * Function: ucsiA0UartInit
 * - configures UCA0 UART to use SMCLK, no parity, 8 bit data, LSB first, one stop bit
 *  BAUD rate = 230.04 Kbps with 16xoversampling.
 *  assumes SMCLK = 20MHz
 * argument:
 * Arguments: none
 *
 * return: none
 * Author: Mohammad Ali Sharifi
 * Date: March 1st, 2017
 * Modified: --
 ************************************************************************************/
void ucsiA0UartInit();


/************************************************************************************
 * Function: ucsiA0UartTxChar
 * - writes a single character to UCA1TXBUF if it is ready
 * argument:
 * Arguments: txChar - byte to be transmitted
 *
 * return: none
 * Author: Mohammad Ali Sharifi
 * Date: March 12, 2019
 * Modified: <date of any mods> usually taken care of by rev control
 ************************************************************************************/
void ucsiA0UartTxChar(char txChar);


/************************************************************************************
 * Function: getBlocks
 * Sends required code through UART to Pixy and requests for number of specific color signature
 * argument:
 * Arguments: -simap: signature number to be found
 *            maxBlocks:  Max number of Blocks
 *
 * return: none.
 * Author: Mohammad Ali Sharifi
 * Date: 2019-Mar-13
 * Modified: --
 ************************************************************************************/
void getBlocks(char sigmap, char maxBlocks);


/**********************************************************************
 * Function: pixyDataParse
 * - Parses data on the data coming from pixy camera.
 * argument:
 * Arguments: data pointer; pointer to an array of data
 *
 * return: Number of objects detected
 * Author: Mohammad Ali Sharifi
 * Date: March 11, 2019
 * Modified: --
 **********************************************************************/
int pixyDataParse(char *data, PIXY_DATA *pixyData);


/**********************************************************************
 * Function: nearestObject
 * - determines which object is the nearest to the robot based on the area of blocks
 * argument:
 * Arguments:
 * - pixyData: data pointer; pointer to an array of data
 * - numElements: number of elements in pixy data array
 *
 * return: PIXY_DATA pointer to the nearest object, returns NULL if no object detected
 *
 * Author: Mohammad Ali Sharifi
 * Date: March 11, 2019
 * Modified: --
 **********************************************************************/
PIXY_DATA *nearestObject(PIXY_DATA *pixyData,  int numElements);


/**********************************************************************
 * Function: inRange
 * - determines if the target object is in range of the Pixy frame.
 *   0 is in range, 1 turn right and 2 means turn left
 * argument:
 * Arguments: Pixy data
 *
 * return:  char - A number between 0 - 2.
 * Author: Mohammad Ali Sharifi
 * Date: March 14, 2019
 * Modified: --
 **********************************************************************/
char inRange(PIXY_DATA *pixyData);


#endif /* PIXY_H_ */
