#include <utils/common/buffer_helper.h>
#include <utils/protocols/ontology/ont_message_helper.h>
#include <utils/common/string_helper.h>
#include "tpe/task/ont_consensus_block_compression_task.h"

namespace task {

OntConsensusBlockCompressionTask::OntConsensusBlockCompressionTask(size_t capacity, size_t minimal_tx_count):
    MainTaskBase(), _minimal_tx_count(minimal_tx_count)
{
    _output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void OntConsensusBlockCompressionTask::init(
        PBlockBuffer_t block_buffer,
        PTransactionService_t tx_service
)
{
    _tx_service = std::move(tx_service);
    _block_buffer = std::move(block_buffer);
    if (_output_buffer.use_count() > 1) {
        _output_buffer =  std::make_shared<ByteArray_t>(
                _block_buffer->size()
        );
    } else {
        _output_buffer->reserve(_block_buffer->size());
        _output_buffer->reset();
    }
    _short_ids.clear();
    _block_hash = _prev_block_hash = _compressed_block_hash = nullptr;
    _txn_count = 0;
}

PByteArray_t OntConsensusBlockCompressionTask::bx_block() {
    assert_execution();
    return _output_buffer;
}

PSha256_t OntConsensusBlockCompressionTask::prev_block_hash() {
    assert_execution();
    return _prev_block_hash;
}

PSha256_t OntConsensusBlockCompressionTask::block_hash() {
    assert_execution();
    return _block_hash;
}
PSha256_t OntConsensusBlockCompressionTask::compressed_block_hash() {
    assert_execution();
    return _compressed_block_hash;
}

uint32_t OntConsensusBlockCompressionTask::txn_count() {
    assert_execution();
    return _txn_count;
}

const std::vector<unsigned int>& OntConsensusBlockCompressionTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t OntConsensusBlockCompressionTask::get_task_byte_size() const {
    size_t block_buffer_size = _block_buffer->size() ? _block_buffer != nullptr: 0;
    return sizeof(OntConsensusBlockCompressionTask) + block_buffer_size + _output_buffer->size();
}

void OntConsensusBlockCompressionTask::cleanup() {
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

void OntConsensusBlockCompressionTask::_execute(task::SubPool_t & sub_pool) {
    ConsensusOntMessage_t msg(*_block_buffer);
    constexpr uint8_t short_id_flag = BTC_SHORT_ID_INDICATOR;
    msg.parse();
    size_t header_offset = msg.get_header_offset();
    size_t output_offset = sizeof(size_t);
    _output_buffer->resize(output_offset);
    constexpr uint8_t is_consensus_flag = 0x01;
    output_offset = utils::common::set_little_endian_value(*_output_buffer, is_consensus_flag, output_offset);
    _block_hash = std::make_shared<Sha256_t>(std::move(msg.block_hash()));
    _prev_block_hash = std::make_shared<Sha256_t>(std::move(msg.prev_block_hash()));
    output_offset = _output_buffer->copy_from_buffer(_block_hash->binary(), output_offset, 0, SHA256_BINARY_SIZE);
    size_t tx_offset = msg.get_txn_count(_txn_count);
    output_offset = utils::common::set_little_endian_value(*_output_buffer, _txn_count, output_offset);
    const auto& payload_tail = msg.payload_tail();
    output_offset = utils::common::set_little_endian_value<uint32_t >(
            *_output_buffer, payload_tail.size(), output_offset
    );
    output_offset = _output_buffer->copy_from_buffer(payload_tail, output_offset, 0, payload_tail.size());
    const auto& owner_and_signature = msg.owner_and_signature();
    output_offset = utils::common::set_little_endian_value<uint32_t >(
            *_output_buffer, owner_and_signature.size(), output_offset
    );
    output_offset = _output_buffer->copy_from_buffer(owner_and_signature, output_offset, 0, owner_and_signature.size());
    output_offset = utils::common::set_little_endian_value<uint32_t>(*_output_buffer, header_offset, output_offset);
    output_offset = _output_buffer->copy_from_buffer(*_block_buffer, output_offset, 0, header_offset);
    output_offset = utils::common::set_little_endian_value(*_output_buffer, msg.consensus_data_type(), output_offset);
    output_offset = utils::common::set_little_endian_value(*_output_buffer, msg.consensus_data_len(), output_offset);
    output_offset = utils::common::set_little_endian_value<uint32_t>(*_output_buffer, tx_offset, output_offset);
    output_offset = _output_buffer->copy_from_buffer(msg.block_msg_buffer(), output_offset, 0, tx_offset);
    size_t from = tx_offset;
    auto block_buffer = std::move(msg.block_msg_buffer());
    for (uint32_t idx = 0 ; idx < _txn_count ; ++idx) {
        tx_offset = msg.get_next_tx_offset(from);
        Sha256_t tx_hash = std::move(utils::protocols::ontology::get_tx_id(block_buffer, from, tx_offset));
        if (_tx_service->has_short_id(tx_hash)) {
            _short_ids.push_back(_tx_service->get_short_id(tx_hash));
            output_offset = utils::common::set_little_endian_value(*_output_buffer, short_id_flag, output_offset);
        } else {
            output_offset = _output_buffer->copy_from_buffer(
                    block_buffer,
                    output_offset,
                    from,
                    tx_offset - from
            );
        }
        from = tx_offset;
    }
    const uint32_t short_ids_size = _short_ids.size();
    utils::common::set_little_endian_value<uint64_t >(*_output_buffer, output_offset, 0);
    output_offset = utils::common::set_little_endian_value<uint32_t>(
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
    utils::crypto::Sha256 temp_compressed_block_hash = utils::crypto::double_sha256(
            _output_buffer->array(),
            0,
            _output_buffer->size()
    );
    temp_compressed_block_hash.reverse();

    _compressed_block_hash = std::make_shared<Sha256_t>(std::move(temp_compressed_block_hash));
    _output_buffer->set_output();
}

} // task
