/*
 * i2c.h
 *
 *  Created on: 13/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_COMMUNICATIONS_PROTOCOLS_I2C_I2C_H_
#define DRIVERS_COMMUNICATIONS_PROTOCOLS_I2C_I2C_H_

/* Include the FIT headers for the I2C communication protocol: */
#include "r_riic_rx600/src/r_riic_rx600.h"
#include "r_riic_rx600/src/r_riic_rx600_master.h"

#define I2C_MASTER_IIC_ADDRESS_LO 0x20 /* Master address low bits [7:0] */
#define I2C_MASTER_IIC_ADDRESS_HI 0x00 /* Master address high bits [2:0] */
#define I2C_CHANNEL               CHANNEL_0
#define I2C_RW_BIT                0x01 /* Read / #write bit value. 1 = read. */

#endif /* DRIVERS_COMMUNICATIONS_PROTOCOLS_I2C_I2C_H_ */
