#include "bench.h"
#include "lwe_kex.h"
#include "params.h"
#include "prng.h"
#include <cstdlib>
#include <iostream>
#include <string>

static void usage( const char* argv0 ) {
    std::cout
        << "usage:\n"
        << "  " << argv0 << " demo [toy|small|med|large] [seed]\n"
        << "  " << argv0 << " bench [toy|small|med|large] [trials] [seed]\n";
}

int main( int argc, char** argv ) {
    if ( argc < 3 ) {
        usage( argv[ 0 ] );
        return 1;
    }

    std::string mode{ argv[ 1 ] };
    std::string pname{ argv[ 2 ] };

    uint64_t seed{ 1 };
    if ( argc >= 4 ) {
        seed = static_cast<uint64_t>(
            std::strtoull( argv[ 3 ], nullptr, 10 )
        );
    }

    try {
        auto p{ pq::params_from_name( pname ) };

        if ( mode == "demo" ) {
            pq::Prng rng{ seed };
            pq::LweKex kex{ p };

            auto [ pk, sk ] = kex.keygen( rng );
            auto [ ct, kb ] = kex.encapsulate( pk, rng );
            auto ar = kex.decapsulate( ct, sk, pk );

            std::cout << "bob key:   " << pq::hex32( kb ) << "\n";
            std::cout << "alice key: " << pq::hex32( ar.key ) << "\n";
            std::cout << "match: " << ( ar.ok && ar.key == kb ? "yes" : "no" ) << "\n";
            return ar.ok && ar.key == kb ? 0 : 2;
        }

        if ( mode == "bench" ) {
            size_t trials{ 200 };
            if ( argc >= 4 ) {
                trials = static_cast<size_t>(
                    std::strtoull( argv[ 3 ], nullptr, 10 )
                );
            }

            auto r{ pq::run_bench( p, trials, seed, false ) };

            std::cout << "keygen ms: " << r.keygen_ms << "\n";
            std::cout << "enc ms:    " << r.enc_ms << "\n";
            std::cout << "dec ms:    " << r.dec_ms << "\n";
            std::cout << "fail rate: " << r.fail_rate << "\n";
            return 0;
        }

        usage( argv[ 0 ] );
        return 1;

    } catch ( const std::exception& e ) {
        std::cerr << "err: " << e.what() << "\n";
        return 1;
    }
}

