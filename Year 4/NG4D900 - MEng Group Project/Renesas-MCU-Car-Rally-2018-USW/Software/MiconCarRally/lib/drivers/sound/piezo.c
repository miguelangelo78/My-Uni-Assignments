/*
 * piezo.c
 *
 *  Created on: 01/02/2018
 *      Author: Miguel
 */

#include <stdlib.h>
#include <platform.h>
#include <rtos_inc.h>
#include <utils.h>
#include <app_config.h>
#include "piezo.h"

bool is_piezo_init = false;

enum MIDI_LUT midi_lut_list[] = {
#if ENABLE_SOUND == 1
	A0, B0,
	C1, D1, E1, F1, G1, A1, B1,
	C2, D2, E2, F2, G2, A2, B2,
	C3, D3, E3, F3, G3, A3, B3,
	C4, D4, E4, F4, G4, A4, B4,
	C5, D5, E5, F5, G5, A5, B5,
	C6, D6, E6, F6, G6, A6, B6,
	C7, D7, E7, F7, G7, A7, B7,
	C8
#else
	MIDI_NULL
#endif
};

static void piezo_player(void * args) {
	piezo_t * piezo_handle = (piezo_t*)args;

	while(1) {
		while(!piezo_handle->is_playing)
			rtos_preempt();

		if(piezo_handle->note.pitch >= SOUND_MAX_FREQ)
			piezo_handle->note.pitch = SOUND_MAX_FREQ;

		/* Update sound with new pitch */
		spwm_set_frequency(piezo_handle->dev_handle, piezo_handle->note.pitch);

		for(int i = 0; i < piezo_handle->note.duration && !piezo_handle->stop_playing; i++)
			rtos_delay(1);

		/* Stop the tone */
		spwm_set_frequency(piezo_handle->dev_handle, 0);

		piezo_handle->is_playing   = false;
		piezo_handle->stop_playing = false;
	}
}

piezo_t * piezo_init(void) {
	if(is_piezo_init)
		return NULL;

	spwm_t * dev_handle = NULL;

#if ENABLE_SOUND == 1
	dev_handle = spwm_create(SOUND_MAX_FREQ, 0, SPWM_MODE_BOTHLVL, 0, SPWM_DEV_PIEZO);
#endif

	if(dev_handle == NULL)
		return NULL;

	piezo_t * ret = (piezo_t*)malloc(sizeof(piezo_t));
	ret->note.duration = 0;
	ret->note.pitch    = 0;
	ret->is_playing    = false;
	ret->dev_handle    = dev_handle;

	/* Set piezo pin as output */
	DIR_PIEZO = 1;

	/* Set second piezo pin as ground */
	DIR_PIEZO_GND = 1;
	DAT_PIEZO_GND = 0;

	/* Set volume to 100% */
	piezo_set_volume(ret, 100);

	/* Create task that handles the actual tune playback */
	rtos_spawn_task_args("piezo_player", piezo_player, ret);

	is_piezo_init = true;

	return ret;
}

enum PIEZO_RETCODE piezo_play(piezo_t * handle, note_t * note, bool serial) {
	if(!handle || !note) return PIEZO_ERR;

	if(serial) {
		while(handle->is_playing)
			rtos_preempt();
	} else {
		if(handle->is_playing)
			return PIEZO_BUSY;
	}

	handle->note.duration = note->duration;
	handle->note.pitch    = note->pitch;
	handle->is_playing    = true;

	return PIEZO_OK;
}

enum PIEZO_RETCODE piezo_play_song_async(piezo_t * handle, note_t * song, uint32_t sequence_count, bool serial) {
	if(!handle || !song) return PIEZO_ERR;

	static int sequence_num = 0;

	enum PIEZO_RETCODE ret;

	if((ret = piezo_play(handle, &song[sequence_num++], serial)) != PIEZO_OK && ret != PIEZO_SONG_DONE) {
		sequence_num = 0;
		return ret;
	}

	if(sequence_num >= sequence_count) {
		sequence_num = 0;
		return PIEZO_SONG_DONE;
	} else {
		return PIEZO_OK;
	}
}

enum PIEZO_RETCODE piezo_play_song_async_backwards(piezo_t * handle, note_t * song, uint32_t sequence_count, bool serial) {
	if(!handle || !song) return PIEZO_ERR;

	static int sequence_num_backwards = -2;

	if(sequence_num_backwards == -2)
		sequence_num_backwards = sequence_count - 1;

	enum PIEZO_RETCODE ret;

	if((ret = piezo_play(handle, &song[sequence_num_backwards--], serial)) != PIEZO_OK && ret != PIEZO_SONG_DONE) {
		sequence_num_backwards = sequence_count - 1;
		return ret;
	}

	if(sequence_num_backwards < 0) {
		sequence_num_backwards = sequence_count - 1;
		return PIEZO_SONG_DONE;
	} else {
		return PIEZO_OK;
	}
}

enum PIEZO_RETCODE piezo_stop(piezo_t * handle, bool stop_and_wait) {
	if(!handle) return PIEZO_ERR;

	handle->stop_playing = true;

	if(stop_and_wait)
		while(handle->is_playing)
			rtos_preempt();

	return PIEZO_OK;
}

enum PIEZO_RETCODE piezo_set_volume(piezo_t * handle, uint8_t volume) {
	if(!handle) return PIEZO_ERR;

	handle->volume = volume;

	spwm_set_duty(handle->dev_handle, (volume * 50) / 100);

	return PIEZO_OK;
}

enum MIDI_LUT midi_lut(uint8_t tone_index) {
	if(tone_index > midi_lut_count())
		return (enum MIDI_LUT)0;

	return midi_lut_list[tone_index];
}

uint16_t midi_lut_count(void) {
	return arraysize(midi_lut_list);
}
