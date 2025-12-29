#pragma once
#include <vector>
#include "modq.h"
#include "prng.h"

namespace pq {

struct Vec final {
    int n{};
    std::vector<int> a{};

    explicit Vec( int nn = 0 );
    int& operator[]( int i ) noexcept;
    int operator[]( int i ) const noexcept;
};

struct Mat final {
    int n{};
    std::vector<int> a{};

    explicit Mat( int nn = 0 );
    int& operator()( int r, int c ) noexcept;
    int operator()( int r, int c ) const noexcept;
};

Vec mat_vec_mul( const Mat& A, const Vec& x, const ModQ& mod );
Vec matT_vec_mul( const Mat& A, const Vec& x, const ModQ& mod );
int dot( const Vec& x, const Vec& y, const ModQ& mod );

Mat uniform_mat( int n, const ModQ& mod, Prng& rng );
Vec uniform_vec( int n, const ModQ& mod, Prng& rng );

Vec cbd_noise_vec( int n, int eta, Prng& rng );

} // namespace pq

