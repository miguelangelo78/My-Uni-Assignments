#include "dsp_functions.h"
#include "output_plot.h"

void question1_b(int dft_points, bool use_hann_window, bool use_double_sided_spectrum)
{
	/* Input signal properties */
	#define SIGNAL_AMPLITUDE          10
	#define SIGNAL_FREQUENCY1         500
	#define SIGNAL_FREQUENCY2         1000
	#define SIGNAL_FREQUENCY3         2000
	#define SIGNAL_SAMPLING_FREQUENCY 15000
	char input_name[256];
	sprintf(
		input_name, "%s - Mixed Sine Wave [f = %d Hz + %d Hz + %d Hz fs = %d Hz amplitude = %d and %d points] %s(input)",
		dft_points == 256 ? (!use_hann_window ? "Q1B1" : "") : (!use_hann_window ? "Q21B1" : "Q22B1"),
		SIGNAL_FREQUENCY1, SIGNAL_FREQUENCY2, SIGNAL_FREQUENCY3,
		SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);

	/* Create 3 sinewaves of different frequencies and a 4th signal which will store the mixed result */
	float * sin1 = create_sinewave(dft_points, 0, (float)dft_points, SIGNAL_AMPLITUDE, SIGNAL_FREQUENCY1, SIGNAL_SAMPLING_FREQUENCY);
	float * sin2 = create_sinewave(dft_points, 0, (float)dft_points, SIGNAL_AMPLITUDE, SIGNAL_FREQUENCY2, SIGNAL_SAMPLING_FREQUENCY);
	float * sin3 = create_sinewave(dft_points, 0, (float)dft_points, SIGNAL_AMPLITUDE, SIGNAL_FREQUENCY3, SIGNAL_SAMPLING_FREQUENCY);
	float * sin_mixed = (float*)malloc(sizeof(float) * dft_points);

	/* Mix the 3 signals and display the result */
	for (int n = 0; n < dft_points; n++)
		sin_mixed[n] = sin1[n] + sin2[n] + sin3[n];

	/* Apply a Hann Window filter on the mixed signal if the user specifies so */
    #define HANN_WINDOW_NAME          "HannWindow - Hann Window [512 points]"
	#define HANN_WINDOW_SPECTRUM_NAME "HannWindow%s - DFT spectrum of Hann Window [512 points %s]"
	char hann_window_name[128];
	char hann_window_spectrum_name_linear[128];
	char hann_window_spectrum_name_dB[128];
	float * hann_window             = NULL;
	float * hann_window_spectrum    = NULL;
	float * hann_window_spectrum_dB = NULL;

	if (use_hann_window) {
		sprintf(hann_window_name,                 HANN_WINDOW_NAME);
		sprintf(hann_window_spectrum_name_linear, HANN_WINDOW_SPECTRUM_NAME,       SSB_OR_DSB, SSB_OR_DSB);
		sprintf(hann_window_spectrum_name_dB,     HANN_WINDOW_SPECTRUM_NAME " dB", SSB_OR_DSB, SSB_OR_DSB);

		hann_window             = create_window(dft_points, WINDOW_HANN);
		hann_window_spectrum    = DFT(hann_window, dft_points, dft_points);
		hann_window_spectrum_dB = convert_to_log_scale(hann_window_spectrum, dft_points);

		/* Multiply input with window data */
		for (int n = 0; n < dft_points; n++)
			sin_mixed[n] *= hann_window[n];
	}

	/* Display to the console our mixed input signal */
	display_signal(input_name, sin_mixed, dft_points);

	/* Output signal properties (the DFT spectrum) */
	char output_name[256];
	char output_name_linear[256];
	char output_name_dB[256];
	
	/* Format the plot filenames for each question and for the linear and dB formats */
	sprintf(
		output_name, "%s - DFT spectrum [%d points %s] of 3 mixed Sine Waves [f = %d Hz %d Hz %d Hz fs = %d Hz amplitude = %d and %d points] %s(output)",
		SSB_OR_DSB, dft_points, SSB_OR_DSB,
		SIGNAL_FREQUENCY1, SIGNAL_FREQUENCY2, SIGNAL_FREQUENCY3,
		SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);
	sprintf(output_name_linear, "%s%s",    dft_points == 256 ? (!use_hann_window ? "Q1B12" : "") : (!use_hann_window ? "Q21B2" : "Q22B2"), output_name);
	sprintf(output_name_dB,     "%s%s dB", dft_points == 256 ? (!use_hann_window ? "Q1B13" : "") : (!use_hann_window ? "Q21B3" : "Q22B3"), output_name);

	/* Perform DFT on the input signal */
	float * dft_spectrum_output    = DFT(sin_mixed, dft_points, dft_points);
	float * dft_spectrum_output_db = convert_to_log_scale(dft_spectrum_output, dft_points);
	display_signal(output_name_dB, dft_spectrum_output_db, dft_points);
	display_spectrum_stats(dft_spectrum_output_db, dft_points, 3);
	
	/* Output all the signals to a CSV file named 'Output.csv' */
	#define CSV_FILENAME "OutputData/Output.csv"
	
	/* If we are NOT using Hann window then we'll output 3 signals, otherwise, we output 6 (which includes the window) */
	unsigned int signal_output_count = !use_hann_window ? 3 : 6;

	export_to_csv(CSV_FILENAME, signal_output_count, SIGNAL_SAMPLING_FREQUENCY,
		/* For each signal we provide: the name, pointer, its size and the starting sample offset (always in this order) */
		input_name,                       sin_mixed,               dft_points, 0,
		output_name_linear,               dft_spectrum_output,     dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		output_name_dB,                   dft_spectrum_output_db,  dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_name,                 hann_window,             dft_points, 0,
		hann_window_spectrum_name_linear, hann_window_spectrum,    dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_spectrum_name_dB,     hann_window_spectrum_dB, dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2
	);

	/* Display all plots and export them into .jpg images and .tex */
	show_plots();

	/* Cleanup signals from the heap */
	free(sin1);
	free(sin2);
	free(sin3);
	free(sin_mixed);
	free(dft_spectrum_output);

	if (use_hann_window) {
		free(hann_window);
		free(hann_window_spectrum);
		free(hann_window_spectrum_dB);
	}
}
