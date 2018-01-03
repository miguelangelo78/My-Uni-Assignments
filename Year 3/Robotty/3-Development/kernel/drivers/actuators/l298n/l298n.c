/*
 * motor.c
 *
 *  Created on: 06/02/2017
 *      Author: Miguel
 */

#include <drivers/onchip/r_mtu_rx/r_mtu_rx_if.h>
#include <drivers/actuators/l298n/l298n.h>

bool l298n_initialized = false;
bool l298n_is_in_safe_mode = true;
uint16_t l298n_stored_speeds[2];
enum L298N_DIRECTION l298n_stored_directions[2];

enum L298N_ERRCODE l298n_init(void) {
	if(l298n_initialized) return L298N_ERR_ALREADY_INIT;

	/* Set all the pins to use the internal peripherals: */
	R_BSP_RegisterProtectDisable(BSP_REG_PROTECT_MPC);
	MPC.PE4PFS.BYTE = 0x02;
	MPC.PA1PFS.BYTE = 0x01;
	MPC.PE5PFS.BYTE = 0x02;
	MPC.P32PFS.BYTE = 0x01;
	R_BSP_RegisterProtectEnable(BSP_REG_PROTECT_MPC);
	PORTE.PMR.BIT.B4 = 1;
	PORTA.PMR.BIT.B1 = 1;
	PORTE.PMR.BIT.B5 = 1;
	PORT3.PMR.BIT.B2 = 1;

	/* Initialize all 4 PWM channels for the H-Bridge: */
	const uint32_t cycle_freq = 320; /* This value was not calculated. It was found by trial and error. The actual desired value is 490 Hz */
	mtu_pwm_chnl_settings_t pwm_settings;

	/* ******************************************** */
	/* First PWM (MTIOC0B - PA1 and MTIOC0C - P32): */
	/* ******************************************** */
	pwm_settings.pwm_mode             = MTU_PWM_MODE_2;
	pwm_settings.clock_src.source     = MTU_CLK_SRC_INTERNAL;
	pwm_settings.clock_src.clock_edge = MTU_CLK_RISING_EDGE;
	pwm_settings.clear_src            = MTU_CLR_TIMER_A;
	pwm_settings.cycle_freq           = cycle_freq;

	pwm_settings.pwm_a.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_a.outputs        = MTU_PIN_LO_GOHI;

	pwm_settings.pwm_b.duty           = 0;
	pwm_settings.pwm_b.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_b.outputs        = MTU_PIN_HI_GOLO;

	pwm_settings.pwm_c.duty           = 0;
	pwm_settings.pwm_c.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_c.outputs        = MTU_PIN_HI_GOLO;

	pwm_settings.pwm_d.actions        = MTU_ACTION_NONE;

	R_MTU_PWM_Open(MTU_CHANNEL_0, &pwm_settings, FIT_NO_FUNC);
	R_MTU_Control(MTU_CHANNEL_0, MTU_CMD_START, FIT_NO_PTR);

	/* *************************** */
	/* Second PWM (MTIOC1A / PE4): */
	/* *************************** */
	pwm_settings.pwm_mode             = MTU_PWM_MODE_1;
	pwm_settings.clock_src.source     = MTU_CLK_SRC_INTERNAL;
	pwm_settings.clock_src.clock_edge = MTU_CLK_RISING_EDGE;
	pwm_settings.clear_src            = MTU_CLR_TIMER_A;
	pwm_settings.cycle_freq           = cycle_freq;

	pwm_settings.pwm_a.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_a.outputs        = MTU_PIN_LO_GOHI;

	pwm_settings.pwm_b.duty           = 0;
	pwm_settings.pwm_b.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_b.outputs        = MTU_PIN_HI_GOLO;

	pwm_settings.pwm_c.actions        = MTU_ACTION_NONE;
	pwm_settings.pwm_d.actions        = MTU_ACTION_NONE;

	R_MTU_PWM_Open(MTU_CHANNEL_1, &pwm_settings, FIT_NO_FUNC);
	R_MTU_Control(MTU_CHANNEL_1, MTU_CMD_START, FIT_NO_PTR);

	/* ************************** */
	/* Third PWM (MTIOC2B / PE5): */
	/* ************************** */
	pwm_settings.pwm_mode             = MTU_PWM_MODE_2;
	pwm_settings.clock_src.source     = MTU_CLK_SRC_INTERNAL;
	pwm_settings.clock_src.clock_edge = MTU_CLK_RISING_EDGE;
	pwm_settings.clear_src            = MTU_CLR_TIMER_A;
	pwm_settings.cycle_freq           = cycle_freq;

	pwm_settings.pwm_a.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_a.outputs        = MTU_PIN_LO_GOHI;

	pwm_settings.pwm_b.duty           = 0;
	pwm_settings.pwm_b.actions        = MTU_ACTION_OUTPUT;
	pwm_settings.pwm_b.outputs        = MTU_PIN_HI_GOLO;

	pwm_settings.pwm_c.actions        = MTU_ACTION_NONE;
	pwm_settings.pwm_d.actions        = MTU_ACTION_NONE;

	R_MTU_PWM_Open(MTU_CHANNEL_2, &pwm_settings, FIT_NO_FUNC);
	R_MTU_Control(MTU_CHANNEL_2, MTU_CMD_START, FIT_NO_PTR);

	for(int i = 0; i < 2; i++) {
		l298n_stored_speeds[i] = 0;
		l298n_stored_directions[i] = DIR_NULL;
	}

