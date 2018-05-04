/*
 * led.h
 *
 *  Created on: 14/11/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ONCHIP_LED_H_
#define LIB_DRIVERS_ONCHIP_LED_H_

#include <stdint.h>
#include <platform.h>

#define start_bar_read() ((uint8_t)(!(INP_STARTBAR & 0x01)))

void debug_leds_init(void);
void debug_leds_reset_all(void);
void debug_leds_reset(int led_index);
void debug_leds_update(int led_index);
void debug_leds_update_pwm(void);

void motor_ctrl_board_led(uint8_t led_val);
void motor_ctrl_board_led_init(void);

#endif /* LIB_DRIVERS_ONCHIP_LED_H_ */
