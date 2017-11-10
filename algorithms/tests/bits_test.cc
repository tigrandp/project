#include "algorithms/bits.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace algorithms {
namespace {

TEST(BitsTest, GetsMaximumOfTwoNegativeNumbers) {
  EXPECT_EQ(-289282, Max(-289282, -123930292));
}

TEST(BitsTest, GetsMaximumOfTwoPositiveNumbers) {
  EXPECT_EQ(123776, Max(91, 123776));
}

TEST(BitsTest, GetsMaximumOfPositiveAndNegativeNumbers) {
  EXPECT_EQ(9, Max(-9, 9));
}

TEST(BitsTest, GetsMaximumOfZero) {
  EXPECT_EQ(0, Max(0, 0));
  EXPECT_EQ(0, Max(0, -7));
  EXPECT_EQ(9, Max(0, 9));
}

TEST(BitsTest, GetsMinimumOfTwoNegativeNumbers) {
  EXPECT_EQ(-123930292, Min(-289282, -123930292));
}

TEST(BitsTest, GetsMinimumOfTwoPositiveNumbers) {
  EXPECT_EQ(91, Min(91, 123776));
}

TEST(BitsTest, GetsMinimumOfPositiveAndNegativeNumbers) {
  EXPECT_EQ(-9, Min(-9, 9));
}

TEST(BitsTest, GetsMinimumOfZero) {
  EXPECT_EQ(0, Min(0, 0));
  EXPECT_EQ(-7, Min(0, -7));
  EXPECT_EQ(0, Min(0, 9));
}

TEST(BitsTest, GetsSumByModuloForIntegers) {
  EXPECT_EQ(1635, AddByModulo(11728, 28290, 38383));
}

TEST(BitsTest, GetsSumByModuloForLongs) {
  EXPECT_EQ(15488191791LL,
            AddByModulo(18189191871LL, 28228229211LL, 30929229291LL));
}

TEST(BitsTest, RoundsUpNonePowerOfTwoIntegerToClosestNumberTest) {
  EXPECT_EQ(4096, RoundUpToPowerOfTwo(4012));
}

TEST(BitsTest, RoundsUpPowerOfTwoIntegerToClosestNumberTest) {
  EXPECT_EQ(8192, RoundUpToPowerOfTwo(8192));
}

TEST(BitsTest, RoundsUpNonePowerOfTwoLongToClosestNumberTest) {
  EXPECT_EQ(17179869184, RoundUpToPowerOfTwo(17179869174LL));
}

TEST(BitsTest, RoundsUpPowerOfTwoLongToClosestNumberTest) {
  EXPECT_EQ(549755813888LL, RoundUpToPowerOfTwo(549755813888LL));
}

}  // namespace
}  // namespace algorithms
