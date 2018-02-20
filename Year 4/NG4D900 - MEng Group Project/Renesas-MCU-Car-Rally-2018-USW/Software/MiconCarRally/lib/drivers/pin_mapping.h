/*
 * pin_layout.h
 *
 *  Created on: 16/02/2018
 *      Author: Miguel
 */

#ifndef LIB_PIN_MAPPING_H_
#define LIB_PIN_MAPPING_H_

#include <iodefine.h>

/************************************/
/******** MCU's Debug LEDs **********/
/************************************/
#define DIR_DBG_LED0 PORTA.DDR.BIT.B0
#define DAT_DBG_LED0 PORTA.DR.BIT.B0
#define DIR_DBG_LED1 PORTA.DDR.BIT.B1
#define DAT_DBG_LED1 PORTA.DR.BIT.B1
#define DIR_DBG_LED2 PORTA.DDR.BIT.B2
#define DAT_DBG_LED2 PORTA.DR.BIT.B2
#define DIR_DBG_LED3 PORTA.DDR.BIT.B3
#define DAT_DBG_LED3 PORTA.DR.BIT.B3
/************************************/

/********************************/
/**** Power board's LED pins ****/
/********************************/
#define DIR_LED0 PORTB.DDR.BIT.B6
#define DAT_LED0 PORTB.DR.BIT.B6
#define DIR_LED1 PORTB.DDR.BIT.B7
#define DAT_LED1 PORTB.DR.BIT.B7
/********************************/

/************************************/
/*********** Start switch ***********/
/************************************/
#define DIR_STARTSW PORTB.DDR.BIT.B0
#define INP_STARTSW PORTB.PORT.BIT.B0
/************************************/

/*************************************/
/************* Start bar *************/
/*************************************/
#define DIR_STARTBAR PORT4.DDR.BIT.B0
#define INP_STARTBAR PORT4.PORT.BIT.B0
/*************************************/

/********************************/
/******* MCU's DIP switch *******/
/********************************/
#define INP_DIPSW PORT6.PORT.BYTE
/********************************/

/***********************************/
/********* IR line tracker *********/
/***********************************/
#define DIR_LTRACKER PORT4.DDR.BYTE
#define INP_LTRACKER PORT4.PORT.BYTE
/***********************************/

/*********************************/
/************* Servo *************/
/*********************************/
#define DIR_SERVO PORT7.DDR.BIT.B6
#define DAT_SERVO PORT7.DR.BIT.B6
/*********************************/

/************************************/
/************** Motors **************/
/************************************/
#define DIR_MOTOR_L  PORTB.DDR.BIT.B2
#define DAT_MOTOR_L  PORTB.DR.BIT.B2
#define DIR_MOTOR_LD PORTB.DDR.BIT.B4
#define DAT_MOTOR_LD PORTB.DR.BIT.B4

#define DIR_MOTOR_R  PORTB.DDR.BIT.B3
#define DAT_MOTOR_R  PORTB.DR.BIT.B3
#define DIR_MOTOR_RD PORTB.DDR.BIT.B5
#define DAT_MOTOR_RD PORTB.DR.BIT.B5
/************************************/

/***************************************/
/*** Hall effect sensor (RPM counter) **/
/***************************************/
#define DIR_LEFT_HALL  PORT1.DDR.BIT.B1
#define INP_LEFT_HALL  PORT1.PORT.BIT.B1
#define DIR_RIGHT_HALL PORT9.DDR.BIT.B6
#define INP_RIGHT_HALL PORT9.PORT.BIT.B6

/************************************/
/*********** Accelerometer **********/
/************************************/
#define DIR_ACCEL_X PORT9.DDR.BIT.B3
#define INP_ACCEL_X PORT9.PORT.BIT.B3
#define DIR_ACCEL_Y PORT9.DDR.BIT.B2
#define INP_ACCEL_Y PORT9.PORT.BIT.B2
/************************************/

/*************************************/
/********** Piezo buzzer *************/
/*************************************/
#define DIR_PIEZO     PORT9.DDR.BIT.B0
#define DAT_PIEZO     PORT9.DR.BIT.B0
#define DIR_PIEZO_GND PORT9.DDR.BIT.B1
#define DAT_PIEZO_GND PORT9.DR.BIT.B1
/*************************************/

/*************************************/
/************* Bluetooth *************/
/*************************************/
#define DIR_SUART_TX PORT9.DDR.BIT.B5
#define DAT_SUART_TX PORT9.DR.BIT.B5
#define DIR_SUART_RX PORT9.DDR.BIT.B4
#define DAT_SUART_RX PORT9.PORT.BIT.B4
/*************************************/

#endif /* LIB_PIN_MAPPING_H_ */
