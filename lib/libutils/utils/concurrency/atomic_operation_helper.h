#include <atomic>

#ifndef UTILS_CONCURRENCY_ATOMIC_OPERATION_HELPER_H_
#define UTILS_CONCURRENCY_ATOMIC_OPERATION_HELPER_H_

namespace utils {
namespace concurrency {

template<typename T>
bool compare_and_swap(T& src, T& expected, T& desired) {
	return __atomic_compare_exchange(
			&src,
			&expected,
			&desired,
			false,
			std::memory_order_seq_cst,
			std::memory_order_seq_cst
	);
}

} // concurrency
} // utils

#endif /* UTILS_CONCURRENCY_ATOMIC_OPERATION_HELPER_H_ */
