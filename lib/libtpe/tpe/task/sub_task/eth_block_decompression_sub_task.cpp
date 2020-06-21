#include <utils/common/buffer_helper.h>
#include <utils/common/buffer_view.h>
#include <utils/encoding/rlp_encoder.h>
#include <utils/protocols/ethereum/eth_consts.h>
#include "tpe/task/sub_task/eth_block_decompression_sub_task.h"
#include <utils/encoding/rlp_encoding_type.h>


namespace task {
typedef utils::common::ByteArray ByteArray_t;

EthBlockDecompressionSubTask::EthBlockDecompressionSubTask(size_t capacity):
    SubTaskBase(), _tx_service(nullptr), _output_buffer(capacity), _tx_offsets(nullptr),
    _block_buffer(nullptr), _content_size(0), _short_ids(nullptr)
{
}

void EthBlockDecompressionSubTask::init(
    PTransactionService_t tx_service,
    const BlockBuffer_t* block_buffer,
    POffests_t tx_offsets,
    const ShortIDs_t* short_ids
)
{
    _tx_service = tx_service;
    _block_buffer = block_buffer;
    _tx_offsets = tx_offsets;
    _short_ids = short_ids;
    _output_buffer.reset();
    _content_size = 0;
    _unknown_tx_hashes.clear();
    _unknown_tx_sids.clear();
}

const utils::common::ByteArray& EthBlockDecompressionSubTask::output_buffer() {
    assert_execution();
    return _output_buffer;
}

const ShortIDs_t* EthBlockDecompressionSubTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t EthBlockDecompressionSubTask::content_size() const {
    return _content_size;
}

const UnknownTxHashes_t& EthBlockDecompressionSubTask::unknown_tx_hashes() {
    assert_execution();
    return _unknown_tx_hashes;
}

const UnknownTxSIDs_t& EthBlockDecompressionSubTask::unknown_tx_sids() {
    assert_execution();
    return _unknown_tx_sids;
}

bool EthBlockDecompressionSubTask::success() {
    assert_execution();
    return _success;
}

EthBlockDecompressionSubTask::TaskData& EthBlockDecompressionSubTask::task_data() {
    return _task_data;
}

void EthBlockDecompressionSubTask::_execute()  {
    size_t output_offset = 0;

    auto short_id_offset = 0;
    PSha256_t tx_hash;

    for (auto& offsets : *_tx_offsets) {
        size_t from = std::get<0>(offsets);
        const service::TxContents_t* p_contents = nullptr;

        uint64_t tx_content_len, tx_item_len;
        size_t offset = utils::encoding::consume_length_prefix(*_block_buffer, tx_item_len, from);

        if ( _block_buffer->at(offset) != ETH_SHORT_ID_INDICATOR ) {
            ++offset;
            offset = utils::encoding::consume_length_prefix(*_block_buffer, tx_content_len, offset);

            if (tx_content_len > 0) {
                output_offset = _output_buffer.copy_from_buffer(*_block_buffer, output_offset, offset, tx_content_len);
                _content_size += tx_content_len;
            }
        } else {
            unsigned int short_id = _short_ids->at(short_id_offset);
            p_contents = _tx_service->get_tx_contents_raw_ptr(short_id);

            ++short_id_offset;

            if (p_contents->size() > 0) {
                output_offset = _output_buffer.copy_from_buffer(*p_contents, output_offset, 0, p_contents->size());
                _content_size += p_contents->size();
            }
        }
    }

    _tx_service = nullptr;
    _tx_offsets = nullptr;
    _block_buffer = nullptr;
}

}
