/*
 * spwm.h
 *
 *  Created on: 14/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_LIBS_SPWM_SPWM_H_
#define DRIVERS_LIBS_SPWM_SPWM_H_

#include <stdint.h>
#include <stdbool.h>

/*
 * SPWM: Software Pulse Width Modulation.
 * It's a general purpose PWM mechanism for executing any "task" / function following the pattern / frequency of a PWM signal.
 * Does not necessarily have to be related to an output pin to which a voltage would be generated out of.
 * Notice the frequency being used below. The value is this low for the following reason: the PWM is being generated inside an Interrupt Handler.
 * This library is therefore not intended to be used for high frequency devices. You can use this for slow / mechanical devices such as
 * LEDs and Keypads.
 */

#define SPWM_MAX_CHANNEL_COUNT SPWM_DEV__COUNT
#define SPWM_MAX_FREQUENCY     20000 /* In Hz */

typedef void (*spwm_cback_t)(uint8_t pinsample);

enum SPWM_ERR_CODE {
	SPWM_OK,
	SPWM_ERR_BADINIT,
	SPWM_ERR_BADARGS,
	SPWM_ERR_BADHANDLE,
	SPWM_ERR__COUNT /* How many different types of errors there are */
};

enum SPWM_MODE {
	SPWM_MODE_NULL,
	SPWM_MODE_POS      = 1, /* Triggers on positive edge / level.  */
	SPWM_MODE_NEG      = 2, /* Triggers on negative edge / level.  */
	SPWM_MODE_BOTHLVL  = SPWM_MODE_POS | SPWM_MODE_NEG, /* Triggers on both positive and negative edge / level */
	SPWM_MODE_REPEAT   = 4, /* Triggers repeatedly (if mode not selected, then it's in one shot mode). */
	SPWM_MODE_PININPUT = 8, /* Pin acts as input instead of output (if mode not selected, then pin is in output mode. For this mode, the cback can NOT be null). */
	SPWM_MODE__COUNT   = 6
};

/* What devices are using the Software PWM library */
enum SPWM_DEVALLOC {
	SPWM_DEV_DBGLED2,    /* MCU's LED2 for displaying reception of packets       */
	SPWM_DEV_DBGLED3,    /* MCU's LED2 for displaying transmission of packets    */
	SPWM_DEV_PIEZO,      /* A piezo buzzer for playing tunes in any given event */
	SPWM_DEV_SERVO,      /* The servo motor                                      */
	SPWM_DEV_LEFTMOTOR,  /* The left  motor of the car                           */
	SPWM_DEV_RIGHTMOTOR, /* The right motor of the car                           */
	SPWM_DEV_ACCELX,     /* The X axis of the accelerometer                      */
	SPWM_DEV_ACCELY,     /* The Y axis of the accelerometer                      */
	SPWM_DEV__COUNT      /* How many devices are being used for Software PWM     */
};

typedef struct {
	bool         is_init;
	uint32_t     current_counter; /* This counter will count until it reaches the match_counter                               */
	uint32_t     match_counter;   /* This counter marks the point at which the SPWM is triggered / the callbacks are executed */
	uint32_t     max_counter;     /* This is the max counter / frequency of the SPWM                                          */
	float        duty_cycle;      /* Used only for storage and recalculation                                                  */
	uint8_t      mode;
	spwm_cback_t cback;
	uint8_t      pin;
	bool         level_latch;
} spwm_t;

spwm_t *           spwm_create(uint32_t frequency_hz, float duty_cycle, uint8_t spwm_mode, spwm_cback_t cback, uint8_t outpin);
void               spwm_poll(void);
void               spwm_poll_all(void);
enum SPWM_ERR_CODE spwm_set_duty(spwm_t * handle, float duty_cycle);
enum SPWM_ERR_CODE spwm_set_frequency(spwm_t * handle, uint32_t frequency_hz);

#endif /* DRIVERS_LIBS_SPWM_SPWM_H_ */
