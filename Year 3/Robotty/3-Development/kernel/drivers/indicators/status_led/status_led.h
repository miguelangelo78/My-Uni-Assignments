/*
 * status_led.h
 *
 *  Created on: 14/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_INDICATORS_STATUS_LED_STATUS_LED_H_
#define DRIVERS_INDICATORS_STATUS_LED_STATUS_LED_H_

#include <drivers/libs/spwm/spwm.h>

#define SLED_MAX_CHANNEL_COUNT    1 /* Each channel contains 3 separate RGB LEDs and each RGB LED has a single SPWM channel handler associated to it */
#define SLED_MAX_FREQUENCY        SPWM_MAX_FREQUENCY
#define SLED_DEFAULT_SPWM_MODE    SPWM_MODE_BOTHLVL
#define SLED_DEFAULT_NOTIFICATION STATUS_NOTE_OK
#define SLED_DEFAULT_ANIMATION    STATUS_ANIM_SLOWFADE

enum SLED_ERR_CODE {
	SLED_OK,
	SLED_ERR_BADINIT,
	SLED_ERR_BADARGS,
	SLED_ERR_BADHANDLE,
	SLED_ERR__COUNT /* How many different types of errors there are */
};

enum STATUS_NOTIFICATION_TYPE {
	STATUS_NOTE_INVAL,
	STATUS_NOTE_OK,      /* Green */
	STATUS_NOTE_INFO,    /* Blue */
	STATUS_NOTE_WARNING, /* Yellow */
	STATUS_NOTE_PANIC,   /* Red */
	STATUS_NOTE_IDLE,    /* Varies all 3 colors */
	STATUS_NOTE_DEMO,    /* Varies all 3 colors */
	STATUS_NOTE_DEBUG,   /* User defined. It is used only for debugging purposes */
	STATUS_NOTE__COUNT
};

enum STATUS_ANIMATION_TYPE {
	STATUS_ANIM_INVAL,
	STATUS_ANIM_NONE,
	STATUS_ANIM_SLOWFADE,
	STATUS_ANIM_FASTFADE,
	STATUS_ANIM_SLOWBLINK,
	STATUS_ANIM_FASTBLINK,
	STATUS_ANIM_FADEDHEARTBEAT,
	STATUS_ANIM_SHARPHEARTBEAT,
	STATUS_ANIM__COUNT
};

typedef struct {
	bool     is_init;
	spwm_t * red_led;
	spwm_t * green_led;
	spwm_t * blue_led;
	bool     anim_update_flag;
	enum STATUS_NOTIFICATION_TYPE notification;
	enum STATUS_ANIMATION_TYPE    animation;
} sled_t;

sled_t           * status_led_init(gpio_port_pin_t redpin, gpio_port_pin_t greenpin, gpio_port_pin_t bluepin);
enum SLED_ERR_CODE status_led_update(sled_t * handle, enum STATUS_NOTIFICATION_TYPE notification, enum STATUS_ANIMATION_TYPE animation);

#endif /* DRIVERS_INDICATORS_STATUS_LED_STATUS_LED_H_ */
