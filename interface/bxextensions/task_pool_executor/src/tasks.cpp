#include "src/tasks.h"
#include <tpe/task/test_task.h>
#include <tpe/task/w2_task.h>
#include "src/task/eth_block_compression_task.h"
#include "src/task/btc_block_compression_task.h"
#include "src/task/btc_block_decompression_task.h"
#include "src/task/ont_block_compression_task.h"
#include "src/task/ont_block_decompression_task.h"
#include "src/task/ont_consensus_block_compression_task.h"
#include "src/task/ont_consensus_block_decompression_task.h"
#include "src/task/btc_compact_block_mapping_task.h"
#include "src/task/btc_compact_block_compression_task.h"
#include "src/task/btc_block_cleanup_task.h"
#include "src/task/ont_block_cleanup_task.h"
#include "src/task/task_base.h"
#include "src/task/decryption_task.h"
#include "src/task/encryption_task.h"
#include "src/task/task_pool_executor.h"
#include "src/task/block_confirmation_cleanup_task.h"


static void bind_test_task(py::module& m) {
  py::class_<task::TestTask,
    task::MainTaskBase,
    std::shared_ptr<task::TestTask>>(m, "TestTask")
      .def(py::init<>())
      .def("t1", &task::TestTask::t1)
      .def("t2", &task::TestTask::t2)
      .def("t3", &task::TestTask::t3)
      .def("init", &task::TestTask::init);
}

static void bind_w2_task(py::module& m) {
  py::class_<task::W2Task,
    task::MainTaskBase,
    std::shared_ptr<task::W2Task>>(m, "W2Task")
      .def(py::init<>())
      .def("init", &task::W2Task::init);
}

void bind_tasks(py::module& m) {
    bind_task_base(m);
    bind_main_task_base(m);
    bind_encryption_task(m);
    bind_decryption_task(m);
    bind_eth_block_compression_task(m);
    bind_btc_block_compression_task(m);
    bind_btc_block_decompression_task(m);
    bind_ont_block_compression_task(m);
    bind_ont_block_decompression_task(m);
    bind_ont_consensus_block_compression_task(m);
    bind_ont_consensus_block_decompression_task(m);
    bind_btc_compact_block_compression_task(m);
    bind_btc_compact_block_mapping_task(m);
    bind_btc_block_cleanup_task(m);
    bind_ont_block_cleanup_task(m);
    bind_block_confirmation_cleanup_task(m);
    bind_tpe(m);
#ifdef BUILD_TYPE
	if (BUILD_TYPE == "TESTING") {
		  bind_test_task(m);
		  bind_w2_task(m);
	}
#endif
}
