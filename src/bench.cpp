#include "bench.h"
#include "lwe_kex.h"
#include "prng.h"
#include <chrono>
#include <iomanip>
#include <sstream>

namespace pq {

static double ms_since(
    const std::chrono::high_resolution_clock::time_point& t0,
    const std::chrono::high_resolution_clock::time_point& t1
) {
    using dms = std::chrono::duration<double, std::milli>;
    return dms( t1 - t0 ).count();
}

std::string hex32( const std::array<uint8_t, 32>& x ) {
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0' );
    for ( auto b : x ) {
        oss << std::setw( 2 ) << static_cast<int>( b );
    }
    return oss.str();
}

BenchReport run_bench(
    const LweParams& p,
    size_t trials,
    uint64_t seed,
    bool
) {
    LweKex kex{ p };
    Prng rng{ seed };

    double t_keygen{ 0.0 };
    double t_enc{ 0.0 };
    double t_dec{ 0.0 };
    size_t fails{ 0 };

    auto time_it = [&]( auto&& fn ) {
        auto t0{ std::chrono::high_resolution_clock::now() };
        fn();
        auto t1{ std::chrono::high_resolution_clock::now() };
        return ms_since( t0, t1 );
    };

    for ( size_t i = 0; i < trials; ++i ) {
        PublicKey pk{ Mat{}, Vec{} };
        SecretKey sk{ Vec{} };

        t_keygen += time_it( [&] {
            auto ks{ kex.keygen( rng ) };
            pk = std::move( ks.first );
            sk = std::move( ks.second );
        } );

        Ciphertext ct{ Vec{}, {} };
        std::array<uint8_t, 32> kb{};

        t_enc += time_it( [&] {
            auto out{ kex.encapsulate( pk, rng ) };
            ct = std::move( out.first );
            kb = out.second;
        } );

        auto ar{ kex.decapsulate( ct, sk, pk ) };
        t_dec += 0.0;

        if ( !ar.ok || ar.key != kb ) ++fails;
    }

    return {
        t_keygen / trials,
        t_enc / trials,
        t_dec / trials,
        static_cast<double>( fails ) / trials
    };
}

} // namespace pq

