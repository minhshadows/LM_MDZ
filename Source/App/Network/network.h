/*
 * network.h
 *
 *  Created on: Aug 24, 2023
 *      Author: Minh
 */

#ifndef SOURCE_APP_NETWORK_NETWORK_H_
#define SOURCE_APP_NETWORK_NETWORK_H_

#include "app/framework/include/af.h"
#include "hal/hal.h"
#include "Source/App/Send/send.h"
#include "Source/App/Receive/received.h"
#include "Source/App/Main/main.h"

typedef enum
{
	NETWORK_HAS_PARENT,
	NETWORK_JOIN_FAIL,
	NETWORK_JOIN_SUCCESS,
	NETWORK_LOST_PARENT,
	NETWORK_OUT_NETWORK
}networkResult;

typedef void (*networkHandle_t)(networkResult result);

void networkInit(networkHandle_t userNetworkHandle);
void userNETWORK_EventHandle(uint8_t networkResult);
void NETWORK_FindAndJoin();
void NETWORK_StopFinAndJoin();
void NETWORK_Leave();

#endif /* SOURCE_APP_NETWORK_NETWORK_H_ */
