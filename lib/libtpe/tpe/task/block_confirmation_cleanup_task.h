#include "tpe/task/main_task_base.h"
#include "tpe/service/transaction_service.h"
#include <utils/common/buffer_view.h>

#ifndef TPE_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H_
#define TPE_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H_
namespace task {

typedef utils::common::BufferView BufferView_t;
typedef service::TransactionService TransactionService_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
typedef utils::crypto::Sha256 Sha256_t;

class BlockConfirmationCleanupTask: public MainTaskBase {
public:

    void init(BufferView_t msg_buffer, PTransactionService_t tx_service);

    const Sha256_t& block_hash();

    uint32_t tx_count();
    size_t total_content_removed();
    const std::vector<uint32_t>& short_ids();
    size_t get_task_byte_size() const override;

protected:
    void _execute(SubPool_t& sub_pool) override;

private:

    BufferView_t _msg_buffer;
    PTransactionService_t _tx_service;
    Sha256_t _block_hash;
    size_t _total_content_removed;
    uint32_t _tx_count;
    std::vector<uint32_t> _short_ids;
};

} // task

#endif //TPE_TASK_BLOCK_CONFIRMATION_CLEANUP_TASK_H_
