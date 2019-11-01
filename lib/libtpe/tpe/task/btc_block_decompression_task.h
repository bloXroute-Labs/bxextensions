#include <iostream>
#include <memory>

#include <utils/common/byte_array.h>
#include <utils/common/buffer_view.h>
#include <utils/crypto/sha256.h>
#include <utils/protocols/bitcoin/btc_consts.h>
#include <utils/protocols/bitcoin/bx_btc_block_message.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"
#include "tpe/task/sub_task/btc_block_decompression_sub_task.h"
#include "tpe/consts.h"

#ifndef TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_
#define TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_

namespace task {

typedef std::shared_ptr<BtcBlockDecompressionSubTask> PSubTask_t;
typedef utils::protocols::bitcoin::BxBtcBlockMessage BxBtcBlockMessage_t;

class BtcBlockDecompressionTask : public MainTaskBase {

public:
	explicit BtcBlockDecompressionTask(
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE,
			size_t minimal_tx_count = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
	);

	void init(
            PBlockBuffer_t block_buffer,
			PTransactionService_t tx_service
	);

	PByteArray_t btc_block();
	const UnknownTxHashes_t& unknown_tx_hashes();
	const UnknownTxSIDs_t& unknown_tx_sids();
	PSha256_t block_hash();
	bool success();
	uint64_t tx_count();
	const std::vector<unsigned int>& short_ids();

    size_t get_task_byte_size() const override;

    void cleanup() override;

protected:
	void _execute(SubPool_t& sub_pool) override;

private:
	void _init_sub_tasks(size_t pool_size);
	size_t _dispatch(
			BxBtcBlockMessage_t& msg,
			size_t offset,
			SubPool_t& sub_pool
	);
	void _enqueue_task(size_t task_idx, SubPool_t& sub_pool);

	BxBtcBlockMessage_t _parse_block_header(
			size_t& offset,
			uint64_t& tx_count
	);

	void _extend_output_buffer(size_t output_offset);


    PBlockBuffer_t _block_buffer;
	PByteArray_t _output_buffer;
	UnknownTxHashes_t _unknown_tx_hashes;
	UnknownTxSIDs_t _unknown_tx_sids;
	std::vector<unsigned int> _short_ids;
	PSha256_t _block_hash;
	PTransactionService_t _tx_service;
	std::vector<PSubTask_t> _sub_tasks;
	const size_t _minimal_tx_count;
	bool _success;
	uint64_t _tx_count;
};

} // task



#endif /* TPE_TASK_BTC_BLOCK_DECOMPRESSION_TASK_H_ */
