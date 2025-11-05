#include "twr.h"

#include <string.h>

#define RANGING 1
#define NO_RANGING 0

#define POLL_RX_TO_RESP_TX_DLY_UUS 3300

/* 测距过程中使用的帧结构 */
static uint8_t rx_poll_msg[] = {0x41, 0x88, 0,   0xCA, 0xDE, 'W',
                                'A',  'V',  'E', 0xE0, 0,    0};
static uint8_t tx_resp_msg[] = {0x41, 0x88, 0,    0xCA, 0xDE, 'V', 'E',
                                'W',  'A',  0xE1, 0,    0,    0,   0,
                                0,    0,    0,    0,    0,    0};

static uint8_t frame_seq_nb = 0;

static uint8_t rx_buffer[FRAME_LEN_MAX];
uint32_t status_reg;
static uint32_t frame_len;

/* 帧传输/接收的时间戳 */
static uint64_t poll_rx_ts = 0;
static uint64_t resp_tx_ts;

/**
 * @brief  获取64位RX时间戳
 * @retval 64位时间戳值
 */
static uint64_t get_rx_timestamp_u64(void) {
  uint8_t rx_timestamp[5];

  // 从DW1000获取接收的时间戳
  dwt_readrxtimestamp(rx_timestamp);

  uint64_t result = 0;

  // Combine the 5 bytes into a 64-bit integer
  for (int i = 0; i < 5; i++) {
    result |= ((uint64_t)rx_timestamp[i] << (i * 8));
  }

  return result;
}

/**
 * @brief  在响应消息中设置发送时间戳
 * @retval None
 */
static void resp_msg_set_ts(uint8_t* ts_field, const uint64_t ts) {
  int i;
  for (i = 0; i < RESP_MSG_TS_LEN; i++) {
    ts_field[i] = (ts >> (i * 8)) & 0xFF;
  }
}

void rxfg_cb(void) {
  /* 清除接收成功状态标志位 */
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

  /* 读取接收到的帧长度 */
  frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;

  /* 安全读取帧数据到缓冲区 */
  if (frame_len <= RX_BUFFER_LEN) {
    dwt_readrxdata(rx_buffer, frame_len, 0);
  }

  /* 重置序列号字段用于比较 */
  rx_buffer[ALL_MSG_SN_IDX] = 0;

  /* 验证接收的帧是否为POLL消息 */
  if (memcmp(rx_buffer, rx_poll_msg, ALL_MSG_COMMON_LEN) == 0) {
    uint32 resp_tx_time;
    int ret;

    /* 获取POLL消息的精确接收时间戳 */
    poll_rx_ts = get_rx_timestamp_u64();

    /* 计算响应消息的发送时间：当前时间 + 固定延迟 */
    resp_tx_time =
        (poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;

    /* 设置DW1000的延迟发送时间 */
    dwt_setdelayedtrxtime(resp_tx_time);

    /* 计算响应消息的实际发送时间戳（包含天线延迟补偿） */
    resp_tx_ts = (((uint64_t)(resp_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;

    /* 在RESP消息中嵌入时间戳信息 */
    resp_msg_set_ts(&tx_resp_msg[RESP_MSG_POLL_RX_TS_IDX],
                    poll_rx_ts);  // POLL接收时间戳
    resp_msg_set_ts(&tx_resp_msg[RESP_MSG_RESP_TX_TS_IDX],
                    resp_tx_ts);  // RESP发送时间戳

    /* 设置消息序列号并准备发送 */
    tx_resp_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
    dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0);
    dwt_writetxfctrl(sizeof(tx_resp_msg), 0, RANGING);

    /* 启动延迟发送 */
    ret = dwt_starttx(DWT_START_TX_DELAYED);

    if (ret == DWT_SUCCESS) {
      /* 发送成功，递增序列号 */
      frame_seq_nb++;
    } else {
      /* 发送失败，清除错误状态并重置接收 */
      dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
      dwt_rxreset();
    }
  }
}
