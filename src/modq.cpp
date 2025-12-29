#include "modq.h"

namespace pq {

ModQ::ModQ( int qq ) : q{ qq } {}

int ModQ::norm( int64_t x ) const noexcept {
    int64_t r{ x % q };
    if ( r < 0 ) r += q;
    return static_cast<int>( r );
}

int ModQ::add( int a, int b ) const noexcept {
    return norm( static_cast<int64_t>( a ) + b );
}

int ModQ::sub( int a, int b ) const noexcept {
    return norm( static_cast<int64_t>( a ) - b );
}

int ModQ::mul( int a, int b ) const noexcept {
    return norm( static_cast<int64_t>( a ) * b );
}

} // namespace pq

