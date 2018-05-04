/*
 * LTRACKER.h
 *
 *  Created on: 08/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_SENSORS_LTRACKER_LTRACKER_H_
#define DRIVERS_SENSORS_LTRACKER_LTRACKER_H_

#include <stdbool.h>
#include <stdint.h>

#define LTRACKER_MAX_CHANNEL_COUNT 1

enum LTRACKER_ERR_CODE {
	LTRACKER_OK,
	LTRACKER_ERR_BADINIT,
	LTRACKER_ERR_BADARGS,
	LTRACKER_ERR_BADHANDLE,
	LTRACKER_ERR__COUNT /* How many different types of errors there are */
};

enum LTRACKER_MODE {
	LTRACKER_MODE_NULL,
	LTRACKER_MODE_LINE_WHITE, /* The tracker detects white lines on a black background */
	LTRACKER_MODE_LINE_BLACK, /* The tracker detects black lines on a white background */
	LTRACKER_MODE__COUNT
};

enum LTRACKER_LINEMASK {
	LTRACKER_LINEMASK_NULL,
	LTRACKER_LINEMASK_LEFT   = 1,
	LTRACKER_LINEMASK_CENTER = 2,
	LTRACKER_LINEMASK_RIGHT  = 4,
	LTRACKER_LINEMASK_COUNT  = 4
};

/* Forward declaration: */
struct ltracker;
typedef struct ltracker ltracker_t;

typedef void (*ltracker_on_change_t)(ltracker_t * handle, uint8_t which_line_changed);

typedef struct {
	bool inval :1;
	bool left  :1;
	bool center:1;
	bool right :1;
} ltracker_line_t;

typedef struct {
	gpio_port_pin_t pin;
	bool            bright; /* Is this pin detecting a bright object? */
} ltracker_pin_t;

struct ltracker {
	bool                 is_init;
	ltracker_pin_t       lpin;
	ltracker_pin_t       cpin;
	ltracker_pin_t       rpin;
	ltracker_on_change_t onchange_callback;
	enum LTRACKER_MODE   mode;
};

ltracker_t   *  ltracker_init(gpio_port_pin_t leftpin, gpio_port_pin_t centerpin, gpio_port_pin_t rightpin, ltracker_on_change_t onchange_callback, enum LTRACKER_MODE mode);
ltracker_line_t ltracker_get_line(ltracker_t * handle);

#endif /* DRIVERS_SENSORS_LTRACKER_LTRACKER_H_ */
