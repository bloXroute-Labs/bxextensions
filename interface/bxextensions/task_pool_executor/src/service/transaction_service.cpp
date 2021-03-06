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
typedef service::Sha256ToTime_t Sha256ToTime_t;
typedef service::ShortIdToTime_t ShortIdToTime_t;
typedef service::PTxContents_t PTxContents_t;
typedef service::PTxSyncTxs_t PTxSyncTxs_t;
typedef service::TxProcessingResult_t TxProcessingResult_t;
typedef service::PTxProcessingResult_t PTxProcessingResult_t;
typedef service::TxFromBdnProcessingResult_t TxFromBdnProcessingResult_t;
typedef service::PTxFromBdnProcessingResult_t PTxFromBdnProcessingResult_t;


static void cleanup_removed_hashes_history(
    Sha256ToTime_t& map, double now, double removed_hashes_expiration_time_s, size_t max_items_count
)
{
    auto hash_iter = map.begin();
    while (
        hash_iter != map.end() and
        (now - hash_iter->second > removed_hashes_expiration_time_s or map.size() > max_items_count)) {

        map.erase(hash_iter->first);
        hash_iter = map.begin();
    }
}


static void cleanup_removed_short_ids_history(
    ShortIdToTime_t& map, double now, double removed_short_ids_expiration_time_s, size_t max_items_count
)
{
    auto hash_iter = map.begin();
    while (
        hash_iter != map.end() and
        (now - hash_iter->second > removed_short_ids_expiration_time_s or map.size() > max_items_count)) {

        map.erase(hash_iter->first);
        hash_iter = map.begin();
    }
}


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
                return val;
			})
            .def("clear", &Sha256ToShortIDsMap_t::clear)
            .def("get_bytes_length" , [](Sha256ToShortIDsMap_t& col) {
                return sizeof(Sha256ToShortIDsMap_t) + col.get_allocator().total_bytes_allocated();
            })
			.def("__contains__", [](Sha256ToShortIDsMap_t& map, const Sha256_t& key) {
					auto iter = map.find(key);
					return iter != map.end();
			});

	py::class_<Sha256ToTime_t>(m, "Sha256ToTime")
			.def("__getitem__",
			        [](
			        		Sha256ToTime_t& map,
							const Sha256_t& sha
			        ) -> double {
			            return map[sha];
			        }
			)
			.def("__len__", &Sha256ToTime_t::size)
			.def("__iter__", [](Sha256ToTime_t& map) {
					return py::make_key_iterator(map.begin(), map.end());
				},
				py::keep_alive<0, 1>()
			)
			.def("__delitem__", [](Sha256ToTime_t& map, const Sha256_t& key) {
					map.erase(key);
				}
			)
			.def("__setitem__", [](Sha256ToTime_t& map, const Sha256_t& key, double val) {
                    auto iter = map.find(key);
                    if (iter != map.end()) {
                        iter->second = val;
                    } else {
                        map.emplace(key, val);
                    }
                 }
            )
			.def("pop", [](Sha256ToTime_t& map, const Sha256_t& key) -> double {
			    double val = map[key];
			    map.erase(key);
                return val;
			})
            .def("clear", &Sha256ToTime_t::clear)
            .def("get_bytes_length" , [](Sha256ToTime_t& col) {
                return sizeof(Sha256ToTime_t) + col.get_allocator().total_bytes_allocated();
            })
            .def("cleanup_removed_hashes_history", &cleanup_removed_hashes_history)
			.def("__contains__", [](Sha256ToTime_t& map, const Sha256_t& key) {
					auto iter = map.find(key);
					return iter != map.end();
			});

	py::class_<ShortIdToTime_t>(m, "ShortIdToTime")
			.def("__getitem__",
			        [](
			        		ShortIdToTime_t& map,
							const uint32_t& short_id
			        ) -> double {
			            return map[short_id];
			        }
			)
			.def("__len__", &ShortIdToTime_t::size)
			.def("__iter__", [](ShortIdToTime_t& map) {
					return py::make_key_iterator(map.begin(), map.end());
				},
				py::keep_alive<0, 1>()
			)
			.def("__delitem__", [](ShortIdToTime_t& map, const uint32_t& key) {
					map.erase(key);
				}
			)
			.def("__setitem__", [](ShortIdToTime_t& map, const uint32_t& key, double val) {
                    auto iter = map.find(key);
                    if (iter != map.end()) {
                        iter->second = val;
                    } else {
                        map.emplace(key, val);
                    }
                 }
            )
			.def("pop", [](ShortIdToTime_t& map, const uint32_t& key) -> double {
			    double val = map[key];
			    map.erase(key);
                return val;
			})
            .def("clear", &ShortIdToTime_t::clear)
            .def("get_bytes_length" , [](ShortIdToTime_t& map) {
                return map.size() * sizeof(uint32_t);
            })
            .def("cleanup_removed_short_ids_history", &cleanup_removed_short_ids_history)
			.def("__contains__", [](ShortIdToTime_t& map, const uint32_t& key) {
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
			        py::init<size_t, std::string, size_t, size_t>(),
			        py::arg("pool_size"),
                    py::arg("protocol"),
                    py::arg("tx_bucket_capacity") = BTC_DEFAULT_TX_BUCKET_SIZE,
                    py::arg("final_tx_confirmations_count") = DEFAULT_FINAL_TX_CONFIRMATIONS_COUNT
            )
			.def(
					"tx_hash_to_short_ids",
					&TransactionService_t::get_tx_hash_to_short_ids,
					py::return_value_policy::reference
			)
			.def(
					"get_tx_sync_buffer",
					&TransactionService_t::get_tx_sync_buffer
			)
			.def(
					"assign_short_id",
					&TransactionService_t::assign_short_id
			)
			.def(
					"set_transaction_contents",
					&TransactionService_t::set_transaction_contents
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
                    "clear",
                    &TransactionService_t::clear
            ).
			def(
                    "tx_not_seen_in_blocks",
                    &TransactionService_t::tx_not_seen_in_blocks,
                    py::return_value_policy::reference
            ).def(
					"tx_hash_to_time_removed",
					&TransactionService_t::tx_hash_to_time_removed,
					py::return_value_policy::reference
            ).def(
					"short_id_to_time_removed",
					&TransactionService_t::short_id_to_time_removed,
					py::return_value_policy::reference
			).def(
					"process_transaction_msg",
					&TransactionService_t::process_transaction_msg,
					py::return_value_policy::reference
			).def(
					"process_gateway_transaction_from_bdn",
					&TransactionService_t::process_gateway_transaction_from_bdn,
					py::return_value_policy::reference
			).def(
					"process_gateway_transaction_from_node",
					&TransactionService_t::process_gateway_transaction_from_node,
					py::return_value_policy::reference
			).def(
                    "get_transactions_by_short_ids",
                    &TransactionService_t::get_transactions_by_short_ids,
                    py::return_value_policy::reference
			).def(
                    "process_txs_msg",
                    &TransactionService_t::process_txs_msg,
                    py::return_value_policy::reference
            ).def(
                    "process_tx_sync_message",
                    &TransactionService_t::process_tx_sync_message,
                    py::return_value_policy::reference
            ).def(
                    "clear_sender_nonce",
                    &TransactionService_t::clear_sender_nonce,
                    py::return_value_policy::reference
            ).def(
                    "set_sender_nonce_reuse_setting",
                    &TransactionService_t::set_sender_nonce_reuse_setting,
                    py::return_value_policy::reference
            );

	py::class_<TxProcessingResult_t, PTxProcessingResult_t>(m, "TxProcessingResult")
			.def(
					"get_tx_status",
					&TxProcessingResult_t::get_tx_status,
					py::return_value_policy::reference
			).def(
                "get_tx_validation_status",
                &TxProcessingResult_t::get_tx_validation_status,
                py::return_value_policy::reference
            ).def(
					"get_existing_short_ids",
					&TxProcessingResult_t::get_existing_short_ids,
					py::return_value_policy::reference
			).def(
					"get_short_id_assigned",
					&TxProcessingResult_t::get_short_id_assigned,
					py::return_value_policy::reference
			).def(
					"get_assign_short_id_result",
					&TxProcessingResult_t::get_assign_short_id_result,
					py::return_value_policy::reference
			).def(
					"get_contents_set",
					&TxProcessingResult_t::get_contents_set,
					py::return_value_policy::reference
			).def(
					"get_set_transaction_contents_result",
					&TxProcessingResult_t::get_set_transaction_contents_result,
					py::return_value_policy::reference
			);

	py::class_<TxFromBdnProcessingResult_t, PTxFromBdnProcessingResult_t>(m, "TxFromBdnProcessingResult")
			.def(
					"get_ignore_seen",
					&TxFromBdnProcessingResult_t::get_ignore_seen,
					py::return_value_policy::reference
			)
			.def(
					"get_existing_short_id",
					&TxFromBdnProcessingResult_t::get_existing_short_id,
					py::return_value_policy::reference
			)
			.def(
					"get_assigned_short_id",
					&TxFromBdnProcessingResult_t::get_assigned_short_id,
					py::return_value_policy::reference
			)
			.def(
					"get_existing_contents",
					&TxFromBdnProcessingResult_t::get_existing_contents,
					py::return_value_policy::reference
			)
			.def(
					"get_set_contents",
					&TxFromBdnProcessingResult_t::get_set_contents,
					py::return_value_policy::reference
			).def(
                    "get_set_contents_result",
                    &TxFromBdnProcessingResult_t::get_set_contents_result,
                    py::return_value_policy::reference
            );
}
