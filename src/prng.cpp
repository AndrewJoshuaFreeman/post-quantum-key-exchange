#include "prng.h"
#include <chrono>

namespace pq {

Prng::Prng() {
    auto t{
        static_cast<uint64_t>(
            std::chrono::high_resolution_clock::now()
                .time_since_epoch()
                .count()
        )
    };
    std::seed_seq seq{
        static_cast<uint32_t>( t ),
        static_cast<uint32_t>( t >> 32 )
    };
    eng_.seed( seq );
}

Prng::Prng( uint64_t seed ) : eng_{ seed } {}

uint32_t Prng::u32() {
    return static_cast<uint32_t>( eng_() );
}

int Prng::uniform_int( int lo, int hi ) {
    std::uniform_int_distribution<int> dist{ lo, hi };
    return dist( eng_ );
}

std::vector<uint8_t> Prng::bytes( size_t n ) {
    std::vector<uint8_t> out( n );
    for ( size_t i = 0; i < n; ++i ) {
        out[ i ] = static_cast<uint8_t>( u32() & 0xffu );
    }
    return out;
}

} // namespace pq

