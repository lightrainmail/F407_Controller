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
#include "flash.h"
#include "pic.h"
#include "stdio.h"
#include "math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */
osSemaphoreId_t BinSem_DataReadyHandle;
const osSemaphoreAttr_t BinSem_DataReady_attributes = {
        .name = "BinSem_DataReady",
};

osSemaphoreId_t NRFSem_TxHandle;
const osSemaphoreAttr_t NRFSem_Tx_attributes = {
        .name = "NRFSem_Tx"
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern uint32_t dmaBuffer[2];
CCMRAM uint16_t adc[4] = {0, 0, 0, 0};
CCMRAM int offset[4] = {0,0,0,0};
CCMRAM uint8_t txBuffer[NRF_AirDataBytes];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* Creat for LCD*/
osThreadId_t lcdHandle;
CCMRAM uint32_t lcdBuffer[128];
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
CCMRAM uint32_t adcBuffer[128];
osStaticThreadDef_t adcControlBlock;
const osThreadAttr_t adc_attributes = {
        .name = "adc",
        .cb_mem = &adcControlBlock,
        .cb_size = sizeof(adcControlBlock),
        .stack_mem = adcBuffer,
        .stack_size = sizeof(adcBuffer),
        .priority = (osPriority_t) osPriorityHigh,
};

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for main */
osThreadId_t mainHandle;
CCMRAM uint32_t mainBuffer[ 128 ];
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

    NRFSem_TxHandle = osSemaphoreNew(1,1,&NRFSem_Tx_attributes);
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
void App_main(void *argument)
{
  /* USER CODE BEGIN App_main */
    taskENTER_CRITICAL();
    FLASH_Read(ADDR_FLASH_SECTOR_10,(uint32_t *)offset,4);
    taskEXIT_CRITICAL();
    /* Infinite loop */
    for (;;) {
        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9) == GPIO_PIN_RESET) {
            taskENTER_CRITICAL();
            FLASH_Write(ADDR_FLASH_SECTOR_10,(uint32_t*)offset,4);
            taskEXIT_CRITICAL();
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_10) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET) {
            offset[0] += 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == GPIO_PIN_RESET) {
            offset[0] -= 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2) == GPIO_PIN_RESET);
            }
        }


        if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_9) == GPIO_PIN_RESET) {
            offset[1] += 10;
            if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_9) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_9) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_8) == GPIO_PIN_RESET) {
            offset[1] -= 10;
            if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_8) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_8) == GPIO_PIN_RESET);
            }
        }


        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == GPIO_PIN_RESET) {
            offset[2] -= 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) == GPIO_PIN_RESET) {
            offset[2] += 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_5) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6) == GPIO_PIN_RESET) {
            offset[3] += 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_6) == GPIO_PIN_RESET);
            }
        }

        if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) == GPIO_PIN_RESET) {
            offset[3] -= 10;
            if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) == GPIO_PIN_RESET) {
                while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_7) == GPIO_PIN_RESET);
            }
        }

        vTaskDelay(1);
    }
  /* USER CODE END App_main */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
CCMRAM uint8_t DisplayBuffer[7372];
CCMRAM uint16_t DisPlayBuffer2[10000];
void App_lcd(void *argument) {
    //显示背景
    taskENTER_CRITICAL();
    LCD_ShowPicture2(30,0,180,48,gImage_Title1);
    LCD_ShowPicture2(5,48,60,16,gImage_Title2);
    LCD_ShowPicture2(19,66,27,190,gImage_KeDu);
    LCD_ShowPicture2(69,79,160,160,gImage_WeiTiaoBG);
    taskEXIT_CRITICAL();

    for(uint16_t i = 0;i < 7372;i++) {
        DisplayBuffer[i] = gImage_Buffer[i];
    }

    for(uint16_t i = 0;i < 10000;i++) {
        DisPlayBuffer2[i] = (gImage_JuXing[2*i] << 8) | (gImage_JuXing[2*i+1]);
    }

    uint16_t y = 0;
    uint16_t num = 0;
    uint16_t num_old = 0;

    while (1) {
        y = -180*adc[0]/4095 + 180;
        num = 38*y;

        //擦除旧箭头
        for(uint16_t i = 0; i < 532;i++) {
            DisplayBuffer[num_old + i] = 0xFF;
        }

        //画新箭头
        for(uint16_t i = 0;i < 532;i++) {
            DisplayBuffer[num + i] = gImage_JianTou[i];
        }

        taskENTER_CRITICAL();
        LCD_ShowPicture2(45,65,19,194,DisplayBuffer);
        LCD_ShowIntNumber(160,89,offset[2] + 1,3,BLACK,WHITE,24);
        LCD_ShowIntNumber(160,129,offset[3] + 1,3,BLACK,WHITE,24);
        LCD_ShowIntNumber(160,167,offset[0] + 1,3,BLACK,WHITE,24);
        LCD_ShowIntNumber(160,204,offset[1] + 1,3,BLACK,WHITE,24);
        taskEXIT_CRITICAL();

        num_old = num;
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
            int adc_temp[4] = {0,0,0,0};
            uint16_t adc_tx[4] = {0,0,0,0};

            for(uint8_t i = 0;i < 4;i++) {
                adc_temp[i] = adc[i];

                adc_temp[i] = BSP_Filter(adc_temp[i],0.35f,i);

                adc_temp[i] += offset[i];

                if(adc_temp[i] >= 4095) {
                    adc_temp[i] = 4095;
                }
                if(adc_temp[i] <= 0) {
                    adc_temp[i] = 0;
                }

                adc_tx[i] = adc_temp[i];

                txBuffer[2*i] = adc_tx[i];
                txBuffer[2*i + 1] = (adc_tx[i] >> 8);
            }

            if(xSemaphoreTake(NRFSem_TxHandle,portMAX_DELAY) == pdTRUE) {
                taskENTER_CRITICAL();
                NRF_WriteTxFIFO(txBuffer,NRF_AirDataBytes);
                taskEXIT_CRITICAL();
            }
//            vTaskDelay(1);
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
        BaseType_t highTaskWoken = pdFALSE;
        if(NRFSem_TxHandle !=NULL) {
            xSemaphoreGiveFromISR(NRFSem_TxHandle,&highTaskWoken);
            portYIELD_FROM_ISR(highTaskWoken);
        }
    }
}

/* USER CODE END Application */

