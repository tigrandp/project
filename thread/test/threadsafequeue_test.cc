#include "thread/threadsafequeue.h"

#include <algorithm>
#include <chrono>
#include <functional>
#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace thread {
namespace {

using NumberRange = std::pair<int, int>;

template<typename Q>
void PushSingleRange(const NumberRange& range, Q* q) {
  for (int i = range.first; i <= range.second; ++i) {
    q->PushBack(i);
    if (i % 10 == 0) {
      // TODO(tigrandp): Replace with thread safe log.
      fprintf(stderr, "Pushed [%d-%d] Thread(%lld).\n",
              std::max(range.first, i - 9), i, std::this_thread::get_id());
    }
  }
}

template<typename Q>
void PushRanges(const std::vector<NumberRange>& ranges, Q* q) {
  std::vector<int> indexes;
  for (int i = 0; i < ranges.size(); ++i) {
    indexes.push_back(i);
  }
  std::random_shuffle(indexes.begin(), indexes.end());
  std::vector<std::thread> threads;
  for (int idx : indexes) {
    threads.emplace_back(PushSingleRange<Q>, std::ref(ranges[idx]), q);
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

TEST(ThreadSafeQueueTest, WaitQueueGeneralUse) {
  WaitSyncQueue<int> q;
  std::vector<NumberRange> ranges = {
    {0, 100}, {500, 600}, {1000, 2000}, {10000, 15000}};
  PushRanges(ranges, &q);

  std::set<int> all_range_numbers;
  for (const NumberRange& range : ranges) {
    for (int i = range.first; i <= range.second; ++i) {
      all_range_numbers.insert(i);
    }
  }

  int popped = -1;
  while (!all_range_numbers.empty()) {
    q.PopBack(&popped);
    EXPECT_EQ(all_range_numbers.count(popped), 1);
    all_range_numbers.erase(popped);
  }
}

// Test scenario which exercises that WaitQueue awaits for elements to appear
// if it is empty.
TEST(ThreadSafeQueueTest, WaitsElementsToAppearBeforePop) {
  constexpr int kPushesPerThread = 7;
  constexpr int kNumThreads = 3;

  WaitSyncQueue<int> q;
  
  // Function which sleeps for a second and pushes element to queue.
  auto pusher_thread = [&q](int iterations) {
    for (int it = 0; it < iterations; ++it) {
      q.PushBack(7);
      std::this_thread::sleep_for(std::chrono::duration<int, std::milli>());
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back(pusher_thread, kPushesPerThread);
  }

  for (int i = 0; i < kPushesPerThread * kNumThreads; ++i) {
    int popped = -1;
    q.PopBack(&popped);
    EXPECT_EQ(popped, 7);
  }
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

// Test scenario which exercises that WaitQueue waits for elements to be
// consumed before pushing new ones.
TEST(ThreadSafeQueueTest, WaitsElementsToBeConsumed) {
  constexpr int kConsumersPerThread = 7;
  constexpr int kNumThreads = 3;

  WaitSyncQueue<int> q;
  q.SetMaxQueueSize(21);
  for (int i = 0; i < kConsumersPerThread * kNumThreads; ++i) {
    q.PushBack(1);
  }
  
  // Function which sleeps for a second and pushes element to queue.
  auto consumer_thread = [&q](int iterations) {
    for (int it = 0; it < iterations; ++it) {
      int popped;
      q.PopFront(&popped);
      std::this_thread::sleep_for(std::chrono::duration<int, std::milli>());
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < kNumThreads; ++i) {
    threads.emplace_back(consumer_thread, kConsumersPerThread);
  }

  for (int i = 0; i < kConsumersPerThread * kNumThreads; ++i) {
    q.PushBack(-1);
  }
  for (int i = 0; i < kConsumersPerThread * kNumThreads; ++i) {
    int popped;
    q.PopBack(&popped);
    EXPECT_EQ(-1, popped);
  }
  std::for_each(threads.begin(), threads.end(),
                std::mem_fn(&std::thread::join));
}

// Test scenario which exercises that all the push waiters are correctly
// stopped.
TEST(ThreadSafeQueueTest, StopsAllPushWaiters) {
  WaitSyncQueue<int> q;
  q.SetMaxQueueSize(0);
  bool result = true;
  std::thread t([&q, &result]() { result = q.PushBack(1); });
  
  // Make sure that thread have already started.
  std::this_thread::sleep_for(std::chrono::seconds(2));
  q.StopAllWaiters();
  t.join();

  EXPECT_FALSE(result);
}

}  // namespace
}  // namespace thread
