#include <utility>
#include <algorithm>

#include <utils/common/buffer_helper.h>
#include <utils/crypto/hash_helper.h>
#include <utils/encoding/rlp_encoder.h>
#include "tpe/task/eth_block_compression_task.h"

namespace task {

EthBlockCompressionTask::EthBlockCompressionTask(
    size_t capacity/* = ETH_DEFAULT_BLOCK_SIZE*/,
    size_t minimal_tx_count/* = ETH_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
):
        MainTaskBase(),
        _tx_service(nullptr),
        _minimal_tx_count(minimal_tx_count),
        _content_size(0),
        _txn_count(0),
        _starting_offset(0),
        _enable_block_compression(false),
        _min_tx_age_seconds(0.0)
{
    _block_buffer = std::make_shared<BlockBuffer_t>(BlockBuffer_t::empty());
    _output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void EthBlockCompressionTask::init(
    PBlockBuffer_t block_buffer,
    PTransactionService_t tx_service,
    bool enable_block_compression,
    double min_tx_age_seconds
)
{
    _tx_service = std::move(tx_service);
    _block_buffer = std::move(block_buffer);
    if (_output_buffer.use_count() > 1) {
        _output_buffer =  std::make_shared<ByteArray_t>(_block_buffer->size());
    } else {
        _output_buffer->reserve(_block_buffer->size());
        _output_buffer->reset();
    }
    _short_ids.clear();
    _ignored_short_ids.clear();
    _block_hash = _prev_block_hash = _compressed_block_hash = nullptr;
    _content_size = 0;
    _txn_count = 0;
    _starting_offset = 0;
    _enable_block_compression = enable_block_compression;
    _min_tx_age_seconds = min_tx_age_seconds;
}

PByteArray_t EthBlockCompressionTask::bx_block() {
    assert_execution();
    return _output_buffer;
}

PSha256_t EthBlockCompressionTask::prev_block_hash() {
    assert_execution();
    return _prev_block_hash;
}

PSha256_t EthBlockCompressionTask::block_hash() {
    assert_execution();
    return _block_hash;
}

PSha256_t EthBlockCompressionTask::compressed_block_hash() {
    assert_execution();
    return _compressed_block_hash;
}

size_t EthBlockCompressionTask::txn_count() {
    return _txn_count;
}

size_t EthBlockCompressionTask::starting_offset() {
    return _starting_offset;
}

const std::vector<unsigned int>& EthBlockCompressionTask::short_ids() {
    assert_execution();
    return _short_ids;
}

const std::vector<unsigned int>& EthBlockCompressionTask::ignored_short_ids() {
    assert_execution();
    return _ignored_short_ids;
}

size_t EthBlockCompressionTask::get_task_byte_size() const {
    size_t block_buffer_size = _block_buffer->size() ? _block_buffer != nullptr: 0;
    return sizeof(EthBlockCompressionTask) + block_buffer_size + _output_buffer->size();
}

void EthBlockCompressionTask::cleanup() {
    assert_execution();
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void EthBlockCompressionTask::_execute(SubPool_t& sub_pool) {
    utils::protocols::ethereum::EthBlockMessage msg(*_block_buffer);
    msg.parse();
    _block_header = BlockBuffer_t(msg.block_header());
    _block_trailer = BlockBuffer_t(msg.block_trailer());
    _prev_block_hash = std::make_shared<Sha256_t>(std::move(msg.prev_block_hash()));

    _block_hash = std::make_shared<Sha256_t>(std::move(msg.block_hash()));

    // assume txs starts from tx_offset where msg_len_prefix and txs_len_prefix are max size (8 bytes)
    size_t output_offset = sizeof(size_t) + sizeof(size_t) + _block_header.size() + sizeof(size_t);
    size_t tx_origin_offset = output_offset;
    size_t tx_from = msg.txn_offset(), txn_end_offset = msg.txn_end_offset(), tx_offset;
    ByteArray_t tx_content_prefix = ByteArray_t();

    _output_buffer->reserve((size_t)(_block_buffer->size() * 1.1));
    std::vector<uint8_t> is_full_tx_bytes, is_short_tx_byte;
    is_full_tx_bytes.push_back(ETH_FULL_TX_INDICATOR);
    is_short_tx_byte.push_back(ETH_RLP_OF_SHORT_TX);
    is_short_tx_byte.push_back(ETH_SHORT_ID_INDICATOR);
    is_short_tx_byte.push_back(ETH_SHORT_ID_INDICATOR);

    double current_time = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    double max_timestamp_for_compression = current_time - _min_tx_age_seconds;

    while (tx_from < txn_end_offset) {
        tx_offset = msg.get_next_tx_offset(tx_from);

        uint64_t tx_item_len;
        size_t tx_item_offset = utils::encoding::consume_length_prefix(*_block_buffer, tx_item_len, tx_from);

        utils::crypto::Sha256 tx_hash = utils::crypto::Sha256(
            utils::crypto::keccak_sha3(
                (uint8_t*)_block_buffer->char_array(), tx_from, tx_item_offset + tx_item_len - tx_from
            )
        );

        size_t tx_content_bytes_len = 0, tx_content_prefix_offset;

        double short_id_assign_time = 0.0;
        if ( _tx_service->has_short_id(tx_hash) ) {
            short_id_assign_time = _tx_service->get_short_id_assign_time(_tx_service->get_short_id(tx_hash));
        }

        bool has_short_id = _tx_service->has_short_id(tx_hash);

        if ( ! has_short_id or
             not _enable_block_compression or
                 short_id_assign_time > max_timestamp_for_compression ) {
            if ( has_short_id ) {
                _ignored_short_ids.push_back(_tx_service->get_short_id(tx_hash));
            }
            tx_content_bytes_len = tx_offset - tx_from;
            tx_content_prefix_offset = utils::encoding::get_length_prefix_str(
                tx_content_prefix, tx_content_bytes_len, 0
            );

            size_t short_tx_content_size = is_full_tx_bytes.size() + tx_content_prefix_offset + tx_content_bytes_len;

            output_offset = utils::encoding::get_length_prefix_list(
                *_output_buffer, short_tx_content_size, output_offset
            );

            output_offset = _output_buffer->copy_from_array(
                is_full_tx_bytes, output_offset, 0, is_full_tx_bytes.size()
            );
            output_offset = _output_buffer->copy_from_array(
                tx_content_prefix.array(), output_offset, 0, tx_content_prefix_offset
            );
            if (tx_content_bytes_len > 0) {
                output_offset = _output_buffer->copy_from_buffer(
                    *_block_buffer, output_offset, tx_from, tx_offset - tx_from
                );
            }
        } else {
            _short_ids.push_back(_tx_service->get_short_id(tx_hash));
            output_offset = _output_buffer->copy_from_array(
                is_short_tx_byte, output_offset, 0, is_short_tx_byte.size()
            );
        }

        tx_from = tx_offset;
        ++_txn_count;
    }

    // handling content length
    _content_size = output_offset - tx_origin_offset;
    ByteArray_t list_of_txs_prefix_buffer = ByteArray_t();
    utils::encoding::get_length_prefix_list(list_of_txs_prefix_buffer, _content_size, 0);
    tx_origin_offset -= list_of_txs_prefix_buffer.size();
    _output_buffer->copy_from_array(
        list_of_txs_prefix_buffer.array(), tx_origin_offset, 0, list_of_txs_prefix_buffer.size()
    );

    // handling block trailer
    if (_block_trailer.size() > 0) {
        _output_buffer->resize(output_offset);
        _output_buffer->operator +=(_block_trailer);
        output_offset += _block_trailer.size();
    }
    _content_size = output_offset - tx_origin_offset;

    // handling block header
    tx_origin_offset -= _block_header.size();
    _content_size += _block_header.size();
    _output_buffer->copy_from_buffer(_block_header, tx_origin_offset, 0, _block_header.size());

    // handling msg len
    ByteArray_t block_msg_prefix_buf = ByteArray_t();
    utils::encoding::get_length_prefix_list(block_msg_prefix_buf, _content_size, 0);
    tx_origin_offset -= block_msg_prefix_buf.size();
    _output_buffer->copy_from_array(
        block_msg_prefix_buf.array(), tx_origin_offset, 0, block_msg_prefix_buf.size()
    );

    // handling short ids offset
    _starting_offset = tx_origin_offset - sizeof(size_t);
    _content_size = output_offset - _starting_offset;
    utils::common::set_little_endian_value(*_output_buffer, (uint64_t)_content_size, _starting_offset);

    // handling short ids list
    const unsigned int short_ids_size = _short_ids.size();

    _content_size += _starting_offset;
    output_offset = utils::common::set_little_endian_value(*_output_buffer, short_ids_size, _content_size);

    const size_t short_ids_byte_size = short_ids_size * sizeof(unsigned int);
    _output_buffer->resize(output_offset + short_ids_byte_size);
    if (short_ids_size > 0) {
        memcpy(&_output_buffer->at(output_offset), (unsigned char*) &_short_ids.at(0), short_ids_byte_size);
    }

    _output_buffer->set_output();

    _compressed_block_hash = std::make_shared<Sha256_t>(
        std::move(
            utils::crypto::double_sha256(_output_buffer->array(), _starting_offset, output_offset - _starting_offset)
        )
    );
}

} // task
