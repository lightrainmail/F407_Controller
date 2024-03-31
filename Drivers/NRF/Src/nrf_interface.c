//
// Created by light on 2024/1/19.
//
#include "nrf_interface.h"


/* @brief 该函数用于初始化用到的CE和CSN引脚*/
void nrf_interface_gpio_init(void) {
    GPIO_InitTypeDef GPIO_Init = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    HAL_GPIO_WritePin(NRF_CE_GPIO_Port,NRF_CE_Pin,GPIO_PIN_SET);
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port,NRF_CSN_Pin,GPIO_PIN_SET);

    GPIO_Init.Pin = NRF_CE_Pin;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NRF_CE_GPIO_Port,&GPIO_Init);

    GPIO_Init.Pin = NRF_CSN_Pin;
    GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Init.Pull = GPIO_NOPULL;
    GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(NRF_CSN_GPIO_Port,&GPIO_Init);
}

/* @brief 下面的四个函数用于对CE和CSN引脚的高低电平进行操作*/
void nrf_interface_ce_set(void) {
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port,NRF_CE_Pin,GPIO_PIN_SET);
}

void nrf_interface_ce_clr(void) {
    HAL_GPIO_WritePin(NRF_CE_GPIO_Port,NRF_CE_Pin,GPIO_PIN_RESET);
}

void nrf_interface_csn_set(void) {
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port,NRF_CSN_Pin,GPIO_PIN_SET);
}

void nrf_interface_csn_clr(void) {
    HAL_GPIO_WritePin(NRF_CSN_GPIO_Port,NRF_CSN_Pin,GPIO_PIN_RESET);
}


/* @brief 调用SPI交换一个字节的数据
 * @param trandata发送的那一个字节的地址
 *          recedata接受的那一个字节地址*/
void nrf_interface_spi_swap(uint8_t *trandata,uint8_t *recedata) {
    HAL_SPI_TransmitReceive(&NRF_SPIHandle,trandata,recedata,1,NRF_SPITimeOut);
}

/* @brief 调用SPI接口，接受多个字节的数据*/
void nrf_interface_spi_receive(uint8_t *datas,uint16_t len) {
    HAL_SPI_Receive(&NRF_SPIHandle,datas,len,NRF_SPITimeOut);
}

/* @brief 调用SPI接口，发送多个字节的数据*/
void nrf_interface_spi_transmit(uint8_t *datas,uint16_t len) {
    HAL_SPI_Transmit(&NRF_SPIHandle,datas,len,NRF_SPITimeOut);
}

/* @brief 以毫秒为单位的延迟*/
void nrf_interface_delayms(uint32_t ms) {
    HAL_Delay(ms);
}

/* @brief us级延迟*/
#define CPU_FREQUENCY_MHZ    72
void nrf_interface_delayus(uint32_t us) {
    int last, curr, val;
    int temp;

    while (us != 0)
    {
        temp = us > 900 ? 900 : us;
        last = SysTick->VAL;
        curr = last - CPU_FREQUENCY_MHZ * temp;
        if (curr >= 0)
        {
            do
            {
                val = SysTick->VAL;
            }
            while ((val < last) && (val >= curr));
        }
        else
        {
            curr += CPU_FREQUENCY_MHZ * 1000;
            do
            {
                val = SysTick->VAL;
            }
            while ((val <= last) || (val > curr));
        }
        us -= temp;
    }
}

