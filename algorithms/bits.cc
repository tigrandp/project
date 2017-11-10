#include "algorithms/bits.h"

namespace algorithms {

int RoundUpToPowerOfTwo(int n) {
  --n;
  return ((n) |
          (n >> 1) |
          (n >> 2) |
          (n >> 4) |
          (n >> 8) |
          (n >> 16) | (n >> 32)) + 1;
}

long long RoundUpToPowerOfTwo(long long n) {
  --n;
  return ((n) |
          (n >> 1LL) |
          (n >> 2LL) |
          (n >> 4LL) |
          (n >> 8LL) |
          (n >> 16LL) | (n >> 32LL) | (n >> 64LL)) + 1;
}

}  // namespace algorithms
