/*
 * temp_hum.h
 *
 *  Created on: Feb 20, 2024
 *      Author: Minh
 */

#ifndef SOURCE_TEMP_HUM_H_
#define SOURCE_TEMP_HUM_H_

#define TEMP_HUM_ADDR				0x40

#define I2C_MASTER_PORT					(gpioPortC)
#define I2C_MASTER_SDA_PIN					2U
#define I2C_MASTER_SCL_PIN					3U

#define WRITE							I2C_FLAG_WRITE
#define READ							I2C_FLAG_READ
#define CMD_HUMI_HOLD					0xE5 // hold master mode
#define CMD_TEM_HOLD					0xE3 // hold master mode

void Clock_Init();

/**
 * @func	TempHum_Init
 *
 * @brief	Initialization sensor
 *
 * @param	none
 *
 * @retval	none
 */
void TempHum_Init();

uint16_t TemHumSensor_getTemperature();

uint16_t TemHumSensor_getHumidity();

void processGetValueSensor();


#endif /* SOURCE_TEMP_HUM_H_ */
