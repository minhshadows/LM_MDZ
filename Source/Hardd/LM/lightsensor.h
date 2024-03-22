/*
 * lightsensor.h
 *
 *  Created on: Feb 21, 2024
 *      Author: Minh
 */

#ifndef SOURCE_HARDD_LM_LIGHTSENSOR_H_
#define SOURCE_HARDD_LM_LIGHTSENSOR_H_

#define LIGHT_SENSOR_ADDR							0x44

#define LIGHT_SENSOR_PORT							(gpioPortC)
#define LIGHT_SENSOR_SDA_PIN						2U
#define LIGHT_SENSOR_SCL_PIN						3U

#define CONFIG_REG              					0x01
#define CONFIG_CMD_MSB								0xC2
#define CONFIG_CMD_LSB								0x00

#define MASK_RESULT									0xFFF



void light_sensor_Init();
void LM_I2C_DeInit();
void OPT3001_Write_Data();
void OPT3001_Read_Data(uint8_t address, uint8_t cmd, uint8_t *rxBuff,uint8_t length);
uint16_t OPT3001_GetValue();

#endif /* SOURCE_HARDD_LM_LIGHTSENSOR_H_ */

