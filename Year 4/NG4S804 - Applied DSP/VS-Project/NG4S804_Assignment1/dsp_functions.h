#pragma once

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define PI 3.14159265358979323846

enum WINDOW_TYPE {
	NO_WINDOW,
	WINDOW_HANN,
	WINDOW_WELCH,
	WINDOW_GAUSSIAN
};

float * DFT(float * input, unsigned int input_size, unsigned int bin_count);
float * create_sinewave(unsigned int size, float x_low, float x_high, float amplitude, float frequency, float sampling_frequency);
float * create_squarewave(unsigned int size, float x_low, float x_high, float y_low, float y_high, float frequency, float sampling_frequency);
float * create_window(unsigned int window_size, enum WINDOW_TYPE window_type);
float * convert_to_log_scale(float * input, unsigned int input_size);
void display_spectrum_stats(float * spectrum, unsigned int bin_count, unsigned int maximum_peak_count);