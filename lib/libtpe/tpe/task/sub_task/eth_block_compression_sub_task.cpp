#include <utils/common/buffer_helper.h>
#include <utils/common/buffer_view.h>
#include <utils/crypto/hash_helper.h>
#include <utils/encoding/rlp_encoder.h>
#include <utils/protocols/ethereum/eth_consts.h>
#include "tpe/task/sub_task/eth_block_compression_sub_task.h"
#include <utils/encoding/rlp_encoding_type.h>


namespace task {
typedef utils::common::ByteArray ByteArray_t;

EthBlockCompressionSubTask::EthBlockCompressionSubTask(
        size_t capacity):
        SubTaskBase(),
        _tx_service(nullptr),
        _output_buffer(capacity),
        _tx_offsets(nullptr),
        _block_buffer(nullptr),
        _content_size(0)
{
}

void EthBlockCompressionSubTask::init(
    PTransactionService_t tx_service,
    const BlockBuffer_t* block_buffer,
    PEthOffests_t tx_offsets
)
{
    _tx_service = tx_service;
    _block_buffer = block_buffer;
    _tx_offsets = tx_offsets;
    _short_ids.clear();
    _output_buffer.reset();
    _content_size = 0;
}

const utils::common::ByteArray&
EthBlockCompressionSubTask::output_buffer() {
    assert_execution();
    return _output_buffer;
}

const std::vector<unsigned int>&
EthBlockCompressionSubTask::short_ids() {
    assert_execution();
    return _short_ids;
}

size_t EthBlockCompressionSubTask::content_size() const {
    return _content_size;
}

void EthBlockCompressionSubTask::_execute()  {
    size_t output_offset = 0;
    ByteArray_t tx_content_prefix = ByteArray_t();

    for (auto& offsets : *_tx_offsets) {
        size_t from = std::get<0>(offsets);
        size_t offset = std::get<1>(offsets);

        uint64_t tx_item_len;
        size_t tx_item_offset = utils::encoding::consume_length_prefix(*_block_buffer, tx_item_len, from);

        utils::crypto::Sha256 tx_hash = utils::crypto::Sha256(
            utils::crypto::keccak_sha3(
                (uint8_t*)_block_buffer->char_array(), from, tx_item_offset + tx_item_len - from
            )
        );

        std::vector<uint8_t> is_full_tx_bytes;
        size_t tx_content_bytes_len = 0, tx_content_prefix_offset;

        if ( ! _tx_service->has_short_id(tx_hash) ) {
            is_full_tx_bytes.push_back(ETH_FULL_TX_INDICATOR);
            tx_content_bytes_len = offset - from;
        } else {
            is_full_tx_bytes.push_back(ETH_SHORT_ID_INDICATOR);
            _short_ids.push_back(_tx_service->get_short_id(tx_hash));

        }

        tx_content_prefix_offset = utils::encoding::get_length_prefix_str(
            tx_content_prefix, tx_content_bytes_len, 0
        );

        size_t short_tx_content_size = is_full_tx_bytes.size() + tx_content_prefix_offset + tx_content_bytes_len;

        size_t short_tx_content_prefix_bytes_offset = utils::encoding::get_length_prefix_list(
            _output_buffer, short_tx_content_size, output_offset
        );

        size_t short_tx_content_prefix_bytes_len = short_tx_content_prefix_bytes_offset - output_offset;
        output_offset = short_tx_content_prefix_bytes_offset;
        output_offset = _output_buffer.copy_from_array(
            is_full_tx_bytes, output_offset, 0, is_full_tx_bytes.size()
        );
        output_offset = _output_buffer.copy_from_array(
            tx_content_prefix.array(),
            output_offset,
            0,
            tx_content_prefix_offset
        );
        if (tx_content_bytes_len > 0) {
            output_offset = _output_buffer.copy_from_buffer(
                *_block_buffer,
                output_offset,
                from,
                offset - from
            );
        }

        _content_size += (short_tx_content_prefix_bytes_len + short_tx_content_size);
    }

    _tx_service = nullptr;
    _tx_offsets = nullptr;
    _block_buffer = nullptr;
}

}
