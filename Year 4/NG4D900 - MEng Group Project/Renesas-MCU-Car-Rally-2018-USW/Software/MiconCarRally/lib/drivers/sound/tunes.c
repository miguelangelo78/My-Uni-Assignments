/*
 * tunes.c
 *
 *  Created on: 14/02/2018
 *      Author: Miguel
 */

#include "tunes.h"

note_t tune_startup[] = {
#if ENABLE_SOUND == 1
	{D5, 80 },
	{E5, 80 },
	{F5, 80 },
	{A5, 80 },
	{D6, 80 },
	{F6, 80 },
	{B6, 80 },
	{E7, 80 },
	{G7, 200}
#else
	{MIDI_NULL, 0}
#endif
};

note_t tune_connected[] = {
#if ENABLE_SOUND == 1
	{D5, 100},
	{0,  80 },
	{E5, 100},
	{0,  80 },
	{C5, 100},
	{0,  80 },
	{C4, 100},
	{0,  80 },
	{G4, 220}
#else
	{MIDI_NULL, 0}
#endif
};

note_t note_startswitch = { C8, 250 };
note_t note_alert       = { C8, 75  };
note_t note_turn_found  = { C8, 100 };
