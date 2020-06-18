from typing import List, Union, Dict, Tuple, Set

__version__: str


class Sha256:
    def __init__(self, input_bytes: InputBytes): ...

    def binary(self) -> bytearray: ...

    def double_sha256(self) -> None: ...

    def hex_string(self) -> str: ...


class UIntList:
    def __init__(self, py_list: List[int]): ...


class InputBytes:
    def __init__(self, py_bytes: Union[bytes, bytearray, memoryview]): ...


class TaskPoolExecutor:
    def init(self, pool_size: int): ...

    def size(self) -> int: ...

    def enqueue_task(self, task: TaskBase): ...


class TaskBase:
    def is_completed(self) -> bool: ...

    def assert_execution(self) -> None: ...

    def get_id(self) -> int: ...


class MainTaskBase(TaskBase):
    def execute(self) -> None: ...

    def get_task_byte_size(self) -> int: ...

    def cleanup(self) -> None: ...


class _BlockCompressionTask(MainTaskBase):
    def __init__(self, capacity: int, min_transaction_count: int): ...

    def init(
            self, block_bytes: InputBytes, transaction_service: TransactionService
    ) -> None: ...

    def bx_block(self) -> bytearray: ...

    def prev_block_hash(self) -> Sha256: ...

    def block_hash(self) -> Sha256: ...

    def compressed_block_hash(self) -> Sha256: ...

    def txn_count(self) -> int: ...

    def short_ids(self) -> List[int]: ...


class _BlockDecompressionTask(MainTaskBase):
    def __init__(self, capacity: int, min_transaction_count: int): ...

    def init(
            self, block_bytes: InputBytes, transaction_service: TransactionService
    ) -> None: ...

    def block_message(self) -> bytearray: ...

    def block_hash(self) -> Sha256: ...

    def tx_count(self) -> int: ...

    def success(self) -> bool: ...

    def short_ids(self) -> List[int]: ...

    def unknown_tx_hashes(self) -> List[Sha256]: ...

    def unknown_tx_sids(self) -> List[int]: ...


class BtcBlockCompressionTask(_BlockCompressionTask):
    ...


class BtcBlockDecompressionTask(_BlockDecompressionTask):
    ...


class BtcCompactBlockMappingTask(MainTaskBase):
    def __init__(self, capacity: int): ...

    def init(
            self, block_bytes: InputBytes, transaction_service: TransactionService
    ) -> None: ...

    def success(self) -> bool: ...

    def compression_task(self) -> BtcCompactBlockCompressionTask: ...

    def missing_indices(self) -> List[int]: ...


class BtcCompactBlockCompressionTask(MainTaskBase):
    def bx_block(self) -> bytearray: ...

    def prev_block_hash(self) -> Sha256: ...

    def block_hash(self) -> Sha256: ...

    def compressed_block_hash(self) -> Sha256: ...

    def txn_count(self) -> int: ...

    def short_ids(self) -> List[int]: ...

    def add_recovered_transactions(
            self, recovered_transactions: TransactionList
    ) -> None: ...


TransactionList = List[InputBytes]


class OntBlockCompressionTask(_BlockCompressionTask):
    ...


class OntBlockDecompressionTask(_BlockDecompressionTask):
    ...


class OntConsensusBlockCompressionTask(_BlockCompressionTask):
    ...


class OntConsensusBlockDecompressionTask(_BlockDecompressionTask):
    ...


class BlockConfirmationCleanupTask(MainTaskBase):
    def init(
            self, message_buffer: InputBytes, transaction_service: TransactionService
    ) -> None: ...

    def short_ids(self) -> List[int]: ...

    def tx_count(self) -> int: ...

    def block_hash(self) -> Sha256: ...

    def total_content_removed(self) -> int: ...


class _BlockCleanupTask(MainTaskBase):
    def __init__(self, capacity: int): ...

    def init(
            self, message_buffer: InputBytes, transaction_service: TransactionService
    ) -> None: ...

    def short_ids(self) -> List[int]: ...

    def txn_count(self) -> int: ...

    def unknown_tx_hashes(self) -> List[Sha256]: ...

    def total_content_removed(self) -> int: ...


class BtcBlockCleanupTask(_BlockCleanupTask):
    ...


class OntBlockCleanupTask(_BlockCleanupTask):
    ...


class TxProcessingResult:
    def get_tx_status(self) -> int: ...

    def get_contents_set(self) -> bool: ...

    def get_set_transaction_contents_result(self) -> Tuple[bool, int]: ...

    def get_short_id_assigned(self) -> bool: ...

    def get_assign_short_id_result(self) -> bool: ...

    def get_existing_short_ids(self) -> List[int]: ...


class TxFromBdnProcessingResult:
    def get_ignore_seen(self) -> bool: ...

    def get_existing_short_id(self) -> bool: ...

    def get_assigned_short_id(self) -> bool: ...

    def get_existing_contents(self) -> bool: ...

    def get_set_contents(self) -> bool: ...


class TransactionService:
    def __init__(
            self, pool_size: int, tx_bucket_capacity: int, final_tx_confirmation_count: int
    ): ...

    def tx_hash_to_short_ids(self) -> Dict[Sha256, List[int]]: ...

    def get_tx_sync_buffer(
            self, all_txs_content_size: int, sync_tx_content: bool
    ) -> bytearray: ...

    def short_id_to_tx_hash(self) -> Dict[int, Sha256]: ...

    def track_seen_transaction(self, transaction_hash: Sha256) -> None: ...

    def remove_transaction_by_hash(self, transaction_hash: Sha256) -> int: ...

    def track_seen_short_ids(
            self, block_hash: Sha256, short_ids: UIntList
    ) -> Tuple[int, List[int]]: ...

    def on_block_cleaned_up(self, block_hash: Sha256) -> None: ...

    def set_final_tx_confirmations_count(self, confirmation_count: int) -> None: ...

    def tx_hash_to_contents(self) -> Dict[Sha256, bytearray]: ...

    def clear_short_ids_seen_in_block(self) -> None: ...

    def tx_not_seen_in_blocks(self) -> Set[Sha256]: ...

    def tx_hash_to_time_removed(self) -> Dict[Sha256, float]: ...

    def tx_hash_to_time_removed(self) -> Dict[Sha256, float]: ...

    def process_transaction_msg(self,
                                transaction_cache_key: Sha256,
                                transaction_contents: InputBytes,
                                network_num: int,
                                short_id: int,
                                timestamp: int,
                                current_time: int) -> TxProcessingResult: ...

    def process_gateway_transaction_from_bdn(self,
                                             transaction_hash: Sha256,
                                             transaction_contents: InputBytes,
                                             short_id: int,
                                             is_compact: bool
                                             ) -> TxFromBdnProcessingResult: ...

    def process_gateway_transaction_from_node(self,
                                              protocol: str,
                                              transaction_msg_contents: InputBytes
                                              ) -> bytearray: ...

    def assign_short_id(self, transaction_hash: Sha256, short_id: int) -> bool: ...

    def set_transaction_contents(self, transaction_hash: Sha256, transaction_contents: InputBytes) -> Tuple[
        bool, int]: ...


class AggregatedException(Exception):
    ...
