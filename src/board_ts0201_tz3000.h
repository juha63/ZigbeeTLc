/*
 * board_mho_c122.h
 *
 *  Created on: 12 нояб. 2023 г.
 *      Author: pvvx
 */
#pragma once

#ifndef BOARD_TS0201_TZ3000_H_
#define BOARD_TS0201_TZ3000_H_
// TLSR825x 1M Flash
// GPIO_PA7 - SWS, free, (debug TX)
// GPIO_PB4 - LED (to GND)
// GPIO_PC0 - KEY (to GND)
// GPIO_PC2 - SDA, used I2C
// GPIO_PC3 - SCL, used I2C
// GPIO_PD7 - ALERT (CHT8305)

// BUTTON
#define BUTTON1             GPIO_PC0
#define PC0_FUNC			AS_GPIO
#define PC0_OUTPUT_ENABLE	0
#define PC0_INPUT_ENABLE	1
#define	PULL_WAKEUP_SRC_PC0	PM_PIN_PULLUP_10K

#define I2C_CLOCK	400000 // Hz
#define SENSOR_TYPE SENSOR_CHT8305
#define I2C_SCL 	GPIO_PC2
#define I2C_SDA 	GPIO_PC3
#define I2C_GROUP 	I2C_GPIO_GROUP_C2C3
#define PULL_WAKEUP_SRC_PC2	PM_PIN_PULLUP_10K
#define PULL_WAKEUP_SRC_PC3	PM_PIN_PULLUP_10K

#define	USE_DISPLAY			0
#define LED_ON				1
#define LED_OFF				0
#define GPIO_LED			GPIO_PB4
#define PB4_FUNC	  		AS_GPIO
#define PB4_OUTPUT_ENABLE	1
#define PB4_INPUT_ENABLE	1
#define PB4_DATA_OUT		LED_OFF

#define SHL_ADC_VBAT		C5P // see in adc.h ADC_InputPchTypeDef
#define GPIO_VBAT			GPIO_PC5 // missing pin on case TLSR825x
#define PC5_INPUT_ENABLE	0
#define PC5_DATA_OUT		1
#define PC5_OUTPUT_ENABLE	1
#define PC5_FUNC			AS_GPIO

// UART
#if ZBHCI_UART
	#error please configurate uart PIN!!!!!!
#endif

// DEBUG
#if UART_PRINTF_MODE
	#define	DEBUG_INFO_TX_PIN	    GPIO_SWS //print
#endif

#endif /* BOARD_TS0201_TZ3000_H_ */