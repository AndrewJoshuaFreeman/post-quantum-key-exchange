#include "params.h"
#include <stdexcept>

namespace pq {

LweParams params_from_name( const std::string& name ) {
    if ( name == "toy" )   return { 256, 3329, 256, 2 };
    if ( name == "small" ) return { 384, 3329, 256, 2 };
    if ( name == "med" )   return { 512, 7681, 256, 3 };
    if ( name == "large" ) return { 640, 12289, 256, 3 };
    throw std::invalid_argument( "bad params name" );
}

} // namespace pq

