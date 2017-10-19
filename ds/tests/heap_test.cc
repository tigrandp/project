#include "ds/heap.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace ds {
namespace {

TEST(HeapTest, ChecksCorrectnessOfRangeConstructorTest) {
  std::vector<int> elements = {5, 3, 1, 2, 4};
  Heap<int> heap(elements.begin(), elements.end());
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(result.size(), 5);
  EXPECT_LE(result[0], result[1]);
  EXPECT_LE(result[0], result[2]);
  EXPECT_LE(result[1], result[3]);
  EXPECT_LE(result[1], result[4]);
}

TEST(HeapTest, ChecksCorrectnessOfInitializerListConstructorTest) {
  Heap<int> heap = {5, 3, 4, 1, 2};
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(result.size(), 5);
  EXPECT_LE(result[0], result[1]);
  EXPECT_LE(result[0], result[2]);
  EXPECT_LE(result[1], result[3]);
  EXPECT_LE(result[1], result[4]);
}

}  // namespace
}  // namespace ds
