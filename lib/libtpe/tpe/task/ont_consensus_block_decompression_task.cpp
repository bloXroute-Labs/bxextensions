#include <utils/encoding/base64_encoder.h>
#include <utils/encoding/json_encoder.h>
#include <utils/protocols/ontology/consensus/ont_consensus_json_payload.h>
#include "tpe/task/ont_consensus_block_decompression_task.h"

namespace task {

OntConsensusBlockDecompressionTask::OntConsensusBlockDecompressionTask(
    size_t capacity/* = ONT_DEFAULT_BLOCK_SIZE*/,
    size_t minimal_tx_count/* = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT*/
): MainTaskBase(), _minimal_tx_count(minimal_tx_count)
{
    _output_buffer = std::make_shared<ByteArray_t>(capacity);
}

void OntConsensusBlockDecompressionTask::init(
    PBlockBuffer_t block_buffer,
    PTransactionService_t tx_service
)
{
    _unknown_tx_hashes.clear();
    size_t capacity = _output_buffer->capacity();
    if (_output_buffer.use_count() > 1) {
        _output_buffer =  std::make_shared<ByteArray_t>(capacity);
    } else {
        _output_buffer->reserve(capacity);
    }
    _unknown_tx_sids.clear();
    _block_buffer = std::move(block_buffer);
    _block_hash = nullptr;
    _tx_service = std::move(tx_service);
    _short_ids.clear();
    _txn_count = 0;
}

PByteArray_t OntConsensusBlockDecompressionTask::block_message()  {
    assert_execution();
    return _output_buffer;
}

const UnknownTxHashes_t & OntConsensusBlockDecompressionTask::unknown_tx_hashes() {
    assert_execution();
    return _unknown_tx_hashes;
}

const UnknownTxSIDs_t & OntConsensusBlockDecompressionTask::unknown_tx_sids() {
    assert_execution();
    return _unknown_tx_sids;
}

PSha256_t OntConsensusBlockDecompressionTask::block_hash() {
    assert_execution();
    return _block_hash;
}

bool OntConsensusBlockDecompressionTask::success() {
    assert_execution();
    return _success;
}

uint32_t OntConsensusBlockDecompressionTask::txn_count() {
    assert_execution();
    return _txn_count;
}

const std::vector<unsigned int> & OntConsensusBlockDecompressionTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t OntConsensusBlockDecompressionTask::get_task_byte_size() const {
    size_t block_buffer_size = _block_buffer->size() ? _block_buffer != nullptr: 0;
    return sizeof(OntConsensusBlockDecompressionTask) + block_buffer_size + _output_buffer->capacity() +
        sizeof(uint32_t) * (_short_ids.capacity() + _unknown_tx_sids.capacity()) +
        sizeof(Sha256_t) * _unknown_tx_hashes.size() +
        sizeof(UnknownTxHashes_t::value_type) * _unknown_tx_hashes.capacity();
}

void OntConsensusBlockDecompressionTask::cleanup() {
    _block_buffer = nullptr;
    _tx_service = nullptr;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void OntConsensusBlockDecompressionTask::_execute(task::SubPool_t &sub_pool) {
    BxOntConsensusMessage_t msg(*_block_buffer);
    msg.parse();
    _block_hash = std::make_shared<Sha256_t>(msg.block_hash());
    _output_buffer->reserve(msg.get_original_block_size());
    _output_buffer->reset();
    msg.deserialize_short_ids(_short_ids);
    _success = _tx_service->get_missing_transactions(_unknown_tx_hashes, _unknown_tx_sids, _short_ids);
    if (!_success) {
        return;
    }
    size_t output_offset = _output_buffer->copy_from_buffer(msg.consensus_payload_header(), 0, 0, msg.consensus_payload_header().size());
    size_t tx_offset = msg.get_tx_count(_txn_count);
    size_t from = tx_offset;
    std::string decoded_payload(msg.header().char_array(), msg.header().size());
    bool is_short;
    Sha256_t tx_hash;
    auto short_id_iter = _short_ids.begin();
    for (uint32_t idx = 0 ; idx < _txn_count ; ++idx) {
        tx_offset = msg.get_next_tx_offset(from, is_short);
        if (is_short) {
            auto tx = std::move(_tx_service->get_transaction(*short_id_iter, tx_hash));
            decoded_payload.append(tx->char_array(), tx->size());
            ++short_id_iter;
        } else {
            decoded_payload.append(&_block_buffer->char_array()[from], tx_offset - from);
        }
        from = tx_offset;
    }
    decoded_payload.append(msg.payload_tail().char_array(), msg.payload_tail().size());
    utils::protocols::ontology::consensus::OntConsensusJsonPayload payload(
        msg.consensus_data_type(),
        msg.consensus_data_len(),
        utils::encoding::base64_encode(decoded_payload)
    );
    std::string json_payload = utils::encoding::encode_json(payload);
    size_t payload_len = json_payload.size();
    _output_buffer->resize(output_offset + payload_len);
    memcpy(&_output_buffer->char_array()[output_offset], json_payload.c_str(), payload_len);
    output_offset += payload_len;
    _output_buffer->copy_from_buffer(msg.owner_and_signature(), output_offset, 0, msg.owner_and_signature().size());
    _output_buffer->set_output();
}
#pragma GCC diagnostic pop

} // tast