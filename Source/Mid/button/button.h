/*
 * button.h
 *
 *  Created on: Jul 14, 2023
 *      Author: Minh
 */

#ifndef SOURCE_MID_BUTTON_BUTTON_H_
#define SOURCE_MID_BUTTON_BUTTON_H_

#include "stddef.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "gpiointerrupt.h"

#define BUTTON_COUNT					1
#define GPIO_DOUT						0
#define BUTTON_DEBOUNCE					5
#define BUTTON_CHECK_HOLD_CYCLES_MS		200
#define BUTTON_CHECK_RELEASE_MS			600

#define HOLD_TIME_MIN					5


#define BUTTON_1_PORT				gpioPortA
#define BUTTON_1_PIN				(6U)

#define BUTTON_INIT					{{BUTTON_1_PORT,BUTTON_1_PIN}}



typedef enum
{
	press_1	= 1,
	press_2 ,
	press_3 ,
	press_4 ,
	press_5,
	press_max=10,
	hold_1s	,
	hold_2s	,
	hold_3s	,
	hold_4s	,
	hold_5s ,
	hold_6s,
	hold_7s,
	hold_8s,
	hold_9s,
	hold_10s,
	hold_max,
	unknown,
}BUTTON_Event_t;

enum
{
	SW_1,
	SW_2
};

enum
{
	BUTTON_PRESS,
	BUTTON_RELEASE
};

typedef struct {
  GPIO_Port_TypeDef		port;
  unsigned int			pin;
  bool					state;
  uint8_t				pressCount;
  uint8_t				holdTime;
  bool					release;
  bool					press;
  bool                  isHolding;
} BUTTONx_t;


typedef void (*BUTTON_holdingEvent_t)(uint8_t btIndex, BUTTON_Event_t holdEven );
typedef void (*BUTTON_pressAndHoldEvent_t)(uint8_t btIndex, BUTTON_Event_t pressAndHoldevent);

void buttonInit(BUTTON_holdingEvent_t holdingHandle,BUTTON_pressAndHoldEvent_t pressAndHoldeHandler);

void userButton_PressAndHoldEventHandle(uint8_t button, uint8_t pressAndHoldEvent);
void userButton_HoldingEventHandle(uint8_t button, BUTTON_Event_t holdingEvent);



#endif /* SOURCE_MID_BUTTON_BUTTON_H_ */
