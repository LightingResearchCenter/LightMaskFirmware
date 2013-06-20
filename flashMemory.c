/*
 * flashMemory.c
 *
 *  Created on: Jan 13, 2013
 *      Author: bierma2
 *  Revised on June 20, 2013
 *  	Author: Andrew Bierman
 */

#include "msp430.h"
#include "main.h"
#include "flashMemory.h"
#include "calibrateMode.h"

#define INFO_D_SEGMENT_ADDR 0x001800

// Write an unsigned integer to flash memory
void flashWriteUnsignedInt(unsigned int position, unsigned int data){
	unsigned short bGIE;
	int i;
	unsigned char prevData[128];
	unsigned char* flashAddr = (unsigned char*)INFO_D_SEGMENT_ADDR;

	// Read the segment to preserve segment data
	for (i=0;i<128;i++){
		prevData[i] = *flashAddr++;
	}
	// Update data with new unsigned integer
	// Multiply position by 2 because integers saved are 2 bytes each, offset is zero for saved integers
	prevData[(position*2)] = (unsigned char)(data>>8); 	// High byte
	prevData[(position*2 + 1)] = (unsigned char)data; 	// Low byte

	// Write data back to flash
	bGIE  = (__get_SR_register() & GIE);  //save interrupt status
	__disable_interrupt();

	// Erase the segment
	flashAddr = (unsigned char*)INFO_D_SEGMENT_ADDR;
	FCTL3 = FWKEY;                            // Clear the lock bit
	FCTL1 = FWKEY+ERASE;                      // Set the Erase bit
	*flashAddr = 0;                           // Dummy write, to erase the segment

	// Write the data to the segment
	FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation
	for (i=0;i<128;i++){
		*flashAddr++ = prevData[i];                 // Write data to flash
	}

	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY+LOCK;                       // Set LOCK bit

	__bis_SR_register(bGIE); //restore interrupt status
}

// Write an alarm structure to flash memory
void flashWriteAlarm(unsigned int position, Calendar data){
	unsigned short bGIE;
	int i;
	unsigned char prevData[128];
	unsigned char* flashAddr = (unsigned char*)INFO_D_SEGMENT_ADDR;

	// Read the segment to preserve segment data
	for (i=0;i<128;i++){
		prevData[i] = *flashAddr++;
	}
	// Update data with new unsigned integer
	// multiply position by 8 because alarm saved are 8 bytes each, alarm data starts at offset 10
	prevData[(position*8  + 10)] = data.Seconds;
	prevData[(position*8  + 11)] = data.Minutes;
	prevData[(position*8  + 12)] = data.Hours;
	prevData[(position*8  + 13)] = data.DayOfWeek;
	prevData[(position*8  + 14)] = data.DayOfMonth;
	prevData[(position*8  + 15)] = data.Month;
	prevData[(position*8  + 16)] = ((data.Year)>>8); 	// High byte
	prevData[(position*8  + 17)] = (data.Year); 		// Low byte

	// Store the number of onTimes set (numalarms)
	if (position < 7){ // numAlarms counts only the number of onTime alarms
		prevData[122]  = (unsigned char)(position + 1); 	// 122 = (6+7)*8 + 18
	}

	bGIE  = (__get_SR_register() & GIE);  //save interrupt status
	__disable_interrupt();

	// Erase the segment
	flashAddr = (unsigned char*)INFO_D_SEGMENT_ADDR;
	FCTL3 = FWKEY;                            // Clear the lock bit
	FCTL1 = FWKEY+ERASE;                      // Set the Erase bit
	*flashAddr = 0;                           // Dummy write, to erase the segment

	// Write the data to the segment
	FCTL1 = FWKEY+WRT;                        // Set WRT bit for write operation
	for (i=0;i<128;i++){
		*flashAddr++ = prevData[i];                 // Write data to flash
	}

	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY+LOCK;                       // Set LOCK bit

	 __bis_SR_register(bGIE); //restore interrupt status
}

// Initialize parameters and alarms from info flash (segment D)
void initFromFlash(void){
	int i;
	unsigned char data[128];
	unsigned char* flashAddr = (unsigned char*)INFO_D_SEGMENT_ADDR;

	// Read the segment
	for (i=0;i<128;i++){
		data[i] = *flashAddr++;
	}
	pulseDur = (unsigned int)(data[1] + 256*data[0]);
	pulseInt = (unsigned int)(data[3] + 256*data[2]);
	pulseRep = (unsigned int)(data[5] + 256*data[4]);
	calLeft  = (unsigned int)(data[7] + 256*data[6]);
	calRight = (unsigned int)(data[9] + 256*data[8]);
	for (i=0;i<7;i++){
		on_Times[i].Seconds = data[i*8 + 10 + 0];
		on_Times[i].Minutes = data[i*8 + 10 + 1];
		on_Times[i].Hours = data[i*8 + 10 + 2];
		on_Times[i].DayOfMonth = data[i*8 + 10 + 4];
		on_Times[i].Month = data[i*8 + 10 + 5];
		on_Times[i].Year = data[i*8 + 10 + 7] + 256*data[i*8 + 10 + 6];

		offTimes[i].Seconds = data[i*8 + 66 + 0];  // 10 + (7*8) = 66
		offTimes[i].Minutes = data[i*8 + 66 + 1];
		offTimes[i].Hours = data[i*8 + 66 + 2];
		offTimes[i].DayOfMonth = data[i*8 + 66 + 4];
		offTimes[i].Month = data[i*8 + 66 + 5];
		offTimes[i].Year = data[i*8 + 66 + 7] + 256*data[i*8 + 66 + 6];
	}
	numAlarms = (unsigned int)(data[122]);
}
