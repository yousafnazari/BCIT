/*
 * inc_SMCLK.h
 *
 *  Created on: Mar 12, 2019
 *      Author: a01010358
 */

#ifndef INC_SMCLK_H_
#define INC_SMCLK_H_

#include <msp430.h>

void SetVcoreUp (unsigned int level);

void smclkChange();

#endif /* INC_SMCLK_H_ */
