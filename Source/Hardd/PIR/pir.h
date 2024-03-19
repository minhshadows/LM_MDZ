/*
 * pir.h
 *
 *  Created on: Mar 4, 2024
 *      Author: Minh
 */

#ifndef SOURCE_HARDD_PIR_PIR_H_
#define SOURCE_HARDD_PIR_PIR_H_

#include "app/framework/include/af.h"
#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#define PIR_DIR_PORT					gpioPortA
#define PIR_DIR_PIN						(0U)
#define PIR_SER_PORT					gpioPortC
#define PIR_SER_PIN						(0U)

#define GPIO_LOW_STATE					0
#define GPIO_HIGH_STATE					1


#define THRESHOLD_DATA_SHIFT			17
#define BLIND_TIME_DATA_SHIFT			13
#define PULSE_COUNTER_DATA_SHIFT		11
#define WINDOW_TIME_DATA_SHIFT			9
#define OPERATION_MODES_DATA_SHIFT		7
#define SIGNAL_SOURCE_DATA_SHIFT		5
#define RESERVED_2_DATA_SHIFT			3
#define HPF_CUT_OFF_DATA_SHIFT			2
#define RESERVED_1_DATA_SHIFT			1
#define COUNT_MODE_DATA_SHIFT			0

#define DL_LINE_INPUT_MODE				GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModeInput,0)
#define DL_LINE_OUTPUT_MODE				GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModePushPull,0)
#define SI_LINE_OUTPUT_MODE				GPIO_PinModeSet(PIR_SER_PORT,PIR_SER_PIN,gpioModePushPull,0)

#define DL_LINE_HIGH_TO_LOW				GPIO_PinOutClear(PIR_DIR_PORT,PIR_DIR_PIN)
#define DL_LINE_LOW_TO_HIGH				GPIO_PinOutSet(PIR_DIR_PORT,PIR_DIR_PIN)

#define SI_LINE_HIGH_TO_LOW				GPIO_PinOutClear(PIR_SER_PORT,PIR_SER_PIN)
#define SI_LINE_LOW_TO_HIGH				GPIO_PinOutSet(PIR_SER_PORT,PIR_SER_PIN)

#define T_SHD							80	// micro seconds
#define T_SLT							650	// micro seconds

#define T_DS							120 //data setup (micro seconds)
#define T_BIT							21	//
#define T_RESET							160	//reset the Motion Detection Unit

/*
 * Config register for PYD
 * */
typedef enum{
	PYD_CONFIG_TYPE_THREADHOLD,
	PYD_CONFIG_TYPE_BLIND_TIME,
	PYD_CONFIG_TYPE_PULSE_COUNTER,
	PYD_CONFIG_TYPE_PULSE_COUNT,
	PYD_CONFIG_TYPE_WINDOW_TIME,
	PYD_CONFIG_TYPE_OPERATION_MODE,
	PYD_CONFIG_TYPE_SIGNAL_SOURCE,
	PYD_CONFIG_TYPE_CUT_OFF,
	PYD_CONFIG_TYPE_COUNT_MODE
}PYDConfigType_e;

typedef enum
{
	PIR_UNMOTION,
	PIR_MOTION
}pirAction_e;

typedef enum
{
	PIR_STATE_DEBOUNCE,
	PIR_STATE_5S,
	PIR_STATE_WAIT_30S,
	PIR_STATE_WAIT_xS
}pirState_t;

typedef enum{
	STATE_WAIT_30S,
	STATE_WAIT_60S,
	STATE_WAIT_120S
}stateWait_e;

typedef void (*pirActionHandle_t)(pirAction_e pinAction);

void PYD1598_Write_Data_Config();
void PYD_ReInit();
void pirInit(pirActionHandle_t userPirHandle);
void PIR_change_time_Scan(stateWait_e state);


#endif /* SOURCE_HARDD_PIR_PIR_H_ */
