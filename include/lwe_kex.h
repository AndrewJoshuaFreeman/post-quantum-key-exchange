#pragma once
#include <array>
#include <cstdint>
#include <optional>
#include <vector>
#include "matrix.h"
#include "modq.h"
#include "params.h"
#include "prng.h"

namespace pq {

struct PublicKey final {
    Mat A;
    Vec b;
};

struct SecretKey final {
    Vec s;
};

struct Ciphertext final {
    Vec u;
    std::vector<uint8_t> v;
};

struct SessionResult final {
    std::array<uint8_t, 32> key{};
    bool ok{ false };
};

class LweKex final {
public:
    explicit LweKex( LweParams p );

    std::pair<PublicKey, SecretKey> keygen( Prng& rng ) const;

    std::pair<Ciphertext, std::array<uint8_t, 32>>
    encapsulate( const PublicKey& pk, Prng& rng ) const;

    SessionResult decapsulate(
        const Ciphertext& ct,
        const SecretKey& sk,
        const PublicKey& pk
    ) const;

private:
    LweParams p_;
    ModQ mod_;

    int encode_bit( int bit ) const noexcept;
    int decode_bit( int x ) const noexcept;

    static void pack_u16_le( std::vector<uint8_t>& out, uint16_t x );
    static std::optional<uint16_t> unpack_u16_le(
        const std::vector<uint8_t>& in,
        size_t off
    );
};

} // namespace pq

