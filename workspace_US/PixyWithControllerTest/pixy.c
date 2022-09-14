/*
 * pixy.c
 *
 *  Created on: Mar 26, 2019
 *  created to serve the pixy cam2 target trakcing
 *      Author: Mohammad ali Sharifi
 */


#include"pixy.h"


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
void ucsiA0UartInit()
{

    P3SEL |= TXD_A0 | RXD_A0;

    UCA0CTL1 |= UCSWRST;            // USCI A1  Software Reset Enabled
    //********************

    UCA0CTL1 |= UCSSEL_2; // select SMCLK. User is responsible for setting this rate.

    UCA0CTL0 = 0;               // RESET UCA1CTL0 before new configuration

    UCA0CTL0 &= ~UCPEN          // No Parity
    & ~UCMSB            // LSB First
            & ~UC7BIT           // 8 bits of data
            & ~UCSPB            // 1 stop bit
            & ~UCSYNC;      // UART Mode

    UCA0BR1 = 0;   // Prescaler high byte
    UCA0BR0 = 5;   // Prescaler low byte
    UCA0MCTL = UCBRS_0 | UCBRF_7 | UCOS16; // CPU clock = 20 MHz , BUAD rate = 230 kbps

    UCA0CTL1 &= ~UCSWRST;       //  configured. take state machine out of reset.
}




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
void ucsiA0UartTxChar(char txChar)
{

    while (!(UCA0IFG & UCTXIFG))
        ; // is this efficient ?
    UCA0TXBUF = txChar; // if TXBUFF ready then transmit a byte by writing to it
}


/************************************************************************************
 * Function: ucsiA0UartTxString
 * - writes a C string of characters, one char at a time to UCA1TXBUF by calling
 *   ucsiA1UartTxChar. Stops when it encounters  the NULL character in the string
 *   does NOT transmit the NULL character
 * argument:
 * Arguments: txChar - pointer to char (string) to be transmitted
 *
 * return: number of characters transmitted
 * Author: Mohammad Ali Sharifi
 * Date: March 12, 2019
 * Modified: <date of any mods> usually taken care of by rev control
 ************************************************************************************/

int ucsiA0UartTxString(unsigned char *txChar)
{
    unsigned int i = 0;
    while (*(txChar + i) != '\0') // while not a null character
    {
        ucsiA0UartTxChar(*(txChar + i)); // transmit a character with ucsiA1UartTxChar and increment the pointer
        i++;
    }

    return i; //  Number of bytes transmitted
}


/************************************************************************************
 * Function: ucsiA0UartTxStringInt
 * Copies a string to global variable txBuffer to be sent by interrupt function
 * argument:
 * Arguments: txChar - pointer to char (string) to be transmitted
 *
 * return: number of characters copied to txBuffer
 * Author: Mohammad Ali Sharifi
 * Date: 2019-Mar-13
 * Modified: --
 ************************************************************************************/
int ucsiA0UartTxStringInt(unsigned char *txChar)
{
    unsigned int i = 0;
    i = 0;
    while (*(txChar + i) != '\0') // while not a null character
    {
        *(txBuffer + i) = *(txChar + i); // Copy a byte at a time to txBuffer
        i++;
    }
    UCA0IE |= UCTXIE; // enable interrupt for Transmit interrupt A0, the interrupt is disabled in ISR when all the bytes are transmitted

    return i; // Number of chars copied to txBuffer
}
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
void getBlocks(char sigmap, char maxBlocks)
{
    char requestArr[6] = { SYNC_1_S, SYNC_2_S, 32, 2, sigmap, maxBlocks };
    ucsiA0UartTxString(requestArr);
}

/**********************************************************************
 * Function: stringParse
 * - Parses data on the data coming from pixy camera.
 * argument:
 * Arguments:
 *  - data pointer; pointer to an array of data
 *  - pixyData: Pointer to PIXY_DATA type array (3 blocks)
 *
 * return: Number of objects detected
 * Author: Mohammad Ali Sharifi
 * Date: March 11, 2019
 * Modified: --
 **********************************************************************/
