#include "matrix.h"
#include <cassert>

namespace pq {

Vec::Vec( int nn ) : n{ nn }, a( static_cast<size_t>( nn ), 0 ) {}

int& Vec::operator[]( int i ) noexcept {
    return a[ static_cast<size_t>( i ) ];
}

int Vec::operator[]( int i ) const noexcept {
    return a[ static_cast<size_t>( i ) ];
}

Mat::Mat( int nn )
    : n{ nn }
    , a( static_cast<size_t>( nn ) * static_cast<size_t>( nn ), 0 )
{}

int& Mat::operator()( int r, int c ) noexcept {
    return a[ static_cast<size_t>( r ) * n + c ];
}

int Mat::operator()( int r, int c ) const noexcept {
    return a[ static_cast<size_t>( r ) * n + c ];
}

Vec mat_vec_mul( const Mat& A, const Vec& x, const ModQ& mod ) {
    assert( A.n == x.n );
    Vec y{ A.n };
    for ( int r = 0; r < A.n; ++r ) {
        int64_t acc{ 0 };
        for ( int c = 0; c < A.n; ++c ) {
            acc += static_cast<int64_t>( A( r, c ) ) * x[ c ];
        }
        y[ r ] = mod.norm( acc );
    }
    return y;
}

Vec matT_vec_mul( const Mat& A, const Vec& x, const ModQ& mod ) {
    assert( A.n == x.n );
    Vec y{ A.n };
    for ( int c = 0; c < A.n; ++c ) {
        int64_t acc{ 0 };
        for ( int r = 0; r < A.n; ++r ) {
            acc += static_cast<int64_t>( A( r, c ) ) * x[ r ];
        }
        y[ c ] = mod.norm( acc );
    }
    return y;
}

int dot( const Vec& x, const Vec& y, const ModQ& mod ) {
    assert( x.n == y.n );
    int64_t acc{ 0 };
    for ( int i = 0; i < x.n; ++i ) {
        acc += static_cast<int64_t>( x[ i ] ) * y[ i ];
    }
    return mod.norm( acc );
}

Mat uniform_mat( int n, const ModQ& mod, Prng& rng ) {
    Mat A{ n };
    for ( int i = 0; i < n * n; ++i ) {
        A.a[ static_cast<size_t>( i ) ] =
            rng.uniform_int( 0, mod.q - 1 );
    }
    return A;
}

Vec uniform_vec( int n, const ModQ& mod, Prng& rng ) {
    Vec v{ n };
    for ( int i = 0; i < n; ++i ) {
        v[ i ] = rng.uniform_int( 0, mod.q - 1 );
    }
    return v;
}

static inline int popcnt8( uint8_t x ) noexcept {
#if defined( __GNUG__ ) || defined( __clang__ )
    return __builtin_popcount( static_cast<unsigned>( x ) );
#else
    int c{ 0 };
    for ( int i = 0; i < 8; ++i ) c += ( x >> i ) & 1u;
    return c;
#endif
}

Vec cbd_noise_vec( int n, int eta, Prng& rng ) {
    Vec e{ n };
    for ( int i = 0; i < n; ++i ) {
        int pos{ 0 }, neg{ 0 };
        for ( int j = 0; j < eta; ++j ) {
            uint8_t b1{ static_cast<uint8_t>( rng.u32() & 0xffu ) };
            uint8_t b2{ static_cast<uint8_t>( ( rng.u32() >> 8 ) & 0xffu ) };
            pos += popcnt8( b1 ) & 1;
            neg += popcnt8( b2 ) & 1;
        }
        e[ i ] = pos - neg;
    }
    return e;
}

} // namespace pq

