/***************************************************************/
/*                                                             */
/*      PROJECT NAME :  Smart Car                              */
/*      FILE         :  kmain.c                                */
/*      DESCRIPTION  :  Main Program                           */
/*      CPU SERIES   :  RX600                                  */
/*      CPU TYPE     :  RX63N                                  */
/*                                                             */
/*      This file is generated by e2 studio.                   */
/*                                                             */
/***************************************************************/

/************************************/
/** Author: Miguel Santos 14031329 **/
/************************************/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <platform.h>
#include <globals.h>

/****************************************************************/
/** Declare the main hardware components used in this project: **/
/****************************************************************/
suart_t    * module_bluetooth;
suart_t    * module_gps;
irrem_t    * module_infrared;
usonic_t   * module_usonic1;
usonic_t   * module_usonic2;
dhall_t    * module_hallsensor;
ltracker_t * module_ltracker;
sled_t     * module_status_led;

/***************************************/
/** This task is running at 101.8KHz: **/
/***************************************/
void poller(void) {
	suart_poll();  /* Poll the Bluetooth and GPS   */
	usonic_poll(); /* Poll the Ultrasonic sensors */
	spwm_poll();   /* Poll the SPWM library (used by the Status LED) */
	irrem_poll();  /* Poll the Infrared receiver   */
}

void bluetooth_on_rx(uint8_t * buff, uint32_t bufflen) {
	robot_on_bt(buff, bufflen); /* Redirect the Bluetooth reception packet to the robot application */
}

void gps_on_rx(uint8_t * buff, uint32_t bufflen) {
	/* TODO: Implement GPS parser library and use it on the robot application */
}

void infrared_on_rx(uint16_t * buff, uint32_t bufflen) {
	robot_on_ir(*buff); /* Redirect the Infrared reception packet to the robot application */
}

void dhall_on_detection(dhall_t * handle) {
	/* TODO: What shall I do with this? I don't have a real Use-Case for this sensor.
	 * Perhaps I should ask the teacher for advice. */
	suart_printf(module_bluetooth, "\n\rHALL DETECTED\n");
}

void kmain_early(void) {
	/* This function gets called before the RTOS starts running.
	 * It was implemented only because this project can also serve as a template
	 * RTOS project to be used in other projects.
	 * We won't do anything here for now... */
}

void kmain(void * args) {
	/* Initialize the motor driver for the H-Bridge L298N: */
	l298n_init();

	/* Initialize all Communication and Navigation channels: */
	module_bluetooth  = suart_init(SUART_MAX_BAUD, BTOOTH_RX,  BTOOTH_TX,          bluetooth_on_rx, 64, 30);
	module_gps        = suart_init(SUART_MAX_BAUD, GPS_RX,     (gpio_port_pin_t)0, gps_on_rx,       0,  30);
	module_infrared   = irrem_init(IRREM_RX, infrared_on_rx, 1);

	/* Initialize Ultrasonic sensor driver for two channels: */
	module_usonic1    = usonic_init(USONIC1_ECHO, USONIC1_TRIG, 1);
	module_usonic2    = usonic_init(USONIC2_ECHO, USONIC2_TRIG, 1);

	/* Initialize Accelerometer: */
	accel_init();

	/* Initialize Tracker sensor driver: */
	module_ltracker   = ltracker_init(TRACKER_LEFT, TRACKER_CENTER, TRACKER_RIGHT, NULL_CBACK, LTRACKER_MODE_LINE_BLACK);

	/* Initialize Hall Effect sensor driver: */
	module_hallsensor = dhall_init(DHALL_SIGNAL, dhall_on_detection, DHALL_MODE_ONESHOT);

	/* Initialize both Servo channels: */
	sg90_init_channels();

	/* Initialize Status LED: */
	module_status_led = status_led_init(SLED1_RED, SLED1_GREEN, SLED1_BLUE);

	/* Start polling! */
	OS_FastTickRegister(poller, 0);

	/* Launch User Application: */
	spawn_task("robot_task", start_robot);

	/* Do nothing for this task: */
	while(1)
		rtos_delay(1000);
}

void kmain_cleanup(void) {
	/*
	 * Nothing to cleanup for now...
	 * (Reason: the RTOS never lets the program reach this point)
	 */
}
