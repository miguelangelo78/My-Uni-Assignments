/*
 * switch.h
 *
 *  Created on: 14/11/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ONCHIP_SWITCH_H_
#define LIB_DRIVERS_ONCHIP_SWITCH_H_

#define start_switch_init() (PORT7.DDR.BIT.B0 = 0)
#define start_switch_read() ((uint8_t)(~PORT7.PORT.BIT.B0 & 0x01))
#define dipswitch_read() ((uint8_t)(PORT6.PORT.BYTE & 0xF))

#endif /* LIB_DRIVERS_ONCHIP_SWITCH_H_ */
