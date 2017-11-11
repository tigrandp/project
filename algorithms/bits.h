// Author: tigrandp@gmail.com (Tigran Hakobyan)
//
// Various bit-twiddling functions, for performance optimization.

#ifndef ALGORITHMS_BITS_H_
#define ALGORITHMS_BITS_H_

namespace algorithms {

namespace internal {

constexpr char kNumBits[] = {
  0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
  4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8 };

}  // namespace internal

// Optimized versions of std::max/std::min. Implementation improves cache branch
// miss-prediction ratio by 90%, and number of instructions executed per cycle
// by 50% compared to std::max/std::min.
// Returns the smallest of the two given integer numbers.
template<typename T>
T Min(const T& lhs, const T& rhs) {
  return lhs ^ ((lhs ^ rhs) & -(rhs < lhs));
}
// Returns the largest of the two given integer numbers.
template<typename T>
T Max(const T& lhs, const T& rhs) {
  return Min(lhs, rhs) ^ lhs ^ rhs;
}

// In prevailing cases of problems involving modular computations, it is the
// case that both arguments of addition are less than modulo, in which case
// cache branch miss-predictions can be decreased by 50% compared to standard
// implementation.
// Returns the remainder of sum of two given numbers by given modulo.
template<typename T>
T AddByModulo(const T& a, const T& b, const T& modulo) {
  T sum = a + b;
  return sum - (modulo & -(sum >= modulo));
}

// Returns the closest power of two which is not less than the given number.
// Note: The result should fit into the same type.
int RoundUpToPowerOfTwo(int n);
long long RoundUpToPowerOfTwo(long long n);

// Returns the lowest bit mask of the given number.
template<typename T> T GetLowestBitMask(T n) {
  return n & (-n);
}

// Returns number of bits in the given integer number.
template<typename T> int PopCount(T x) {
  int pop_count = 0;
  for( ; x > 0; x >>= 8) {
    pop_count += internal::kNumBits[x & 0xFF];
  }
  return pop_count;
}

}  // namespace algorithms

#endif  // ALGORITHMS_BITS_H_
