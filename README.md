# pqkex

post-quantum key exchange based on learning with errors (LWE)

## overview

this implements a lattice-based key encapsulation mechanism using the hardness of learning with errors. the protocol allows two parties to agree on a shared 256-bit key that remains secure against quantum adversaries.

core components:
- parameter sets tunable for security/performance tradeoff
- centered binomial noise sampling for error injection
- reconciliation using nearest-plane decoding
- SHA-256 key derivation from agreed bits

this is an educational implementation demonstrating the structure of post-quantum protocols, not a production library.

## build

```bash
make
```

requires g++ with C++17 support.

## usage

demo mode runs a single key exchange:
```bash
./pqkex demo toy 1
```

benchmark mode measures average runtime over multiple trials:
```bash
./pqkex bench small 200 1
```

parameter sets: `toy`, `small`, `med`, `large`

## architecture

```
include/
  params.h      - parameter configurations (n, q, k, eta)
  modq.h        - modular arithmetic mod q
  prng.h        - pseudorandom number generation
  matrix.h      - vector/matrix ops, noise sampling
  sha256.h      - key derivation
  lwe_kex.h     - key exchange protocol
  bench.h       - timing and failure rate measurement

src/
  [corresponding .cpp implementations]
  main.cpp      - CLI entry point
```

## protocol sketch

alice generates public key `(A, b = As + e)` where `A` is uniform, `s` and `e` are small noise vectors.

bob samples ephemeral secret `s'`, computes `u = A^T s' + e'`, encodes random bits into `v_i = <b, s'> + e'' + enc(m_i)`, and derives key from bits.

alice decodes `v_i - <u, s>` to recover bits, derives same key.

correctness depends on noise remaining small enough for decoding. failure probability scales with parameter choices.

## security notes

- security relies on hardness of LWE with chosen parameters
- demo params prioritize clarity over cryptographic strength
- no side-channel protections (timing, cache)
- not constant-time
- not intended for production use

## references

lattice-based cryptography underpins NIST post-quantum standards (Kyber, Dilithium). this implementation uses a simplified reconciliation approach for educational purposes.

for parameter selection methodology and security estimates, see foundational work on Ring-LWE and module-LWE in the literature.
