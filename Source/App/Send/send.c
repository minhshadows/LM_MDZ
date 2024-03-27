/*
 * send.c
 *
 *  Created on: Sep 20, 2023
 *      Author: Minh
 */

#include "send.h"

/**
 * @func	SEND_FillBufferGlobalCommand
 *
 * @brief	Fill data to buffer
 *
 * @param	[EmberAfClusterId]	: cluster id
 *
 * @param	[attributeID]		: attribute id
 *
 * @param	[globalCommand]		: global command
 *
 * @param	[value]				: pointer value
 *
 * @param	[length]			: length value
 *
 * @param	[datatype]			: data type
 *
 * @retval	none
 */
static void SEND_FillBufferGlobalCommand(EmberAfClusterId clusterID,
										EmberAfAttributeId aitributeID,
										uint8_t globalCommand,
										uint8_t* value,
										uint8_t length,
										uint8_t datatype)
{
	uint8_t data[255];
	data[0] = (uint8_t) (aitributeID & 0x00FF);
	data[1] = (uint8_t) ((aitributeID & 0xFF00) >> 8);
	data[2] = EMBER_SUCCESS;
	data[3] = (uint8_t) datatype;
	memcpy(&data[4], value, length);

	(void) emberAfFillExternalBuffer((ZCL_GLOBAL_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
										clusterID,
										globalCommand,
										"b",
										data,
										length+4);
}

/**
 * @func	SEND_SendCommandUnicast
 *
 * @brief	send command from source to destination
 *
 * @param	[source]: source send
 *
 * @param	[destination]: destination receive
 *
 * @param	[address]: address of destination
 *
 * @retval	none
 */
static void SEND_SendCommandUnicast(uint8_t source,uint8_t destination,uint16_t address)
{
	emberAfSetCommandEndpoints(source,destination);
	(void) emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, address);
}

/**
 * @func	SEND_CommanBinding
 *
 * @brief	Send command from initiator to target
 *
 * @param	[cmdID]: command ID
 *
 * @param	[indexTable]: index of table
 *
 * @retval	none
 */
void SEND_CommanBinding(uint8_t cmdID, uint8_t indexTable)
{
	emberAfFillExternalBuffer(ZCL_CLUSTER_SPECIFIC_COMMAND | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER ,
								ZCL_ON_OFF_CLUSTER_ID,
								cmdID,
								"");

	emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT,emberGetBindingRemoteNodeId(indexTable));
}

/**
 * @func	SEND_ReportInfoToHc
 *
 * @brief	Send model ID to HC
 *
 * @param	none
 *
 * @retval	none
 */
void SEND_ReportInfoToHc()
{
	uint8_t modelID[24] = {23, 'P', 'I', 'R','1','_','L','M','1','_','T','M','P','1','_','H','M','P','1','_','B','A','T','1'};
//	uint8_t manufactureID[5] = {4, 'L', 'u', 'm', 'i'};
//	uint8_t version = 1;
	uint16_t addressHC = 0x0000;//emberGetParentId();
	emberAfCorePrintln("Report!!!");
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	SEND_FillBufferGlobalCommand(ZCL_BASIC_CLUSTER_ID,
								ZCL_MODEL_IDENTIFIER_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								modelID,
								24,
								ZCL_CHAR_STRING_ATTRIBUTE_TYPE);


	SEND_SendCommandUnicast(1,1,addressHC);
}

/**
 * @func	SEND_OnOffStateReport
 *
 * @brief	Send led state to HC
 *
 * @param	[endpoint]: endpoint of source
 *
 * @param	[state]: state of led
 *
 * @retval	none
 */
void SEND_OnOffStateReport(uint8_t endpoint, ledOnOffState_e state)
{
	uint16_t addressHC =0x0000; //emberGetParentId();
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
								ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								(uint8_t*) &state,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_ON_OFF_CLUSTER_ID,
								ZCL_ON_OFF_ATTRIBUTE_ID,
								ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
								(uint8_t*) &state,
								1,
								ZCL_BOOLEAN_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(endpoint,1,addressHC);

}


/**
 * @func	SEND_measuredValueReport
 *
 * @brief	Send value of light sensor to HC
 *
 * @param	[endpoint]: endpoint of source
 *
 * @param	[value]: value to send
 *
 * @retval	none
 */
