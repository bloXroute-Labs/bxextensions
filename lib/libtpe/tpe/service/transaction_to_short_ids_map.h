#include <iostream>

#include <utils/crypto/sha256.h>

#ifndef TPE_SERVICE_TRANSACTION_TO_SHORT_IDS_MAP_H_
#define TPE_SERVICE_TRANSACTION_TO_SHORT_IDS_MAP_H_

namespace service {

typedef utils::crypto::Sha256BucketContainer_t TxNotSeenInBlocks_t;
typedef std::unordered_set<unsigned int> TxShortIds_t;
typedef utils::crypto::Sha256 Sha256_t;
typedef utils::crypto::Sha256Hasher Sha256Hasher_t;
typedef utils::crypto::Sha256Equal Sha256Equal_t;
typedef utils::common::DefaultMap<Sha256_t, TxShortIds_t, Sha256Hasher_t, Sha256Equal_t> Sha256ToShortIDsMap_t;


class Sha256ToShortIdsMap : public Sha256ToShortIDsMap_t {
public:
	Sha256ToShortIdsMap(TxNotSeenInBlocks_t& tx_not_seen);

	TxShortIds_t& operator[](const Sha256_t& key) override;

	void erase(const Sha256_t& key) override;

	void clear(void) override;
private:
	TxNotSeenInBlocks_t& _tx_not_seen;
};

} // service

#endif /* TPE_SERVICE_TRANSACTION_TO_SHORT_IDS_MAP_H_ */
