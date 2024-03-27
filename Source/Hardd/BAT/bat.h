/*
 * bat.h
 *
 *  Created on: Mar 18, 2024
 *      Author: Minh
 */

#ifndef SOURCE_HARDD_BAT_BAT_H_
#define SOURCE_HARDD_BAT_BAT_H_

#include "app/framework/include/af.h"
#include "stddef.h"
#include "em_iadc.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_gpio.h"
#include "em_ldma.h"

#define ADC_BAT_PORT					gpioPortC
#define ADC_BAT_PIN						(4U)

#define IADC_INPUT_0_PORT_PIN		iadcPosInputPortCPin4

#define IADC_INPUT_0_BUS         	CDBUSALLOC
#define IADC_INPUT_0_BUSALLOC    	_GPIO_CDBUSALLOC_CDEVEN0_ADC0

#define IADC_LDMA_CH             	0

// Set CLK_ADC to 10 MHz
#define CLK_SRC_ADC_FREQ    		20000000  // CLK_SRC_ADC
#define CLK_ADC_FREQ        		10000000  // CLK_ADC - 10 MHz max in normal mode


void adc_Initt();
uint32_t read_ADCvalue();
uint8_t batteryCalculate();

#endif /* SOURCE_HARDD_BAT_BAT_H_ */
