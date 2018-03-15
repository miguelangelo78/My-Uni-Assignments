/*
 * app_template_generator.c
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#include <app_config.h>
#if ENABLE_TEMPLATE_GENERATION == 1

#include <debug.h>
#include <app_template_generator.h>

uint8_t sampled_template_sensor_data[TEMPLATE_MAX_SAMPLES];
uint8_t template_samples_collected = 0;
bool    finished_sampling = false;
bool    timed_out         = false;
bool    samples_dumped    = false;
bool    start_sampling    = false;
bool    is_sampling       = false;


void template_generator_begin(void) {
	start_sampling = true;
}

bool template_generator_is_finished(void) {
	return finished_sampling && !samples_dumped;
}

void template_generator_update(uint8_t sensor_data) {
	if(start_sampling)
		is_sampling = true;

	if(is_sampling && sensor_data == b00000000)
		finished_sampling = true;

	if(!finished_sampling && is_sampling && rtos_get_timeout(false, false, 10, 0)) {
		sampled_template_sensor_data[template_samples_collected] = sensor_data;

		if(++template_samples_collected > TEMPLATE_MAX_SAMPLES) {
			finished_sampling = true;
			timed_out         = true;
		}
	}
}

bool template_generator_dump(void) {
	if(samples_dumped)
		return true;

	DEBUG(">> \nSamples collected: %d (timed out? -> %d)\n", template_samples_collected, timed_out);
	for(int i = 0; i < template_samples_collected; i++)
		printf("\n%d", sampled_template_sensor_data[i]);
	samples_dumped = true;

	return false;
}

#endif
