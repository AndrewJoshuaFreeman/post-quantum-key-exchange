#include "sha256.h"
#include <cstring>

namespace pq {

static inline uint32_t rotr( uint32_t x, uint32_t n ) noexcept {
    return ( x >> n ) | ( x << ( 32u - n ) );
}

static inline uint32_t ch(
    uint32_t x,
    uint32_t y,
    uint32_t z
) noexcept {
    return ( x & y ) ^ ( ~x & z );
}

static inline uint32_t maj(
    uint32_t x,
    uint32_t y,
    uint32_t z
) noexcept {
    return ( x & y ) ^ ( x & z ) ^ ( y & z );
}

static inline uint32_t bsig0( uint32_t x ) noexcept {
    return rotr( x, 2 ) ^ rotr( x, 13 ) ^ rotr( x, 22 );
}

static inline uint32_t bsig1( uint32_t x ) noexcept {
    return rotr( x, 6 ) ^ rotr( x, 11 ) ^ rotr( x, 25 );
}

static inline uint32_t ssig0( uint32_t x ) noexcept {
    return rotr( x, 7 ) ^ rotr( x, 18 ) ^ ( x >> 3 );
}

static inline uint32_t ssig1( uint32_t x ) noexcept {
    return rotr( x, 17 ) ^ rotr( x, 19 ) ^ ( x >> 10 );
}

static constexpr uint32_t K[ 64 ] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,
    0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
    0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,
    0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,
    0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
    0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,
    0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,
    0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
    0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

Sha256::Sha256() {
    h_ = {
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };
}

void Sha256::update( const uint8_t* data, size_t n ) {
    bitlen_ += static_cast<uint64_t>( n ) * 8u;

    size_t i{ 0 };

    if ( buf_used_ > 0 ) {
        size_t take{
            n < ( 64 - buf_used_ ) ? n : ( 64 - buf_used_ )
        };
        std::memcpy( buf_.data() + buf_used_, data, take );
        buf_used_ += take;
        i += take;

        if ( buf_used_ == 64 ) {
            compress( buf_.data() );
            buf_used_ = 0;
        }
    }

    for ( ; i + 64 <= n; i += 64 ) {
        compress( data + i );
    }

    if ( i < n ) {
        buf_used_ = n - i;
        std::memcpy( buf_.data(), data + i, buf_used_ );
    }
}

void Sha256::update( const std::vector<uint8_t>& v ) {
    update( v.data(), v.size() );
}

void Sha256::compress( const uint8_t block[ 64 ] ) {
    uint32_t w[ 64 ];

    for ( int i = 0; i < 16; ++i ) {
        w[ i ] =
            ( static_cast<uint32_t>( block[ 4 * i + 0 ] ) << 24 ) |
            ( static_cast<uint32_t>( block[ 4 * i + 1 ] ) << 16 ) |
            ( static_cast<uint32_t>( block[ 4 * i + 2 ] ) << 8 )  |
            ( static_cast<uint32_t>( block[ 4 * i + 3 ] ) );
    }

    for ( int i = 16; i < 64; ++i ) {
        w[ i ] =
            ssig1( w[ i - 2 ] ) +
            w[ i - 7 ] +
            ssig0( w[ i - 15 ] ) +
            w[ i - 16 ];
    }

    uint32_t a{ h_[ 0 ] };
    uint32_t b{ h_[ 1 ] };
    uint32_t c{ h_[ 2 ] };
    uint32_t d{ h_[ 3 ] };
    uint32_t e{ h_[ 4 ] };
    uint32_t f{ h_[ 5 ] };
    uint32_t g{ h_[ 6 ] };
    uint32_t h{ h_[ 7 ] };

    for ( int i = 0; i < 64; ++i ) {
        uint32_t t1{
            h + bsig1( e ) + ch( e, f, g ) + K[ i ] + w[ i ]
        };
        uint32_t t2{
            bsig0( a ) + maj( a, b, c )
        };

        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    h_[ 0 ] += a;
    h_[ 1 ] += b;
    h_[ 2 ] += c;
    h_[ 3 ] += d;
    h_[ 4 ] += e;
    h_[ 5 ] += f;
    h_[ 6 ] += g;
    h_[ 7 ] += h;
}

std::array<uint8_t, 32> Sha256::digest() {
    uint8_t pad[ 128 ];
    size_t padlen{ 0 };

    pad[ padlen++ ] = 0x80u;

    size_t rem{ ( buf_used_ + padlen ) % 64 };
    size_t z{ rem <= 56 ? ( 56 - rem ) : ( 56 + 64 - rem ) };

    std::memset( pad + padlen, 0, z );
    padlen += z;

    uint64_t bl{ bitlen_ };
    for ( int i = 7; i >= 0; --i ) {
        pad[ padlen++ ] =
            static_cast<uint8_t>( ( bl >> ( 8 * i ) ) & 0xffu );
    }

    update( pad, padlen );

    std::array<uint8_t, 32> out{};

    for ( int i = 0; i < 8; ++i ) {
        out[ 4 * i + 0 ] = static_cast<uint8_t>( h_[ i ] >> 24 );
        out[ 4 * i + 1 ] = static_cast<uint8_t>( h_[ i ] >> 16 );
        out[ 4 * i + 2 ] = static_cast<uint8_t>( h_[ i ] >> 8 );
        out[ 4 * i + 3 ] = static_cast<uint8_t>( h_[ i ] );
    }

    return out;
}

std::array<uint8_t, 32> Sha256::hash(
    const std::vector<uint8_t>& v
) {
    Sha256 s{};
    s.update( v );
    return s.digest();
}

} // namespace pq

