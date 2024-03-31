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
#include "bsp.h"
#include "semphr.h"
#include "nrf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
osSemaphoreId_t BinSem_DataReadyHandle;
const osSemaphoreAttr_t BinSem_DataReady_attributes = {
        .name = "BinSem_DataReady",
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
extern uint32_t dmaBuffer[2];
uint16_t adc[4] = {0, 0, 0, 0};

uint8_t txBuffer[16];
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

/* Creat for adc */
osThreadId_t adcHandle;
uint32_t adcBuffer[128];
osStaticThreadDef_t adcControlBlock;
const osThreadAttr_t adc_attributes = {
        .name = "adc",
        .cb_mem = &adcControlBlock,
        .cb_size = sizeof(adcControlBlock),
        .stack_mem = adcBuffer,
        .stack_size = sizeof(adcBuffer),
        .priority = (osPriority_t) osPriorityNormal,
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

void App_adc(void *param);

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
    BinSem_DataReadyHandle = osSemaphoreNew(1, 1, &BinSem_DataReady_attributes);
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

    adcHandle = osThreadNew(App_adc, NULL, &adc_attributes);
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
void App_lcd(void *argument) {
    taskENTER_CRITICAL();
    LCD_ShowString(30, 0, "LCD display fine", BLACK, WHITE, 24, 1);
    taskEXIT_CRITICAL();
    while (1) {
//        adc[0] = BSP_Filter(adc[0], 0.3, 0);
//        adc[1] = BSP_Filter(adc[1], 0.3, 1);
//        adc[2] = BSP_Filter(adc[2], 0.3, 2);
//        adc[3] = BSP_Filter(adc[3], 0.3, 3);
//        taskENTER_CRITICAL();
//        LCD_ShowIntNum(30, 50, adc[0], 4, BLACK, WHITE, 24);
//        LCD_ShowIntNum(30, 50 + 30, adc[1], 4, BLACK, WHITE, 24);
//        LCD_ShowIntNum(30, 50 + 60, adc[2], 4, BLACK, WHITE, 24);
//        LCD_ShowIntNum(30, 50 + 90, adc[3], 4, BLACK, WHITE, 24);
//        taskEXIT_CRITICAL();
        vTaskDelay(1);
    }
}

/* ADC thread*/
void App_adc(void *param) {
    taskENTER_CRITICAL();
    NRF_TxMode();
    taskEXIT_CRITICAL();
    while (1) {
        if (xSemaphoreTake(BinSem_DataReadyHandle, portMAX_DELAY) == pdTRUE) {
            //进行ADC滤波
            adc[0] = BSP_Filter(adc[0], 0.3f, 0);
            adc[1] = BSP_Filter(adc[1], 0.3f, 1);
            adc[2] = BSP_Filter(adc[2], 0.3f, 2);
            adc[3] = BSP_Filter(adc[3], 0.3f, 3);

            //进行数据发送
            txBuffer[0] = (uint8_t) adc[0];
            txBuffer[1] = (uint8_t)(adc[0] >> 8);

            txBuffer[2] = (uint8_t) adc[1];
            txBuffer[3] = (uint8_t)(adc[1] >> 8);

            txBuffer[4] = (uint8_t) adc[2];
            txBuffer[5] = (uint8_t)(adc[2] >> 8);

            txBuffer[6] = (uint8_t) adc[3];
            txBuffer[7] = (uint8_t)(adc[3] >> 8);

            taskENTER_CRITICAL();
            NRF_WriteTxFIFO(txBuffer,16);
            taskEXIT_CRITICAL();
            vTaskDelay(1);
        }
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    adc[0] = (uint16_t) dmaBuffer[0];
    adc[1] = (uint16_t) dmaBuffer[1];
    adc[2] = (uint16_t) (dmaBuffer[0] >> 16);
    adc[3] = (uint16_t) (dmaBuffer[1] >> 16);
    BaseType_t highTaskWoken = pdFALSE;
    if (BinSem_DataReadyHandle != NULL) {
        xSemaphoreGiveFromISR(BinSem_DataReadyHandle, &highTaskWoken);
        portYIELD_FROM_ISR(highTaskWoken);  //主动发起一次任务调度
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_0) {
        NRF_TxIRQ();
    }
}

/* USER CODE END Application */

