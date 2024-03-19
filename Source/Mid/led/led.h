/*
 * led.h
 *
 *  Created on: Aug 13, 2023
 *      Author: Minh
 */

#ifndef SOURCE_MID_LED_LED_H_
#define SOURCE_MID_LED_LED_H_

/******************************************************************************/
/*                     		DEFINITIONS            				              */
/******************************************************************************/
#define LED_RGB_COUNT				1
#define LED_RGB_ELEMENT				2

#define LED_RGB_DIM_T				10000

#define LED_PORT					gpioPortB
#define LED_BLUE_PIN				(0U)
#define LED_RED_PIN					(1U)

#define LED_RGB_1                       { { LED_PORT, LED_RED_PIN },{ LED_PORT, LED_BLUE_PIN } }

#define TIMER_DIM_INIT                                                           \
{                                                                                   \
    false,                 /* Enable timer when initialization completes. */           \
    false,                /* Stop counter during debug halt. */                       \
    timerPrescale1,       /* No prescaling. */                                        \
    timerClkSelHFPerClk,  /* Select HFPER / HFPERB clock. */                          \
    false,                /* Not 2x count mode. */                                    \
    false,                /* No ATI. */                                               \
    timerInputActionNone, /* No action on falling input edge. */                      \
    timerInputActionNone, /* No action on rising input edge. */                       \
    timerModeUp,          /* Up-counting. */                                          \
    false,                /* Do not clear DMA requests when DMA channel is active. */ \
    false,                /* Select X2 quadrature decode mode (if used). */           \
    false,                /* Disable one shot. */                                     \
    false                 /* Not started/stopped/reloaded by other timers. */         \
}
typedef enum{
	LED1,
	LED2
}ledNumber;
typedef enum
{
	ledOff 				= 0x000,
	ledRed				= BIT(0),
	ledBlue				= BIT(1),
	ledPink				= ledRed  | ledBlue,
}ledColor_e;

typedef enum
{
	red,
	green,
	blue,
	off
}ledState_e;

enum{
	LED_FREE,
	LED_TOGGLE
};

typedef enum
{
	LED_OFF,
	LED_ON
}ledOnOffState_e;

/* Function prototypes -----------------------------------------------*/
void ledInit(void);
void turnOffRBGLed(ledNumber index);
void toggleLedHandle(ledNumber ledIndex);
void toggleLed(ledNumber ledIndex, ledColor_e color, uint8_t toggleTime, uint32_t onTimeMs, uint32_t offTimeMs);
void turnOnLed(ledNumber index, ledColor_e color);

void toggleLedHandle(ledNumber ledIndex);
void toggleLed(ledNumber ledIndex, ledColor_e color, uint8_t toggleTime, uint32_t onTimeMs, uint32_t offTimeMs);


#endif /* SOURCE_MID_LED_LED_H_ */
