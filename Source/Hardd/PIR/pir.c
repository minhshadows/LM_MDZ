/*
 * pir.c
 *
 *  Created on: Mar 4, 2024
 *      Author: Minh
 */
#include "pir.h"
#include "ustimer.h"
#include "Source/Mid/led/led.h"

pirState_t pirState;
static uint32_t configData = 0;
static uint32_t time_delay = 30000 ;
static bool initializedPIR = false;

EmberEventControl pirDetechEventControl;
pirActionHandle_t pirHardHandle = NULL;
static void PIR_INTSignalHandle(uint8_t pin);
void PIR_Enable(boolean en);

/**
 * @func	pirInit
 *
 * @brief	Initialize PIR sensor
 *
 * @param	[userPirHandle]	: user Handler
 *
 * @retval	none
 */
void pirInit(pirActionHandle_t userPirHandle)
{

	CMU_ClockEnable(cmuClock_HCLK, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	USTIMER_Init();

	GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModePushPull,0);
	GPIO_PinModeSet(PIR_SER_PORT,PIR_SER_PIN,gpioModePushPull,0);

	pirHardHandle=userPirHandle;

	configData = 0x00304D10;
	//config
	PYD1598_WriteRegister(0x00304D10);	//Default window time, threadhold,...
	PYD1598_WriteConfig(PYD_CONFIG_TYPE_BLIND_TIME, 50);	//0.5S
//	PYD1598_WriteConfig(PYD_CONFIG_TYPE_PULSE_COUNT, 1);	//1 PULSES
	PYD1598_WriteConfig(PYD_CONFIG_TYPE_WINDOW_TIME, 5);
	PYD1598_WriteConfig(PYD_CONFIG_TYPE_THREADHOLD, 15);

	/*Register callbacks before setting up and enabling pin interrupt. */

	initializedPIR = true;
	PYD_ReInit();

	emberAfCorePrintln("PIR init done");
	/*Set rising and falling edge interrupts */
}

/**
 * @func	PYD1598_WriteRegister
 *
 * @brief	Write data to resister PYD1598
 *
 * @param	[value]	: value to write
 *
 * @retval	none
 */
void PYD1598_WriteRegister(uint32_t value)
{
	uint8_t nextBit = 0;
	uint32_t regMask = 0x01000000;

	for(uint8_t i = 0; i < 25; i++)
	{
		nextBit = (value & regMask) != 0;
		regMask = regMask >> 1;
		SI_LINE_HIGH_TO_LOW;
		USTIMER_DelayIntSafe(1);
		SI_LINE_LOW_TO_HIGH;
		USTIMER_DelayIntSafe(1);
		if(nextBit == 0 )
		{
			SI_LINE_HIGH_TO_LOW;
		}
		else
		{
			SI_LINE_LOW_TO_HIGH;
		}
		USTIMER_DelayIntSafe(T_SHD);

	}
	SI_LINE_HIGH_TO_LOW;
	USTIMER_DelayIntSafe(T_SLT);

}

/**
 * @func	PIR_ConvertBlindTime
 *
 * @brief	calculate Bind time
 *
 * @param	[byBlindTime]	: Bind time user want
 *
 * @retval	[uint8_t]		: Bind time
 */
//blind time(0.5 -> 8s), = 0.5 s + [Reg Val] · 0.5 s => Reg Val = (time-0.5)/0.5
//byBlidTime = x10 time
uint8_t PIR_ConvertBlindTime(uint8_t byBlindTime){
	if(byBlindTime < 5 || byBlindTime > 80){
		return 0;
	}
	return (uint8_t)(byBlindTime-5)/5;
}

/**
 * @func	PIR_ConvertWindowTime
 *
 * @brief	calculate window time of PYD1598
 *
 * @param	[byWindowTime]	: window time user want
 *
 * @retval	[uint8_t]		: window time
 */
//TIME = 2 s + [Reg Val] · 2 s
uint8_t PIR_ConvertWindowTime(uint8_t byWindowTime){
	if(byWindowTime < 2 || byWindowTime > 8){
		return 0;
	}
	return (uint8_t)(byWindowTime-2)/2;
}

/**
 * @func	PYD1598_WriteConfig
 *
 * @brief	Write configure data to PYD1598 resister
 *
 * @param	[byType]	: Type config
 *
 * @param	[byValue]	: value config
 *
 * @retval	none
 */
void PYD1598_WriteConfig(uint8_t byType, uint8_t byValue){
	switch(byType){
	case PYD_CONFIG_TYPE_BLIND_TIME:
		configData &= 0xFEFE1FFF;	//Clear bit 13->16
		configData |= (((uint32_t)PIR_ConvertBlindTime(byValue) << 13) & 0xFFFFFFFF);
		break;
	case PYD_CONFIG_TYPE_PULSE_COUNTER:
		configData &= 0xFEFFF7FF;	//Clear bit 11->12
		configData |= (((uint32_t)(byValue-1) << 11) & 0xFFFFFFFF);
		break;
	case PYD_CONFIG_TYPE_WINDOW_TIME:
		configData &= 0xFFFFF7FF;	//Clear bit 9->10 |
		configData |= (((uint32_t)PIR_ConvertWindowTime(byValue) << 9) & 0xFFFFFFFF);
		break;
	case PYD_CONFIG_TYPE_THREADHOLD:	//17->24
		configData &= 0xFE01FFFF;	//Clear bit 17->24
		configData |= (((uint32_t)byValue << 17) & 0xFFFFFFFF);
		break;
	default:
		break;
	}
	PYD1598_WriteRegister(configData);
}

