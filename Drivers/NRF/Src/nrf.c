//
// Created by light on 2024/1/19.
//
#include "nrf.h"

AirDataRate airDataRate = _2Mbps;
Channel channel_x = Channel_0;
RF_OutputPower rfOutputPower = P7dBm;

/* @brief 用于读取指定寄存器中的数据
 * @param reg */
uint8_t NRF_ReadReg(uint8_t reg,uint8_t *datas,uint16_t len) {
    NRF_CSN_Clr();
    uint8_t status = 0;
    nrf_interface_spi_swap(&reg,&status);
    nrf_interface_spi_receive(datas,len);
    NRF_CSN_Set();

    return status;
}

/* @brief 两个延迟函数*/
void NRF_Delayms(uint32_t ms) {
    nrf_interface_delayms(ms);
}

void NRF_Delayus(uint32_t us) {
    nrf_interface_delayus(us);
}

/* @brief 用于向指定寄存器写入数据*/
uint8_t NRF_WriteReg(uint8_t reg,uint8_t *datas,uint16_t len) {
    uint8_t writereg = reg + 0x20;
    NRF_CSN_Clr();
    uint8_t status = 0;
    nrf_interface_spi_swap(&writereg,&status);
    nrf_interface_spi_transmit(datas,len);
    NRF_CSN_Set();

    return status;
}

/* @brief 用于对该模块使用到的引脚进行初始化*/
void NRF_GPIO_Init(void) {
    nrf_interface_gpio_init();
    //从NRF的状态转换图中得知上电Power on reset需10.3ms
    nrf_interface_delayms(11);
    uint8_t data = 0;
    //每次上电第一次读取的的数字有概率会出错，所以上电后直接随便读一个数据，等正式读取时可以避免出错
    NRF_ReadReg(0x00,&data,1);
}

/* @brief 将NRF配置为发送模式,*/
void NRF_TxMode(void) {
    NRF_Reset();

    //修改CONFIG寄存器，设定为发送模式
    uint8_t data = 0;
    NRF_ReadReg(NRF_CONFIG,&data,1);
    data &= 0xFE;
    NRF_WriteReg(NRF_CONFIG,&data,1);



    //设定通道0发送的字节数
    NRF_AirDataWidth(NRF_AirDataBytes, NRF_Channel);

    //设定发送频率
    NRF_SetFrequency(NRF_Frequency);

    //设定发送速率
    NRF_AirDataRate(NRF_AirDataSpeed);
    //设定发射功率
    NRF_RFOutputPower(NRF_RF_OutputPower);

    //设定CRC校验位为1字节 (默认是开启CRC校验的，且当开启自动应答后，强制开启CRC校验)
    NRF_CRC_EncodingScheme(NRF_CRCO);

    //设定地址宽度为5字节
    NRF_AddressFieldWidth(NRF_AddressFiledWidth);

    //设定自动重发次数为3次,重发延迟为250us
    NRF_AutomaticRetransmission(NRF_AutoRetransmitDelay,NRF_AutoRetransmitCount);

    //上电
    data |= 0x02;
    NRF_WriteReg(NRF_CONFIG,&data,1);

    NRF_CE_Set();
}

/* @brief 将NRF配置成接收模式
 * @param bytes 接收的字节数
 * */
void NRF_RxMode(void) {
    NRF_Reset();
    //修改CONFIG寄存器，设定为发送模式
    uint8_t data = 0;
    NRF_ReadReg(NRF_CONFIG,&data,1);
    data &= 0xFE;
    data |= 0x01;
    NRF_WriteReg(NRF_CONFIG,&data,1);

    //上电
    data |= 0x02;
    NRF_WriteReg(NRF_CONFIG,&data,1);

    //设定通道0发送的字节数
    NRF_AirDataWidth(NRF_AirDataBytes,NRF_Channel);

    //设定发送频率
    NRF_SetFrequency(NRF_Frequency);

    //设定发送速率
    NRF_AirDataRate(NRF_AirDataSpeed);
    //设定发射功率
    NRF_RFOutputPower(NRF_RF_OutputPower);

    //设定CRC校验位为1字节 (默认是开启CRC校验的，且当开启自动应答后，强制开启CRC校验)
    NRF_CRC_EncodingScheme(NRF_CRCO);

     //设定地址宽度为5字节
    NRF_AddressFieldWidth(NRF_AddressFiledWidth);

    //设定自动重发次数为3次,重发延迟为250us
    NRF_AutomaticRetransmission(NRF_AutoRetransmitDelay,NRF_AutoRetransmitCount);

    NRF_CE_Set();
}

/* @brief NRF中RF_SETUP寄存器设置，设置Air Data Rate 和 */