int pixyDataParse(char *data, PIXY_DATA *pixyData)
{

    char counter = 0;
    int offset = 0;
    int num = 0;

    PIXY_DATA noObject;
    noObject.age = 0;
    noObject.index = 0;
    noObject.y = 0;
    noObject.x = 0;
    noObject.width = 0;
    noObject.height = 0;
    noObject.angle = 0;

    pixyData[0].payLoad = *(data + 3);

    if ((*data == SYNC_1_R) && (*(data + 1) == SYNC_2_R)
            && (pixyData[0].payLoad))
    {
        if (pixyData[0].payLoad == 0xE) // if the pay load is equal to 14, Parse the data (1 block detected)
        {
            offset = 0;   // Set appropriate offset
            pixyData[0].sig = *(data + 6 + offset);
            pixyData[0].x = *(data + 9 + offset);
            pixyData[0].x = pixyData[0].x << 8;
            pixyData[0].x |= *(data + 8 + offset);
            pixyData[0].y = *(data + 11 + offset);
            pixyData[0].y = pixyData[0].y << 8;
            pixyData[0].y |= *(data + 10 + offset);
            pixyData[0].width = *(data + 13 + offset);
            pixyData[0].width = pixyData[0].width << 8;
            pixyData[0].width |= *(data + 12 + offset);
            pixyData[0].height = *(data + 15 + offset);
            pixyData[0].height = pixyData[0].height << 8;
            pixyData[0].height |= *(data + 14 + offset);
            pixyData[0].angle = *(data + 17 + offset);
            pixyData[0].angle = pixyData[0].angle << 8;
            pixyData[0].angle |= *(data + 16 + offset);
            pixyData[0].index = *(data + 18 + offset);
            pixyData[0].age = *(data + 19 + offset);
            num = 1; // Number of objects detected
        }
        else if (pixyData[0].payLoad == 0x1C) // if the pay load is equal to 28, Parse the data (2 block detected)
        {
            // First Object
            offset = 0;  // Set appropriate offset
            pixyData[0].sig = *(data + 6 + offset);
            pixyData[0].x = *(data + 9 + offset);
            pixyData[0].x = pixyData[0].x << 8;
            pixyData[0].x |= *(data + 8 + offset);
            pixyData[0].y = *(data + 11 + offset);
            pixyData[0].y = pixyData[0].y << 8;
            pixyData[0].y |= *(data + 10 + offset);
            pixyData[0].width = *(data + 13 + offset);
            pixyData[0].width = pixyData[0].width << 8;
            pixyData[0].width |= *(data + 12 + offset);
            pixyData[0].height = *(data + 15 + offset);
            pixyData[0].height = pixyData[0].height << 8;
            pixyData[0].height |= *(data + 14 + offset);
            pixyData[0].angle = *(data + 17 + offset);
            pixyData[0].angle = pixyData[0].angle << 8;
            pixyData[0].angle |= *(data + 16 + offset);
            pixyData[0].index = *(data + 18 + offset);
            pixyData[0].age = *(data + 19 + offset);
            // Second Object
            offset = 14;  // Set appropriate offset
            pixyData[1].sig = *(data + 6 + offset);
            pixyData[1].x = *(data + 9 + offset);
            pixyData[1].x = pixyData[1].x << 8;
            pixyData[1].x |= *(data + 8 + offset);
            pixyData[1].y = *(data + 11 + offset);
            pixyData[1].y = pixyData[1].y << 8;
            pixyData[1].y |= *(data + 10 + offset);
            pixyData[1].width = *(data + 13 + offset);
            pixyData[1].width = pixyData[1].width << 8;
            pixyData[1].width |= *(data + 12 + offset);
            pixyData[1].height = *(data + 15 + offset);
            pixyData[1].height = pixyData[1].height << 8;
            pixyData[1].height |= *(data + 14 + offset);
            pixyData[1].angle = *(data + 17 + offset);
            pixyData[1].angle = pixyData[1].angle << 8;
            pixyData[1].angle |= *(data + 16 + offset);
            pixyData[1].index = *(data + 18 + offset);
            pixyData[1].age = *(data + 19 + offset);
            num = 2; // Number of objects detected
        }
        else if (pixyData[0].payLoad == 0x2A) // if the pay load is equal to 42, Parse the data (3 block detected)
        {
            // First Object
            offset = 0;   // Set appropriate offset
            pixyData[0].sig = *(data + 6 + offset);
            pixyData[0].x = *(data + 9 + offset);
            pixyData[0].x = pixyData[0].x << 8;
            pixyData[0].x |= *(data + 8 + offset);
            pixyData[0].y = *(data + 11 + offset);
            pixyData[0].y = pixyData[0].y << 8;
            pixyData[0].y |= *(data + 10 + offset);
            pixyData[0].width = *(data + 13 + offset);
            pixyData[0].width = pixyData[0].width << 8;
            pixyData[0].width |= *(data + 12 + offset);
            pixyData[0].height = *(data + 15 + offset);
            pixyData[0].height = pixyData[0].height << 8;
            pixyData[0].height |= *(data + 14 + offset);
            pixyData[0].angle = *(data + 17 + offset);
            pixyData[0].angle = pixyData[0].angle << 8;
            pixyData[0].angle |= *(data + 16 + offset);
            pixyData[0].index = *(data + 18 + offset);
            pixyData[0].age = *(data + 19 + offset);
            // Second Object
            offset = 14;  // Set appropriate offset
            pixyData[1].sig = *(data + 6 + offset);
            pixyData[1].x = *(data + 9 + offset);
            pixyData[1].x = pixyData[1].x << 8;
            pixyData[1].x |= *(data + 8 + offset);
            pixyData[1].y = *(data + 11 + offset);
            pixyData[1].y = pixyData[1].y << 8;
            pixyData[1].y |= *(data + 10 + offset);
            pixyData[1].width = *(data + 13 + offset);
            pixyData[1].width = pixyData[1].width << 8;
            pixyData[1].width |= *(data + 12 + offset);
            pixyData[1].height = *(data + 15 + offset);
            pixyData[1].height = pixyData[1].height << 8;
            pixyData[1].height |= *(data + 14 + offset);
            pixyData[1].angle = *(data + 17 + offset);
            pixyData[1].angle = pixyData[1].angle << 8;
            pixyData[1].angle |= *(data + 16 + offset);
            pixyData[1].index = *(data + 18 + offset);
            pixyData[1].age = *(data + 19 + offset);

            // Third object
            offset = 28; // Set appropriate offset
            pixyData[2].sig = *(data + 6 + offset);
            pixyData[2].x = *(data + 9 + offset);
            pixyData[2].x = pixyData[2].x << 8;
            pixyData[2].x |= *(data + 8 + offset);
            pixyData[2].y = *(data + 11 + offset);
            pixyData[2].y = pixyData[2].y << 8;
            pixyData[2].y |= *(data + 10 + offset);
            pixyData[2].width = *(data + 13 + offset);
            pixyData[2].width = pixyData[2].width << 8;
            pixyData[2].width |= *(data + 12 + offset);
            pixyData[2].height = *(data + 15 + offset);
            pixyData[2].height = pixyData[2].height << 8;
            pixyData[2].height |= *(data + 14 + offset);
            pixyData[2].angle = *(data + 17 + offset);
            pixyData[2].angle = pixyData[2].angle << 8;
            pixyData[2].angle |= *(data + 16 + offset);
            pixyData[2].index = *(data + 18 + offset);
            pixyData[2].age = *(data + 19 + offset);
            num = 3; // Number of objects detected
        }
        else
        {
            for (counter = 0; counter < 3; counter++) // Initialize pixy data to 0
            {
                pixyData[counter] = noObject; // No data (no object detected)
            }
            num = 0; // Number of objects detected
        }
    }
    return num;
}

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
PIXY_DATA *nearestObject(PIXY_DATA *pixyData, int numElements)
{
    char i;
    PIXY_DATA *resultPtr;
    int area[3];

    if (numElements) // If number of objects is not zero
    {
        if (numElements == 1)
        {
            resultPtr = pixyData;
        }
        else
        {
            for (i = 0; i < numElements; i++) // Store the height of each element
            {
                area[i] = pixyData[i].height * pixyData[i].width;
            }
            if (numElements == 2)
            {
                if (area[0] > area[1])
                {
                    resultPtr = pixyData;
                }
                else
                {
                    resultPtr = pixyData + 1;
                }
            }
            else
            {
                if ((area[0] > area[1]) && (area[0] > area[2]))
                {
                    resultPtr = pixyData;
                }
                else if ((area[1] > area[0]) && (area[1] > area[2]))
                {
                    resultPtr = pixyData + 1;
                }
                else if ((area[2] > area[1]) && (area[2] > area[0]))
                {
                    resultPtr = pixyData + 2;
                }
            }
        }
    }
    else
    {
        resultPtr = NULL;
    }
    return resultPtr;
}

