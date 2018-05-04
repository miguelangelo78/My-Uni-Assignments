/*
 * sg90.c
 *
 *  Created on: 06/03/2017
 *      Author: Miguel
 */

#include <drivers/onchip/r_mtu_rx/r_mtu_rx_if.h>
#include <drivers/actuators/sg90/sg90.h>
#include <libs/utils.h>

bool sg90_initialized          = false;
const uint32_t sg90_cycle_freq = 31;
mtu_pwm_chnl_settings_t sg90_pwm_settings;
uint8_t sg90_stored_angles[2];

#define angle_offset(angle) ((angle) <= 15 ? 15 : angle) - 15

enum SG90_ERRCODE sg90_init_channels(void) {
	if(sg90_initialized) return SG90_ERR_ALREADY_INIT;

	/* Set all the pins to use the internal peripherals: */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
	MPC.PC7PFS.BYTE = 0x01;
	MPC.PC6PFS.BYTE = 0x01;
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
	PORTC.PMR.BIT.B7 = 1;
	PORTC.PMR.BIT.B6 = 1;

	/* *************************************************************************** */
	/* Initialize the PWM pins for both Servos (MTIOC3A / PC7 and (MTIOC3C / PC6): */
	/* *************************************************************************** */
	sg90_pwm_settings.pwm_mode             = MTU_PWM_MODE_1;
	sg90_pwm_settings.clock_src.source     = MTU_CLK_SRC_INTERNAL;
	sg90_pwm_settings.clock_src.clock_edge = MTU_CLK_RISING_EDGE;
	sg90_pwm_settings.clear_src            = MTU_CLR_TIMER_A;
	sg90_pwm_settings.cycle_freq           = sg90_cycle_freq;

	sg90_pwm_settings.pwm_a.actions        = MTU_ACTION_OUTPUT;
	sg90_pwm_settings.pwm_a.outputs        = MTU_PIN_LO_GOHI;

	sg90_pwm_settings.pwm_b.duty           = angle_offset(90);
	sg90_pwm_settings.pwm_b.actions        = MTU_ACTION_OUTPUT;
	sg90_pwm_settings.pwm_b.outputs        = MTU_PIN_HI_GOLO;

	sg90_pwm_settings.pwm_c.duty           = angle_offset(90);
	sg90_pwm_settings.pwm_c.actions        = MTU_ACTION_OUTPUT;
	sg90_pwm_settings.pwm_c.outputs        = MTU_PIN_HI_GOLO;

	sg90_pwm_settings.pwm_d.actions        = MTU_ACTION_OUTPUT;
	sg90_pwm_settings.pwm_d.outputs        = MTU_PIN_LO_GOHI;

	R_MTU_PWM_Open(MTU_CHANNEL_3, &sg90_pwm_settings, FIT_NO_FUNC);
	R_MTU_Control(MTU_CHANNEL_3, MTU_CMD_START, FIT_NO_PTR);

	sg90_ctrl(SG90_AXIS_X, 90);
	sg90_ctrl(SG90_AXIS_Y, 90);

	sg90_initialized = true;

	return SG90_OK;
}

enum SG90_ERRCODE sg90_ctrl(enum SG90_AXIS axis, uint8_t angle) {
	if(axis == SG90_AXIS_X) {
		if(angle > SG90_MAX_X_AXIS_ANGLE) return SG90_ERR_INVAL_ANGLE;

		sg90_stored_angles[0] = angle;

		R_MTU_Close(MTU_CHANNEL_3);
		sg90_pwm_settings.pwm_b.duty = angle_offset(angle);
		R_MTU_PWM_Open(MTU_CHANNEL_3, &sg90_pwm_settings, FIT_NO_FUNC);
		R_MTU_Control(MTU_CHANNEL_3, MTU_CMD_START, FIT_NO_PTR);
	} else if(axis == SG90_AXIS_Y) {
		if(angle > SG90_MAX_Y_AXIS_ANGLE) return SG90_ERR_INVAL_ANGLE;

		sg90_stored_angles[1] = angle;

		MTU3.TGRC = map(SG90_MAX_Y_AXIS_ANGLE - angle, 0, SG90_MAX_Y_AXIS_ANGLE, 1, 1300);
	} else {
		return SG90_ERR_INVAL_AXIS;
	}

	return SG90_OK;
}

enum SG90_ERRCODE sg90_accum_ctrl(enum SG90_AXIS axis, int16_t sum_angle) {
	int16_t sum1 = (int16_t)sg90_stored_angles[0] + sum_angle;
	int16_t sum2 = (int16_t)sg90_stored_angles[1] + sum_angle;
	if(sum1 < 0) sum1 = 0;
	if(sum2 < 0) sum2 = 0;

	switch(axis) {
		case SG90_AXIS_X:
			sg90_ctrl(SG90_AXIS_X, (uint8_t)sum1);
			break;
		case SG90_AXIS_Y:
			sg90_ctrl(SG90_AXIS_Y, (uint8_t)sum2);
			break;
		case SG90_BOTH_AXIS:
			sg90_ctrl(SG90_AXIS_X, (uint8_t)sum1);
			sg90_ctrl(SG90_AXIS_Y, (uint8_t)sum2);
			break;
		default: /* Invalid axis */ break;
	}
	return SG90_OK;
}

void sg90_lock(enum SG90_AXIS lock_axis) {
	switch(lock_axis) {
		case SG90_AXIS_X:
			sg90_ctrl(SG90_AXIS_X, sg90_stored_angles[0]);
			break;
		case SG90_AXIS_Y:
			sg90_ctrl(SG90_AXIS_X, sg90_stored_angles[1]);
			break;
		case SG90_BOTH_AXIS:
			sg90_ctrl(SG90_AXIS_X, sg90_stored_angles[0]);
			sg90_ctrl(SG90_AXIS_Y, sg90_stored_angles[1]);
			break;
		default: /* Invalid axis */ break;
	}
}

void sg90_unlock(enum SG90_AXIS unlock_axis) {
	switch(unlock_axis) {
		case SG90_AXIS_X:    MTU3.TGRB = (uint16_t)-1; break;
		case SG90_AXIS_Y:    MTU3.TGRC = (uint16_t)-1; break;
		case SG90_BOTH_AXIS: MTU3.TGRB = MTU3.TGRC = (uint16_t)-1; break;
		default: /* Invalid axis */ break;
	}
}
