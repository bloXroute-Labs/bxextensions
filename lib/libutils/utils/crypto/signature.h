#include <iostream>
#include <vector>

extern "C" {
#include <secp256k1_recovery.h>
}

#include <utils/crypto/keccak.h>
#include <utils/protocols/ethereum/eth_consts.h>

#ifndef UTILS_CRYPTO__SIGNATURE_H
#define UTILS_CRYPTO__SIGNATURE_H

namespace utils {
namespace crypto {

class Signature {
public:
    void encode_signature(
        uint64_t v, const std::vector<uint8_t>& r, const std::vector<uint8_t>& s
    );
    bool verify(const std::vector<uint8_t>&, std::vector<uint8_t>);
    std::vector<uint8_t> recover(Sha256);
    const std::vector<uint8_t>& signature() const;

protected:
    const secp256k1_context* _get_ctx();

private:
    std::vector<uint8_t> _signature;
};

} // crypto
} // utils

#endif // UTILS_CRYPTO__SIGNATURE_H
