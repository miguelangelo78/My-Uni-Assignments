/*
 * tunes.h
 *
 *  Created on: 14/02/2018
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_SOUND_TUNES_H_
#define LIB_DRIVERS_SOUND_TUNES_H_

#include "piezo.h"
#include <app_config.h>

#if ENABLE_SOUND == 1
extern note_t tune_startup[9];
extern note_t tune_connected[9];
extern note_t tune_lap_finished[3];
#else
extern note_t tune_startup[1];
extern note_t tune_connected[1];
extern note_t tune_lap_finished[1];
#endif
extern note_t note_startswitch;
extern note_t note_alert;
extern note_t note_turn_found;

#endif /* LIB_DRIVERS_SOUND_TUNES_H_ */
