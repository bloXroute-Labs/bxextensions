#ifndef UTILS_COMMON_TX_STATUS_CONSTS_H
#define UTILS_COMMON_TX_STATUS_CONSTS_H

#define TX_STATUS_IGNORE_SEEN 1 << 0                // 1
#define TX_STATUS_MSG_HAS_SHORT_ID 1 << 1           // 2
#define TX_STATUS_MSG_HAS_CONTENT 1 << 2            // 4
#define TX_STATUS_MSG_NO_CONTENT 1 << 3             // 8
#define TX_STATUS_MSG_NO_SHORT_ID 1 << 4            // 16
#define TX_STATUS_SEEN_SHORT_ID 1 << 5              // 32
#define TX_STATUS_SEEN_HASH 1 << 6                  // 64
#define TX_STATUS_SEEN_CONTENT 1 << 7               // 128
#define TX_STATUS_SEEN_REMOVED_TRANSACTION 1 << 8   // 256
#define TX_STATUS_TIMED_OUT 1 << 9                  // 512

#endif //UTILS_COMMON_TX_STATUS_CONSTS_H
