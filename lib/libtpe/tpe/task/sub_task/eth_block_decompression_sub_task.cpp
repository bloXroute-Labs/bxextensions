#include <utils/common/buffer_helper.h>
#include <utils/common/buffer_view.h>
#include "tpe/task/sub_task/eth_block_decompression_sub_task.h"
#include <utils/encoding/rlp_encoding_type.h>
#include <iomanip>


namespace task {
typedef utils::common::ByteArray ByteArray_t;

EthBlockDecompressionSubTask::EthBlockDecompressionSubTask():
    SubTaskBase(),
    _block_buffer(nullptr),
    _tx_offsets(nullptr),
    _tx_service(nullptr),
    _short_ids(nullptr),
    _content_size(0)
{
}

void EthBlockDecompressionSubTask::init(
    PTransactionService_t tx_service,
    const BlockBuffer_t* block_buffer,
    utils::common::ByteArray* output_buffer,
    PEthOffests_t tx_offsets,
    const ShortIDs_t* short_ids
)
{
    _tx_service = tx_service;
    _block_buffer = block_buffer;
    _tx_offsets = tx_offsets;
    _short_ids = short_ids;
    _output_buffer = output_buffer;
    _content_size = 0;
    _unknown_tx_hashes.clear();
    _unknown_tx_sids.clear();
}

const ShortIDs_t* EthBlockDecompressionSubTask::short_ids() {
    assert_execution();
    return _short_ids;
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
    size_t output_offset = _task_data.output_offset;
    auto short_id_offset = _task_data.short_ids_offset;

    for (auto& offsets : *_tx_offsets) {
        size_t tx_content_offset = std::get<0>(offsets);
        size_t is_short = std::get<1>(offsets);
        size_t tx_content_len = std::get<2>(offsets);
        const service::TxContents_t* p_contents = nullptr;

        if ( ! is_short ) {
            if (tx_content_len > 0) {
                output_offset = _output_buffer->copy_from_buffer(
                    *_block_buffer, output_offset, tx_content_offset, tx_content_len
                );
            }
        } else {
            unsigned int short_id = _short_ids->at(short_id_offset);
            p_contents = _tx_service->get_tx_contents_raw_ptr(short_id);
            ++short_id_offset;

            if (p_contents->size() > 0) {
                output_offset = _output_buffer->copy_from_buffer(
                    *p_contents, output_offset, 0, p_contents->size()
                );
            }
        }
    }

    _tx_service = nullptr;
    _tx_offsets = nullptr;
    _block_buffer = nullptr;
    _short_ids = nullptr;
}

}