/* @brief 向TxFIFO中写入要发送的数据*/
uint8_t NRF_WriteTxFIFO(uint8_t *datas,uint8_t len) {
    uint8_t command = NRF_W_RX_PAYLOAD;
    uint8_t status = 0;
    NRF_CSN_Clr();
    nrf_interface_spi_swap(&command,&status);
    nrf_interface_spi_transmit(datas,len);
    NRF_CSN_Set();

    return status;
}

/* @brief 从FIFO中读取数据*/
uint8_t NRF_ReadRxFIFO(uint8_t *datas,uint8_t len) {
    uint8_t status = 0;
    uint8_t command = NRF_R_RX_PAYLOAD;

    NRF_CSN_Clr();
    nrf_interface_spi_swap(&command,&status);
    nrf_interface_spi_receive(datas,len);
    NRF_CSN_Set();

    return status;
}

/* 接收模式下的接收数据函数,这个函数应该放在接收中断中*/
void NRF_ReceiveData(uint8_t *data,uint8_t len) {
    NRF_ReadRxFIFO(data,len);

    //清除RX FIFO寄存器
    uint8_t temp = 0xff;
    NRF_WriteReg(NRF_FLUSH_RX,&temp,1);

    //接收完数据后手动的清除接收中断
    NRF_ClearRxInterrupt();
}

/* @brief 清除TxFIFO 在发射模式下使用*/
uint8_t NRF_ClearTxFIFO(void) {
    uint8_t command = NRF_FLUSH_TX;
    uint8_t status = 0;
    NRF_CSN_Clr();
    nrf_interface_spi_swap(&command,&status);
    NRF_CSN_Set();

    return status;
}

/* @brief 清除RxFIFO*/
uint8_t NRF_ClearRxFIFO(void) {
    uint8_t command = NRF_FLUSH_RX;
    uint8_t status = 0;
    NRF_CSN_Clr();
    nrf_interface_spi_swap(&command,&status);
    NRF_CSN_Set();

    return status;
}



/* @brief 清除中断标志位*/
//清除接收数据中断
void NRF_ClearRxInterrupt(void) {
    uint8_t status = 0;
    status = NRF_GetSTATUS();
    status |= 0x40;
    NRF_WriteReg(NRF_STATUS,&status,1);
}

//清除数据发送完成中断
void NRF_ClearTxDSInterrupt(void) {
    uint8_t status = 0;
    status = NRF_GetSTATUS();
    status |= 0x20;
    NRF_WriteReg(NRF_STATUS,&status,1);
}

//清除达到最多次重发中断
void NRF_ClearMaxRTInterrupt(void) {
    uint8_t status = 0;
    status = NRF_GetSTATUS();
    status |= 0x10;
    NRF_WriteReg(NRF_STATUS,&status,1);
}

void NRF_TxIRQ(void) {
    uint8_t status = 0;
    status = NRF_GetSTATUS();

    status &= 0x20; //0010 0000 Data send TX FIFO interrupt

    if(status) {
        NRF_ClearTxDSInterrupt();
    }
    else {
//        //清除TxFIFO寄存器
//        uint8_t data = 0xff;
//        NRF_WriteReg(NRF_FLUSH_TX,&data,1);
        NRF_ClearMaxRTInterrupt();
    }
}



/* 重新设置NRF中的寄存器*/
void NRF_Reset(void) {
    NRF_CSN_Set();
    NRF_CE_Clr();

    uint8_t data = 0x08;
    NRF_WriteReg(NRF_CONFIG,&data,1);

    data = 0x3F;
    NRF_WriteReg(NRF_EN_AA,&data,1);

    data = 0x03;
    NRF_WriteReg(NRF_EN_RXADDR,&data,1);

    data = 0x03;
    NRF_WriteReg(NRF_SETUP_AW,&data,1);

    data = 0x03;
    NRF_WriteReg(NRF_SETUP_RETR,&data,1);

    data = 0x02;
    NRF_WriteReg(NRF_RF_CH,&data,1);

    data = 0x0E; //0x27
    NRF_WriteReg(NRF_RF_SETUP,&data,1);

    data = 0x7E;    //0x7E
    NRF_WriteReg(NRF_STATUS,&data,1);

    data = 0x00;
    NRF_WriteReg(NRF_RX_PW_P0,&data,1);
    NRF_WriteReg(NRF_RX_PW_P1,&data,1);
    NRF_WriteReg(NRF_RX_PW_P2,&data,1);
    NRF_WriteReg(NRF_RX_PW_P3,&data,1);
    NRF_WriteReg(NRF_RX_PW_P4,&data,1);
    NRF_WriteReg(NRF_RX_PW_P5,&data,1);

    data = 0x11;
    NRF_WriteReg(NRF_FIFO_STATUS,&data,1);

    data = 0x00;
    NRF_WriteReg(NRF_DYNPD,&data,1);
    NRF_WriteReg(NRF_FEATURE,&data,1);

    //清空FIFO
    NRF_ClearTxFIFO();
    NRF_ClearRxFIFO();
}

