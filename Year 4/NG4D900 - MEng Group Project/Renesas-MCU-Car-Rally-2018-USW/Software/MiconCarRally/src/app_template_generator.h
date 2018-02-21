/*
 * app_template_generator.h
 *
 *  Created on: 29/11/2017
 *      Author: Miguel
 */

#ifndef SRC_APP_TEMPLATE_GENERATOR_H_
#define SRC_APP_TEMPLATE_GENERATOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <utils.h>
#include <rtos_inc.h>
#include <app_config.h>

void template_generator_begin(void);
bool template_generator_is_finished(void);
void template_generator_update(uint8_t sensor_data);
bool template_generator_dump(void);

#endif /* SRC_APP_TEMPLATE_GENERATOR_H_ */
