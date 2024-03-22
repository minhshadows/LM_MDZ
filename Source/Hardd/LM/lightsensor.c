/*
 * lightsensor.c
 *
 *  Created on: Feb 21, 2024
 *      Author: Minh
 */

#include "app/framework/include/af.h"
#include "em_cmu.h"
#include "em_i2c.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "lightsensor.h"
#include "math.h"

uint8_t light_sensor_rxBuffer[10];

static uint16_t Opt3001_CorrectionPlasticCover (uint16_t data);

/*
 * @func	light_sensor_Init
 * @brief	Initialize OPT3001 light sensor
 * @param	None
 * @retval	None
 */
void light_sensor_Init()
{
	I2C_Init_TypeDef i2c_struct = I2C_INIT_DEFAULT;

	//configure GPIO alternate function mode
	GPIO_PinModeSet(LIGHT_SENSOR_PORT,LIGHT_SENSOR_SDA_PIN,gpioModeWiredAndPullUpFilter,1);
	GPIO_PinModeSet(LIGHT_SENSOR_PORT,LIGHT_SENSOR_SCL_PIN,gpioModeWiredAndPullUpFilter,1);

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
//	LM_I2C_Init();
	I2C_Enable(I2C0, 1);
	OPT3001_Write_Data();
//	LM_I2C_DeInit();
}

/*
 * @func	LM_I2C_DeInit
 * @brief	Deinit sensor
 * @param	None
 * @retval	None
 */
void LM_I2C_DeInit(void) {
	GPIO_PinModeSet(LIGHT_SENSOR_PORT, LIGHT_SENSOR_SCL_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(LIGHT_SENSOR_PORT, LIGHT_SENSOR_SDA_PIN, gpioModeDisabled, 0);
}

/*
 * @func	OPT3001_Write_Data
 * @brief	Write command to Configuration REGISTER
 * @param	None
 * @retval	None
 */
void OPT3001_Write_Data()
{
	I2C_TransferSeq_TypeDef i2cTransferStruct;
	I2C_TransferReturn_TypeDef result;

	const uint8_t configData[3] = { CONFIG_REG, CONFIG_CMD_MSB, CONFIG_CMD_LSB };
	// Initialize I2C transfer
	i2cTransferStruct.addr			= (LIGHT_SENSOR_ADDR << 1);
	i2cTransferStruct.flags			= I2C_FLAG_WRITE;
	i2cTransferStruct.buf[0].data	= (uint8_t *)configData;
	i2cTransferStruct.buf[0].len	= 3;
	i2cTransferStruct.buf[1].data	= NULL;
	i2cTransferStruct.buf[1].len	= 0;

	result = I2C_TransferInit(I2C0,&i2cTransferStruct);

	while(result == i2cTransferInProgress)
	{
		result = I2C_Transfer(I2C0);
	}
}

/*
 * @func	OPT3001_Read_Data
 * @brief	Read data from Result REGISTER
 * @param	address : OPT3001 address
 * @param	cmd		: command
 * @param	*rxBuff	: pointer to receive buffer
 * @param	length	: length of data
 * @retval	None
 */
void OPT3001_Read_Data(uint8_t address, uint8_t cmd, uint8_t *rxBuff,uint8_t length)
{
	I2C_TransferSeq_TypeDef i2cTransferStruct;
	I2C_TransferReturn_TypeDef result;

	// Initialize I2C transfer
	i2cTransferStruct.addr			= (address << 1);
	i2cTransferStruct.flags			= I2C_FLAG_WRITE_READ;
	i2cTransferStruct.buf[0].data	= &cmd;
	i2cTransferStruct.buf[0].len	= 1;
	i2cTransferStruct.buf[1].data	= rxBuff;
	i2cTransferStruct.buf[1].len	= length;

	result = I2C_TransferInit(I2C0,&i2cTransferStruct);

	while(result == i2cTransferInProgress)
	{
		result = I2C_Transfer(I2C0);
	}
}

/*
 * @func	OPT3001_GetValue
 *
 * @brief	Read data and calib it
 *
 * @param	none
 *
 * @retval	[uint16_t] : lux value
 */
uint16_t OPT3001_GetValue()
{
	uint16_t temp =0;
	uint16_t exp =0;
	uint32_t LSB_Size = 0;
	uint32_t lux = 0 ;
	uint16_t Result =0;
	OPT3001_Write_Data();
	OPT3001_Read_Data(LIGHT_SENSOR_ADDR,0x00,light_sensor_rxBuffer,2);
	temp |= (uint16_t) light_sensor_rxBuffer[0] << 8;
	temp |= (uint16_t) light_sensor_rxBuffer[1] << 0;
	exp = ((temp & 0xF000) >> 12);
	Result =temp & 0x0FFF;

	LSB_Size =1<<exp;//( pow(2,exp));

	lux =(LSB_Size *  Result)/100;

	// calib data
	lux=Opt3001_CorrectionPlasticCover(lux);
	return (uint16_t)lux;
}

/*
 * @func	Opt3001_CorrectionPlasticCover
 *
 * @brief	Calib data
 *
 * @param	[uint16_t] : lux in
 *
 * @retval	[uint16_t] : data calib
 */
static uint16_t Opt3001_CorrectionPlasticCover (uint16_t data) {
	uint16_t retVal = 0;

	if (data < 500) retVal = data * 20;
	else if (data < 1000) retVal = data * 15;
	else if (data < 2000) retVal = data * 10;
	else if (data < 5000) retVal = data * 8;
	else if (data < 10000) retVal = data * 5;
	else if (data < 20000) retVal = data * 3;
	else retVal = data * 2;

	return retVal;

}
