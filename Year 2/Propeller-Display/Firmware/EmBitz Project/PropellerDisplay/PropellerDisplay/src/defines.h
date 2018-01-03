/**
 *  Defines for your entire project at one place
 *
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *	@website	http://stm32f4-discovery.com
 *	@version 	v1.0
 *	@ide		Keil uVision 5
 *	@license	GNU GPL v3
 *
 * |----------------------------------------------------------------------
 * | Copyright (C) Tilen Majerle, 2014
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */
#ifndef TM_DEFINES_H
#define TM_DEFINES_H

#define USE_USART3
#define TM_USART3_USE_CUSTOM_IRQ

/* SD Card defines: */
#define FATFS_USE_SDIO 0
#define FATFS_SPI SPI2
#define FATFS_SPI_PINSPACK TM_SPI_PinsPack_2
#define FATFS_CS_PORT GPIOB
#define FATFS_CS_PIN GPIO_Pin_12

/* LED Driver's SPI defines: */
#define TM_SPI1_PRESCALER	SPI_BaudRatePrescaler_2
#define USE_SPI1


#define TM_DELAY_TIM            TIM2
#define TM_DELAY_TIM_IRQ        TIM2_IRQn
#define TM_DELAY_TIM_IRQ_HANDLER    TIM2_IRQHandler
#endif
