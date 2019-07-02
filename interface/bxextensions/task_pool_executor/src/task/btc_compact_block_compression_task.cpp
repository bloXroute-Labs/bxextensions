#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "src/task/main_task_base.h"
#include "src/task/btc_compact_block_compression_task.h"

typedef task::RecoveredTransactions_t TransactionList_t;
typedef task::PRecoveredTransactions_t PTransactionList_t;

PYBIND11_MAKE_OPAQUE(TransactionList_t);

void bind_btc_compact_block_compression_task(py::module& m) {
	py::bind_vector<TransactionList_t, PTransactionList_t>(
			m, "TransactionList"
	);
	py::class_<
	BtcCompactBlockCompressionTask_t,
	MainTaskBase_t,
	PBtcCompactBlockCompressionTask_t>(m, "BtcCompactBlockCompressionTask")
	  .def(
			  "add_recovered_transactions",
			  &BtcCompactBlockCompressionTask_t::add_recovered_transactions
	  )
	  .def("bx_block", &BtcCompactBlockCompressionTask_t::bx_block)
	  .def("prev_block_hash", &BtcCompactBlockCompressionTask_t::prev_block_hash)
	  .def("block_hash", &BtcCompactBlockCompressionTask_t::block_hash)
	  .def("compressed_block_hash", &BtcCompactBlockCompressionTask_t::compressed_block_hash)
	  .def("txn_count", &BtcCompactBlockCompressionTask_t::txn_count)
	  .def(
			  "short_ids",
			  &BtcCompactBlockCompressionTask_t::short_ids
	  );
}
