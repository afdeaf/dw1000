#include "deca_device_api.h"
#include "deca_regs.h"
#include "stm32g031xx.h"

static dwt_config_t config = {
    2,               /* 信道编号 */
    DWT_PRF_64M,     /* 脉冲重复频率 */
    DWT_PLEN_1024,   /* 前导码长度（仅发送） */
    DWT_PAC32,       /* 前导码采集块大小（仅接收） */
    9,               /* 发送前导码代码 */
    9,               /* 接收前导码代码 */
    1,               /* 使用非标准SFD */
    DWT_BR_110K,     /* 数据速率 */
    DWT_PHRMODE_STD, /* PHY头模式 */
    (1025 + 64 - 32) /* SFD超时时间 */
};

#define FRAME_LEN_MAX 128
static uint8_t rx_buffer[FRAME_LEN_MAX];
static uint32_t status_reg;
static uint32_t frame_len;