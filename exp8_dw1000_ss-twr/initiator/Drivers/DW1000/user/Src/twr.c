#include "twr.h"

#include <string.h>

#define RANGING 1
#define NO_RANGING 0

static uint8_t frame_seq_nb = 0;

app_state_t state;

static uint8_t rx_buffer[FRAME_LEN_MAX];
uint32_t status_reg;
static uint32_t frame_len;

static uint8 tx_poll_msg[] = {0x41, 0x88, 0,   0xCA, 0xDE, 'W',
                              'A',  'V',  'E', 0xE0, 0,    0};
static uint8 rx_resp_msg[] = {0x41, 0x88, 0,    0xCA, 0xDE, 'V', 'E',
                              'W',  'A',  0xE1, 0,    0,    0,   0,
                              0,    0,    0,    0,    0,    0};

static void resp_msg_get_ts(uint8* ts_field, uint32_t* ts) {
  int i;
  *ts = 0;
  for (i = 0; i < RESP_MSG_TS_LEN; i++) {
    *ts += ts_field[i] << (i * 8);
  }
}

void send_poll_message(void) {
  /* 更新帧序列号 */
  tx_poll_msg[ALL_MSG_SN_IDX] = frame_seq_nb > 128 ? 0 : frame_seq_nb;

  /* 清除TX帧发送状态 */
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

  /* 写入TX数据 */
  dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0);
  dwt_writetxfctrl(sizeof(tx_poll_msg), 0, RANGING); /* 启用测距 */

  /* 开始传输，指示期望响应以便在帧发送后自动启用接收 */
  dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);
  frame_seq_nb++;

  /* 更新状态为IDLE */
  state = APP_STATE_WAIT_RESP;
}

float calculate_distance(void) {
  uint32_t poll_tx_ts, resp_rx_ts, poll_rx_ts, resp_tx_ts;
  int32 rtd_init, rtd_resp;
  float clockOffsetRatio;

  /* Poll传输和响应接收时间戳 */
  poll_tx_ts = dwt_readtxtimestamplo32();
  resp_rx_ts = dwt_readrxtimestamplo32();

  /* 读取载波积分器值并计算时钟偏移比率 */
  clockOffsetRatio =
      dwt_readcarrierintegrator() *
      (FREQ_OFFSET_MULTIPLIER * HERTZ_TO_PPM_MULTIPLIER_CHAN_2 / 1.0e6);

  /* 获取响应消息中嵌入的时间戳 */
  resp_msg_get_ts(&rx_buffer[RESP_MSG_POLL_RX_TS_IDX], &poll_rx_ts);
  resp_msg_get_ts(&rx_buffer[RESP_MSG_RESP_TX_TS_IDX], &resp_tx_ts);

  /* 计算飞行时间和距离，使用时钟偏移比率校正本地和远程时钟速率的差异 */
  rtd_init = resp_rx_ts - poll_tx_ts;
  rtd_resp = resp_tx_ts - poll_rx_ts;

  float tof =
      ((rtd_init - rtd_resp * (1 + clockOffsetRatio)) / 2.0) * DWT_TIME_UNITS;
  // tof = ((rtd_init - rtd_resp) / 2.0) * DWT_TIME_UNITS;

  return tof * SPEED_OF_LIGHT;
}

float rxfg_cb(void) {
  dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
  /* 读取接收到的帧数据 */
  frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
  if (frame_len <= FRAME_LEN_MAX) {
    dwt_readrxdata(rx_buffer, frame_len, 0);
    rx_buffer[ALL_MSG_SN_IDX] = 0;
    /* 检查帧是否为预期的响应消息 */
    if (memcmp(rx_buffer, rx_resp_msg, ALL_MSG_COMMON_LEN) == 0) {
      state = APP_STATE_CALC_DISTANCE;
      float dist = calculate_distance();
      state = APP_STATE_IDLE;
      return dist;
    }
  }
  return -1;
}
