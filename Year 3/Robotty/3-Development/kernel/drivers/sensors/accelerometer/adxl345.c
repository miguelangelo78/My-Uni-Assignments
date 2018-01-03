/*
 * adxl345.c
 *
 *  Created on: 13/03/2017
 *      Author: Miguel
 */

#include <globals.h>

#if DRIVER_ENABLE_ACCELEROMETER == (1)
#if (1) /**** REGION: ADXL345 DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
bool accel_is_init = false;
volatile accel_t g_accel_zero;
volatile accel_t buffered_readings;

/* Function Prototype: */
riic_ret_t accel_i2c_read(uint8_t slave_addr, uint8_t register_number, uint8_t * dest_buff, uint32_t num_bytes);

int16_t accel_axis_read(uint8_t axis) {
	uint8_t    accel_data[2];
	riic_ret_t ret;

	switch(axis) {
		case ADXL345_DATAX0_REG:
		case ADXL345_DATAY0_REG:
		case ADXL345_DATAZ0_REG:
			if((ret = accel_i2c_read(ADXL345_ADDR, axis, accel_data, 2)) != RIIC_OK)
				return (int16_t)ret;

			/* Return axis value: */
			return (int16_t)((accel_data[1] << 8) + accel_data[0]);
		default: return -1;
	}
}

riic_ret_t accel_i2c_start(uint8_t slave_addr, uint8_t register_number) {
	uint8_t addr_and_register[2]; /* Storage for the slave address and target register. */

	/* To read / write to a specific register in the accelerometer, first transmit the
		accelerometer I2C slave address together with the register number. */
	addr_and_register[0] = slave_addr; /* The 7-bit I2C address of the ADXL345 and the R/W bit. */
	addr_and_register[1] = register_number;

	return R_RIIC_MasterTransmitHead(I2C_CHANNEL, addr_and_register, 2);
}

riic_ret_t accel_i2c_write(uint8_t slave_addr, uint8_t register_number, uint8_t * source_buff, uint32_t num_bytes) {
	riic_ret_t ret = RIIC_OK;

	/* Start I2C communication: */
	if((ret = accel_i2c_start(slave_addr, register_number)) != RIIC_OK)
		return ret;

	/* Now write the data from the source buffer into the target register. */
	return R_RIIC_MasterTransmit(I2C_CHANNEL, source_buff, num_bytes);
}

riic_ret_t accel_i2c_read(uint8_t slave_addr, uint8_t register_number, uint8_t * dest_buff, uint32_t num_bytes) {
	riic_ret_t ret = RIIC_OK;

	/* Start I2C communication: */
	if((ret = accel_i2c_start(slave_addr, register_number)) != RIIC_OK)
		return ret;

	/* Now read the data from the target register into the destination buffer. */
	return R_RIIC_MasterReceive(I2C_CHANNEL, slave_addr, dest_buff, num_bytes);
}

void accel_calibrate(void) {
	/* Get baseline readings to calibrate accelerometer: */
	for (uint8_t i = 0; i < 8; i++) {
		g_accel_zero.raw_x += accel_axis_read(ADXL345_DATAX0_REG);
		g_accel_zero.raw_y += accel_axis_read(ADXL345_DATAY0_REG);
		g_accel_zero.raw_z += accel_axis_read(ADXL345_DATAZ0_REG);
	}

	/* Determine the average reading: */
	g_accel_zero.raw_x /= 8;
	g_accel_zero.raw_y /= 8;
	g_accel_zero.raw_z /= 8;

	g_accel_zero.x = g_accel_zero.raw_x;
	g_accel_zero.y = g_accel_zero.raw_y;
	g_accel_zero.z = g_accel_zero.raw_z;
}