void SEND_measuredValueReport(uint8_t endpoint,uint16_t value)
{
	uint16_t addressHC = 0x0000;

	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}

	emberAfWriteServerAttribute(endpoint,
			ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
			ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
			(uint8_t*) &value,
			ZCL_INT16U_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_ILLUM_MEASUREMENT_CLUSTER_ID,
			ZCL_ILLUM_MEASURED_VALUE_ATTRIBUTE_ID,
			ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
			(uint8_t *)&value,
			2,
			ZCL_INT16U_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(endpoint,1,addressHC);
}

/**
 * @func	SEND_PirStateReport
 *
 * @brief	send PIR action to HC
 *
 * @param	[endpoint]: endpoint of source
 *
 * @param	[pirAction]: motion or unmotion
 *
 * @retval	none
 */
void SEND_PirStateReport(uint8_t endpoint,pirAction_e pirAction)
{
	uint16_t addressHC = 0x00; //HC
	emberAfCorePrintln("Report!!!");
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
			ZCL_IAS_ZONE_CLUSTER_ID,
			ZCL_ZONE_STATUS_ATTRIBUTE_ID,
			(uint8_t*) &pirAction,
			ZCL_ENUM8_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,
			ZCL_ZONE_STATUS_ATTRIBUTE_ID,
			ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
			&pirAction,
			1,
			ZCL_ENUM8_ATTRIBUTE_TYPE);
	//ZCL_ENUM8_ATTRIBUTE_TYPE

	SEND_SendCommandUnicast(endpoint,1,addressHC);
}

/**
 * @func	SEND_temperatureValueReport
 *
 * @brief	send value of temperature to HC
 *
 * @param	[endpoint]: endpoint of source
 *
 * @param	[value]: value of temperature
 *
 * @retval	none
 */
void SEND_temperatureValueReport(uint8_t endpoint,uint16_t value)
{
	uint16_t addressHC = 0x00; //HC
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
									ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
									ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
									(uint8_t*) &value,
									ZCL_INT16S_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_TEMP_MEASUREMENT_CLUSTER_ID,
									ZCL_TEMP_MEASURED_VALUE_ATTRIBUTE_ID,
									ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
									(uint8_t *)&value,
									2,
									ZCL_INT16S_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint,1,addressHC);
}

/**
 * @func	SEND_humidityValueReport
 *
 * @brief	send value of humidity to HC
 *
 * @param	[endpoint]: endpoint of source
 *
 * @param	[value]: value of humidity
 *
 * @retval	none
 */
void SEND_humidityValueReport(uint8_t endpoint,uint16_t value)
{
	uint16_t addressHC = 0x00; //HC
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
									ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID,
									ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID,
									(uint8_t*) &value,
									ZCL_INT16S_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_RELATIVE_HUMIDITY_MEASUREMENT_CLUSTER_ID,
									ZCL_RELATIVE_HUMIDITY_MEASURED_VALUE_ATTRIBUTE_ID,
									ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,
									(uint8_t *)&value,
									2,
									ZCL_INT16S_ATTRIBUTE_TYPE);
	SEND_SendCommandUnicast(endpoint,1,addressHC);
}

/**
 * @func	SEND_BatteryReport
 *
 * @brief	Send battery to HC
 *
 * @param	[endpoint]:
 *
 * @param	[percent]: percent power battery
 *
 * @retval	none
 */
void SEND_BatteryReport(uint8_t endpoint,uint8_t percent)
{
	uint16_t address = 0x0000;
	emberAfCorePrintln("Report!!!");
	if(emberAfNetworkState() != EMBER_JOINED_NETWORK)
	{
		return;
	}
	emberAfWriteServerAttribute(endpoint,
								ZCL_IAS_ZONE_CLUSTER_ID,
								ZCL_ZONE_STATUS_ATTRIBUTE_ID,
								(uint8_t*) &percent,
								ZCL_BITMAP16_ATTRIBUTE_TYPE);
	SEND_FillBufferGlobalCommand(ZCL_IAS_ZONE_CLUSTER_ID,ZCL_ZONE_STATUS_ATTRIBUTE_ID,ZCL_READ_ATTRIBUTES_RESPONSE_COMMAND_ID,&percent,1,ZCL_BITMAP16_ATTRIBUTE_TYPE);

	SEND_SendCommandUnicast(endpoint,1,address);
}
