/*
 * calibrateMode.c
 *
 *  Created on: February 14, 2013
 *      Author: Andrew Bierman
 */

#include "msp430.h"
#include "timers.h"
#include "i2c.h"

unsigned int calLeft = 262, calRight = 262; 	// Irradiance calibration milliWatts/m^2 per DAC count

void calFlash(unsigned int irrad, unsigned int duration){ 	// irradiance in milliWatts/m^2, duration in milliseconds

	USBtiming = 1; 		// Can't go into LMP3 when connected to USB (set in USBEventHandling.c, but set here again to make sure)
	writeDAC(irrad); 				// Mask on
	tbStart((float)duration); 		// Pulse duration in milliseconds
	__bis_SR_register(LPM0_bits + GIE); 	// Enter LPM0 (USB timing)
	__no_operation();
	writeDAC(0); 					// Mask off
}


