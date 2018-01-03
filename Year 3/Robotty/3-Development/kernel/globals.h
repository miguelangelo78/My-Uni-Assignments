/*
 * globals.h
 *
 *  Created on: 24/02/2017
 *      Author: Miguel
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

/* ********************************************************************************************************************************************** */
/* VERY IMPORTANT NOTE ABOUT THE I2C PROTOCOL AND THE ACCELEROMETER + TEMPERATURE DRIVER WHEN USED IN AN RTOS ENVIRONMENT (specifically Micrium): */
/* ********************************************************************************************************************************************** */
/* - Problem:
 *     The I2C FIT module driver does not work properly on an environment that uses RTOS.
 *     Because of this, we will disable it from this project using the macro defined below.
 * - Cause:
 *     The I2C driver has too many problems due to the timing of the RTOS and the fact that the RIIC FIT Module uses an algorithm which blocks the thread.
 * This module was clearly not developed to be used together with an RTOS, and to be fair, shouldn't block the thread in the first place.
 * It should've been developed using asynchronous functions and callbacks (basically, event driven I2C).
 * Another solution would be to introduce task preemption faster than 1 ms (1KHz). This Microcontroller (RX63N) can obviously reach very high frequencies
 * using the PLL, so there's no excuse to limit the programmer to only 1KHz of preemption. At least allow 10KHz so that simple drivers for simples devices
 * (such as Infrared / Ultrasonic / UART) can be created without using the (fast) interrupt handler outside the "realm" of RTOS.
 * This preemption constraint SEVERELY hurts the reliability and quality of the RTOS.
 * Regardless of this problem, you can still set the macro below to 1 if you wish to do so. */
#define DRIVER_ENABLE_ACCELEROMETER (0)
/* ********************************************************************************************************************************************** */

#include <rtos_inc.h>
#include <drivers/communications/protocols/suart/suart.h>
#include <drivers/communications/protocols/i2c/i2c.h>
#include <drivers/communications/infrared/vs1838b.h>
#include <drivers/sensors/ultrasonic/hc_sr04.h>
#include <drivers/sensors/hall/ky003.h>
#include <drivers/sensors/ltracker/ltracker.h>
#include <drivers/sensors/temperature/adt7420.h>
#include <drivers/actuators/l298n/l298n.h>
#include <drivers/actuators/sg90/sg90.h>
#include <drivers/indicators/status_led/status_led.h>

/* Include application header: */
#include <robot/robot.h>

#if DRIVER_ENABLE_ACCELEROMETER == (1)
#include <drivers/sensors/accelerometer/adxl345.h>
#else
typedef struct { int16_t x, y, z, raw_x, raw_y, raw_z, slope; } accel_t;
#define accel_get_readings() ((accel_t *)0)
#define accel_init() (0)
#endif

/*********************************************************/
/** Pin Definitions for the Robot Car User Application: **/
/*********************************************************/
#define BTOOTH_RX      GPIO_PORT_2_PIN_4 /* P24 */
#define BTOOTH_TX      GPIO_PORT_E_PIN_3 /* PE3 */
#define ESP8266_RX     GPIO_PORT_E_PIN_1 /* PE1 */
#define ESP8266_TX     GPIO_PORT_E_PIN_0 /* PE0 */
#define GPS_RX         GPIO_PORT_D_PIN_7 /* PD7 */
#define IRREM_RX       GPIO_PORT_D_PIN_2 /* PD2 */
#define USONIC1_TRIG   GPIO_PORT_C_PIN_4 /* PC4 */
#define USONIC1_ECHO   GPIO_PORT_C_PIN_0 /* PC0 */
#define USONIC2_TRIG   GPIO_PORT_4_PIN_4 /* P44 */
#define USONIC2_ECHO   GPIO_PORT_4_PIN_1 /* P41 */
#define MOTOR_LEFT1    GPIO_PORT_E_PIN_4 /* PE4 */
#define MOTOR_LEFT2    GPIO_PORT_A_PIN_1 /* PA1 */
#define MOTOR_RIGHT1   GPIO_PORT_3_PIN_2 /* P32 */
#define MOTOR_RIGHT2   GPIO_PORT_E_PIN_5 /* PE5 */
#define SERVO1_XAXIS   GPIO_PORT_C_PIN_7 /* PC7 */
#define SERVO2_YAXIS   GPIO_PORT_C_PIN_6 /* PC6 */
#define DHALL_SIGNAL   GPIO_PORT_4_PIN_3 /* P43 */
#define TRACKER_LEFT   GPIO_PORT_E_PIN_6 /* PE6 */
#define TRACKER_CENTER GPIO_PORT_E_PIN_7 /* PE7 */
#define TRACKER_RIGHT  GPIO_PORT_C_PIN_5 /* PC5 */
#define SLED1_RED      GPIO_PORT_1_PIN_2 /* P12 */
#define SLED1_GREEN    GPIO_PORT_1_PIN_3 /* P13 */
#define SLED1_BLUE     GPIO_PORT_4_PIN_0 /* P40 */
/*********************************************************/
/*********************************************************/
/*********************************************************/

#define POLLFREQ    OS_CFG_TICK_RATE_HZ /* Frequency of the polling */
#define NULL_CBACK  (0)

extern suart_t    * module_bluetooth;
extern suart_t    * module_gps;
extern irrem_t    * module_infrared;
extern usonic_t   * module_usonic1;
extern usonic_t   * module_usonic2;
extern dhall_t    * module_hallsensor;
extern ltracker_t * module_ltracker;
extern sled_t     * module_status_led;

#endif /* GLOBALS_H_ */
