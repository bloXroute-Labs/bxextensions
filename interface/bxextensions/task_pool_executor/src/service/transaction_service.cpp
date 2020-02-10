#include <memory>
#include "src/service/transaction_service.h"

#include <tpe/service/transaction_service.h>

typedef service::TransactionService TransactionService_t;
typedef std::shared_ptr<TransactionService_t> PTransactionService_t;
typedef service::Sha256ToShortIDsMap_t Sha256ToShortIDsMap_t;
typedef std::unordered_set<unsigned int> ShortIDs_t;
typedef std::shared_ptr<ShortIDs_t> PShortIDs_t;
typedef service::Sha256_t Sha256_t;
typedef service::TxNotSeenInBlocks_t TxNotSeenInBlocks_t;
typedef service::Sha256ToContentMap_t Sha256ToContentMap_t;
typedef service::PTxContents_t PTxContents_t;


void bind_transaction_service(py::module& m) {
	py::class_<ShortIDs_t, PShortIDs_t>(m, "UIntSet")
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
			.def("__getitem__",
			        [](
			        		Sha256ToShortIDsMap_t& map,
							const Sha256_t& sha
			        ) -> ShortIDs_t& {
			            return map[sha];
			        },
			        py::return_value_policy::reference_internal
			)
			.def("__len__", &Sha256ToShortIDsMap_t::size)
			.def("__iter__", [](Sha256ToShortIDsMap_t& map) {
					return py::make_key_iterator(map.begin(), map.end());
				},
				py::keep_alive<0, 1>()
			)
			.def("__delitem__", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) {
					map.erase(key);
				}
			)
			.def("pop", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) -> ShortIDs_t {
			    auto val = std::move(map.at(key));
			    map.erase(key);
                return std::move(val);
			})
            .def("clear", &Sha256ToShortIDsMap_t::clear)
            .def("get_bytes_length" , [](Sha256ToShortIDsMap_t& col) {
                return sizeof(Sha256ToShortIDsMap_t) + col.get_allocator().total_bytes_allocated();
            })
			.def("__contains__", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) {
					auto iter = map.find(key);
					return iter != map.end();
			});

    py::class_<Sha256ToContentMap_t>(m, "Sha256ToContentMap")
            .def("__getitem__",
                 [](
                         Sha256ToContentMap_t& map,
                         const Sha256_t& sha
                 ) -> PTxContents_t& {
                     return map[sha];
                 },
                 py::return_value_policy::reference_internal
            )
            .def("__len__", &Sha256ToContentMap_t::size)
            .def("__iter__", [](Sha256ToContentMap_t& map) {
                     return py::make_key_iterator(map.begin(), map.end());
                 },
                 py::keep_alive<0, 1>()
            )
            .def("__delitem__", [](Sha256ToContentMap_t& map, const Sha256_t& key) {
                     map.erase(key);
                 }
            )
            .def("__setitem__", [](Sha256ToContentMap_t& map, const Sha256_t& key, PTxContents_t p_contents) {
                    auto iter = map.find(key);
                    if (iter != map.end()) {
                        iter->second = std::move(p_contents);
                    } else {
                        map.emplace(key, std::move(p_contents));
                    }
                 }
            )
            .def("clear", &Sha256ToContentMap_t::clear)
            .def("get_bytes_length" , [](Sha256ToContentMap_t& col) {
                return sizeof(Sha256ToContentMap_t) +
                    col.get_allocator().total_bytes_allocated() +
                    col.get_value_tracker().total_bytes_allocated();
            })
            .def("__contains__", [](Sha256ToContentMap_t& map, const Sha256_t& key) {
                auto iter = map.find(key);
                return iter != map.end();
            });

    py::class_<TxNotSeenInBlocks_t>(m, "TxNotSeenInBlocks")
            .def("__len__", &TxNotSeenInBlocks_t::size)
            .def("get_bytes_length" , [](TxNotSeenInBlocks_t& col) {
                return sizeof(TxNotSeenInBlocks_t) + (sizeof(TxNotSeenInBlocks_t::Bucket_t) * col.bucket_count()) +
                        (sizeof(Sha256_t) * col.size());
            });

	py::class_<TransactionService_t, PTransactionService_t>(m, "TransactionService")
			.def(
			        py::init<size_t, size_t, size_t>(),
			        py::arg("pool_size"),
                    py::arg("tx_bucket_capacity") = BTC_DEFAULT_TX_BUCKET_SIZE,
                    py::arg("final_tx_confirmations_count") = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT
            )
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
                    "remove_transaction_by_hash",
                    &TransactionService_t::remove_transaction_by_hash
            )
			.def(
					"track_seen_short_ids",
                    &TransactionService_t::track_seen_short_ids
			)
            .def(
                    "on_block_cleaned_up",
                    &TransactionService_t::on_block_cleaned_up
            )
            .def(
                    "set_final_tx_confirmations_count",
                    &TransactionService_t::set_final_tx_confirmations_count
            )
			.def(
					"tx_hash_to_contents",
					&TransactionService_t::get_tx_hash_to_contents,
					py::return_value_policy::reference
			).
			def(
                    "clear_short_ids_seen_in_block",
                    &TransactionService_t::clear_short_ids_seen_in_block
            ).
			def(
                    "tx_not_seen_in_blocks",
                    &TransactionService_t::tx_not_seen_in_blocks,
                    py::return_value_policy::reference
            ).def(
					"removed_tx_hashes_time",
					&TransactionService_t::removed_tx_hashes_time,
					py::return_value_policy::reference
			);
}