void accel_update_readings(accel_t * dataptr) {
	dataptr->raw_x = accel_axis_read(ADXL345_DATAX0_REG);
	dataptr->raw_y = accel_axis_read(ADXL345_DATAY0_REG);
	dataptr->raw_z = accel_axis_read(ADXL345_DATAZ0_REG);

	dataptr->x = dataptr->raw_x - g_accel_zero.raw_x;
	dataptr->y = dataptr->raw_y - g_accel_zero.raw_y;
	dataptr->z = dataptr->raw_z - g_accel_zero.raw_z;

	if(dataptr->x == 0)
		dataptr->x = 1;

	dataptr->slope = (100 * dataptr->y) / dataptr->x;
}

void accel_poller(void * args) {
	rtos_preempt();
	while(1) {
		/* Keep updating axis readings: */
		accel_update_readings((accel_t*)&buffered_readings);
		rtos_preempt();
	}
}
#endif

#if (1) /**** REGION: ADXL345 DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
enum ACCEL_ERR_CODE accel_init(void) {
	if(accel_is_init) return ACCEL_ERR_ALREADYINIT;

	riic_ret_t    ret;
	riic_config_t riic_master_config = {I2C_CHANNEL, RIIC_MASTER_CONFIG,
										0,
										0,
										0,
										0,
										I2C_MASTER_IIC_ADDRESS_LO,
										I2C_MASTER_IIC_ADDRESS_HI};

	/* Initialize the RIIC channel for communication with the accelerometer. */
	ret = R_RIIC_Init(&riic_master_config);
	if(ret != RIIC_OK)
		return ACCEL_ERR_I2C_BADINIT;

	/* Check if the accelerometer is present: */
	uint8_t accel_data;
	ret = accel_i2c_read(ADXL345_ADDR, ADXL345_ID_REG, &accel_data, 1);
	if ((accel_data != ADXL345_DEVICE_ID) || (ret != RIIC_OK))
		return ACCEL_ERR_DEV_NOTFOUND;

	/* Initialize buffer / pre-calculation variables: */
	g_accel_zero.x     = g_accel_zero.y     = g_accel_zero.z     = 0;
	g_accel_zero.raw_x = g_accel_zero.raw_y = g_accel_zero.raw_z = 0;
	g_accel_zero.slope = 0;

	buffered_readings.x     = buffered_readings.y     = buffered_readings.z     = 0;
	buffered_readings.raw_x = buffered_readings.raw_y = buffered_readings.raw_z = 0;
	buffered_readings.slope = 0;

	/* Set up the accelerometer data format register: */
	accel_data = 3; /* Set data format register range bits to +/- 16g. */
	if((ret = accel_i2c_write(ADXL345_ADDR, ADXL345_DATA_FORMAT_REG, &accel_data, 1)) != RIIC_OK)
		return ACCEL_ERR_BADINITIAL_CONFIG;

	/* Put accelerometer FIFO into bypass mode: */
	accel_data = 0; /* FIFO bypass mode. */
	if((ret = accel_i2c_write(ADXL345_ADDR, ADXL345_FIFO_CTL_REG, &accel_data, 1)) != RIIC_OK)
		return ACCEL_ERR_BADINITIAL_CONFIG;

	/* Set the measure bit in the accelerometer power control register: */
	accel_data = 8; /* Measure bit. */
	if((ret = accel_i2c_write(ADXL345_ADDR, ADXL345_POWER_CTL_REG, &accel_data, 1)) != RIIC_OK)
		return ACCEL_ERR_BADINITIAL_CONFIG;

	/* Calibrate the accelerometer: */
	accel_calibrate();

	/* And finally, activate accelerometer X, Y and Z to detect activity: */
	accel_data = 0x70;
	if((ret = accel_i2c_write(ADXL345_ADDR, ADXL345_ACT_INACT_CTL_REG, &accel_data, 1)) != RIIC_OK)
		return ACCEL_ERR_COULDNOTINIT;

	/* Keep updating the accelerometer pre-calculated values using an RTOS task: */
	spawn_task("accel_poller", accel_poller);

	accel_is_init = true;
	return ACCEL_OK;
}

accel_t * accel_get_readings(void) {
	if(!accel_is_init) return 0;
	return (accel_t*)&buffered_readings;
}
#endif

#endif
