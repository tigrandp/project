#include "ds/heap.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace ds {
namespace {

TEST(HeapTest, ChecksRangeConstructionTest) {
  std::vector<int> elements = {5, 3, 1, 2, 4};
  Heap<int> heap(elements.begin(), elements.end());
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  EXPECT_LE(result[0], result[1]);
  EXPECT_LE(result[0], result[2]);
  EXPECT_LE(result[1], result[3]);
  EXPECT_LE(result[1], result[4]);
}

TEST(HeapTest, ChecksInitializerListConstructionTest) {
  Heap<int> heap = {5, 3, 4, 1, 2};
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  EXPECT_LE(result[0], result[1]);
  EXPECT_LE(result[0], result[2]);
  EXPECT_LE(result[1], result[3]);
  EXPECT_LE(result[1], result[4]);
}

TEST(HeapTest, ChecksEmptyHeapConstructionTest) {
  Heap<int> heap;
  EXPECT_TRUE(heap.Empty());
}

TEST(HeapTest, ChecksTopElementReturnedTest) {
  Heap<int> heap = {3, 1, 1};
  EXPECT_EQ(1, heap.Top());
}

TEST(HeapTest, ChecksTopElementRemovalTest) {
  Heap<int> heap = {5, 6, 2, 2, 3, 1, 1};
  ASSERT_EQ(7, heap.Size());
  heap.Pop();
  EXPECT_EQ(1, heap.Top());
  heap.Pop();
  EXPECT_EQ(2, heap.Top());
  // Checking whether heap invariant is respected.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  EXPECT_LE(result[0], result[1]);
  EXPECT_LE(result[0], result[2]);
  EXPECT_LE(result[1], result[3]);
  EXPECT_LE(result[1], result[4]);
}

TEST(HeapTest, ChecksConsumeTopElementTest) {
  Heap<int> heap = {5, 0, -1};
  int top_element = heap.ConsumeTop();
  EXPECT_EQ(-1, top_element);
  EXPECT_EQ(0, heap.Top());
}

}  // namespace
}  // namespace ds
