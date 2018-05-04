#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <windows.h>
#include "output_plot.h"
#include "dsp_functions.h"

/* DISCLAIMER: This file is only used for outputting signals into .csv files.
   It does not contain any information regarding DSP theory. */

#define PLOTS_PATH "..\\..\\..\\LaTeX\\res\\plots"

char spectrum_side_type[2][4] = { "SSB", "DSB" };

bool dir_exists(const char* dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in);
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	return false;
}

void display_signal(char * message, float * signal, unsigned int size)
{
	printf("\n****************** Signal: %s ******************\n", message);
	for (unsigned int n = 0; n < size; n++)
		printf("%.4f\n", signal[n]);
	printf("************************************\n");
}

void export_to_csv(char * csv_filename, unsigned int signal_count, unsigned int sampling_frequency, ...)
{
	char * filename = dir_exists("OutputData\\gnuplot") ? csv_filename : "OutputData.csv";
	FILE * csv_file = fopen(filename, "w");
	va_list va_args;

	char csv_line[256] = "";

	/* Print header cells into the CSV file */
	va_start(va_args, sampling_frequency);
	for (unsigned int i = 0; i < signal_count; i++) {
		char  * signal_name = va_arg(va_args, char*);
		va_arg(va_args, float*);
		va_arg(va_args, unsigned int);
		va_arg(va_args, int);

		fprintf(csv_file, "%s%s%s", i > 0 ? "k,f," : ",n,", signal_name, i < signal_count - 1 ? "," : "");
	}
	va_end(va_args);

	fprintf(csv_file, "\n");

	/* Print the entire sample data for all signals row by row */
	for (unsigned int sample_idx = 0, done_storing = 0; done_storing < signal_count; sample_idx++) {
		done_storing = 0;

		va_start(va_args, sampling_frequency);
		for (unsigned int i = 0; i < signal_count; i++) {

			/* Grab the signal pointers and its size */
			va_arg(va_args, char*);
			float *      signal            = va_arg(va_args, float*);
			unsigned int sample_count      = va_arg(va_args, unsigned int);
			int          sample_offset     = va_arg(va_args, int);
			int          sample_offset_idx = sample_offset < 0 ? 0 : sample_offset;

			int   bin_off       = sample_offset < 0 ? (int)(sample_count / 2) : 0;
			float frequency_off = sample_offset < 0 ? (float)(sampling_frequency / 2) : 0;

			if (sample_idx + sample_offset_idx < sample_count) {
				if(i > 0 && i != 3)
					sprintf(csv_line, "%s%d,%.4f,%.4f%s", csv_line, sample_idx - bin_off, (float)((sample_idx * sampling_frequency) / sample_count - frequency_off), signal[sample_idx + sample_offset_idx], i < signal_count - 1 ? "," : "");
				else
					sprintf(csv_line, "%s,%.4f,%.4f%s", csv_line, (float)((float)(sample_idx + sample_offset) / (sampling_frequency) * 1000), signal[sample_idx + sample_offset_idx], i < signal_count - 1 ? "," : "");
			} else {
				sprintf(csv_line, "%s,,%s", csv_line, i < signal_count - 1 ? "," : "");
				done_storing++;
			}
		}
		va_end(va_args);

		if (done_storing < signal_count)
			fprintf(csv_file, "%s\n", csv_line);

		csv_line[0] = '\0';
	}
	fclose(csv_file);

	printf("\n>> All signals were exported to the CSV file: '%s'", filename);
}

void show_plots(void)
{
	if(!dir_exists("OutputData\\gnuplot"))
		return;

	printf("\n>> Displaying plots...");
	system("mkdir res\\plots >nul 2>&1");
	system("OutputData\\gnuplot\\bin\\gnuplot.exe --persist OutputData\\gnuplot\\default.gnuplot");
	printf("\n>> Moving .tex plots into: '" PLOTS_PATH "'...");
	system("move /y res\\plots\\*.tex " PLOTS_PATH " >nul 2>&1");
	system("move /y res\\plots\\*.eps " PLOTS_PATH " >nul 2>&1");
	system("rmdir /s /q res");
	printf("\n>> Exported all plots into separate .jpg and .tex files...");
}
