#include "dw1000.h"
#include "deca_device_api.h"
#include "deca_regs.h"
#include <stdio.h>


void change_spi_rate(SPI_TypeDef* SPIx, uint16_t scal_fatcor) {
    uint16_t tmpreg = SPIx->CR1;
    tmpreg &= 0xFFC7;
    tmpreg |= scal_fatcor;
    SPIx->CR1 = tmpreg;
}

void set_spi_rate_low(void) {
    change_spi_rate(DW1000_SPI_Handle.Instance, SPI_BAUDRATEPRESCALER_32);
}

void set_spi_rate_high(void) {
    change_spi_rate(DW1000_SPI_Handle.Instance, SPI_BAUDRATEPRESCALER_8);
}

void reset_DW1000(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    // 初始化DW1000复位引脚为推挽输出
    GPIO_InitStruct.Pin   = RST_N_DW1000_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RST_N_DW1000_GPIO_Port, &GPIO_InitStruct);

    // 复位, 拉低RST_N_DW1000_Pin
    HAL_GPIO_WritePin(RST_N_DW1000_GPIO_Port, RST_N_DW1000_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);

    // 把RST_N_DW1000_Pin设置为三态门
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(RST_N_DW1000_GPIO_Port, &GPIO_InitStruct);
}

void send_msg(uint8_t *msg, uint8_t len) {
  dwt_writetxdata(len, msg, 0); /* Zero offset in TX buffer. */
  dwt_writetxfctrl(len, 0, 0);  /* Zero offset in TX buffer, no ranging. */
  /* Start transmission. */
  dwt_starttx(DWT_START_TX_IMMEDIATE);
}
