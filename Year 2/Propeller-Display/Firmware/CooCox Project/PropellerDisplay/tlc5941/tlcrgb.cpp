/*
 * tlcrgb.c
 *
 *  Created on: Nov 18, 2015
 *      Author: Miguel
 */

#include "tlcrgb.h"

TLC5941 TLCRGB::tlc;

void TLCRGB::init(int rgb_index) {
	index = rgb_index;
	rgb_off = index * 3;
	tlc.init(); /* will be ignored if the TLC5941 has already been initialized once somewhere other than here */
}

TLCRGB::TLCRGB(int rgb_index) { init(rgb_index); }
TLCRGB::TLCRGB() {}

void TLCRGB::updateColor() {
	tlc.setChannel(rgb_off, (uint16_t)rgb.r);
	tlc.setChannel(rgb_off + 1, (uint16_t)rgb.g);
	tlc.setChannel(rgb_off + 2, (uint16_t)rgb.b);
}

void TLCRGB::updateColor(uint16_t r, uint16_t g, uint16_t b) {
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;
	updateColor();
}

void TLCRGB::updateColor(rgb_t rgb_struct) {
	set(rgb_struct);
	updateColor();
}

void TLCRGB::updateAll() {
	updateColor();
	TLC5941::update();
}

void TLCRGB::updateAll(rgb_t rgb_struct) {
	updateColor(rgb_struct);
	TLC5941::update();
}

void TLCRGB::set(rgb_t rgb_struct) {
	rgb.r = rgb_struct.r;
	rgb.g = rgb_struct.g;
	rgb.b = rgb_struct.b;
}

void TLCRGB::set(uint16_t r, uint16_t g, uint16_t b) {
	rgb.r = r;
	rgb.g = g;
	rgb.b = b;
}

void TLCRGB::setR(uint16_t r) {
	rgb.r = r;
}

void TLCRGB::setG(uint16_t g) {
	rgb.g = g;
}

void TLCRGB::setB(uint16_t b) {
	rgb.b = b;
}

TLC5941 TLCRGB::getTLC5941() {
	return tlc;
}

void TLCRGB::led_test() {
	#define LED_COUNT 3
	TLCRGB leds[LED_COUNT];

	#define update_all_leds() for(int i = 0; i < LED_COUNT; i++) leds[i].updateColor();

	for(int i = 0; i < LED_COUNT; i++)
		leds[i].init(i);

	for(;;) {
		INC(leds[0].rgb.r, 0.1f, 0xFF, 0);
		INC(leds[1].rgb.g, 0.2f, 0xFF, 0);
		INC(leds[2].rgb.b, 0.4f, 0xFF, 0);

		update_all_leds();
		TLC5941::update();
	}
}
