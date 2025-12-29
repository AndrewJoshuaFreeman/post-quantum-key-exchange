#include "lwe_kex.h"
#include "sha256.h"

namespace pq {

LweKex::LweKex( LweParams p )
    : p_{ p }
    , mod_{ p.q }
{}

std::pair<PublicKey, SecretKey> LweKex::keygen( Prng& rng ) const {
    PublicKey pk{
        uniform_mat( p_.n, mod_, rng ),
        Vec{ p_.n }
    };

    SecretKey sk{ cbd_noise_vec( p_.n, p_.noise_eta, rng ) };
    Vec e{ cbd_noise_vec( p_.n, p_.noise_eta, rng ) };

    Vec As{ mat_vec_mul( pk.A, sk.s, mod_ ) };
    for ( int i = 0; i < p_.n; ++i ) {
        pk.b[ i ] = mod_.add( As[ i ], e[ i ] );
    }

    return { pk, sk };
}

int LweKex::encode_bit( int bit ) const noexcept {
    return bit ? ( p_.q / 2 ) : 0;
}

int LweKex::decode_bit( int x ) const noexcept {
    int t{ mod_.norm( x ) };
    return ( t > p_.q / 4 && t < 3 * p_.q / 4 ) ? 1 : 0;
}

void LweKex::pack_u16_le( std::vector<uint8_t>& out, uint16_t x ) {
    out.push_back( static_cast<uint8_t>( x & 0xffu ) );
    out.push_back( static_cast<uint8_t>( ( x >> 8 ) & 0xffu ) );
}

std::optional<uint16_t> LweKex::unpack_u16_le(
    const std::vector<uint8_t>& in,
    size_t off
) {
    if ( off + 2 > in.size() ) return std::nullopt;
    uint16_t lo{ in[ off ] };
    uint16_t hi{ in[ off + 1 ] };
    return static_cast<uint16_t>( lo | ( hi << 8 ) );
}

std::pair<Ciphertext, std::array<uint8_t, 32>>
LweKex::encapsulate( const PublicKey& pk, Prng& rng ) const {
    Vec sB{ cbd_noise_vec( p_.n, p_.noise_eta, rng ) };
    Vec eB{ cbd_noise_vec( p_.n, p_.noise_eta, rng ) };

    Vec u{ matT_vec_mul( pk.A, sB, mod_ ) };
    for ( int i = 0; i < p_.n; ++i ) {
        u[ i ] = mod_.add( u[ i ], eB[ i ] );
    }

    int base{ dot( pk.b, sB, mod_ ) };

    Ciphertext ct{ u, {} };
    ct.v.reserve( static_cast<size_t>( 2 * p_.k ) );

    std::vector<uint8_t> bits{};
    bits.reserve( static_cast<size_t>( p_.k ) );

    for ( int i = 0; i < p_.k; ++i ) {
        int m{ rng.uniform_int( 0, 1 ) };
        bits.push_back( static_cast<uint8_t>( m ) );

        int eprime{ cbd_noise_vec( 1, p_.noise_eta, rng )[ 0 ] };
        int vi{
            mod_.add(
                mod_.add( base, eprime ),
                encode_bit( m )
            )
        };

        pack_u16_le( ct.v, static_cast<uint16_t>( mod_.norm( vi ) ) );
    }

    std::vector<uint8_t> kdf{};
    kdf.insert( kdf.end(), bits.begin(), bits.end() );

    for ( int i = 0; i < p_.n; ++i ) {
        pack_u16_le(
            kdf,
            static_cast<uint16_t>( mod_.norm( u[ i ] ) )
        );
    }

    kdf.insert( kdf.end(), ct.v.begin(), ct.v.end() );

    return { ct, Sha256::hash( kdf ) };
}

SessionResult LweKex::decapsulate(
    const Ciphertext& ct,
    const SecretKey& sk,
    const PublicKey&
) const {
    int us{ dot( ct.u, sk.s, mod_ ) };

    std::vector<uint8_t> bits{};
    bits.reserve( static_cast<size_t>( p_.k ) );

    bool ok{ true };

    for ( int i = 0; i < p_.k; ++i ) {
        auto vi_opt{ unpack_u16_le( ct.v, static_cast<size_t>( 2 * i ) ) };
        if ( !vi_opt ) {
            ok = false;
            break;
        }

        int m{
            decode_bit(
                mod_.sub(
                    static_cast<int>( *vi_opt ),
                    us
                )
            )
        };
        bits.push_back( static_cast<uint8_t>( m ) );
    }

    std::vector<uint8_t> kdf{};
    kdf.insert( kdf.end(), bits.begin(), bits.end() );

    for ( int i = 0; i < p_.n; ++i ) {
        pack_u16_le(
            kdf,
            static_cast<uint16_t>( mod_.norm( ct.u[ i ] ) )
        );
    }

    kdf.insert( kdf.end(), ct.v.begin(), ct.v.end() );

    return { Sha256::hash( kdf ), ok };
}

} // namespace pq

