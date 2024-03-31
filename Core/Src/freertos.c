/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern uint32_t dmaBuffer[20];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Creat for LCD*/
osThreadId_t lcdHandle;
uint32_t lcdBuffer[128];
osStaticThreadDef_t lcdControlBlock;
const osThreadAttr_t lcd_attributes = {
        .name = "lcd",
        .cb_mem = &lcdControlBlock,
        .cb_size = sizeof(lcdControlBlock),
        .stack_mem = lcdBuffer,
        .stack_size = sizeof(lcdBuffer),
        .priority = (osPriority_t) osPriorityLow,
};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for main */
osThreadId_t mainHandle;
uint32_t mainBuffer[128];
osStaticThreadDef_t mainControlBlock;
const osThreadAttr_t main_attributes = {
        .name = "main",
        .cb_mem = &mainControlBlock,
        .cb_size = sizeof(mainControlBlock),
        .stack_mem = &mainBuffer[0],
        .stack_size = sizeof(mainBuffer),
        .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void App_lcd(void *argument);

/* USER CODE END FunctionPrototypes */

void App_main(void *argument);

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
    /* creation of main */
    mainHandle = osThreadNew(App_main, NULL, &main_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    lcdHandle = osThreadNew(App_lcd, NULL, &lcd_attributes);
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_App_main */
/**
  * @brief  Function implementing the main thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_App_main */
void App_main(void *argument) {
    /* USER CODE BEGIN App_main */
    /* Infinite loop */
    for (;;) {

        vTaskDelay(100);
    }
    /* USER CODE END App_main */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
uint16_t adc[4];
void App_lcd(void *argumnet) {
    taskENTER_CRITICAL();
    LCD_ShowString(30, 0, "LCD display fine", BLACK, WHITE, 24, 1);
    taskEXIT_CRITICAL();
    while (1) {
        LCD_ShowIntNum(30,50,adc[0],4,BLACK,WHITE,24);
        LCD_ShowIntNum(30,50 + 30,adc[1],4,BLACK,WHITE,24);
        LCD_ShowIntNum(30,50 + 60,adc[2],4,BLACK,WHITE,24);
        LCD_ShowIntNum(30,50 + 90,adc[3],4,BLACK,WHITE,24);
        vTaskDelay(1);
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    uint16_t adcSum[2];
    for(uint8_t i = 0;i < 10;i++) {
        adcSum[0] = dmaBuffer[i*2] + adcSum[0];
        adcSum[1] = dmaBuffer[i*2 + 1] + adcSum[1];
    }
    adcSum[0] = adcSum[0] / 10;
    adcSum[1] = adcSum[1] / 10;

    

}
/* USER CODE END Application */

