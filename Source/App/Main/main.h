/*
 * main.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_MAIN_MAIN_H_
#define SOURCE_APP_MAIN_MAIN_H_

#include <Source/App/Network/network.h>
#include "app/framework/include/af.h"
#include "protocol/zigbee/stack/include/binding-table.h"
#include "protocol/zigbee/app/framework/util/af-main.h"
#include "hal/hal.h"
#include "Source/Mid/led/led.h"
#include "Source/Mid/button/button.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Receive/received.h"
#include "Source/Hardd/TEMP_HUM/temp_hum.h"
#include "Source/Hardd/LM/lightsensor.h"
#include "Source/Hardd/PIR/pir.h"
#include "Source/Hardd/BAT/bat.h"

#define PIR_ENDPOINT					1
#define LIGHTSENSOR_ENDPOINT			2
#define TEMPERATURE_ENDPOINT			3
#define HUMIDITY_ENDPOINT				4

typedef enum
{
	POWER_ON_STATE,
	REPORT_STATE,
	IDLE_STATE,
	SETTING_STATE,
	INPUT_MODE_SETTING_STATE,
	OUTPUT_MODE_SETTING_STATE,
	POWER_MODE_SETTING_STATE,
	REBOOT_STATE
}sysState;

EmberEventControl mainStateEventControl,TempHumEventControl,sendAfter30mEventControl;
sysState systemState ;
bool networkReady;



void mainStateEventHandle();
void userButton_PressAndHoldEventHandle(uint8_t button, uint8_t pressAndHoldEvent);
void userButton_HoldingEventHandle(uint8_t button, BUTTON_Event_t holdingEvent);
void USER_pirHardHandle(pirAction_e pirAction);



#endif /* SOURCE_APP_MAIN_MAIN_H_ */
