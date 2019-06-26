#include <memory>
#include "src/service/transaction_service.h"

#include <tpe/service/transaction_service.h>

typedef service::TransactionService TransactionService_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
typedef service::Sha256ToShortIDsMap_t Sha256ToShortIDsMap_t;
typedef std::unordered_set<unsigned int> ShortIDs_t;
typedef service::Sha256_t Sha256_t;


void bind_transaction_service(py::module& m) {
	py::class_<ShortIDs_t>(m, "UIntSet")
			.def(py::init<>())
			.def("clear", &ShortIDs_t::clear)
			.def("add", [](ShortIDs_t& set, unsigned int val) {
					set.insert(val);
				}
			)
			.def("__len__", &ShortIDs_t::size)
			.def("__iter__", [](ShortIDs_t& set) {
					return py::make_iterator(set.begin(), set.end());
				},
				py::keep_alive<0, 1>()
			)
			.def("remove", [](ShortIDs_t& set, unsigned int val) {
					set.erase(val);
				}
			);
	py::class_<Sha256ToShortIDsMap_t>(m, "Sha256ToShortIDsMap")
			.def(py::init<>())
			.def("__getitem__",
			        [](
			        		Sha256ToShortIDsMap_t& map,
							const Sha256_t& sha
			        ) -> std::unordered_set<unsigned int>& {
			            return map[sha];
			        },
			        py::return_value_policy::reference_internal
			)
			.def("__len__", &Sha256ToShortIDsMap_t::size)
			.def("__iter__", [](Sha256ToShortIDsMap_t& map) {
					return py::make_iterator(map.begin(), map.end());
				},
				py::keep_alive<0, 1>()
			)
			.def("__delitem__", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) {
					map.erase(key);
				}
			)
			.def("__contains__", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) {
					auto iter = map.find(key);
					return iter != map.end();
			});
	py::class_<TransactionService_t, PTransactionService_t>(m, "TransactionService")
			.def(
			        py::init<size_t, size_t, size_t>(),
			        py::arg("pool_size"),
                    py::arg("tx_bucket_capacity") = BTC_DEFAULT_TX_BUCKET_SIZE,
                    py::arg("final_tx_confirmations_count") = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT)
			.def(
					"tx_hash_to_short_ids",
					&TransactionService_t::get_tx_hash_to_short_ids,
					py::return_value_policy::reference
			)
			.def(
					"short_id_to_tx_hash",
					&TransactionService_t::get_short_id_to_tx_hash,
					py::return_value_policy::reference
			)
            .def(
                    "track_seen_transaction",
                    &TransactionService_t::track_seen_transaction
            )
			.def(
					"track_seen_short_ids",
                    &TransactionService_t::track_seen_short_ids
			)
            .def(
                    "set_final_tx_confirmations_count",
                    &TransactionService_t::set_final_tx_confirmations_count
            )
			.def(
					"tx_hash_to_contents",
					&TransactionService_t::get_tx_hash_to_contents,
					py::return_value_policy::reference
			);
}
