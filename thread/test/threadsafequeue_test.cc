#include "thread/threadsafequeue.h"

#include <algorithm>
#include <functional>
#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace thread {
namespace {

using NumberRange = std::pair<int, int>;

void PushSingleRange(const NumberRange& range, SynchronizedQueue<int>* q) {
  for (int i = range.first; i <= range.second; ++i) {
    q->PushBack(i);
    if (i % 10 == 0) {
      // TODO(tigrandp): Replace with thread safe log.
      fprintf(stderr, "Pushed [%d-%d] Thread(%lld).\n",
              std::max(range.first, i - 9), i, std::this_thread::get_id());
    }
  }
}

void PushRanges(const std::vector<NumberRange>& ranges,
                SynchronizedQueue<int>* q) {
  std::vector<int> indexes;
  for (int i = 0; i < ranges.size(); ++i) {
    indexes.push_back(i);
  }
  std::random_shuffle(indexes.begin(), indexes.end());
  std::vector<std::thread> threads;
  for (int idx : indexes) {
    threads.emplace_back(PushSingleRange, std::ref(ranges[idx]), q);
  }
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

TEST(ThreadSafeQueueTest, ThreadSafeQueueSemantics) {
  SynchronizedQueue<int> q;
  std::vector<NumberRange> ranges = {
      {0, 100}, {500, 600}, {1000, 2000}, {10000, 15000}};
  PushRanges(ranges, &q);

  std::set<int> all_range_numbers;
  for (const NumberRange& range : ranges) {
    for (int i = range.first; i <= range.second; ++i) {
      all_range_numbers.insert(i);
    }
  }
  int poped;
  while (q.PopFront(&poped)) {
    EXPECT_EQ(all_range_numbers.count(poped), 1);
    all_range_numbers.erase(poped);
  }
  EXPECT_TRUE(all_range_numbers.empty());
}

TEST(ThreadSafeQueueTest, ThreadSafeStackSemantics) {
  SynchronizedQueue<int> q;
  std::vector<NumberRange> ranges = {
      {0, 100}, {500, 600}, {1000, 2000}, {10000, 15000}};
  PushRanges(ranges, &q);

  std::set<int> all_range_numbers;
  for (const NumberRange& range : ranges) {
    for (int i = range.first; i <= range.second; ++i) {
      all_range_numbers.insert(i);
    }
  }
  int poped;
  while (q.PopBack(&poped)) {
    EXPECT_EQ(all_range_numbers.count(poped), 1);
    all_range_numbers.erase(poped);
  }
  EXPECT_TRUE(all_range_numbers.empty());
}

}  // namespace
}  // namespace thread