/**********************************************************************
 * Function: inRange
 * - determines if the target object is in range of the Pixy frame.
 *   0 is in range, 1 turn right and 2 means turn left
 * argument:
 * Arguments: Pixy data
 *
 * return: PIXY_DATA pointer to the nearest object
 * Author: Mohammad Ali Sharifi
 * Date: March 11, 2019
 * Modified: --
 **********************************************************************/
char inRange(PIXY_DATA *pixyData)
{
    char dir = 0;
    int max = 168;
    int min = 148;
    if (pixyData->x > max)
    {
        dir = 1;  // Turn right
    }
    else if (pixyData->x < min)
    {
        dir = 2;  // Turn left
    }
    else
    {
        dir = 0;  // Object in range
    }

    return dir;  // Return the direction
}

// USCI_A0_ISR is responsible to handle interrupt requested by TXIFG and RXIFG
// of USCI A0
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{

    switch (__even_in_range(UCA0IV, 4))
    {
    case 0:
        break;
    case 2:
        *(rxBuffer + rxCount++) = UCA0RXBUF; // Read UCA0RXBUF

        if (rxCount == 4) // Check if the pay load is read
        {
            if (!*(rxBuffer + 3)) // If pay load is zero (No object is detected)
            {
                MaxCount = 3; // So that it start from zero again
            }
            else if (*(rxBuffer + 3) == 14) // If 1 box in the frame
            {
                MaxCount = 19;
            }
            else if (*(rxBuffer + 3) == 28) // If 2 box in the frame
            {
                MaxCount = 33;
            }
            else                           // If 3 box in the frame
            {
                MaxCount = 47;
            }
        }
        if (rxCount > MaxCount)
        {
            rxCount = 0;
            response |= BIT0;
        }
        break;
    case 4:
        UCA0TXBUF = *(txBuffer + txCount++);
        //  transmit a byte by writing to TXBUFF
        if (txCount > 5)
        {
            txCount = 0;
            UCA0IE &= ~UCTXIE; // Disable interrupt for transmit Buffer A0
        }
        break;
    default:
        break;
    }
}


// Timer0 ISR vector
#pragma vector = TIMER0_A0_VECTOR
__interrupt void timerA0_ISR(void)
{
    char maxBlocks = 3;
    char sig = 1;
    txCount = 0;
    getBlocks(sig, maxBlocks);
    TA0CCTL0 &= ~CCIFG;
}


