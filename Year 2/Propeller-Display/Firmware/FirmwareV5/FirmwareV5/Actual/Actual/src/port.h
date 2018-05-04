/*
 * port.h
 *
 *  Created on: Nov 16, 2015
 *      Author: Miguel
 */

#ifndef PORT_H_
#define PORT_H_

/* Port handling macros: */
#define PA(val) GPIOA->ODR = val; /* Output to port */
#define PAB(pin, val) GPIO_WriteBit(GPIOA, (pin), (BitAction)(val)); /* Output to port's pin */
#define PAI(pin) GPIO_ReadInputData(GPIOA); /* Read entire port */
#define PABI(pin) GPIO_ReadInputDataBit(GPIOA, (pin)); /* Read port's pin */

#define PB(val) GPIOB->ODR = val; /* Output to port */
#define PBB(pin, val) GPIO_WriteBit(GPIOB, (pin), (BitAction)(val)); /* Output to port's pin */
#define PBI(pin) GPIO_ReadInputData(GPIOB); /* Read entire port */
#define PBBI(pin) GPIO_ReadInputDataBit(GPIOB, (pin)); /* Read port's pin */

#define PC(val) GPIOC->ODR = val; /* Output to port */
#define PCB(pin, val) GPIO_WriteBit(GPIOC, (pin), (BitAction)(val)); /* Output to port's pin */
#define PCI(pin) GPIO_ReadInputData(GPIOC); /* Read entire port */
#define PCBI(pin) GPIO_ReadInputDataBit(GPIOC, (pin)); /* Read port's pin */

#define PD(val) GPIOD->ODR = val; /* Output to port */
#define PDB(pin, val) GPIO_WriteBit(GPIOD, (pin), (BitAction)(val)); /* Output to port's pin */
#define PDI(pin) GPIO_ReadInputData(GPIOD); /* Read entire port */
#define PDBI(pin) GPIO_ReadInputDataBit(GPIOD, (pin)); /* Read port's pin */

/* General purpose Port initializer function (one port at a time): */
inline void init_gpio(GPIO_TypeDef * gpio, uint8_t port_index, uint32_t out_pins, uint32_t in_pins) {
	/* Default Port initializations: */
	GPIO_InitTypeDef port_out;
	port_out.GPIO_Pin = out_pins;
	port_out.GPIO_Mode = GPIO_Mode_OUT;
	port_out.GPIO_OType = GPIO_OType_PP;
	port_out.GPIO_PuPd = GPIO_PuPd_DOWN;
	port_out.GPIO_Speed = GPIO_Speed_100MHz;

	GPIO_InitTypeDef port_in;
	port_in.GPIO_Pin = in_pins;
	port_in.GPIO_Mode = GPIO_Mode_IN;
	port_in.GPIO_OType = GPIO_OType_PP;
	port_in.GPIO_PuPd = GPIO_PuPd_DOWN;
	port_in.GPIO_Speed = GPIO_Speed_100MHz;

	/* Initialize port: */
	switch(port_index) {
	case 0: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); break;
	case 1: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); break;
	case 2: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); break;
	case 3: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE); break;
	case 4: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); break;
	case 5: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); break;
	case 6: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE); break;
	case 7: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE); break;
	case 8: RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOI, ENABLE); break;
	}

	if(out_pins) /* Won't initialize if out_pins is 0 */
		GPIO_Init(gpio, &port_out);
	if(in_pins) /* Won't initialize if in_pins is 0 */
		GPIO_Init(gpio, &port_in);
}

#endif /* PORT_H_ */
