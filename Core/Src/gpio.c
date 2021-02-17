/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 * Free pins are configured automatically as Analog (this feature is enabled through
 * the Code Generation settings)
 */
void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(UsrLed_Port, UsrLed_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(SPISS_Port, SS595_Pin | SS165_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = UsrLed_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(UsrLed_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PtPin */
	GPIO_InitStruct.Pin = UsrKey_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(UsrKey_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : PBPin PBPin */
	 GPIO_InitStruct.Pin = SS595_Pin|SS165_Pin;
	 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	 HAL_GPIO_Init(SPISS_Port, &GPIO_InitStruct);

	 /*Configure GPIO pins : PA1 PA2 PA3 PA4
	 PA5 PA6 PA7 PA8
	 PA13 PA14 PA15
	GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4
			| GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_13
			| GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PB10 PB12 PB3 PB4
	 PB5 PB6 PB7 PB8
	 PB9
	 GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_3|GPIO_PIN_4
	 |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
	 |GPIO_PIN_9;
	 GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	 GPIO_InitStruct.Pull = GPIO_NOPULL;
	 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	 */
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
