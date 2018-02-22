/*
 * tunes.c
 *
 *  Created on: 14/02/2018
 *      Author: Miguel
 */

#include "tunes.h"

note_t tune_startup[] = {
	{D5, 80 },
	{E5, 80 },
	{F5, 80 },
	{A5, 80 },
	{D6, 80 },
	{F6, 80 },
	{B6, 80 },
	{E7, 80 },
	{G7, 200}
};

note_t tune_connected[] = {
	{D5, 100},
	{0,  80 },
	{E5, 100},
	{0,  80 },
	{C5, 100},
	{0,  80 },
	{C4, 100},
	{0,  80 },
	{G4, 220}
};

note_t note_startswitch = { C8, 250 };
note_t note_alert       = { A6, 500 };
note_t note_turn_found  = { C8, 100 };
