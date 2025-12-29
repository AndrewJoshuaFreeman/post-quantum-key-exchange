#pragma once
#include <cstdint>
#include <random>
#include <vector>

namespace pq {

class Prng final {
public:
    Prng();
    explicit Prng( uint64_t seed );

    uint32_t u32();
    int uniform_int( int lo, int hi );
    std::vector<uint8_t> bytes( size_t n );

private:
    std::mt19937_64 eng_;
};

} // namespace pq

