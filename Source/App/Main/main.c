/*
 * main.c
 *
 *  Created on: Aug 13, 2023
 *      Author: Minh
 */
#include "Source/App/Main/main.h"

uint8_t temperaturePrev = 0;
uint8_t humidityPrev = 0;
uint16_t luxPrev = 0;

void checkTemperatureValue_toSend();
void checkHumidityValue_toSend();
void checkLightValue_toSend();


void fristSend();

/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{
	emberAfCorePrintln("Main Init!!!");
	ledInit();
	buttonInit(userButton_HoldingEventHandle,userButton_PressAndHoldEventHandle);
	networkInit(userNETWORK_EventHandle);
	turnOnLed(LED1,ledRed);
	TempHum_Init();
//	adc_Initt();
//	light_sensor_Init();
//	if (Opt3001_CheckInit() != true) {
//		emberAfCorePrintln("OPT3001 error");
//	}
//	Opt3001_MeasureLux();
	emberEventControlSetDelayMS(TempHumEventControl,3000);
	pirInit(USER_pirHardHandle);
	systemState = POWER_ON_STATE;
	emberEventControlSetActive(mainStateEventControl);
	fristSend();
}

/**
 * @func	mainStateEventHandle
 *
 * @brief	main process
 *
 * @param	[none]
 *
 * @retval	none
 */
void mainStateEventHandle()
{
	EmberNetworkStatus nwkStatusCurrent;

	emberEventControlSetInactive(mainStateEventControl);

	switch(systemState)
	{
	case POWER_ON_STATE:
		nwkStatusCurrent = emberNetworkState();
		if(nwkStatusCurrent == EMBER_NO_NETWORK)
		{
			toggleLed(LED1,ledBlue,3,200,200);
			NETWORK_FindAndJoin();
		}
		systemState = IDLE_STATE;
		break;
	case REPORT_STATE:
		toggleLed(LED1,ledRed,3,300,300);
		networkReady = true;
		systemState = IDLE_STATE;
		SEND_ReportInfoToHc();
		break;
	case IDLE_STATE:
		break;
	case SETTING_STATE:
		turnOnLed(LED1,ledBlue);
		break;
	case REBOOT_STATE:
		systemState = IDLE_STATE;
		halReboot();
		break;
	default:
		break;
	}
}

/**
 * @func	userNETWORK_EventHandle
 *
 * @brief	process follow network state
 *
 * @param	[networkResult]: result from callback
 *
 * @retval	none
 */
void userNETWORK_EventHandle(uint8_t networkResult)
{
	emberAfCorePrintln("NETWORK_EventHandle!!!");
	switch(networkResult)
	{
	case NETWORK_HAS_PARENT:
		toggleLed(LED1,ledBlue,3,300,300);
		emberAfCorePrintln("Has Parenttt!!!");
		networkReady = true;
		systemState = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_JOIN_FAIL:
		systemState = IDLE_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_JOIN_SUCCESS:
		emberAfCorePrintln("Join Success!!!");
		networkReady = true;
		systemState = REPORT_STATE;
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_LOST_PARENT:
		toggleLed(LED1,ledRed,3,300,300);
		systemState = IDLE_STATE;
		emberAfCorePrintln("Losttt!!!");
		emberEventControlSetDelayMS(mainStateEventControl,1000);
		break;
	case NETWORK_OUT_NETWORK:
		if(networkReady)
		{
			toggleLed(LED1,ledPink,3,300,300);
			systemState = REBOOT_STATE;
			emberEventControlSetDelayMS(mainStateEventControl,3000);
		}
		break;
	default:
		break;
	}
}

/**
 * @func	userButton_PressAndHoldEventHandle
 *
 * @brief	button press and hold handle
 *
 * @param	[button]
 *
 * @param	[pressAndHoldEvent]
 *
 * @retval	none
 */
