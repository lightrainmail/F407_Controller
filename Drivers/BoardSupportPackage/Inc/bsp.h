//
// Created by light on 2024/4/1.
//

#ifndef F407_CONTROLLER_BSP_H
#define F407_CONTROLLER_BSP_H

#include "main.h"
#include "gpio.h"
#include "stdio.h"


/* @brief 用于ADC滤波的滤波器*/
int BSP_Filter(int adcValue,float k,uint8_t num);


/* @brief 用于微调按键引脚初始化*/
void BSP_FineTuningKey_GPIO_Init(void);

#endif //F407_CONTROLLER_BSP_H
