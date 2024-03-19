/*
 * received.c
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#include "received.h"
void GET_BindingTalbe(uint8_t cmdID,EmberNodeId source);

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
boolean emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd)
{
	if(cmd->clusterSpecific)
	{
		switch(cmd->apsFrame->clusterId)
		{
		case ZCL_ON_OFF_CLUSTER_ID:
			RECEIVE_HandleOnOffCluster(cmd);
			return true;
		default:
			break;
		}
	}
  return false;
}

/**
 * @func	RECEIVE_HandleBasicCluster
 *
 * @brief	Turn on/off led follow command ID
 *
 * @param	[cmd]: packet message result from callback
 *
 * @retval	none
 */
void RECEIVE_HandleOnOffCluster(EmberAfClusterCommand* cmd)
{
	uint8_t commandID		= cmd->commandId;
	uint8_t endPoint 		= cmd->apsFrame->destinationEndpoint;
	EmberNodeId source		= cmd->source;

	if(cmd->type == EMBER_INCOMING_UNICAST)
	{
		switch(commandID)
		{
		case ZCL_OFF_COMMAND_ID:
			if(endPoint == 1)
			{
				turnOffRBGLed(LED1);
				SEND_OnOffStateReport(endPoint,LED_OFF);
				GET_BindingTalbe(commandID,source);
			}
			else if(endPoint == 2)
			{
				turnOffRBGLed(LED2);
				SEND_OnOffStateReport(endPoint,LED_OFF);
			}

			break;
		case ZCL_ON_COMMAND_ID:
			if(endPoint == 1)
			{
				turnOnLed(LED1,ledBlue);
				SEND_OnOffStateReport(endPoint,LED_ON);
				GET_BindingTalbe(commandID,source);
			}
			else if(endPoint == 2)
			{
				turnOnLed(LED2,ledBlue);
				SEND_OnOffStateReport(endPoint,LED_ON);
			}

			break;
		default:
			break;
		}
	}
}

/** @brief Pre Message Received
 *
 * This callback is the first in the Application Framework's message processing
 * chain. The Application Framework calls it when a message has been received
 * over the air but has not yet been parsed by the ZCL command-handling code. If
 * you wish to parse some messages that are completely outside the ZCL
 * specification or are not handled by the Application Framework's command
 * handling code, you should intercept them for parsing in this callback.

 *   This callback returns a Boolean value indicating whether or not the message
 * has been handled. If the callback returns a value of true, then the
 * Application Framework assumes that the message has been handled and it does
 * nothing else with it. If the callback returns a value of false, then the
 * application framework continues to process the message as it would with any
 * incoming message.
        Note: 	This callback receives a pointer to an
 * incoming message struct. This struct allows the application framework to
 * provide a unified interface between both Host devices, which receive their
 * message through the ezspIncomingMessageHandler, and SoC devices, which
 * receive their message through emberIncomingMessageHandler.
 *
 * @param incomingMessage   Ver.: always
 */
boolean emberAfPreMessageReceivedCallback(EmberAfIncomingMessage* incomingMessage)
{
  return false;
}

void RECEIVE_HandleMessageCluster(EmberAfIncomingMessage* incomingMessage)
{

}

/**
 * @func	GET_BindingTalbe
 *
 * @brief	get binding table
 *
 * @param	[cmdID] : command ID
 *
 * @param	[source] : NodeID of initiatal
 *
 * @retval	none
 */
void GET_BindingTalbe(uint8_t cmdID,EmberNodeId source)
{
	EmberBindingTableEntry talbe;
	for(uint8_t i=0; i<emberAfGetBindingTableSize();i++)
	{
		if(emberGetBinding(i,&talbe) == EMBER_SUCCESS)
		{
			if(talbe.clusterId == ZCL_ON_OFF_CLUSTER_ID)
			{
				emberAfCorePrintln("----------%d-----------",i);
				emberAfCorePrintln("cluster id: %X",talbe.clusterId);
				emberAfCorePrintln("networkIndex: %d",talbe.networkIndex);
				emberAfCorePrintln("type		: %d",talbe.type);
				emberAfCorePrintln("identifier	: %X",talbe.identifier);
				emberAfCorePrintln("local		: %d",talbe.local);
				emberAfCorePrintln("remote		: %d",talbe.remote);
				emberAfCorePrintln("-----------------------");
				if(source != emberGetBindingRemoteNodeId(i))
				{
					SEND_CommanBinding(cmdID,i);
				}
			}
		}
	}
}
