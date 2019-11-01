#include "src/task/main_task_base.h"
#include "src/task/btc_block_cleanup_task.h"

#include <utils/crypto/hash_helper.h>


void bind_btc_block_cleanup_task(py::module& m) {
    py::class_<
            BtcBlockCleanupTask_t,
            MainTaskBase_t,
            PBtcBlockCleanupTask_t>(m, "BtcBlockCleanupTask")
            .def(py::init<size_t>(),
                 "initializing",
                 py::arg("minimal_tx_count") = BTC_DEFAULT_MINIMAL_SUB_TASK_TX_COUNT
            )
            .def(
                    "short_ids",
                    &BtcBlockCleanupTask_t::short_ids
            )
            .def(
                    "tx_count",
                    &BtcBlockCleanupTask_t::tx_count
            )
            .def(
                    "unknown_tx_hashes",
                    &BtcBlockCleanupTask_t::unknown_tx_hashes
            )
            .def(
                    "total_content_removed",
                    &BtcBlockCleanupTask_t::total_content_removed
            )
            .def("init", &BtcBlockCleanupTask_t::init);
}
