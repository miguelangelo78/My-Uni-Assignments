/*
 * platform.h
 *
 *  Created on: 13/10/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ONCHIP_GPIO_R_GPIO_RX_SRC_PLATFORM_H_
#define LIB_DRIVERS_ONCHIP_GPIO_R_GPIO_RX_SRC_PLATFORM_H_

#include <iodefine.h>
#include <stdint.h>
#include <stdbool.h>

#define CPU_RESET() do { WDT.WRITE.WINA = 0x5AFF; WDT.WRITE.WINB = 0x5A5F; WDT.WRITE.WINA = 0xA5FD; } while(0);

#endif /* LIB_DRIVERS_ONCHIP_GPIO_R_GPIO_RX_SRC_PLATFORM_H_ */
