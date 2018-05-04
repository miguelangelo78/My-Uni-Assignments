#pragma once

#include <stdio.h>

void display_signal(char * message, float * signal, unsigned int size);
void export_to_csv(char * csv_filename, unsigned int signal_count, unsigned int sampling_frequency, ...);
void show_plots(void);

extern char spectrum_side_type[2][4];

#define SSB_OR_DSB (spectrum_side_type[use_double_sided_spectrum])