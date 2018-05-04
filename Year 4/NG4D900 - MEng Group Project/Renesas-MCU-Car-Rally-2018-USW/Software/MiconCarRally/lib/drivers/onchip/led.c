/*
 * led.c
 *
 *  Created on: 16/11/2017
 *      Author: Miguel
 */

#include <stddef.h>
#include <rtos_inc.h>
#include <libs/spwm/spwm.h>
#include <utils.h>
#include <app_config.h>
#include "led.h"

spwm_t * status_led1 = NULL;
spwm_t * status_led2 = NULL;

void debug_leds_update(int led_index) {
	switch(led_index) {
	case 0: DAT_DBG_LED0 = !DAT_DBG_LED0; break;
	case 1: DAT_DBG_LED1 = !DAT_DBG_LED1; break;
	case 2: DAT_DBG_LED2 = !DAT_DBG_LED2; break;
	case 3: DAT_DBG_LED3 = !DAT_DBG_LED3; break;
	}
}

void debug_leds_update_pwm(void) {
	static bool status_led_dir = false;
	#define max_brightness 1000
	static float brightness = max_brightness;
	static const float delta_brightness = 0.5f;
	static const int delta_update = 10;

	if(rtos_time() % delta_update == 0) {
		if(status_led_dir) {
			if(brightness >= max_brightness) {
				status_led_dir = !status_led_dir;
			} else {
				uint8_t mapped_brightness = map((brightness += delta_brightness), 0, max_brightness, 0, 100);
				spwm_set_duty(status_led1, mapped_brightness);
				spwm_set_duty(status_led2, mapped_brightness);
			}
		} else {
			if(brightness <= 0.0f) {
				status_led_dir = !status_led_dir;
			}
			else {
				uint8_t mapped_brightness = map((brightness -= delta_brightness), 0, max_brightness, 0, 100);
				spwm_set_duty(status_led1, mapped_brightness);
				spwm_set_duty(status_led2, mapped_brightness);
			}
		}
	}
}

void debug_leds_reset(int led_index) {
	switch(led_index) {
	case 0: DAT_DBG_LED0 = 1; break;
	case 1: DAT_DBG_LED1 = 1; break;
	case 2: DAT_DBG_LED2 = 1; break;
	case 3: DAT_DBG_LED3 = 1; break;
	}
}

void debug_leds_reset_all(void) {
	for(int i = 0; i < 4; i++)
		debug_leds_reset(i);
}

void debug_leds_init(void) {
	DIR_DBG_LED0 = DIR_DBG_LED1 = DIR_DBG_LED2 = DIR_DBG_LED3 = 1;
	debug_leds_reset_all();

	status_led1 = spwm_create(STATUS_LED_FREQ, 100, SPWM_MODE_BOTHLVL, 0, SPWM_DEV_DBGLED2);
	status_led2 = spwm_create(STATUS_LED_FREQ, 0,   SPWM_MODE_BOTHLVL, 0, SPWM_DEV_DBGLED3);
}

void motor_ctrl_board_led(uint8_t led_val) {
	led_val = ~led_val;
	DAT_LED0 = led_val & 0x01;
	DAT_LED1 = (led_val >> 1) & 0x01;
}

void motor_ctrl_board_led_init(void) {
	DIR_LED0 = DIR_LED1 = 1;
	motor_ctrl_board_led(0);
}
