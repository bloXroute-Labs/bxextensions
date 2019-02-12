#include <iostream>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/btc_block_message.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"

#ifndef TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_
#define TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_

namespace task {

class BTCBlockDecompressionTask : public MainTaskBase {

	typedef std::unique_ptr<utils::common::BufferView> PBufferView_t;
	typedef const std::vector<unsigned short>& BlockMessage_t;

	struct TxData {

		const utils::crypto::Sha256* psha;
		PBufferView_t ptx;

		TxData(): psha(nullptr), ptx(nullptr) {}

		operator bool() const {
			return psha != nullptr && ptx != nullptr;
		}

	};

public:
	BTCBlockDecompressionTask(
			const ShortIDToSha256Map_t& short_id_map,
			const Sha256ToTxMap_t& tx_map,
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE
	);

	void init(const BlockBuffer_t& block_buffer);

	BlockMessage_t block_message(void);
	const UnknownTxHashes_t& unknown_tx_hashes(void);
	const UnknownTxSIDs_t& unknown_tx_sids(void);
	const utils::crypto::Sha256& block_hash(void);


protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(size_t pool_size, size_t tx_count);
	void _dispatch(
			size_t tx_count,
			utils::protocols::BTCBlockMessage& msg,
			size_t offset,
			SubPool_t& sub_pool
	);
	void _enqueue_task(size_t task_idx, SubPool_t& sub_pool);

	int _try_get_tx_data(
			unsigned int short_id,
			TxData& tx_data
	);


	BlockBuffer_t _block_buffer;
	utils::common::ByteArray _output_buffer;
	UnknownTxHashes_t _unknown_tx_hashes;
	UnknownTxSIDs_t _unknown_tx_sids;
	utils::crypto::Sha256 _block_hash;
	const ShortIDToSha256Map_t& _short_id_map;
	const Sha256ToTxMap_t& _tx_map;
	volatile bool _partial_block;
};

} // task



#endif /* TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_ */
