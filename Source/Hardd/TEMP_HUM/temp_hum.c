/*
 * temp_hum.c
 *
 *  Created on: Feb 20, 2024
 *      Author: Minh
 */

#include "app/framework/include/af.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "temp_hum.h"
#include "em_emu.h"
#include "em_gpio.h"

uint8_t i2c_rxBuffer[10];
uint8_t temperature = 0;
uint8_t humidity = 0;

/**
 * @func	TempHum_Init()
 *
 * @brief	Initialize sensor Si7020-A20
 *
 * @param	none
 *
 * @retval	none
 */
void TempHum_Init()
{

	I2C_Init_TypeDef i2c_struct = I2C_INIT_DEFAULT;

	//configure GPIO alternate function mode
	GPIO_PinModeSet(I2C_MASTER_PORT,I2C_MASTER_SDA_PIN,gpioModeWiredAndPullUpFilter,1);
	GPIO_PinModeSet(I2C_MASTER_PORT,I2C_MASTER_SCL_PIN,gpioModeWiredAndPullUpFilter,1);

	//use ~400KHz SCK
	i2c_struct.freq = I2C_FREQ_FAST_MAX;
	//use 6:3 Tlow - Thigh SCK
	i2c_struct.clhr = i2cClockHLRAsymetric;

	GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
									| (gpioPortC << _GPIO_I2C_SCLROUTE_PORT_SHIFT
									| (3 << _GPIO_I2C_SCLROUTE_PIN_SHIFT));
	GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
									| (gpioPortC << _GPIO_I2C_SDAROUTE_PORT_SHIFT
									| (2 <<_GPIO_I2C_SDAROUTE_PIN_SHIFT));

	GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SCLPEN | GPIO_I2C_ROUTEEN_SDAPEN;


	// Initialize the I2C0
	I2C_Init(I2C0,&i2c_struct);

	// Automatic STOP on NACK
	I2C0->CTRL = I2C_CTRL_AUTOSN;

}

/**
 * @func	I2C_Read_Data_From_Slave
 *
 * @brief	Read data from sensor
 *
 * @param	[address]	: sensor address
 *
 * @param	[cmd]		: command
 *
 * @param	[rxBuff]	: pointer rx buffer
 *
 * @param	[numByte]	: length data receive
 *
 * @retval	none
 */
void I2C_Read_Data_From_Slave(uint8_t address,uint8_t cmd, uint8_t *rxBuff,uint8_t numByte)
{
	I2C_TransferSeq_TypeDef i2cTransferStruct;
	I2C_TransferReturn_TypeDef result;

	// Initialize I2C transfer
	i2cTransferStruct.addr			= (address << 1);
	i2cTransferStruct.flags			= I2C_FLAG_WRITE_READ;
	i2cTransferStruct.buf[0].data	= &cmd;
	i2cTransferStruct.buf[0].len	= 1;
	i2cTransferStruct.buf[1].data	= rxBuff;
	i2cTransferStruct.buf[1].len	= 2;

	result = I2C_TransferInit(I2C0,&i2cTransferStruct);

	while(result == i2cTransferInProgress)
	{
		result = I2C_Transfer(I2C0);
	}
}

/**
 * @func	TemHumSensor_getTemperature
 *
 * @brief	get Temperature value
 *
 * @param	none
 *
 * @retval	[uint16_t] : temperature
 */
uint16_t TemHumSensor_getTemperature()
{
	uint16_t data_temp=0;
	uint16_t temp_value=0;

	I2C_Enable(I2C0, 1);

	I2C_Read_Data_From_Slave(TEMP_HUM_ADDR,CMD_TEM_HOLD,i2c_rxBuffer,2);

	data_temp |= (uint16_t)(i2c_rxBuffer[0] <<8);
	data_temp |= (uint16_t)(i2c_rxBuffer[1] <<0);

	temp_value = ((17572*data_temp)/65536) - 4685;

	return temp_value;
}

/**
 * @func	TemHumSensor_getHumidity
 *
 * @brief	get humidity value
 *
 * @param	none
 *
 * @retval	[uint16_t] : Humidity
 */
uint16_t TemHumSensor_getHumidity()
{
	uint16_t data_temp=0;
	uint16_t humi_value=0;

	I2C_Enable(I2C0, 1);

	I2C_Read_Data_From_Slave(TEMP_HUM_ADDR,CMD_HUMI_HOLD,i2c_rxBuffer,2);

	data_temp |= (uint16_t)(i2c_rxBuffer[0] <<8);
	data_temp |= (uint16_t)(i2c_rxBuffer[1] <<0);

	humi_value = ((125*data_temp)/65536) - 6;

	return humi_value;
}
