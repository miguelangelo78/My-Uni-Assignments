/*
 * piezo.h
 *
 *  Created on: 01/02/2018
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_SOUND_PIEZO_H_
#define LIB_DRIVERS_SOUND_PIEZO_H_

#include <stdint.h>
#include <stdbool.h>
#include <libs/spwm/spwm.h>

typedef struct {
	float    pitch;
	uint32_t duration;
} note_t;

typedef struct {
	spwm_t * dev_handle;
	note_t   note;
	float    volume;
	bool     is_playing;
	bool     stop_playing;
} piezo_t;

enum MIDI_LUT {
	MIDI_NULL = 0,
	A0 = 27,   B0 = 30,
	C1 = 32,   D1 = 36,   E1 = 41,   F1 = 43,   G1 = 49,   A1 = 55,   B1 = 61,
	C2 = 65,   D2 = 73,   E2 = 82,   F2 = 87,   G2 = 98,   A2 = 110,  B2 = 123,
	C3 = 130,  D3 = 146,  E3 = 164,  F3 = 174,  G3 = 196,  A3 = 220,  B3 = 246,
	C4 = 261,  D4 = 293,  E4 = 329,  F4 = 349,  G4 = 392,  A4 = 440,  B4 = 493,
	C5 = 523,  D5 = 587,  E5 = 659,  F5 = 698,  G5 = 784,  A5 = 880,  B5 = 987,
	C6 = 1046, D6 = 1174, E6 = 1318, F6 = 1397, G6 = 1568, A6 = 1760, B6 = 1975,
	C7 = 2093, D7 = 2349, E7 = 2637, F7 = 2793, G7 = 3136, A7 = 3520, B7 = 3951,
	C8 = 4186
};

enum PIEZO_RETCODE {
	PIEZO_OK,
	PIEZO_SONG_DONE,
	PIEZO_BUSY,
	PIEZO_ERR,
	PIEZO_ERR__COUNT
};

piezo_t *          piezo_init(void);
enum PIEZO_RETCODE piezo_play(piezo_t * handle, note_t * note, bool serial);
enum PIEZO_RETCODE piezo_play_song_async(piezo_t * handle, note_t * song, uint32_t sequence_count, bool serial);
enum PIEZO_RETCODE piezo_play_song_async_backwards(piezo_t * handle, note_t * song, uint32_t sequence_count, bool serial);
enum PIEZO_RETCODE piezo_stop(piezo_t * handle, bool stop_and_wait);
enum PIEZO_RETCODE piezo_set_volume(piezo_t * handle, float volume);

enum MIDI_LUT      midi_lut(uint8_t tone_index);
uint16_t           midi_lut_count(void);

#endif /* LIB_DRIVERS_SOUND_PIEZO_H_ */