uint8_t in_1,in_2 = 0;
void userButton_PressAndHoldEventHandle(uint8_t button, uint8_t pressAndHoldEvent)
{
	if (button == SW_1) // for endpoint 1
	{
		switch(pressAndHoldEvent)
		{
		case press_1:
			if(systemState == SETTING_STATE)
			{
				PIR_change_time_Scan(STATE_WAIT_30S);
				systemState = IDLE_STATE;
				emberEventControlSetActive(mainStateEventControl);
			}
			emberAfCorePrintln("press_1!!!");
			break;
		case press_2:
			emberAfCorePrintln("press_2!!!");
			if(systemState == SETTING_STATE)
			{
				PIR_change_time_Scan(STATE_WAIT_60S);
				systemState = IDLE_STATE;
				emberEventControlSetActive(mainStateEventControl);
			}
			break;
		case press_3:// target find
			emberAfCorePrintln("press_3!!!");
			if(systemState == SETTING_STATE)
			{
				PIR_change_time_Scan(STATE_WAIT_120S);
				systemState = IDLE_STATE;
				emberEventControlSetActive(mainStateEventControl);
			}
			break;
		case press_4:// initiator find
			emberAfCorePrintln("press_4!!!");
			break;
		case press_5:
			emberAfCorePrintln("press_5!!!");
//			NETWORK_Leave();
			break;
		case unknown:
			break;
		default:
			break;
		}
	}
}

/**
 * @func	userButton_HoldingEventHandle
 *
 * @brief	button hold handle
 *
 * @param	[button]
 *
 * @param	[holdingEvent]
 *
 * @retval	none
 */
void userButton_HoldingEventHandle(uint8_t button, BUTTON_Event_t holdingEvent)
{
	if (button == SW_1) // for endpoint 1
	{
		switch(holdingEvent)
		{
		case hold_1s:
			emberAfCorePrintln("hold_1s!!!");
			break;
		case hold_2s:
			emberAfCorePrintln("hold_2s!!!");
			break;
		case hold_3s:// target find
			emberAfCorePrintln("hold_3s!!!");
			break;
		case hold_4s:// initiator find
			emberAfCorePrintln("hold_4s!!!");
			break;
		case hold_5s:
			emberAfCorePrintln("hold_5s!!!");
			break;
		case hold_6s:
			emberAfCorePrintln("hold_6s!!!");
			break;
		case hold_7s:
			emberAfCorePrintln("hold_7s!!!");
			break;
		case hold_8s:
			emberAfCorePrintln("hold_8s!!!");
			break;
		case hold_9s:
			emberAfCorePrintln("hold_9s!!!");
			break;
		case hold_10s:
			emberAfCorePrintln("hold_10s!!!");
			systemState = SETTING_STATE;
			emberEventControlSetActive(mainStateEventControl);
			break;
		case unknown:
			break;
		default:
			break;
		}
	}
}

/**
 * @func	USER_pirHardHandle
 *
 * @brief	PIR handle
 *
 * @param	[pirAction] : Motion or Unmotion
 *
 * @retval	none
 */
void USER_pirHardHandle(pirAction_e pirAction)
{
	static boolean sendFlag = true;
	switch(pirAction)
	{
	case PIR_MOTION:
		if(sendFlag)
		{
			sendFlag = false;
			// send PIR report
			SEND_PirStateReport(PIR_ENDPOINT,PIR_MOTION);
		}
		toggleLed(LED1,ledPink,1,300,300);
		emberAfCorePrintln("motion!!!");
		break;
	case PIR_UNMOTION:
		sendFlag = true;
		// send PIR report
		SEND_PirStateReport(PIR_ENDPOINT,PIR_UNMOTION);
		emberAfCorePrintln("unmotionnnnnnnnnnn!!!");
		break;
	default:
		break;
	}
}

/**
 * @func	checkTemperatureValue_toSend
 *
 * @brief	Calculate temperature send to HC
 *
 * @param	none
 *
 * @retval	none
 */
void checkTemperatureValue_toSend()
{
	uint8_t temperature = (TemHumSensor_getTemperature()/100);

	if (temperature > temperaturePrev)
	{
		if((temperature - temperaturePrev) >= 2)
		{
			//send temp
			temperaturePrev = temperature;
			SEND_temperatureValueReport(TEMPERATURE_ENDPOINT,temperature);
			emberAfCorePrintln("send!!!");
		}
	}
	else if(temperature < temperaturePrev)
	{
		if((temperaturePrev - temperature) >= 2)
		{
			//send temp
			temperaturePrev = temperature;
			SEND_temperatureValueReport(TEMPERATURE_ENDPOINT,temperature);
			emberAfCorePrintln("send!!!");
		}
	}
	emberAfCorePrintln("temperature = %d!!!",temperature);

}

/**
 * @func	checkHumidityValue_toSend
 *
 * @brief	Calculate humidity send to HC
 *
 * @param	none
 *
 * @retval	none
 */
