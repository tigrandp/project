#include "ds/heap.h"

#include <memory>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace ds {
namespace {

#define CHECK_HEAP_PROPERTY(heap_vector, func) \
  for (std::size_t i = 0; i < heap_vector.size(); ++i) { \
    if (i * 2 + 1 < heap_vector.size()) { \
      func(heap_vector[i], heap_vector[i * 2 + 1]); \
    } \
    if (i * 2 + 2 < heap_vector.size()) { \
      func(heap_vector[i], heap_vector[i * 2 + 2]); \
    } \
  }

TEST(HeapTest, ConstructsHeapFromGivenRangeTest) {
  std::vector<int> elements = {5, 3, 1, 2, 4};
  Heap<int> heap(elements.begin(), elements.end());
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, ConstructsHeapFromInitializerListTest) {
  Heap<int> heap = {5, 3, 4, 1, 2};
  // Checking whether heap property was restored.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, ConstructsEmptyHeapTest) {
  Heap<int> heap;
  EXPECT_TRUE(heap.Empty());
}

TEST(HeapTest, ReturnsTopOfHeapTest) {
  Heap<int> heap = {3, 1, 1};
  EXPECT_EQ(1, heap.Top());
}

TEST(HeapTest, RemovesTopElementTestTest) {
  Heap<int> heap = {5, 6, 2, 2, 3, 1, 1};
  ASSERT_EQ(7, heap.Size());
  heap.Pop();
  EXPECT_EQ(1, heap.Top());
  heap.Pop();
  EXPECT_EQ(2, heap.Top());
  // Checking whether heap invariant is respected.
  std::vector<int> result(heap.Begin(), heap.End());
  ASSERT_EQ(5, result.size());
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, ConsumesTopElementTest) {
  Heap<int> heap = {5, 0, -1};
  int top_element = heap.ConsumeTop();
  EXPECT_EQ(-1, top_element);
  EXPECT_EQ(0, heap.Top());
}

TEST(HeapTest, PushesNonTopElementToFullHeapTest) {
  Heap<int> heap = {5, 0, -1};
  heap.SetMax(3);
  heap.Push(0);
  // Top element should have remained unchanged.
  EXPECT_EQ(-1, heap.Top());
  EXPECT_EQ(3, heap.Size());
}

TEST(HeapTest, PushesTopElementToFulHeapTest) {
  Heap<int> heap = {5, 0, -1};
  heap.SetMax(3);
  heap.Push(-2);
  // Top element should have been updated.
  EXPECT_EQ(-2, heap.Top());
  // Size should have been remained the same.
  EXPECT_EQ(3, heap.Size());
}

TEST(HeapTest, PushesElementToNoneFullHeapTest) {
  Heap<int> heap = {5, 0, -1, 7};
  heap.SetMax(6);

  heap.Push(2);
  EXPECT_EQ(5, heap.Size());
  // Top element should have been the same.
  EXPECT_EQ(heap.Top(), -1);
  std::vector<int> result(heap.Begin(), heap.End());
  // Heap property after insertion should have been respected.
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);

  heap.Push(-10);
  EXPECT_EQ(6, heap.Size());
  // Top element should have been updated.
  EXPECT_EQ(-10, heap.Top());
  result = std::vector<int>(heap.Begin(), heap.End());
  // Heap property after insertion should have been respected.
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, ReplacesElementsOfTheHeapTest) {
  Heap<int> heap = {5, 2, -1, 1};
  std::vector<int> vec = {2, 3, 0, 4};
  heap.ReplaceElements(&vec);
  // Correct heap should have been replaced.
  CHECK_HEAP_PROPERTY(vec, EXPECT_LE);
  std::vector<int> result(heap.Begin(), heap.End());
  // And heap structure should have been restored.
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, AssignsVectorToHeapTest) {
  Heap<int> heap;
  std::vector<int> vec = {1, 0, -1};
  heap.Assign(vec.begin(), vec.end());
  std::vector<int> result(heap.Begin(), heap.End());
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

TEST(HeapTest, RemovesElementFromHeapTest) {
  Heap<int> heap = {0, 6, 9, 7};
  auto it = heap.Begin();
  // Point at second element of the heap.
  ++it;
  // Removing that particular element.
  heap.Remove(it);
  EXPECT_EQ(3, heap.Size());
  EXPECT_EQ(0, heap.Top());
  std::vector<int> result(heap.Begin(), heap.End());
  CHECK_HEAP_PROPERTY(result, EXPECT_LE);
}

#undef CHECK_HEAP_PROPERTY

TEST(HeapTest, MovesElementToNoneFullHeapTest) {
  class UniquePtrComparator {
   public:
    bool operator() (const std::unique_ptr<int>& lhs,
                     const std::unique_ptr<int>& rhs) {
      return *lhs < *rhs;
    }
  };
  Heap<std::unique_ptr<int>, UniquePtrComparator> heap;
  heap.Push(std::unique_ptr<int>(new int(5)));
  heap.Push(std::unique_ptr<int>(new int(0)));
  heap.Push(std::unique_ptr<int>(new int(1)));
  heap.Push(std::unique_ptr<int>(new int(2)));

  EXPECT_EQ(4, heap.Size());
  EXPECT_EQ(0, *heap.Top());

  std::unique_ptr<int> to_move(new int(-1));
  heap.Push(std::move(to_move));

  EXPECT_EQ(5, heap.Size());
  EXPECT_DEATH(heap.Size(), "asd");
  EXPECT_EQ(-1, *heap.Top());
}

}  // namespace
}  // namespace ds