	l298n_initialized = true;

	return L298N_OK;
}

enum L298N_ERRCODE l298n_ctrl(enum L298N_CHANNEL side, enum L298N_DIRECTION direction, uint16_t speed) {
	if(speed >= L298N_MOTOR_MAX_SPEED) speed = L298N_MOTOR_MAX_SPEED; /* Cap the speed if we're going faster than allowed */
	if(l298n_is_in_safe_mode && speed >= L298N_MOTOR_SAFE_MODE_LEVEL) {
		/* TODO: The right back motor is broken. For this reason, I'll be temporarily adding this 'if' below so that the
		 * right front motor can spin faster to compensate for the broken back motor.
		 * I'm doing this because I only have 14 days to finish this project and I don't have a new working motor
		 * next to me right now ... */
		if(side == L298N_CHANNEL_LEFT || side == L298N_CHANNEL_BOTH_SIDES)
			speed = L298N_MOTOR_SAFE_MODE_LEVEL; /* Cap the speed in case we're in safe mode */
	}

	if(side == L298N_CHANNEL_LEFT) {
		l298n_stored_speeds[0]     = speed;
		l298n_stored_directions[0] = direction;
	}
	else if(side == L298N_CHANNEL_RIGHT) {
		l298n_stored_speeds[1]     = speed;
		l298n_stored_directions[1] = direction;
	} else if(side == L298N_CHANNEL_BOTH_SIDES) {
		l298n_stored_speeds[0]     = speed;
		l298n_stored_directions[0] = direction;
		l298n_stored_speeds[1]     = speed;
		l298n_stored_directions[1] = direction;
	}

	/* Fix the PWM value offset (values too low will not make the motors spin. Only noise will come out): */
	speed += L298N_MOTOR_MIN_SPEED;

	if(side == L298N_CHANNEL_RIGHT) {
		if(direction == DIR_FORW) {
			MTU1.TGRB = speed;
			MTU0.TGRB = 0;
		} else if (direction == DIR_BACK) {
			MTU1.TGRB = 0;
			MTU0.TGRB = speed;
		} else {
			return L298N_ERR_INVAL_DIRECTION;
		}
	} else if(side == L298N_CHANNEL_LEFT) {
		if(direction == DIR_FORW) {
			MTU0.TGRC = 0;
			MTU2.TGRB = speed;
		} else if (direction == DIR_BACK) {
			MTU0.TGRC = speed;
			MTU2.TGRB = 0;
		} else {
			return L298N_ERR_INVAL_DIRECTION;
		}
	} else if(L298N_CHANNEL_BOTH_SIDES) {
		if(direction == DIR_FORW) {
			MTU1.TGRB = speed;
			MTU0.TGRB = 0;
			MTU0.TGRC = 0;
			MTU2.TGRB = speed;
		} else if (direction == DIR_BACK) {
			MTU1.TGRB = 0;
			MTU0.TGRB = speed;
			MTU0.TGRC = speed;
			MTU2.TGRB = 0;
		} else {
			return L298N_ERR_INVAL_DIRECTION;
		}
	} else {
		return L298N_ERR_INVAL_CHANNEL;
	}
	return L298N_OK;
}

void l298n_stop(void) {
	MTU0.TGRB = 0;
	MTU0.TGRC = 0;
	MTU1.TGRB = 0;
	MTU2.TGRB = 0;
}

void l298n_stop_single_motor(enum L298N_CHANNEL side) {
	if(side == L298N_CHANNEL_LEFT) {
		MTU0.TGRC = 0;
		MTU2.TGRB = 0;
	} else if(L298N_CHANNEL_RIGHT) {
		MTU1.TGRB = 0;
		MTU0.TGRB = 0;
	} else if(L298N_CHANNEL_BOTH_SIDES) {
		MTU0.TGRB = 0;
		MTU0.TGRC = 0;
		MTU1.TGRB = 0;
		MTU2.TGRB = 0;
	}
}

void l298n_resume(enum L298N_CHANNEL side) {
	if(side == L298N_CHANNEL_LEFT) {
		l298n_ctrl(L298N_CHANNEL_LEFT, l298n_stored_directions[0], l298n_stored_speeds[0]);
	} else if(side == L298N_CHANNEL_RIGHT) {
		l298n_ctrl(L298N_CHANNEL_RIGHT, l298n_stored_directions[1], l298n_stored_speeds[1]);
	} else if(side == L298N_CHANNEL_BOTH_SIDES) {
		l298n_ctrl(L298N_CHANNEL_LEFT, l298n_stored_directions[0], l298n_stored_speeds[0]);
		l298n_ctrl(L298N_CHANNEL_RIGHT, l298n_stored_directions[1], l298n_stored_speeds[1]);
	}
}

void l298n_set_safe_mode(bool enable) {
	l298n_is_in_safe_mode = enable;
	if(enable && (l298n_stored_speeds[0] >= L298N_MOTOR_SAFE_MODE_LEVEL || l298n_stored_speeds[1] >= L298N_MOTOR_SAFE_MODE_LEVEL)) {
		l298n_ctrl(L298N_CHANNEL_LEFT, l298n_stored_directions[0], L298N_MOTOR_SAFE_MODE_LEVEL);
		l298n_ctrl(L298N_CHANNEL_RIGHT, l298n_stored_directions[1], L298N_MOTOR_SAFE_MODE_LEVEL);
	}
}