/**
 * @func	PYD1598_Reset_PIR
 *
 * @brief	reset PIR sensor
 *
 * @param	none
 *
 * @retval	none
 */
void PYD1598_Reset_PIR()
{
	DL_LINE_OUTPUT_MODE;
	DL_LINE_HIGH_TO_LOW;
	USTIMER_Delay(120);
	DL_LINE_INPUT_MODE;
}

/**
 * @func	PYD_ReInit
 *
 * @brief	Reinitialize PIR
 *
 * @param	none
 *
 * @retval	none
 */
void PYD_ReInit(){
	GPIOINT_Init();
	CMU_ClockEnable(cmuClock_HCLK, true);
	CMU_ClockEnable(cmuClock_GPIO, true);
	GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModeInput,0);
	GPIOINT_CallbackRegister(PIR_DIR_PIN,PIR_INTSignalHandle);
	GPIO_ExtIntConfig(PIR_DIR_PORT,PIR_DIR_PIN,PIR_DIR_PIN,true,true,true);
}

/**
 * @func	PIR_Enable
 *
 * @brief	enable pir
 *
 * @param	[en]	: enable or disable
 *
 * @retval	none
 */
void PIR_Enable(boolean en)
{
	if(en)
	{
		GPIO_ExtIntConfig(PIR_DIR_PORT,PIR_DIR_PIN,PIR_DIR_PIN,true,true,true);
	}
	else
	{
		GPIO_ExtIntConfig(PIR_DIR_PORT,PIR_DIR_PIN,PIR_DIR_PIN,false,false,false);
	}
}

/**
 * @func	PIR_INTSignalHandle
 *
 * @brief	interrupt of PIR sensor
 *
 * @param	[pin]	: interrupt pin
 *
 * @retval	none
 */
static void PIR_INTSignalHandle(uint8_t pin)
{
	uint8_t pinStateNow;
	emberAfCorePrintln("%d_%d",pin,GPIO_PinInGet(PIR_DIR_PORT, PIR_DIR_PIN));
	if(pin != PIR_DIR_PIN)
	{
		return;
	}
	pinStateNow = GPIO_PinInGet(PIR_DIR_PORT, PIR_DIR_PIN);
	//Xử lý nhiễu và chỉ thay đổi trạng thái mới vào đây

	pirState = PIR_STATE_DEBOUNCE;
	emberEventControlSetActive(pirDetechEventControl);
		emberAfCorePrintln("PIR state = %d", pinStateNow);

	GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModePushPull,0);
	USTIMER_DelayIntSafe(35);


	GPIO_PinModeSet(PIR_DIR_PORT,PIR_DIR_PIN,gpioModeInput,0);
	GPIOINT_CallbackRegister(PIR_DIR_PIN,PIR_INTSignalHandle);
	GPIO_ExtIntConfig(PIR_DIR_PORT,PIR_DIR_PIN,PIR_DIR_PIN,true,true,true);
}

/**
 * @func	pirDetechEventHandle
 *
 * @brief	Handler of pirDetechEvent
 *
 * @param	none
 *
 * @retval	none
 */
void pirDetechEventHandle()
{
	emberAfCorePrintln("pirDetechEventHandle");
	emberEventControlSetInactive(pirDetechEventControl);

	switch(pirState)
	{
	case PIR_STATE_DEBOUNCE:
		emberAfCorePrintln("PIR_DETECH_MOTION");
		pirState = PIR_STATE_WAIT_xS;
		if(pirHardHandle != NULL)
		{
			pirHardHandle(PIR_MOTION);
		}
		emberEventControlSetDelayMS(pirDetechEventControl,time_delay);
		break;
	case PIR_STATE_WAIT_xS:

		if(pirHardHandle != NULL)
		{
			emberAfCorePrintln("PIR_DETECH_UNMOTION");
			pirHardHandle(PIR_UNMOTION);
		}
		break;
	default:
		break;
	}
}


/*
 * @func	PIR_change_time_Scan
 *
 * @brief	change pir time scan
 *
 * @param	state wait scan
 *
 * @retval	None
 */
void PIR_change_time_Scan(stateWait_e state)
{
	switch(state)
	{
	case STATE_WAIT_30S:
		time_delay = 30000;
		break;
	case STATE_WAIT_60S:
		time_delay = 60000;
		break;
	case STATE_WAIT_120S:
		time_delay = 120000;
		break;
	default:
		break;
	}
	emberAfCorePrintln("%d!!!",state);
}
