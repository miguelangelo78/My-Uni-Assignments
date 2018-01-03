#include <avr/io.h>
#include <stdlib.h>
#define F_CPU 10000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

#include "LCD/lcd.h"
#include "LCD/fonts.h"
#include "LCD/bmp.h"

#define VREF 5.0f
#define ADC_MAX 1024.0f

// Edge constants:
#define EDGE_SIZE 3
#define EDGE_LEFT EDGE_SIZE
#define EDGE_RIGHT SCRN_RIGHT-EDGE_SIZE
#define EDGE_TOP EDGE_SIZE-2
#define EDGE_BOT SCRN_BOTTOM-(EDGE_SIZE-2)

// Maximum deltas:
#define MAX_DELTA_T 2
#define MAX_DELTA_V 1.4f

// Channels for pots:
#define CHANNEL_DT 2
#define CHANNEL_DV 3

// Time and voltage declaration:
float t = EDGE_LEFT, delta_t = 0.5f, delta_v = MAX_DELTA_V;
float volt_list[5]={5.0f,-5.0f,2.5f,0.0f,-2.5f};

void clear_row(float x);
void update_screen(float volts);
float get_coord(float volt);
float read_adc(uint16_t channel);
float read_volt();
void draw_start();
void init_adc();
float map(float x, float in_min, float in_max, float out_min, float out_max);
void update_deltas();
void init_screen();
void init();

void clear_col(float x){
	int i;
	if(x<=EDGE_LEFT+ceil(delta_t)) return;
	for(i=EDGE_TOP+1;i<EDGE_BOT;i++)
		lcd_dot_clear(x,i);
}

void update_screen(float volts){
	if(t>=EDGE_RIGHT-delta_t) t=EDGE_LEFT;
	t+=delta_t;
	clear_col(t);
	lcd_dot_set(t,get_coord(volts));
	lcd_update_all();
}

float get_coord(float volt){
	// Convert volts to proper coordinates on the screen:
	return SCRN_BOTTOM-((volt*delta_v)*2+SCRN_BOTTOM/2);
}

float read_adc(uint16_t channel){
	ADMUX=(ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADSC));
	return ADC;
}

float read_volt(){
	float channels[2], voltage = .0f;
	uint16_t c, channel_count = 2;
	
	for(c=0;c<channel_count;c++)
		channels[c] = ((read_adc(c)*VREF)/ADC_MAX);
		
	voltage = channels[0]-channels[1];
	return voltage;
}

void draw_start(){
	int i,j;
	
	// Draw edge lines:
	for(i=EDGE_LEFT;i<EDGE_RIGHT;i++){
		lcd_dot_set(i,EDGE_TOP);
		lcd_dot_set(i,EDGE_BOT);
	}
	for(i=EDGE_TOP;i<EDGE_BOT;i++){
		lcd_dot_set(EDGE_LEFT,i);
		lcd_dot_set(EDGE_RIGHT,i);
	}
	
	// Draw voltage lines:
	for(j=0;j<2;j++)
		for(i=0;i<EDGE_SIZE;i++)
			lcd_dot_set(i,get_coord(volt_list[j]));
	
	for(j=2;j<5;j++)
		for(i=EDGE_SIZE-2;i<EDGE_SIZE;i++)
			lcd_dot_set(i,get_coord(volt_list[j]));
	
	t = EDGE_LEFT;
	
	lcd_update_all();
}

void init_adc(){
	ADMUX=(1<<REFS0);
	ADCSRA=(1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void update_deltas(){
	float temp_v,temp_t;
	// Delta_t:
	if(delta_t!=(temp_t = map(read_adc(CHANNEL_DT),0.0f,ADC_MAX,0.0f,MAX_DELTA_T))){
		init_screen();
		delta_t = temp_t;
	}
	
	// Delta_v:
	if(delta_v!=(temp_v=map(read_adc(CHANNEL_DV),0.0f,ADC_MAX,0.0f,MAX_DELTA_V))){
		init_screen();
		delta_v = temp_v;
	}
}

void init_screen(){
	lcd_erase();
	draw_start();
}

void init(){
	init_adc();
	lcd_init();
	update_deltas();
	init_screen();
}

int main(void) {
	init();
	while(1){
		update_screen(read_volt());
		update_deltas();
	}
}