void checkHumidityValue_toSend()
{
	uint8_t humidity = TemHumSensor_getHumidity();

	if (humidity > humidityPrev)
	{
		if((humidity - humidityPrev) >= 2)
		{
			//send humi
			humidityPrev = humidity;
			SEND_humidityValueReport(HUMIDITY_ENDPOINT,humidity);
			emberAfCorePrintln("send 1111!!!");
		}
	}
	else if(humidity < humidityPrev)
	{
		if((humidityPrev - humidity) >= 2)
		{
			//send humi
			humidityPrev = humidity;
			SEND_humidityValueReport(HUMIDITY_ENDPOINT,humidity);
			emberAfCorePrintln("send 2222!!!");
		}
	}
	emberAfCorePrintln("humidity = %d %%!!!",humidity);
}

/**
 * @func	USER_pirHarcheckLightValue_toSenddHandle
 *
 * @brief	Calculate light value send to HC
 *
 * @param	none
 *
 * @retval	none
 */
void checkLightValue_toSend()
{
	uint16_t lux = OPT3001_GetValue();

	if (lux > luxPrev)
	{
		if((lux - luxPrev) >= 100)
		{
			//send humi
			luxPrev = lux;
			SEND_measuredValueReport(LIGHTSENSOR_ENDPOINT,lux);
		}
	}
	else if(lux < luxPrev)
	{
		if((luxPrev - lux) >= 100)
		{
			//send humi
			luxPrev = lux;
			SEND_measuredValueReport(LIGHTSENSOR_ENDPOINT,lux);
		}
	}
	emberAfCorePrintln("humidity = %d %%!!!",humidity);
}

/**
 * @func	TempHumEventHandler()
 *
 * @brief	Handler of TempHumEvent
 *
 * @param	none
 *
 * @retval	none
 */
void TempHumEventHandler()
{
	// First thing to do inside a delay event is to disable the event till next usage
	emberEventControlSetInactive(TempHumEventControl);

	//Do something
	emberAfCorePrintln("--------------------------");
	checkTemperatureValue_toSend();
	checkHumidityValue_toSend();
	checkLightValue_toSend();
//	battery=read_ADCvalue();
//	emberAfCorePrintln("battery before %d ",battery);
//	battery = (battery * 3.3) / 4095;
//	emberAfCorePrintln("battery %d ",battery);
//	LM_I2C_Init();
//	Opt3001_MeasureLux();
//	uint16_t lux = Opt3001_GetLuxVal();
//	emberAfCorePrintln("lux = %d",lux);
//	LM_I2C_DeInit();
	emberAfCorePrintln("--------------------------");

	//Reschedule the event after a delay of 10 seconds
	emberEventControlSetDelayMS(TempHumEventControl,5000);
}

/**
 * @func	fristSend
 *
 * @brief	send and set event to send every 30m
 *
 * @param	none
 *
 * @retval	none
 */
void fristSend()
{
	uint8_t temperature = (TemHumSensor_getTemperature()/100);
	uint8_t humidity	= TemHumSensor_getHumidity();
	uint16_t lux		= OPT3001_GetValue();

	SEND_temperatureValueReport(TEMPERATURE_ENDPOINT,temperature);
	SEND_humidityValueReport(HUMIDITY_ENDPOINT,humidity);
	SEND_measuredValueReport(LIGHTSENSOR_ENDPOINT,lux);

	//Send every 30m
	emberEventControlSetDelayMinutes(sendAfter30mEventControl,30);
}

/**
 * @func	sendAfter30mEventHandle
 *
 * @brief	Handler of sendAfter30mEvent
 *
 * @param	none
 *
 * @retval	none
 */
void sendAfter30mEventHandle()
{
	emberEventControlSetInactive(sendAfter30mEventControl);
	uint8_t temperature = (TemHumSensor_getTemperature()/100);
	uint8_t humidity	= TemHumSensor_getHumidity();
	uint16_t lux		= OPT3001_GetValue();

	SEND_temperatureValueReport(TEMPERATURE_ENDPOINT,temperature);
	SEND_humidityValueReport(HUMIDITY_ENDPOINT,humidity);
	SEND_measuredValueReport(LIGHTSENSOR_ENDPOINT,lux);

	emberEventControlSetDelayMinutes(sendAfter30mEventControl,30);
}
