/*
 * send.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_SEND_SEND_H_
#define SOURCE_APP_SEND_SEND_H_

#include "app/framework/include/af.h"
#include "Source/Mid/led/led.h"
#include "Source/App/Network/network.h"
#include "Source/App/Receive/received.h"
#include "Source/App/Main/main.h"
#include "Source/Hardd/PIR/pir.h"

void SEND_ReportInfoToHc();
void SEND_OnOffStateReport(uint8_t endpoint, ledOnOffState_e state);
void SEND_measuredValueReport(uint8_t endpoint,uint16_t value);
void SEND_CommanBinding(uint8_t cmdID, uint8_t indexTable);
void SEND_PirStateReport(uint8_t endpoint,pirAction_e pirAction);
void SEND_temperatureValueReport(uint8_t endpoint,uint16_t value);
void SEND_humidityValueReport(uint8_t endpoint,uint16_t value);
void SEND_BatteryReport(uint8_t endpoint,uint8_t percent);

#endif /* SOURCE_APP_SEND_SEND_H_ */
