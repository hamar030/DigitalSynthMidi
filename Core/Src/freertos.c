/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "keyboard.h"
#include "controller.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = { .name = "defaultTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };

osThreadId_t KeyboardTaskHandle;
const osThreadAttr_t KeyboardTask_attributes = { .name = "KeyboardTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };

osThreadId_t ControllerTaskHandle;
const osThreadAttr_t ControllerTask_attributes = { .name = "ControllerTask",
		.priority = (osPriority_t) osPriorityNormal, .stack_size = 128 * 4 };

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
extern void StartKeyboardTask(void *argument);
extern void StartControllerTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL,
			&defaultTask_attributes);

	KeyboardTaskHandle  = osThreadNew(StartKeyboardTask, NULL,
	 &KeyboardTask_attributes);

	 //ControllerTaskHandle = osThreadNew(StartControllerTask, NULL,
	// &ControllerTask_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
	/* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */

	uint32_t Tick1 = osKernelGetTickCount();
	static uint8_t pwmset;
	static uint16_t time;
	static uint8_t timeflag;
	static uint8_t timecount;

	for (;;) {
		if ((osKernelGetTickCount() - Tick1 >= 1)) {
			Tick1 = osKernelGetTickCount();

			if (breathsw == 1) {
				/* Breathing Lamp */
				if (timeflag == 0) {
					time++;
					if (time >= 1200)
						timeflag = 1;
				} else {
					time--;
					if (time <= 2)
						timeflag = 0;
				}
				/* Duty Cycle Setting */
				if (time >= 600) {
					pwmset = time / 120;
				} else {
					pwmset = time / 30;
				}
				/* 20ms Pulse Width */
				if (timecount > 1)
					timecount = 0;
				else
					timecount++;

				if (timecount >= pwmset)
					HAL_GPIO_WritePin(UsrLed_Port, UsrLed_Pin, GPIO_PIN_RESET);
				else
					HAL_GPIO_WritePin(UsrLed_Port, UsrLed_Pin, GPIO_PIN_SET);
			}
		}
	}
	/* USER CODE END StartDefaultTask */
}


/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
