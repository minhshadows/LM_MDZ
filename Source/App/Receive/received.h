/*
 * received.h
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_RECEIVE_RECEIVED_H_
#define SOURCE_APP_RECEIVE_RECEIVED_H_

#include "app/framework/include/af.h"
#include "Source/Mid/led/led.h"
#include "Source/App/Network/network.h"
#include "Source/App/Send/send.h"
#include "Source/App/Main/main.h"

void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd);
void RECEIVE_HandleBasicCluster(EmberAfClusterCommand* cmd);

#endif /* SOURCE_APP_RECEIVE_RECEIVED_H_ */
