#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace pq {

class Sha256 final {
public:
    Sha256();

    void update( const uint8_t* data, size_t n );
    void update( const std::vector<uint8_t>& v );

    std::array<uint8_t, 32> digest();

    static std::array<uint8_t, 32> hash(
        const std::vector<uint8_t>& v
    );

private:
    void compress( const uint8_t block[ 64 ] );

    uint64_t bitlen_{ 0 };
    std::array<uint32_t, 8> h_{};
    std::array<uint8_t, 64> buf_{};
    size_t buf_used_{ 0 };
};

} // namespace pq

