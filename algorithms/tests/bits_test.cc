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

}  // namespace
}  // namespace algorithms
