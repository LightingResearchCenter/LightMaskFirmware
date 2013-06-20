/*
 * flashMemory.h
 *
 *  Created on: Jan 13, 2013
 *      Author: bierma2
 */

#ifndef FLASHMEMORY_H_
#define FLASHMEMORY_H_

#include "main.h"

void flashWriteUnsignedInt(unsigned int position, unsigned int data);
void flashWriteAlarm(unsigned int position, Calendar data);
void initFromFlash(void);


#endif /* FLASHMEMORY_H_ */
