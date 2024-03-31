//
// Created by light on 2024/4/1.
//
#include "bsp.h"

/* @brief 用于ADC滤波的滤波器*/
uint16_t adcStaticValue[4] = {0,0,0,0};
uint16_t BSP_Filter(uint16_t adcValue,float k,uint8_t num) {
    uint16_t retValue = 0;
    retValue = k*adcValue + (1 - k)*adcStaticValue[num];
    adcStaticValue[num] = adcValue;
    return retValue;
}