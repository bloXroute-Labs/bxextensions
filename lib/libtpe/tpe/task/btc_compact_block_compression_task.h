#include <utils/protocols/bitcoin/btc_compact_block_message.h>
#include <utils/protocols/bitcoin/btc_consts.h>

#include "tpe/task/main_task_base.h"
#include "tpe/task/btc_task_types.h"
#include "tpe/service/compact_block_data_service.h"


#ifndef TPE_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_
#define TPE_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_

namespace task {

typedef std::shared_ptr<TxBuffer_t> PTxBuffer_t;
typedef std::vector<PTxBuffer_t> RecoveredTransactions_t;
typedef std::shared_ptr<RecoveredTransactions_t> PRecoveredTransactions_t;
typedef service::CompactBlockDataService CompactBlockDataService_t;

class BtcCompactBlockCompressionTask : public MainTaskBase {
public:

	BtcCompactBlockCompressionTask(
			size_t capacity = BTC_DEFAULT_BLOCK_SIZE
	);

	void init(
			CompactBlockDataService_t data_service,
			int magic
	);

	void add_recovered_transactions(
			PRecoveredTransactions_t recovered_transactions
	);

	PByteArray_t bx_block();

	PSha256_t prev_block_hash();
	PSha256_t block_hash();
	PSha256_t compressed_block_hash();

	uint64_t txn_count();

	const std::vector<uint32_t>& short_ids();

protected:
	void _execute(SubPool_t& sub_pool) override;

private:

	size_t _set_header();

	size_t _fill_block_transactions(size_t offset);

	void _set_output_buffer(
			size_t output_offset,
			uint32_t payload_length,
			uint32_t checksum
	);

	CompactBlockDataService_t _data_service;
	PByteArray_t _output_buffer;
	PRecoveredTransactions_t _recovered_transactions;
	int _magic;
	PSha256_t _prev_block_hash;
	PSha256_t _block_hash;
	PSha256_t _compressed_block_hash;
	uint64_t _txn_count;
	std::vector<uint32_t> _short_ids;
	std::vector<uint8_t> _block_header;

};

} // task

#endif /* TPE_TASK_BTC_COMPACT_BLOCK_COMPRESSION_TASK_H_ */
