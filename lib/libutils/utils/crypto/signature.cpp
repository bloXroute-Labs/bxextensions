#include <assert.h>

extern "C" {
#include <secp256k1_recovery.h>
}

#include "utils/crypto/signature.h"

namespace utils {
namespace crypto {

void Signature::encode_signature(
    const uint64_t v,
    const std::vector<uint8_t>& r,
    const std::vector<uint8_t>& s,
    const char payload_type,
    const uint8_t y_parity
)
{
    uint8_t parity;
    if (payload_type == ETH_TX_TYPE_0) {
        if (v > EIP155_CHAIN_ID_OFFSET) {
            if (v % 2 == 0) {
                parity = V_RANGE_END;
            } else {
                parity = V_RANGE_START;
            }
        } else {
            parity = uint8_t(v - 27);
        }

        if (parity != V_RANGE_START and parity != V_RANGE_END) {
            throw std::runtime_error("v is expected to be int or long in range (27, 28)"); // TODO
        }
    } else if (payload_type <= ETH_TX_TYPE_2) {
        parity = y_parity;
    }

    _signature.resize(SIGNATURE_LEN);
    std::fill(_signature.begin(), _signature.begin() + SIGNATURE_LEN, 0);
    _signature.insert(
        _signature.begin() + MAX_R_SIZE - r.size(), r.begin(), r.end()
    );
    _signature.insert(
        _signature.begin() + MAX_R_SIZE + MAX_T_SIZE - s.size(), s.begin(), s.end()
    );
    _signature[64] = parity;
    _signature.resize(SIGNATURE_LEN);
}


// taken from https://github.com/ethereum/aleth/libsevcrypto/Common.cpp
std::vector<uint8_t> Signature::recover(Sha256 unsigned_msg_hash) {
    int v = _signature[SIGNATURE_LEN - 1];
    if ( v > 3 ) {
        return std::vector<uint8_t>();
    }

    const secp256k1_context* ctx = _get_ctx();
    secp256k1_ecdsa_recoverable_signature raw_signature;
    if (
        ! secp256k1_ecdsa_recoverable_signature_parse_compact(
            ctx,
            &raw_signature,
            _signature.data(),
            v
            )
        )
    {
        return {};
    }

    secp256k1_pubkey raw_public_key;
    if (
            ! secp256k1_ecdsa_recover(
                ctx, &raw_public_key, &raw_signature, (uint8_t const *)unsigned_msg_hash.binary().byte_array()
            )
        ) {
        return {};
    }

    std::array<uint8_t , SIGNATURE_LEN> serialized_public_key;
    size_t serialized_public_key_size = serialized_public_key.size();
    secp256k1_ec_pubkey_serialize(
        ctx,
        serialized_public_key.data(),
        &serialized_public_key_size,
        &raw_public_key,
        SECP256K1_EC_UNCOMPRESSED
    );
    assert(serialized_public_key_size == serialized_public_key.size());
    // Expect single byte header of value 0x04 -- uncompressed public key.
    assert(serialized_public_key[0] == 0x04);
    // Create the Public skipping the header.
    return std::vector<uint8_t>(serialized_public_key.begin() + 1,  serialized_public_key.end());
}

// taken from https://github.com/ethereum/aleth/libsevcrypto/Common.cpp
const secp256k1_context* Signature::_get_ctx()
{
    static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx{
        secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
        &secp256k1_context_destroy
    };
    return s_ctx.get();
}

// taken from https://github.com/ethereum/aleth/libsevcrypto/Common.cpp
bool Signature::verify(const std::vector<uint8_t>& public_key, std::vector<uint8_t> unsigned_msg) {
    if ( public_key.empty() ) {
        return false;
    }

    return public_key == recover(utils::crypto::keccak_sha3(unsigned_msg.data(), 0, unsigned_msg.size()));
}

const std::vector<uint8_t>& Signature::signature() const {
    return _signature;
}

} // crypto
} // utils
