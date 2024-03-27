/*
 * bat.c
 *
 *  Created on: Mar 18, 2024
 *      Author: Minh
 */

#include "bat.h"

/**
 * @func	adc_Initt
 *
 * @brief	initialization ADC
 *
 * @param	[none]
 *
 * @retval	none
 */
void adc_Initt()
{
	// Declare initialization structures
	IADC_Init_t adc_struct = IADC_INIT_DEFAULT;
	IADC_AllConfigs_t  init_AllConfigs = IADC_ALLCONFIGS_DEFAULT;

	//enable ADC clock
	CMU_ClockEnable(cmuClock_IADC0, true);
//	CMU_ClockEnable(cmuClock_GPIO, true);

	//reset ADC0
//	IADC_reset(IADC0);

	// Use the FSRC0 as the IADC clock so it can run in EM2
//	CMU_ClockSelectSet(cmuClock_IADCCLK, cmuSelect_FSRCO);

//	adc_struct.warmup = iadcWarmupNormal; // shutdown after each conversion.

	// Set the HFSCLK prescale value here
//	adc_struct.srcClkPrescale = (IADC_calcSrcClkPrescale(IADC0, CLK_SRC_ADC_FREQ, 0));

	init_AllConfigs.configs[0].reference 		= iadcCfgReferenceVddx;
	init_AllConfigs.configs[1].reference 		= iadcCfgReferenceVddx;
//	init_AllConfigs.configs[0].vRef 			= 3300; 					// Vref
//	init_AllConfigs.configs[0].analogGain 		= iadcCfgAnalogGain0P5x;

//	init_AllConfigs.configs[0].adcClkPrescale 	= IADC_calcAdcClkPrescale(IADC0,
//																		CLK_ADC_FREQ,
//																		0,
//																		iadcCfgModeNormal,
//																		adc_struct.srcClkPrescale);

//	init_AllConfigs.configs[0].osrHighSpeed = iadcCfgOsrHighSpeed32x;

//	init_Single.dataValidLevel = _IADC_SINGLEFIFOCFG_DVL_VALID1;



	// Initialize IADC
	IADC_init(IADC0, &adc_struct, &init_AllConfigs);


	// Allocate the analog bus for ADC0 inputs
	GPIO->IADC_INPUT_0_BUS |= GPIO_CDBUSALLOC_CDEVEN0_ADC0;

}

/**
 * @func	read_ADCvalue
 *
 * @brief	read ADC data
 *
 * @param	[none]
 *
 * @retval	none
 */
uint32_t read_ADCvalue()
{

	IADC_InitSingle_t init_Single = IADC_INITSINGLE_DEFAULT;
	IADC_SingleInput_t single_Input = IADC_SINGLEINPUT_DEFAULT;
	single_Input.posInput = iadcPosInputPortCPin4 ;
	single_Input.negInput = iadcNegInputGnd;
	// Initialize scan
	IADC_initSingle(IADC0, &init_Single, &single_Input);
//	uint32_t sample = 0;
	IADC_command(IADC0, iadcCmdStartSingle);

	while((IADC0->STATUS & (_IADC_STATUS_CONVERTING_MASK | _IADC_STATUS_SINGLEFIFODV_MASK)) != IADC_STATUS_SINGLEFIFODV);

	IADC_Result_t sample = IADC_readSingleResult(IADC0);//IADC_pullSingleFifoResult(IADC0).data;

	return sample.data;
}

/**
 * @func	batteryCalculate
 *
 * @brief	convert data to percent power battery
 *
 * @param	[none]
 *
 * @retval	uint8_t : percent power of battery
 */
uint8_t batteryCalculate()
{
	uint32_t temp =0;
	uint16_t voltage;
	uint8_t percent = 0;
	temp = read_ADCvalue();
	voltage = (temp * 3300 ) / 0xFFF;
	emberAfCorePrintln("voltage = %d_", voltage);

	if(voltage > 1500) percent = 100;	//>1.5V
	else {
		percent = (voltage*100) /(1500-900);	//<0.9V out of battery
	}
	emberAfCorePrintln("Battery = %d_", percent);

	return percent;
}
