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

/* 默认天线延迟值，用于64MHz PRF */
#define TX_ANT_DLY 16436
#define RX_ANT_DLY 16436

#define FRAME_LEN_MAX 128
/* 消息公共部分的长度 */
#define ALL_MSG_COMMON_LEN 10
/* 访问帧中字段的索引 */
#define ALL_MSG_SN_IDX 2
#define RESP_MSG_POLL_RX_TS_IDX 10
#define RESP_MSG_RESP_TX_TS_IDX 14
#define RESP_MSG_TS_LEN 4

/* UWB微秒到设备时间单位的转换因子 */
#define UUS_TO_DWT_TIME 65536

/* 帧间延迟，单位UWB微秒 */
#define POLL_TX_TO_RESP_RX_DLY_UUS 140
/* 接收响应超时时间 */
#define RESP_RX_TIMEOUT_UUS 5000

/* 光速，单位米/秒 */
#define SPEED_OF_LIGHT 299702547


/* 应用状态机 */
typedef enum {
  APP_STATE_IDLE,         /* 空闲状态 */
  APP_STATE_TX_POLL,      /* 发送Poll消息 */
  APP_STATE_WAIT_RESP,    /* 等待响应消息 */
  APP_STATE_CALC_DISTANCE /* 计算距离 */
} app_state_t;


void send_poll_message(void);
float calculate_distance(void);

float rxfgg_cb(void);