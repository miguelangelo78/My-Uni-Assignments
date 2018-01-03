/*
 * ky003.h
 *
 *  Created on: 08/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_SENSORS_HALL_KY003_H_
#define DRIVERS_SENSORS_HALL_KY003_H_

#include <stdbool.h>
#include <stdint.h>

#define DHALL_MAX_CHANNEL_COUNT 1

enum DHALL_RETCODE {
	DHALL_OK,
	DHALL_DETECTED,
	DHALL_NOTDETECTED,
	DHALL_ERR_BADINIT,
	DHALL_ERR_BADARGS,
	DHALL_ERR_BADHANDLE,
	DHALL_ERR__COUNT /* How many different types of errors there are */
};

enum DHALL_MODE {
	DHALL_MODE_NULL,
	DHALL_MODE_ONESHOT,    /* When a magnetic field is detected, the callback will be called only once, until the magnet is re-detected later on */
	DHALL_MODE_PERSISTENT, /* When a magnetic field is detected, the callback will be persistently called while the force is being detected */
	DHALL_MODE__COUNT
};

struct dhall;
typedef struct dhall dhall_t;

typedef void (*dhall_cback_t)(dhall_t * handle);

struct dhall {
	bool            is_init;
	gpio_port_pin_t signalpin;
	dhall_cback_t   cback;
	enum DHALL_MODE mode;
};

dhall_t * dhall_init(gpio_port_pin_t signalpin, dhall_cback_t cback, enum DHALL_MODE mode);
enum DHALL_RETCODE dhall_is_present(dhall_t * handle);

#endif /* DRIVERS_SENSORS_HALL_KY003_H_ */
