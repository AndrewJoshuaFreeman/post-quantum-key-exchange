#pragma once
#include <array>
#include <cstddef>
#include <string>
#include "params.h"

namespace pq {

struct BenchReport final {
    double keygen_ms{};
    double enc_ms{};
    double dec_ms{};
    double fail_rate{};
};

BenchReport run_bench(
    const LweParams& p,
    size_t trials,
    uint64_t seed,
    bool verbose
);

std::string hex32( const std::array<uint8_t, 32>& x );

} // namespace pq

