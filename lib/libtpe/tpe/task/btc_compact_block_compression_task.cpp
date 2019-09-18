#include <utility>

#include <array>
#include "tpe/task/btc_compact_block_compression_task.h"
#include "tpe/task/sub_task/btc_compact_block_compression_sub_task.h"

#include <utils/protocols/bitcoin/btc_message_helper.h>
#include <utils/common/buffer_helper.h>



namespace task {

typedef service::TransactionPlaceholder TransactionPlaceholder_t;
typedef service::TransactionPlaceholderType TransactionPlaceholderType_t;
typedef std::shared_ptr<BtcCompactBlockCompressionSubTask> PBtcCompactBlockCompressionSubTask;

BtcCompactBlockCompressionTask::BtcCompactBlockCompressionTask(
			size_t capacity/* = BTC_DEFAULT_BLOCK_SIZE*/
):
		_magic(0),
		_block_hash(nullptr),
		_prev_block_hash(nullptr),
		_compressed_block_hash(nullptr),
		_txn_count(0)
{
	_output_buffer = std::make_shared<ByteArray_t>(capacity);
	_recovered_transactions = std::make_shared<RecoveredTransactions_t>();
}

void BtcCompactBlockCompressionTask::init(
		CompactBlockDataService_t data_service,
		int magic
)
{
	_magic = magic;
	size_t capacity = std::max(
			_output_buffer->capacity(),
			data_service.size()
	);
	if (_output_buffer.use_count() > 1) {
		_output_buffer = std::make_shared<ByteArray_t>(capacity);
	} else {
		_output_buffer->clear();
	}
	_data_service = std::move(data_service);
	_block_hash = std::make_shared<Sha256_t>(std::move(
			_data_service.msg().block_hash()
	));
	_prev_block_hash = std::make_shared<Sha256_t>(std::move(
			_data_service.msg().prev_block_hash()
	));
	_block_header.clear();
	_short_ids.clear();
	_compressed_block_hash = nullptr;
    _recovered_transactions = nullptr;
    _txn_count = 0;
}

void BtcCompactBlockCompressionTask::add_recovered_transactions(
		PRecoveredTransactions_t recovered_transactions
)
{
	_recovered_transactions = std::move(recovered_transactions);
}

PByteArray_t BtcCompactBlockCompressionTask::bx_block() {
	assert_execution();
	return _output_buffer;
}

PSha256_t BtcCompactBlockCompressionTask::prev_block_hash() {
	assert_execution();
	return _prev_block_hash;
}

PSha256_t BtcCompactBlockCompressionTask::block_hash() {
	assert_execution();
	return _block_hash;
}

PSha256_t BtcCompactBlockCompressionTask::compressed_block_hash() {
	assert_execution();
	return _compressed_block_hash;
}

uint64_t BtcCompactBlockCompressionTask::txn_count() {
	assert_execution();
	return _txn_count;
}

const std::vector<uint32_t>&
BtcCompactBlockCompressionTask::short_ids() {
	assert_execution();
	return _short_ids;
}

size_t BtcCompactBlockCompressionTask::get_task_byte_size() const {
    size_t recovered_tx_size = 0;
    if (_recovered_transactions != nullptr) {
        for (const PTxBuffer_t &recovered_tx: *_recovered_transactions) {
            recovered_tx_size += (sizeof(PTxBuffer_t) + sizeof(TxBuffer_t) + recovered_tx->size());
        }
    }
    return sizeof(BtcCompactBlockCompressionTask) + _output_buffer->capacity() + recovered_tx_size +
            _block_header.size() + (_short_ids.size() * sizeof(uint32_t) + _data_service.byte_size());
}

void BtcCompactBlockCompressionTask::_execute(SubPool_t& sub_pool) {
	_txn_count = _data_service.total_tx_count();
	size_t offset = _set_header();
	PBtcCompactBlockCompressionSubTask sub_task =
			std::make_shared<BtcCompactBlockCompressionSubTask>(
					_data_service.tx_placeholders(),
					*_recovered_transactions,
					_block_header
			);
	sub_pool.enqueue_task(sub_task);
	offset = _fill_block_transactions(offset);
	_compressed_block_hash = std::make_shared<Sha256_t>(std::move(
			utils::crypto::double_sha256(
					_output_buffer->array(),
					0,
					_output_buffer->size()
			)
	));
	sub_task->wait();
	_set_output_buffer(
			offset,
			sub_task->payload_length(),
			sub_task->checksum()
	);
	_data_service.on_compression_completed();
}

size_t BtcCompactBlockCompressionTask::_set_header() {
	static constexpr size_t MSG_TYPE_LENGTH = 12;
	static const std::array<uint8_t, MSG_TYPE_LENGTH> _BLOCK_MSG_TYPE = {'b', 'l', 'o', 'c', 'k', 0};
	static const service::BufferView_t BLOCK_MSG_TYPE(
			&_BLOCK_MSG_TYPE.at(0),
			MSG_TYPE_LENGTH
	);
	service::BufferView_t cmpt_header = std::move(
			_data_service.msg().get_block_header()
	);
	size_t offset = sizeof(uint64_t);
	offset = utils::common::set_little_endian_value<int>(
			*_output_buffer,
			_magic,
			offset
	);
	offset = _output_buffer->copy_from_buffer(
			BLOCK_MSG_TYPE, offset, 0, BLOCK_MSG_TYPE.size()
	);
	offset += 2 * sizeof(uint32_t);
	offset = _output_buffer->copy_from_buffer(
			cmpt_header, offset, 0, cmpt_header.size()
	);
	_txn_count = _data_service.total_tx_count();
	size_t txn_count_size = offset;
	offset = utils::protocols::bitcoin::set_varint(
			*_output_buffer,
			_txn_count,
			offset
	);
	txn_count_size = offset - txn_count_size;
	_block_header.resize(BTC_BLOCK_HDR_SIZE + txn_count_size, 0);
	memcpy(
		&_block_header.at(0),
		&_output_buffer->at(sizeof(uint64_t) + BTC_HDR_COMMON_OFFSET),
		BTC_BLOCK_HDR_SIZE + txn_count_size
	);
	return offset;
}

size_t BtcCompactBlockCompressionTask::_fill_block_transactions(
		size_t offset
)
{
	auto recovered_iter = _recovered_transactions->begin();
	for(
			const TransactionPlaceholder_t& placeholder:
			_data_service.tx_placeholders()
	)
	{
		PTxBuffer_t tx = nullptr;
		switch(placeholder.type) {

			case TransactionPlaceholderType_t::missing_compact_id:
				tx = *recovered_iter;
				offset = _output_buffer->copy_from_buffer(
						*tx, offset, 0, tx->size()
				);
				++recovered_iter;
				break;

			case TransactionPlaceholderType_t::content:
				tx = placeholder.contents;
				offset = _output_buffer->copy_from_buffer(
						*tx, offset, 0, tx->size()
				);
				break;

			case TransactionPlaceholderType_t::short_id:
				offset = utils::common::set_little_endian_value<uint8_t>(
						*_output_buffer,
						BTC_SHORT_ID_INDICATOR,
						offset
				);
				_short_ids.push_back(placeholder.sid);
				break;

			default:
				break;

		}
	}
	return offset;
}

void BtcCompactBlockCompressionTask::_set_output_buffer(
		size_t output_offset,
		uint32_t payload_length,
		uint32_t checksum
)
{
	size_t offset = utils::common::set_little_endian_value(
			*_output_buffer,
			(uint64_t)output_offset,
			0
	);
	offset += BTC_BLOCK_LENGTH_OFFSET;
	offset = utils::common::set_little_endian_value(
			*_output_buffer,
			payload_length,
			offset
	);
	utils::common::set_little_endian_value(
				*_output_buffer,
				checksum,
				offset
	);
	const unsigned int short_ids_size = _short_ids.size();
	output_offset = utils::common::set_little_endian_value(
			*_output_buffer,
			short_ids_size,
			output_offset
	);
	if (short_ids_size > 0) {
		const size_t short_ids_byte_size  =
				short_ids_size * sizeof(unsigned int);
		_output_buffer->resize(output_offset + short_ids_byte_size);
		memcpy(
				&_output_buffer->at(output_offset),
				(unsigned char*) &_short_ids.at(0),
				short_ids_byte_size
		);
	}
	_output_buffer->set_output();
}


} // task
