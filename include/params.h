#pragma once
#include <cstdint>
#include <string>

namespace pq {

struct LweParams final {
    int n{};
    int q{};
    int k{};
    int noise_eta{};
};

LweParams params_from_name( const std::string& name );

} // namespace pq

