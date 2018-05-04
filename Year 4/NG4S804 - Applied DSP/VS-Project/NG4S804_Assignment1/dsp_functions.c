#include <stdio.h>
#include "dsp_functions.h"

float * DFT(float * input, unsigned int input_size, unsigned int bin_count)
{
	/* In order to calculate the DFT, we'll create three signals /

	/* The real component of the DFT */
	float * dft_real = (float*)malloc(sizeof(float) * bin_count);
	/* The imaginary component */
	float * dft_imaginary = (float*)malloc(sizeof(float) * bin_count);
	/* And the magnitude between the two */
	float * dft_spectrum_output = (float*)malloc(sizeof(float) * bin_count);

	/* The frequency bins go to negative values -(N/2), so we'll calculate it here once */
	int bin = (bin_count / 2);

	for (int k = -bin, k_positive = 0; k < bin; k++, k_positive++) {
		/* Clear all magnitudes at this particular frequency bin */
		dft_real[k_positive]      = 0.0;
		dft_imaginary[k_positive] = 0.0;

		/* Add up all the coefficients multiplied with every single data sample */
		for (unsigned int n = 0; n < input_size; n++) {
			dft_real[k_positive]      += (float)(input[n] * cos(((2 * PI * k * n) / input_size)) / input_size);
			dft_imaginary[k_positive] += (float)(input[n] * sin(((2 * PI * k * n) / input_size)) / input_size);
		}

		/* And store the magnitude of the real and imaginary components */
		dft_spectrum_output[k_positive] = (float)((sqrt(pow(dft_real[k_positive], 2) + pow(dft_imaginary[k_positive], 2))));
	}

	/* We only care about the magnitude signal, 
	   so we'll cleanup the real and imaginary signals */
	free(dft_real);
	free(dft_imaginary);

	/* And finally return the DFT signal */
	return dft_spectrum_output;
}

float * create_sinewave(unsigned int size, float x_low, float x_high, float amplitude, float frequency, float sampling_frequency)
{
	/* Create sine wave array */
	float * output = (float*)malloc(sizeof(float) * size);

	unsigned int n = 0;

	/* Store sine wave data in the array using 'x' as an 'n' variable
	   that can go negative and 'n' the positive index to that sample */
	for (float x = x_low; x < x_high && n < size; x++, n++)
		output[n] = (float)(amplitude * sin(2 * PI * (frequency / sampling_frequency) * x));

	return output;
}

float * create_squarewave(unsigned int size, float x_low, float x_high, float y_low, float y_high, float frequency, float sampling_frequency)
{
	/* Create square wave array */
	float * output = (float*)malloc(sizeof(float) * size);

	unsigned int n = 0;
	
	/* In this case, the square wave uses a sine wave function to toggle its
	   amplitude when the sinusoid goes positive or negative. */
	for (float x = x_low; x < x_high && n < size; x++, n++)
		output[n] = sin(2 * PI * (frequency / sampling_frequency) * x) >= 0.0 ? y_high : y_low;

	return output;
}

float * create_window(unsigned int window_size, enum WINDOW_TYPE window_type)
{
	/* Create array for the window data */
	float * window = (float*)malloc(sizeof(float) * window_size);

	switch (window_type) {
		case WINDOW_HANN:
			/* Calculate each coefficient based on the 
			   Hann window formula and store it */
			for (unsigned int n = 0; n < window_size; n++)
				window[n] = (float)(0.5 * (1 - cos((2 * PI * n) / (window_size - 1))));
			break;
		case WINDOW_WELCH: {
			float a = (float)(window_size - 1) / 2;
			for (unsigned int n = 0; n < window_size; n++)
				window[n] = (float)(1 - pow((n - a) / a, 2));
			break;
		}
		case WINDOW_GAUSSIAN: {
			float deviation = 0.27f;
			float a = (float)((window_size - 1) / 2);
			for (unsigned int n = 0; n < window_size; n++)
				window[n] = (float)(exp(-0.5f * pow((n-a)/(deviation * a), 2)));
			break;
		}
	}

	return window;
}

float * convert_to_log_scale(float * input, unsigned int input_size)
{
	/* Create array that will hold a copy of the input
	   but with values in the log scale */
	float * input_copy = (float*)malloc(sizeof(float) * input_size);

	/* Convert the data */
	for(unsigned int n = 0; n < input_size; n++)
		input_copy[n] = (float)(20 * log10(input[n]));

	return input_copy;
}

float peaks_magnitude[20];
unsigned int peaks[20];
unsigned int peaks_ctr = 0;

void display_spectrum_stats(float * spectrum, unsigned int bin_count, unsigned int maximum_peak_count)
{
	/* Clean the peaks array first */
	for (unsigned int i = 0; i < 20; i++) {
		peaks[i] = 0;
		peaks_magnitude[i] = 0;
		peaks_ctr = 0;
	}
	
	/* Try to find as many peaks as possible */
	for (unsigned int i = 0; i < maximum_peak_count; i++) {
		float max = spectrum[0];
		unsigned int max_idx = 0;

		for (unsigned int k = 0; k < bin_count; k++) {
			if (spectrum[k] > max) {
				bool is_peak_stored = false;
				for (unsigned int i = 0; i < peaks_ctr; i++)
					if (peaks_magnitude[i] == spectrum[k])
						is_peak_stored = true;

				if (!is_peak_stored) {
					max = spectrum[k];
					max_idx = k;
				}
			}
		}

		if (max != 0) {
			peaks_magnitude[peaks_ctr] = max;
			peaks[peaks_ctr++] = max_idx;
		}
	}

	printf("\n>> SIGNAL STATISTICS <<\n>> DFT bin count: %d\n>> Peaks found: %d", bin_count, peaks_ctr);

	for (unsigned int k = 0, peak_no = 1; k < bin_count / 2; k++) {
		for (unsigned int j = 0; j < peaks_ctr; j++) {
			if (peaks[j] == k) {
				printf("\n>>> Peak # %d: k = %d | magnitude = %.2f dB", 
					(maximum_peak_count - peak_no++) + 1,
					(bin_count / 2) - k,
					peaks_magnitude[j]
				);
			}
		}
	}

	printf("\n");
}