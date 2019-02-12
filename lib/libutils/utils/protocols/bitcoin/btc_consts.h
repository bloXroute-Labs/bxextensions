
#ifndef UTILS_PROTOCOLS_BITCOIN_BTC_CONSTS_H_
#define UTILS_PROTOCOLS_BITCOIN_BTC_CONSTS_H_

#define BTC_DEFAULT_TX_COUNT 10000
#define BTC_DEFAULT_TX_SIZE 62
#define BTC_DEFAULT_BLOCK_SIZE 112 + BTC_DEFAULT_TX_COUNT * BTC_DEFAULT_TX_SIZE
#define BTC_SHORT_TX_SIZE 5
#define BTC_SHORT_ID_INDICATOR 0xff
#define BTC_HDR_COMMON_OFFSET 24

#define BTC_BLOCK_HDR_SIZE 80
#define BTC_BLOCK_TX_COUNT_OFFSET BTC_HDR_COMMON_OFFSET + BTC_BLOCK_HDR_SIZE
#define BTC_TX_VERSION_SIZE 4
#define BTC_TX_SEGWIT_FLAG_VALUE 1
#define BTC_TX_PREV_OUTPUT_SIZE 36
#define BTC_TXIN_SEQUENCE_SIZE 4
#define BTC_TX_PREV_OUTPUT_SIZE 36
#define BTC_TXOUT_VALUE_SIZE 8
#define BTC_TX_SEGWIT_FLAG_SIZE 2
#define BTC_TX_LOCK_TIME_SIZE 4




#endif /* UTILS_PROTOCOLS_BITCOIN_BTC_CONSTS_H_ */
