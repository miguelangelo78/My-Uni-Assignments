#include "dsp_functions.h"
#include "output_plot.h"

void question1_c(int dft_points, bool use_hann_window, bool use_double_sided_spectrum)
{
	/* Input signal properties */
	#define SIGNAL_AMPLITUDE          20
	#define SIGNAL_FREQUENCY          500
	#define SIGNAL_SAMPLING_FREQUENCY 15000
	char input_name[128];
	sprintf(
		input_name, "%s - Square Wave [f = %d Hz fs = %d Hz amplitude = %d and %d points] %s(input)",
		dft_points == 256 ? (!use_hann_window ? "Q1C1" : "") : (!use_hann_window ? "Q21C1" : "Q22C1"),
		SIGNAL_FREQUENCY, SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);

	/* Create input signal (square wave) */
	float * input_squarewave = create_squarewave(dft_points, 0, (float)dft_points, 0, SIGNAL_AMPLITUDE, SIGNAL_FREQUENCY, SIGNAL_SAMPLING_FREQUENCY);
	
	/* Apply a Hann Window filter if the user specifies so */
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
			input_squarewave[n] *= hann_window[n];
	}
	
	/* Display to the console our input signal */
	display_signal(input_name, input_squarewave, dft_points);

	/* Output signal properties (the DFT spectrum) */
	char output_name[256];
	char output_name_linear[256];
	char output_name_dB[256];

	/* Format the plot filenames for each question and for the linear and dB formats */
	sprintf(
		output_name, "%s - DFT spectrum [%d points %s] of Square Wave [f = %d Hz fs = %d Hz amplitude = %d and %d points] %s(output)",
		SSB_OR_DSB, dft_points, SSB_OR_DSB,
		SIGNAL_FREQUENCY, SIGNAL_SAMPLING_FREQUENCY, SIGNAL_AMPLITUDE,
		dft_points, use_hann_window ? "with Hann window " : ""
	);
	sprintf(output_name_linear, "%s%s",    dft_points == 256 ? (!use_hann_window ? "Q1C2" : "") : (!use_hann_window ? "Q21C2" : "Q22C2"), output_name);
	sprintf(output_name_dB,     "%s%s dB", dft_points == 256 ? (!use_hann_window ? "Q1C3" : "") : (!use_hann_window ? "Q21C3" : "Q22C3"), output_name);

	/* Perform DFT on the input signal */
	float * dft_spectrum_output    = DFT(input_squarewave, dft_points, dft_points);
	float * dft_spectrum_output_db = convert_to_log_scale(dft_spectrum_output, dft_points);
	display_signal(output_name_dB, dft_spectrum_output_db, dft_points);
	display_spectrum_stats(dft_spectrum_output_db, dft_points, 14);
	
	/* Output all the signals to a CSV file named 'Output.csv' */
	#define CSV_FILENAME "OutputData/Output.csv"
	
	/* If we are NOT using Hann window then we'll output 3 signals, otherwise, we output 6 (which includes the window) */
	unsigned int signal_output_count = !use_hann_window ? 3 : 6;

	export_to_csv(CSV_FILENAME, signal_output_count, SIGNAL_SAMPLING_FREQUENCY,
		/* For each signal we provide: the name, pointer, its size and the starting sample offset (always in this order) */
		input_name,                       input_squarewave,        dft_points, 0,
		output_name_linear,               dft_spectrum_output,     dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		output_name_dB,                   dft_spectrum_output_db,  dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_name,                 hann_window,             dft_points, 0,
		hann_window_spectrum_name_linear, hann_window_spectrum,    dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2,
		hann_window_spectrum_name_dB,     hann_window_spectrum_dB, dft_points, use_double_sided_spectrum ? -dft_points / 2 : dft_points / 2
	);

	/* Display all plots and export them into .jpg images and .tex */
	show_plots();

	/* Cleanup signals from the heap */
	free(input_squarewave);
	free(dft_spectrum_output);

	if (use_hann_window) {
		free(hann_window);
		free(hann_window_spectrum);
		free(hann_window_spectrum_dB);
	}
}
