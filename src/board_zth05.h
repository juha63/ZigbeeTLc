/*
 * board_mho_zth5.h
 *
 *      Author: pvvx
 *	Edited by: juha63
 */

#ifndef _BOARD_ZTH05__H_
#define _BOARD_ZTH05__H_

#include "version_cfg.h"

#if (BOARD == BOARD_ZTH05)

#define ZIGBEE_TUYA_OTA 	1

#define RF_TX_POWER_DEF		RF_POWER_INDEX_P1p99dBm // CR2032 !

// TLSR825x 1M Flash
// GPIO_PB1 - TX
// GPIO_PB4 - KEY
// GPIO_PB5 - LED
// GPIO_PB7 - RX
// GPIO_PC2 - SDA
// GPIO_PC3 - SCL

// VBAT
#define SHL_ADC_VBAT		B0P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PB0 // missing pin on case TLSR825x
#define PB0_INPUT_ENABLE	0
#define PB0_DATA_OUT		1
#define PB0_OUTPUT_ENABLE	1
#define PB0_FUNC			AS_GPIO

// BUTTON
#define BUTTON1             GPIO_PB4
#define PB4_FUNC			AS_GPIO
#define PB4_OUTPUT_ENABLE	0
#define PB4_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PB4	PM_PIN_PULLUP_1M

// No LED on ZTH05

// LED
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB5
#define PB5_FUNC	  		AS_GPIO
#define PB5_OUTPUT_ENABLE	1
#define PB5_INPUT_ENABLE	1
#define PB5_DATA_OUT		LED_OFF


// I2C
#define I2C_CLOCK	400000 // Hz
#define SENSOR_TYPE SENSOR_AHT2X3X
#define USE_SENSOR_ID		0

#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

// DISPLAY
#define SHOW_SMILEY			1
#define	USE_DISPLAY			5
#define LCD_BUF_SIZE			0xB
#define LCD_INIT_DELAY()

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif // BOARD == BOARD_ZTH05
#endif /* _BOARD_ZTH05_H_ */