/* @brief 获取状态寄存器*/
uint8_t NRF_GetSTATUS(void) {
    uint8_t command = NRF_NOP;
    uint8_t status = 0;

    NRF_CSN_Clr();
    nrf_interface_spi_swap(&command,&status);
    NRF_CSN_Set();

    return status;
}

/* @brief 设定指定通道的数据宽度
 * @param bytes 数据宽度，以字节为单位
 *        channel 具体到哪一个接收通道,可选通道：0，1，2，3，4，5*/
void NRF_AirDataWidth(uint8_t bytes, Channel channel) {
    switch (channel) {
        case Channel_0:
            NRF_WriteReg(NRF_RX_PW_P0,&bytes,1);
            break;
        case Channel_1:
            NRF_WriteReg(NRF_RX_PW_P1,&bytes,1);
            break;
        case Channel_2:
            NRF_WriteReg(NRF_RX_PW_P2,&bytes,1);
            break;
        case Channel_3:
            NRF_WriteReg(NRF_RX_PW_P3,&bytes,1);
            break;
        case Channel_4:
            NRF_WriteReg(NRF_RX_PW_P4,&bytes,1);
            break;
        case Channel_5:
            NRF_WriteReg(NRF_RX_PW_P5,&bytes,1);
            break;
        default:
            break;
    }
}

/* @brief 下面这个函数设定Air Data Rate*/
void NRF_AirDataRate(AirDataRate air_data_rate) {
    uint8_t data = 0;
    NRF_ReadReg(NRF_RF_SETUP,&data,1);
    data &= 0xD7;   //1111 0111 配合寄存器手册看
    if(air_data_rate == _1Mbps) {
        data |= 0x00;
        NRF_WriteReg(NRF_RF_SETUP,&data,1);
    }
    if(air_data_rate == _2Mbps) {
        data |= 0x08;
        NRF_WriteReg(NRF_RF_SETUP,&data,1);
    }
    if(air_data_rate == _250Kbps) {
        data |= 20;
        NRF_WriteReg(NRF_RF_SETUP,&data,1);
    }
}

/* @brief 设定发送功率*/
void NRF_RFOutputPower(RF_OutputPower outputPower) {
    uint8_t data = 0;
    NRF_ReadReg(NRF_RF_SETUP,&data,1);
    data &= 0xF8;
    switch (outputPower) {
        case N12dBm:
            data |= 0x00;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case N6dBm:
            data |= 0x01;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case N4dBm:
            data |= 0x02;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case P0dBm:
            data |= 0x03;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case P1dBm:
            data |= 0x04;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case P3dBm:
            data |= 0x05;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case P4dBm:
            data |= 0x06;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        case P7dBm:
            data |= 0x07;
            NRF_WriteReg(NRF_RF_SETUP,&data,1);
            break;
        default:
            break;
    }
}

/* @brief 以MHz为单位设定NRF工作频率,NRF可以工作的频率在2400MHz到2525MHz之间*/
void NRF_SetFrequency(uint16_t frequency) {
    uint8_t data = (uint8_t)(frequency - 2400);
    NRF_WriteReg(NRF_RF_CH,&data,1);
}

/* @brief 选择CRC方案
 * @param 可以选择1个字节或2个字节*/
void NRF_CRC_EncodingScheme(uint8_t bytes) {
    uint8_t data = 0;
    NRF_ReadReg(NRF_CONFIG,&data,1);

    data &= 0xFB;

    if(bytes == 1) {
        data |= 0x00;
        NRF_WriteReg(NRF_CONFIG,&data,1);
    }
    else {
        data |= 0x04;
        NRF_WriteReg(NRF_CONFIG,&data,1);
    }
}

/* @brief 设定发送/接收地址宽度
 * @param 指定的以字节为单位的宽度，可以是3字节、4字节、5字节*/
void NRF_AddressFieldWidth(uint8_t width) {
    uint8_t data = 0;
    NRF_ReadReg(NRF_SETUP_AW,&data,1);
    data &= 0xFC;
    switch (width) {
        case 3:
            data |= 0x01;
            break;
        case 4:
            data |= 0x02;
            break;
        case 5:
            data |= 0x03;
            break;
        default:
            break;
    }
    NRF_WriteReg(NRF_SETUP_AW,&data,1);
}

/* @brief 重发设定
 * @param   AutoRetransmitDelay自动重发延迟,延迟时间只能是250us、500us、750us...4000us
 *          AutoRetransmitCount自动重发次数,次数只能是0、1、2、3...15*/
void NRF_AutomaticRetransmission(uint16_t AutoRetransmitDelay,uint8_t AutoRetransmitCount) {
    uint8_t data = 0;
    uint8_t delay = 0;
    delay = (AutoRetransmitDelay - 250)/250;
    delay = delay << 4;
    data = delay | AutoRetransmitCount;
    NRF_WriteReg(NRF_SETUP_RETR,&data,1);
}
