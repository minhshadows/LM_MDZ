/*
 * opt3001.c
 *
 *  Created on: Oct 15, 2020
 *      Author: ryan
 */

/***************************************************************************/

#include "app/framework/include/af.h"
#include <stddef.h>
#include "em_i2c.h"
//#include <Source/Utility/Debug.h>
//#include <Source/Hard/I2C/I2C.h>
#include "lightsensor2.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/
static uint8_t LSB;
static uint8_t MSB;

static uint16_t Opt3001_CorrectionPlasticCover (uint16_t data);

/**************************************************************************/

void LM_I2C_Init(void) {
	// Using default settings
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_I2C0, true);

	// Using PC3 (SCL) and PC4 (SDA)
	GPIO_PinModeSet(I2C_SENSOR_SCL_PORT, I2C_SENSOR_SCL_PIN, gpioModeWiredAndPullUpFilter, 1);
	GPIO_PinModeSet(I2C_SENSOR_SDA_PORT, I2C_SENSOR_SDA_PIN, gpioModeWiredAndPullUpFilter, 1);

	/* In some situations, after a reset during an I2C transfer, the slave
	 device may be left in an unknown state. Send 9 clock pulses to
	 set slave in a defined state. */

  for (uint8_t i = 0; i < 9; i++)
  {
	  GPIO_PinOutSet(I2C_SENSOR_SCL_PORT, I2C_SENSOR_SCL_PIN);
	  GPIO_PinOutClear(I2C_SENSOR_SCL_PORT, I2C_SENSOR_SCL_PIN);
  }

  // Route GPIO pins to I2C module
  GPIO->I2CROUTE[0].ROUTEEN = GPIO_I2C_ROUTEEN_SDAPEN | GPIO_I2C_ROUTEEN_SCLPEN;
  GPIO->I2CROUTE[0].SDAROUTE = (GPIO->I2CROUTE[0].SDAROUTE & ~_GPIO_I2C_SDAROUTE_MASK)
		  	  	  	  	  	  | (I2C_SENSOR_SDA_PORT << _GPIO_I2C_SDAROUTE_PORT_SHIFT
							  | (I2C_SENSOR_SDA_PIN << _GPIO_I2C_SDAROUTE_PIN_SHIFT));
  GPIO->I2CROUTE[0].SCLROUTE = (GPIO->I2CROUTE[0].SCLROUTE & ~_GPIO_I2C_SCLROUTE_MASK)
		  	  	  	  	  	  | (I2C_SENSOR_SCL_PORT << _GPIO_I2C_SCLROUTE_PORT_SHIFT
							  | (I2C_SENSOR_SCL_PIN << _GPIO_I2C_SCLROUTE_PIN_SHIFT));

  // Initializing the I2C
  i2cInit.enable = true;
  i2cInit.master = true;

  i2cInit.freq = I2C_FREQ_STANDARD_MAX;
  i2cInit.refFreq = 0;
  i2cInit.clhr = i2cClockHLRStandard;
  I2C_Init(I2C0, &i2cInit);
}

I2C_TransferReturn_TypeDef LMI2C_Transfer (I2C_TransferSeq_TypeDef *seq) {
	I2C_TransferReturn_TypeDef retVal;

	uint32_t timeout = I2C_TRANSFER_TIMEOUT;

	/* Do a polled transfer */
	retVal = I2C_TransferInit(I2C_DEFAULT, seq);
	while (retVal == i2cTransferInProgress && timeout--) {
		retVal = I2C_Transfer(I2C_DEFAULT);
	}
	return retVal;
}

void LM_I2C_DeInit(void) {
	GPIO_PinModeSet(I2C_SENSOR_SCL_PORT, I2C_SENSOR_SCL_PIN, gpioModeDisabled, 0);
	GPIO_PinModeSet(I2C_SENSOR_SDA_PORT, I2C_SENSOR_SDA_PORT, gpioModeDisabled, 0);
}

