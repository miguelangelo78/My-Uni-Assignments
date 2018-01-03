#include <stdio.h>
#include "sdcard_inc.h" /* include sdcard library */
#include "tlc5941/tlcrgb.h" /* include TLC5941 library (which includes rgb led handling) */
#include "cpufreq.h" /* include cpu overclock */
#include "port.h" /* include port / gpio handling header */
#include "debug.h" /* include debug.h, which helps indicate when there's problems with the program */
extern "C" {
#include "hc06/hc06.h"
}

class Propeller {
public:
	#define LED_COUNT 3 /* Will be 32 leds in the end */
	#define HC06_BAUD 115200
	#define HALL_PIN GPIO_Pin_0
	#define HALL_PORT GPIOD
	#define HALL_PORT_INDEX 3

	TLC5941 tlc;
	TLCRGB leds[LED_COUNT];
	//TLCRGB::rgb_t pix[1024]; /* Small buffer for storing an animation frame. The size of the buffer hasn't been calculated yet */
	FATFS fs;
	FIL file;

	Propeller() {
		/* Initialize everything: */
		SystemInit(); /* Normal init */
		rcc_set_frequency(SYSCLK_84_MHZ); /* Overclock CPU (i'll keep it at the original clock for now) */
		HC06_Init(HC06_BAUD); /* initialize early to allow debugging messages */
#if DEBUGMODE==1
		HC06_PutStr("Initializing Propeller Display...\n\r");
#endif
		/* Initialize Board's LEDs:
		 * (RED - Error | Green - Success and program ended | Orange - Initialized successfully) */
		init_board_debug();
		init_sdcard(); /* Initialize SD Card */
		hall_init();
		calibrate();
		tlc.init(); /* Initialize TLC5941 */
		/* Initialize LEDs: */
		for(int i = 0; i < LED_COUNT; i++) leds[i].init(i);
		/* Initialization success: */
		SET_BOARD_LED(BLUE);
#if DEBUGMODE==1
		HC06_PutStr("Propeller Display online\n\r");
#endif
	}

	void stop() {
		deinit_sdcard();
#if DEBUGMODE==1
		HC06_PutStr("Propeller Display offline\n\r");
#endif
		IDLE();
	}

	void px(char x, char y, uint16_t r, uint16_t g, uint16_t b) {
		/* TODO: px should apply the color to a space buffer, not directly to the leds */
		leds[(int)x].updateColor(r, g, b);
	}

	void update() {
		/* Update everything related to the propeller display here */

		/* TODO: For every update, a rotation probably happened. Because of this,
		 * we'll change the leds according to a certain buffer */

		tlc.update();
	}

	uint8_t hall_get(void) {
		return PDBI(HALL_PIN);
	}

private:
	uint8_t is_first_spin;

	void calibrate() {
		return;
#if DEBUGMODE==1
		HC06_PutStr("Calibrating...\n\r");
#endif
		uint8_t old_hall = is_first_spin;
		while(old_hall == hall_get()); /* Wait until the hall effect sensor toggles */
		is_first_spin = old_hall; /* store the toggled value */
	}

	void deinit_sdcard(void) {
		f_mount(0, "", 1);
		PA(0);
		printf("SD Card unmounted\n\n");
	}

	void init_sdcard(void) {
		if(f_mount(&fs, "", 1) != FR_OK) ERROR("SD Card failed!");
		printf("SD Card mounted\n\n");
	}

	void hall_init(void) {
		init_gpio(HALL_PORT, HALL_PORT_INDEX, 0, HALL_PIN);
		is_first_spin = hall_get();
	}
};

void init()
{
	SystemInit(); /* Normal init */
	rcc_set_frequency(SYSCLK_84_MHZ); /* Overclock CPU */
}

int main(void)
{
	init();

	TLCRGB::led_test();
}
