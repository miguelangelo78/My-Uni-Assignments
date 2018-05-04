/*
 * switch.h
 *
 *  Created on: 14/11/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ONCHIP_SWITCH_H_
#define LIB_DRIVERS_ONCHIP_SWITCH_H_

#include <app_config.h>

#define start_switch_init() (DIR_STARTSW = 0)
#define start_switch_read() ((uint8_t)(~INP_STARTSW & 0x01))
#define dipswitch_read()    ((uint8_t)(INP_DIPSW & 0xF))

#endif /* LIB_DRIVERS_ONCHIP_SWITCH_H_ */
