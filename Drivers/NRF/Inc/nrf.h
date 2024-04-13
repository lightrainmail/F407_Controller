//
// Created by light on 2024/1/19.
//

#ifndef G4_NRF_NRF_H
#define G4_NRF_NRF_H

#include "nrf_interface.h"

typedef enum {
    _1Mbps = 0,
    _2Mbps = 1,
    _250Kbps = 2
} AirDataRate;

typedef enum {
    Channel_0 = 0,
    Channel_1 = 1,
    Channel_2 = 2,
    Channel_3 = 3,
    Channel_4 = 4,
    Channel_5 = 5
} Channel;

typedef enum {
    N12dBm = 0,
    N6dBm = 1,
    N4dBm = 2,
    P0dBm = 3,
    P1dBm = 4,
    P3dBm = 5,
    P4dBm = 6,
    P7dBm = 7
}RF_OutputPower;

/* 这里是CONFIG设置选项，接收双方需要保持一致*/
//这一项用于设定Air data rate
#define NRF_AirDataSpeed _1Mbps

//下面这个宏用于设定发送接收的字节数
#define NRF_AirDataBytes    9

//下面这个宏用于设定NRF工作的通信频率，以MHz为单位，频率范围在2400MHz到2525MHz之间
#define NRF_Frequency   2500

//这一项用于设定信道，注：这个代码的应用场景是一发一收
#define NRF_Channel Channel_0

//这一项用于设定RF output power in TX mode，设定发射模式下的发射功率
#define NRF_RF_OutputPower  P7dBm

//这一项用于设定CRC校验的模式，可以选择1个或2个字节的长度作为CRC的校验码
#define NRF_CRCO    2

//这一项用于设定地址宽度
#define NRF_AddressFiledWidth   5

//这一项用于设定自动重发延迟时间（单位为us）
#define NRF_AutoRetransmitDelay 500

//这一项用于设定自动重发次数
#define NRF_AutoRetransmitCount 10

#define NRF_CE_Set()    nrf_interface_ce_set()
#define NRF_CE_Clr()    nrf_interface_ce_clr()

#define NRF_CSN_Set()   nrf_interface_csn_set()
#define NRF_CSN_Clr()   nrf_interface_csn_clr()

/* @brief 用于对该模块使用到的引脚进行初始化*/
void NRF_GPIO_Init(void);

/* @brief 用于读取指定寄存器中的数据
 * @param reg */
uint8_t NRF_ReadReg(uint8_t reg,uint8_t *datas,uint16_t len);

/* @brief 用于向指定寄存器写入数据*/
uint8_t NRF_WriteReg(uint8_t reg,uint8_t *datas,uint16_t len);

/* @brief 获取状态寄存器*/
uint8_t NRF_GetSTATUS(void);

/* @brief 发送初始化*/
void NRF_TxMode(void);

/* @brief 将NRF配置成接收模式*/
void NRF_RxMode(void);

/* @brief 向TxFIFO中写入要发送的数据*/
uint8_t NRF_WriteTxFIFO(uint8_t *datas,uint8_t len);

/* @brief 从FIFO中读取数据*/
uint8_t NRF_ReadRxFIFO(uint8_t *datas,uint8_t len);

/* 接收模式下的接收数据函数*/
void NRF_ReceiveData(uint8_t *data,uint8_t len);

/* @brief 两个延迟函数*/
void NRF_Delayms(uint32_t ms);

void NRF_Delayus(uint32_t us);

/* @brief 清除TxFIFO 在发射模式下使用*/
uint8_t NRF_ClearTxFIFO(void);

/* @brief 清除RxFIFO*/
uint8_t NRF_ClearRxFIFO(void);

/* @brief 清除中断标志位*/
//清除接收数据中断
void NRF_ClearRxInterrupt(void);

//清除数据发送完成中断
void NRF_ClearTxDSInterrupt(void);

//清除达到最多次重发中断
void NRF_ClearMaxRTInterrupt(void);

/* 查明是哪一种中断*/
void NRF_TxIRQ(void);

/* @brief 设定指定通道的数据宽度
 * @param bytes 数据宽度，以字节为单位
 *        channel 具体到哪一个接收通道,可选通道：0，1，2，3，4，5*/
void NRF_AirDataWidth(uint8_t bytes, Channel channel);

/* @brief 下面这个函数设定Air Data Rate*/
void NRF_AirDataRate(AirDataRate air_data_rate);

/* @brief 以MHz为单位设定NRF工作频率,NRF可以工作的频率在2400MHz到2525MHz之间*/
void NRF_SetFrequency(uint16_t frequency);

/* @brief 选择CRC方案
 * @param 可以选择1个字节或2个字节*/
void NRF_CRC_EncodingScheme(uint8_t bytes);

/* @brief 设定发送/接收地址宽度
 * @param 指定的以字节为单位的宽度，可以是3字节、4字节、5字节*/
void NRF_AddressFieldWidth(uint8_t width);

/* 重新设置NRF中的寄存器*/
void NRF_Reset(void);

/* @brief 设定发送功率*/
void NRF_RFOutputPower(RF_OutputPower outputPower);

/* @brief 重发设定
 * @param   AutoRetransmitDelay自动重发延迟,延迟时间只能是250us、500us、750us...4000us
 *          AutoRetransmitCount自动重发次数,次数只能是0、1、2、3...15*/
void NRF_AutomaticRetransmission(uint16_t AutoRetransmitDelay,uint8_t AutoRetransmitCount);

#define NRF_CONFIG      0X00
#define NRF_EN_AA       0X01
#define NRF_EN_RXADDR   0X02
#define NRF_SETUP_AW    0X03
#define NRF_SETUP_RETR  0X04
#define NRF_RF_CH       0X05
#define NRF_RF_SETUP    0X06
#define NRF_STATUS      0X07
#define NRF_OBSERVE_TX  0X08
#define NRF_CD          0X09
#define NRF_RX_ADDR_P0  0X0A
#define NRF_RX_ADDR_P1  0X0B
#define NRF_RX_ADDR_P2  0X0C
#define NRF_RX_ADDR_P3  0X0D
#define NRF_RX_ADDR_P4  0X0E
#define NRF_RX_ADDR_P5  0X0F
#define NRF_TX_ADDR     0X10
#define NRF_RX_PW_P0    0X11
#define NRF_RX_PW_P1    0X12
#define NRF_RX_PW_P2    0X13
#define NRF_RX_PW_P3    0X14
#define NRF_RX_PW_P4    0X15
#define NRF_RX_PW_P5    0X16
#define NRF_FIFO_STATUS 0X17
#define NRF_DYNPD       0X1C
#define NRF_FEATURE     0X1D

#define NRF_R_RX_PAYLOAD    0X61
#define NRF_W_RX_PAYLOAD    0XA0
#define NRF_FLUSH_TX        0XE1
#define NRF_FLUSH_RX        0XE2
#define NRF_REUSE_TX_PL     0XE3
#define NRF_ACTIVATE        0X50
#define NRF_R_RX_PL_WID     0X60
#define NRF_NOP         0XFF

#endif //G4_NRF_NRF_H
