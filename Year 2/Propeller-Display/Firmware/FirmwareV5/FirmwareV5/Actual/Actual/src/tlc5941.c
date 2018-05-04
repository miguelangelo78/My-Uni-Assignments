/*
 * tlc5941.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: Miguel
 */

#include "tlc5941.h"
#include "port.h"
#include <string.h>

/* TLC5941 Datasheet: http://www.ti.com/lit/ds/symlink/tlc5941.pdf */

/* Channel sizes and counts: */
#define TLC5941_COUNT 1
#define CHANNEL_COUNT 16 /* How many channels per TLC5941 chip */
#define TLC5941_DATASIZE TLC5941_COUNT * CHANNEL_COUNT
#define GS_CHANNEL_LEN 12 /* Each GS channel is 12 bits */
#define DC_CHANNEL_LEN 6 /* Each DC channel is 6 bits */
#define GS_SIZE GS_CHANNEL_LEN * CHANNEL_COUNT /* Total size in bits of Grayscale PWM control */
#define DC_SIZE DC_CHANNEL_LEN * CHANNEL_COUNT /* Total size in bits of Dot Correction */

/* Helper macros for controlling the GPIO: */
#define readpin(pin) PABI((pin)) /* Reads state of pin, same as PABI, but forcing the port */
#define PULSE(pin) { PAB(pin, 1); PAB(pin, 0); } /* Send a pulse to a pin */
#define SET_BLANK(blank) PAB(BLANK, blank)
#define SET_MODE(md) PAB(MODE, md)

TLC5941 * this_tlc = 0; /* Used for statically updating the device. */

/* There is no need for hardcoding all the 1's and 0's here. This is here only for reference purpose */
/* Dot correction data: */
uint8_t dcData[TLC5941_DATASIZE];
/* Example representation:
 = {
	 // MSB LSB
	0b111111, // Channel 15
	0b111111, // Channel 14
	0b111111, // Channel 13
	0b111111, // Channel 12
	0b111111, // Channel 11
	0b111111, // Channel 10
	0b111111, // Channel 9
	0b111111, // Channel 8
	0b111111, // Channel 7
	0b111111, // Channel 6
	0b111111, // Channel 5
	0b111111, // Channel 4
	0b111111, // Channel 3
	0b111111, // Channel 2
	0b111111, // Channel 1
	0b111111  // Channel 0
}; */

/* Grayscale data */
uint16_t gsData[TLC5941_DATASIZE];
/* Example representation:
 = {
	// MSB     LSB
	0b000000000000, // Channel 15
	0b000000000000, // Channel 14
	0b000000000000, // Channel 13
	0b000000000000, // Channel 12
	0b000000000000, // Channel 11
	0b000000000000, // Channel 10
	0b000000000000, // Channel 9
	0b000000000000, // Channel 8
	0b000000000000, // Channel 7
	0b000000000000, // Channel 6
	0b000000000000, // Channel 5
	0b000000000000, // Channel 4
	0b000000000000, // Channel 3
	0b000000000000, // Channel 2
	0b000000000000, // Channel 1
	0b000000000000  // Channel 0
};*/

void init() {
	if(!this_tlc) {
		init_gpio(GPIOA, 0, MODE | SIN | SCLK | XLAT | BLANK, XERR);
		PA(0);

		SET_MODE(MD_DC);
		SET_BLANK(1);

		/* Default dot correction and gs data: */
		memset(dcData, 0b000011, TLC5941_DATASIZE);
		memset(gsData, 0, TLC5941_DATASIZE);

		/* Send default dot correction data: */
		sendDot();
		this_tlc = this;
	}
}

uint8_t TLC5941::getXERR() {
	return readpin(XERR);
}

/* Sets dot correction value (from 0 to 63) for a certain channel */
void TLC5941::setDot(uint8_t channel, uint8_t dot_val) {
	dcData[(CHANNEL_COUNT - 1) - channel] = dot_val;
}

/* Sets brightness value (from 0 to 4095, 12 BITS (IMPORTANT)) for a certain channel */
void TLC5941::setChannel(uint8_t channel, uint16_t brightness) {
	gsData[(CHANNEL_COUNT - 1) - channel] = brightness;
}

/* Sets brightness and updates the device in one go */
void TLC5941::setUpdate(uint8_t channel, uint16_t brightness) {
	setChannel(channel, brightness);
	update();
}

#define fetch_bit(indexed_array, nthbit) (indexed_array & (1 << nthbit)) >> nthbit;

/* Send dot correction data: */
void TLC5941::sendDot(void) {
	uint8_t channel = 0;
	int16_t mask = DC_CHANNEL_LEN - 1;

	SET_MODE(MD_DC);

	for(uint8_t ctr = 0; ctr <= TLC5941_COUNT * DC_SIZE - 1; ctr++) {
		char val = fetch_bit(dcData[channel], mask);

		if(--mask < 0) {
			mask = DC_CHANNEL_LEN - 1;
			channel++;
		}

		PAB(SIN, val);
		PULSE(SCLK);
	}
	PULSE(XLAT);
}

/* Send grayscale data: */
void TLC5941::sendGS(void) {
	uint8_t channel = 0;
	int16_t mask = GS_CHANNEL_LEN - 1;

	/* Start of GS cycle: */
	SET_MODE(MD_GS);

	for(uint16_t gsclk_ctr = 0; gsclk_ctr <= TLC5941_COUNT * GS_SIZE - 1; gsclk_ctr++) { /* gsclk_ctr shouldn't go as far as 4095 */
		char val = fetch_bit(gsData[channel], mask);

		if(--mask < 0) {
			mask = GS_CHANNEL_LEN - 1;
			channel++;
		}

		PAB(SIN, val);
		PULSE(SCLK);
	}

	/* End of GS cycle */
	PULSE(BLANK);
	PULSE(XLAT);
	PULSE(SCLK);
}

/* Updates the chip by outputting the GS data */
void TLC5941::update() {
	this_tlc->sendGS();
}