boolean Opt3001_GetRegVal(uint8_t command) {
	I2C_TransferSeq_TypeDef seq;
	I2C_TransferReturn_TypeDef retVal;
	uint8_t i2c_read_data[2];
	uint8_t i2c_write_data[1];

	seq.addr = OPT3001_ADDR_GND;
	seq.flags = I2C_FLAG_WRITE_READ;
	/* Select command to issue */
	i2c_write_data[0] = command;
	seq.buf[0].data = i2c_write_data;
	seq.buf[0].len = 1;
	/* Select location/length of data to be read */
	seq.buf[1].data = i2c_read_data;
	seq.buf[1].len = 2;

	retVal = LMI2C_Transfer(&seq);

	if (retVal != i2cTransferDone) {
		return false;
	}

	MSB = i2c_read_data[0];
	LSB = i2c_read_data[1];
	return true;
}

/**
 * @func   	<functionName>
 *
 * @status	building...
 *
 * @brief  	This function do <functionName>: ...
 *
 * @param
 *
 * @retval 	None
 */
uint16_t Opt3001_GetLuxVal(void) {
	uint16_t rawData, tmpData, LSB_Size,mantissa;
	uint16_t retVal = 0;

	boolean result = Opt3001_GetRegVal(RESULT_REG);

	/* Read Lux value */
	if (result) {
		rawData = ((uint16_t) (MSB) << 8) + (uint16_t) (LSB);
	} else {
		rawData = 0;
		return 0;
	}

	/* Formula: LUX = 0.01 * 2 ^ E [3:0] * R [11 : 0]  */
	LSB_Size = 1 << ((rawData & 0xf000) >> 12); // LSB_Size = 2 ^ exponent
	tmpData = (rawData & 0x0fff) * LSB_Size; // LUX = R[11:0] * LSB_Size * 0.01
	mantissa = rawData & 0x0fff;

	emberAfCorePrintln("raw lightsensor2.h");
	emberAfCorePrintln("LSB_Size = %d",LSB_Size);
	emberAfCorePrintln("MSB = %d",MSB);
	emberAfCorePrintln("LSB = %d",LSB);
	emberAfCorePrintln("raw data = %d",rawData);
	emberAfCorePrintln("mantissa = %d",mantissa);
	tmpData = Opt3001_CorrectionPlasticCover (tmpData);

	if ((tmpData % 100) <= 50) {
		retVal = (tmpData / 100);
	} else {
		retVal = (tmpData / 100) + 1;
	}
//	DEBUG_PrintString ( " Measure luxury done! ");

	if (retVal == 0) retVal = 1; // Neu lux = 0 van report len la 1, phan biet truong hop khong doc duoc gia tri tu cam bien

	return retVal;

}

/**
 * @func   	<functionName>
 *
 * @status	building...
 *
 * @brief  	This function do <functionName>: ...
 *
 * @param  	int x :
 *		   	int y :
 *
 * @retval 	None
 */

void Opt3001_MeasureLux(void) {
	const uint8_t configData[3] = { CONFIG_REG, CONFIG_CMD_MSB, CONFIG_CMD_LSB };
	uint16_t length = 3;
	I2C_TransferSeq_TypeDef seq;

	/* Send configure that include start measure command to Opt3001 */
	seq.addr = OPT3001_ADDR_GND;
	seq.flags = I2C_FLAG_WRITE;
	seq.buf[0].data = (uint8_t*)configData;
	seq.buf[0].len = length;

	(void) LMI2C_Transfer(&seq);
}


/**
 * @func   	<functionName>
 *
 * @status	building...
 *
 * @brief  	This function do <functionName>: ...
 *
 * @param  	int x :
 *		   	int y :
 *
 * @retval 	None
 */
uint16_t Opt3001_GetDevIdVal(void) {
	Opt3001_GetRegVal(DEVID_REG);

	return (uint16_t) (MSB << 8) + (uint16_t) (LSB);
}


boolean Opt3001_CheckInit (void) {
	uint16_t deviceID = Opt3001_GetDevIdVal();

	if (deviceID == OPT3001_DEV_ID) {
//		DEBUG_PrintString("Detect Opt3001 Success");
		return true;
	}
//	DEBUG_PrintString("Detect Opt3001 failure !");
	return false;
}

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
