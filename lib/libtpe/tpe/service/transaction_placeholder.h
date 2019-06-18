#include <iostream>
#include <memory>

#include <utils/crypto/compact_short_id.h>

#ifndef TPE_SERVICE_TRANSACTION_PLACE_HOLDER_H_
#define TPE_SERVICE_TRANSACTION_PLACE_HOLDER_H_

namespace service {

typedef std::shared_ptr<utils::common::BufferView> PTxContents_t;
typedef std::shared_ptr<utils::crypto::CompactShortId> PCompactShortId_t;

enum class TransactionPlaceholderType {
	short_id,
	missing_compact_id,
	content
};

struct TransactionPlaceholder {
	TransactionPlaceholder():
		type(TransactionPlaceholderType::missing_compact_id),
		cmpt(nullptr),
		contents(nullptr),
		sid(0)
	{
	}

	TransactionPlaceholderType type;
	PCompactShortId_t cmpt;
	PTxContents_t contents;
	uint32_t sid;
};

} // service

#endif /* TPE_SERVICE_TRANSACTION_PLACE_HOLDER_H_ */
