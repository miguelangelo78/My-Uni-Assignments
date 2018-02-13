/*
 * piezo.h
 *
 *  Created on: 01/02/2018
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_SOUND_PIEZO_H_
#define LIB_DRIVERS_SOUND_PIEZO_H_

typedef struct {

} piezo_t;

enum PIEZO_RETCODE {
	PIEZO_OK,
	PIEZO_ERR,
	PIEZO_ERR__COUNT
};

piezo_t * piezo_init(void);
enum PIEZO_RETCODE piezo_play(uint8_t);

#endif /* LIB_DRIVERS_SOUND_PIEZO_H_ */
