#ifndef DW1000_H_
#define DW1000_H_

#include "main.h"

//DW1000中断号
#define DW1000_EXTI_IRQn EXTI4_15_IRQn

//SPI定义
extern SPI_HandleTypeDef hspi2;
#define DW1000_SPI_Handle hspi2

// 改变SPI速率
void set_spi_rate_low(void);
void set_spi_rate_high(void);

// DW1000复位
void reset_DW1000(void);

// 发送信息
void send_msg(uint8_t *msg, uint8_t len);

#endif // !DW1000_H_
