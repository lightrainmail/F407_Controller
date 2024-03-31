//
// Created by light on 2024/1/19.
//

#ifndef G4_NRF_NRF_INTERFACE_H
#define G4_NRF_NRF_INTERFACE_H

#include "main.h"
#include "spi.h"
#include "gpio.h"

#define NRF_SPIHandle   hspi3
#define NRF_SPITimeOut  100
/*
 * SPI2_SCK-----PB13
 * SPI2_MOSI----PB15
 * SPI2_MISO----PB14*/

#define NRF_CE_GPIO_Port    GPIOA
#define NRF_CE_Pin          GPIO_PIN_15

#define NRF_CSN_GPIO_Port   GPIOD
#define NRF_CSN_Pin         GPIO_PIN_1

//此外IRQ引脚你需要通过CubeMX配置成外部中断，下降沿捕获

/* @brief 该函数用于初始化用到的CE和CSN引脚*/
void nrf_interface_gpio_init(void);

/* @brief 下面的四个函数用于对CE和CSN引脚的高低电平进行操作*/
void nrf_interface_ce_set(void);

void nrf_interface_ce_clr(void);

void nrf_interface_csn_set(void);

void nrf_interface_csn_clr(void);


/* @brief 调用SPI交换一个字节的数据
 * @param trandata发送的那一个字节的地址
 *          recedata接受的那一个字节地址*/
void nrf_interface_spi_swap(uint8_t *trandata,uint8_t *recedata);

/* @brief 调用SPI接口，接受多个字节的数据*/
void nrf_interface_spi_receive(uint8_t *datas,uint16_t len);

/* @brief 调用SPI接口，发送多个字节的数据*/
void nrf_interface_spi_transmit(uint8_t *datas,uint16_t len);

/* @brief 以毫秒为单位的延迟*/
void nrf_interface_delayms(uint32_t ms);

void nrf_interface_delayus(uint32_t us);

#endif //G4_NRF_NRF_INTERFACE_H
