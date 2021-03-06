#ifndef UTILS_PROTOCOLS_ETHEREUM_ETH_CONSTS_H
#define UTILS_PROTOCOLS_ETHEREUM_ETH_CONSTS_H

#define ETH_DEFAULT_TX_COUNT 200
#define ETH_DEFAULT_TX_SIZE 1000
#define ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT 100
#define ETH_DEFAULT_BLOCK_SIZE 100000

#define ETH_FULL_TX_INDICATOR 0x01
#define ETH_SHORT_ID_INDICATOR 0x80
#define ETH_RLP_OF_SHORT_TX 0xc2
#define ETH_BLOCK_LENGTH_OFFSET 16

#define EIP155_CHAIN_ID_OFFSET 35
#define V_RANGE_START 0x00
#define V_RANGE_END 0x01
#define SIGNATURE_LEN 65
#define MAX_R_SIZE 32
#define MAX_T_SIZE 32
#define ETH_TX_TYPE_0 0x00
#define ETH_TX_TYPE_1 0x01
#define START_RANGE_ETH_LEGACY_TX 0xc0
#define END_RANGE_ETH_LEGACY_TX 0xfe

#define RLP_BYTE 1
#define RLP_STRING 2
#define RLP_LIST 3

#endif //UTILS_PROTOCOLS_ETHEREUM_ETH_CONSTS_H
