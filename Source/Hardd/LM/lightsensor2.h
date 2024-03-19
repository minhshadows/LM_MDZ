/*
 * opt3001.h
 *
 *  Created on: Oct 15, 2020
 *      Author: ryan
 */

#ifndef SOURCE_HARD_OPT3001_OPT3001_H_
#define SOURCE_HARD_OPT3001_OPT3001_H_

#include "em_device.h"
#include <stdbool.h>
//#include <Source/Utility/typedefs.h>

/*******************************************************************************/



/*******************************************************************************/

#define OPT3001_DEV_ID				0x3001

#define OPT3001_ADDR_GND        0x88
#define OPT3001_ADDR_VDD        0x8A
#define OPT3001_ADDR_SDA        0x8C
#define OPT3001_ADDR_SCL        0x8E

#define CONFIG_CMD_LSB			0x00
#define CONFIG_CMD_MSB			0xC2

#define RESULT_REG              0x00
#define CONFIG_REG              0x01
#define LOWLIM_REG              0x02
#define HIGHLIM_REG             0x03
#define MANUID_REG              0x7E
#define DEVID_REG               0x7F

#define CONFIG_RN_MASK          (0xF << 12) /* [15..12] Range Number */
#define CONFIG_RN_AUTO          (0xC << 12)

#define CONFIG_CT               BIT(11) /* [11] Conversion Time */

#define CONFIG_MC_MASK          (0x3 << 9) /* [10..9] Mode of Conversion */
#define CONFIG_MC_SHUTDOWN      (0 << 9)
#define CONFIG_MC_SINGLE        (0x1 << 9)
#define CONFIG_MC_CONTINUOS     (0x2 << 9)

#define CONFIG_OVF              BIT(8) /* [8] Overflow */
#define CONFIG_CRF              BIT(7) /* [7] Conversion Ready Field */
#define CONFIG_FH               BIT(6) /* [6] Flag High */
#define CONFIG_FL               BIT(5) /* [5] Flag Low */
#define CONFIG_L                BIT(4) /* [4] Latch */
#define CONFIG_POL              BIT(3) /* [3] Polarity */
#define CONFIG_ME               BIT(2) /* [2] Mask Exponent */

#define CONFIG_FC_MASK          (0x3 << 0)/* [1..0] Fault Count */

#define STATE_SLEEPING          0
#define STATE_ACTIVE            1
#define STATE_DATA_READY        2

#define GET_EXP(n)              ((n) >> 12)
#define GET_MATISSA(n)          ((n) & 0xFFFF)

#define 	I2C_DEFAULT						I2C0

#define I2C_SENSOR_SCL_PIN					(3U)
#define	I2C_SENSOR_SCL_PORT					(gpioPortC)

#define I2C_SENSOR_SDA_PIN					(2U)
#define I2C_SENSOR_SDA_PORT					(gpioPortC)

#define I2C_TRANSFER_TIMEOUT                      300000


/*******************************************************************************/

boolean Opt3001_CheckInit (void);
uint16_t Opt3001_GetDevIdVal(void);
void Opt3001_MeasureLux(void) ;
uint16_t Opt3001_GetLuxVal(void);

void LM_I2C_Init(void);
void LM_I2C_DeInit(void);

/*******************************************************************************/





#endif /* SOURCE_HARD_OPT3001_OPT3001_H_ */
