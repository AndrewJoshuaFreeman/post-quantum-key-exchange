#pragma once
#include <cstdint>

namespace pq {

struct ModQ final {
    int q{};

    explicit ModQ( int qq );

    int norm( int64_t x ) const noexcept;
    int add( int a, int b ) const noexcept;
    int sub( int a, int b ) const noexcept;
    int mul( int a, int b ) const noexcept;
};

} // namespace pq

