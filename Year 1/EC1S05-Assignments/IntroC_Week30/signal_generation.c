#include <conio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Signal global variables:
#define A 0
#define B 0
#define C 0
#define SIGNAL_PRECISION 10
#define SIGNAL_COUNT 	 3

// Signal generation types:
#define SINEWAVE   0
#define SQUAREWAVE 1
#define DCSIGNAL   2
#define NOISE 	   3

// Signal arithmetic types:
#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3

// Misc macros:
#define TRUE  1
#define FALSE 0
#define M_PI  3.14

void generate(float *signal,int size,float ampl,float t, char mode){
	int i;
	for(i=0;i<size;i++){
		switch(mode){
			case SINEWAVE: 
				signal[i] = ampl * (float)sin(2.f*M_PI*((i*1.f)/t));		
				break;
			case SQUAREWAVE: 
				signal[i] = (i%(int)t<t/2.f)?ampl:.0f;
				break;
			case DCSIGNAL:
				signal[i] = ampl; 
				break;
			case NOISE:
				signal[i] = rand()%(int)ampl*2-ampl;
				break;
		}
	}
}

void mix_signals(float *result,float *signal_a,float *signal_b,int size,char operation){
	int i;
	for(i=0;i<size;i++)
		switch(operation){
			case ADD: result[i] = signal_a[i] + signal_b[i]; break;
			case SUB: result[i] = signal_a[i] - signal_b[i]; break;
			case MUL: result[i] = signal_a[i] * signal_b[i]; break;
			case DIV: result[i] = signal_a[i] / signal_b[i]; break;
		}
}

void display_signal(float * arr,int length,char moredata){
	int i;
	for(i = 0;i<length;i++)
		if(moredata)
			printf("x = %d, y = %.2f\n",i,arr[i]);
		else 
			printf("%.2f\n",arr[i]);
}

void main(){
	float signals[SIGNAL_COUNT][SIGNAL_PRECISION],i;

	srand(time(NULL));
	
	printf("Sinewave (Signal A):\n");
	generate(signals[A],SIGNAL_PRECISION,5,2,SINEWAVE);
	display_signal(signals[A],SIGNAL_PRECISION,FALSE);

	printf("\nNoise (Signal B):\n");
	generate(signals[B],SIGNAL_PRECISION,5,10,NOISE);
	display_signal(signals[B],SIGNAL_PRECISION,FALSE);
	
	printf("\nMixed signals (Signal A + Signal B):\n");
	mix_signals(signals[C],signals[A],signals[B],SIGNAL_PRECISION,ADD);
	display_signal(signals[C],SIGNAL_PRECISION,FALSE);

	getch();
}
