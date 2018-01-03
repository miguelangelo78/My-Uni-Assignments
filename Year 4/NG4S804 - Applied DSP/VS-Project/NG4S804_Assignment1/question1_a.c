#include "dsp_functions.h"
#include "output_plot.h"

void question1_a(int dft_points, bool use_hann_window, bool use_double_sided_spectrum)
{
	/* Input signal properties */
	#define SIGNAL_AMPLITUDE          10
	#define SIGNAL_FREQUENCY          2000
	#define SIGNAL_SAMPLING_FREQUENCY 15000
	char input_name[128];
	sprintf(
		input_name, "%s - Sine Wave [f = %d Hz fs = %d amplitude = %d and %d points] %s(input)",
		dft_points == 256 ? (!use_hann_window ? "Q1A1" : "") : (!use_hann_window ? "Q21A1" : "Q22A1"),
		SIGNAL_FREQUENCY, SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);

	/* Create input signal (sine wave) */
	float * input_sinewave = create_sinewave(
		dft_points,               /* The total size of the signal                                      */
		0,                        /* The starting point from which the signal begins (starts at x = 0) */
		(float)dft_points,        /* The end point at which the signal ends (ends at n = dft_points)   */
		SIGNAL_AMPLITUDE,         /* Amplitude of the signal                                           */
		SIGNAL_FREQUENCY,         /* Its frequency (Hz)                                                */
		SIGNAL_SAMPLING_FREQUENCY /* And finally, the sampling frequency (Hz) associated with it       */
	);

	/* Apply a Hann Window filter if the user specifies so */
	#define HANN_WINDOW_NAME          "HannWindow - Hann Window [512 points]"
	#define HANN_WINDOW_SPECTRUM_NAME "HannWindow%s - DFT spectrum of Hann Window [512 points %s]"
	char hann_window_name[128];
	char hann_window_spectrum_name_linear[128];
	char hann_window_spectrum_name_dB[128];
	float * hann_window             = NULL;
	float * hann_window_spectrum    = NULL;
	float * hann_window_spectrum_dB = NULL;
	
	if(use_hann_window) {
		sprintf(hann_window_name,                 HANN_WINDOW_NAME);
		sprintf(hann_window_spectrum_name_linear, HANN_WINDOW_SPECTRUM_NAME,       SSB_OR_DSB, SSB_OR_DSB);
		sprintf(hann_window_spectrum_name_dB,     HANN_WINDOW_SPECTRUM_NAME " dB", SSB_OR_DSB, SSB_OR_DSB);

		hann_window             = create_window(dft_points, WINDOW_HANN);
		hann_window_spectrum    = DFT(hann_window, dft_points, dft_points);
		hann_window_spectrum_dB = convert_to_log_scale(hann_window_spectrum, dft_points);

		/* Multiply input with window data */
		for(int n = 0; n < dft_points; n++)
			input_sinewave[n] *= hann_window[n];
	}

	/* Display to the console our input signal */
	display_signal(input_name, input_sinewave, dft_points);

	/* Output signal properties (the DFT spectrum) */
	char output_name[256];
	char output_name_linear[256];
	char output_name_dB[256];
	
	/* Format the plot filenames for each question and for the linear and dB formats */
	sprintf(
		output_name, "%s - DFT spectrum [%d points %s] of Sine Wave [f = %d Hz fs = %d Hz amplitude = %d and %d points] %s(output)",
		SSB_OR_DSB, dft_points, SSB_OR_DSB,
		SIGNAL_FREQUENCY, SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);
	sprintf(output_name_linear, "%s%s",    dft_points == 256 ? (!use_hann_window ? "Q1A2" : "") : (!use_hann_window ? "Q21A2" : "Q22A2"), output_name);
	sprintf(output_name_dB,     "%s%s dB", dft_points == 256 ? (!use_hann_window ? "Q1A3" : "") : (!use_hann_window ? "Q21A3" : "Q22A3"), output_name);

	/* Perform DFT on the input signal */
	float * dft_spectrum_output    = DFT(input_sinewave, dft_points, dft_points);
	float * dft_spectrum_output_db = convert_to_log_scale(dft_spectrum_output, dft_points);
	display_signal(output_name_dB, dft_spectrum_output_db, dft_points);
	display_spectrum_stats(dft_spectrum_output_db, dft_points, 1);

	/* Output all the signals to a CSV file named 'Output.csv' */
	#define CSV_FILENAME "OutputData/Output.csv"
	
	/* If we are NOT using Hann window then we'll output 3 signals, otherwise, we output 6 (which includes the window) */
	unsigned int signal_output_count = !use_hann_window ? 3 : 6;

	export_to_csv(CSV_FILENAME, signal_output_count, SIGNAL_SAMPLING_FREQUENCY,
		/* For each signal we provide: the name, pointer, its size and the starting sample offset (always in this order) */
		input_name,                       input_sinewave,          dft_points, 0,
		output_name_linear,               dft_spectrum_output,     dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		output_name_dB,                   dft_spectrum_output_db,  dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_name,                 hann_window,             dft_points, 0,
		hann_window_spectrum_name_linear, hann_window_spectrum,    dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_spectrum_name_dB,     hann_window_spectrum_dB, dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2
	);

	/* Display all plots and export them into .jpg images and .tex */
	show_plots();

	/* Cleanup signals from the heap */
	free(input_sinewave);
	free(dft_spectrum_output);
	free(dft_spectrum_output_db);

	if (use_hann_window) {
		free(hann_window);
		free(hann_window_spectrum);
		free(hann_window_spectrum_dB);
	}
}
