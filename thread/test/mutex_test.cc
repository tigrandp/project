#include "thread/mutex.h"

#include <cassert>
#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>

#include <sys/wait.h>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace thread {
namespace {

using Mutex = HierarchicalMutex<std::mutex>;
using Mutexes = std::map<int, std::unique_ptr<Mutex>>;
using Locks = std::map<int, std::unique_ptr<std::unique_lock<Mutex>>>;

namespace helpers {

struct LockInstruction {
  enum Command {LOCK, UNLOCK};

  int hierarchy_level;
  Command lock_command;
};

void CreateMutexWithoutLock(int level, Mutexes* mutexes, Locks* locks) {
  mutexes->emplace(level, std::unique_ptr<Mutex>(new Mutex(level)));
  // TODO(tigrandp): Replace this with c++14 std::make_unique when available.
  typename Locks::mapped_type lock(
      new std::unique_lock<Mutex>(*(*mutexes)[level].get(), std::defer_lock));
  locks->emplace(level, std::move(lock));
}

void RemoveMutex(int level, Mutexes* mutexes, Locks* locks) {
  // Order is important, dangling pointers.
  locks->erase(locks->find(level));
  mutexes->erase(mutexes->find(level));
}

void LockMutexes(const std::vector<LockInstruction>& instructions) {
  Mutexes mutexes;
  Locks locks;
  for (const auto& instruction : instructions) {
    if (instruction.lock_command == LockInstruction::LOCK) {
      // We didn't lock mutex at this level previously, create mutex and lock.
      assert(locks.count(instruction.hierarchy_level) == 0);
      CreateMutexWithoutLock(instruction.hierarchy_level, &mutexes, &locks);
      assert(!locks[instruction.hierarchy_level]->owns_lock());
      locks[instruction.hierarchy_level]->lock();
      assert(locks[instruction.hierarchy_level]->owns_lock());
    } else {
      // We should have had lock at this level.
      assert(locks.count(instruction.hierarchy_level) == 1);
      assert(locks[instruction.hierarchy_level]->owns_lock());
      locks[instruction.hierarchy_level]->unlock();
      assert(!locks[instruction.hierarchy_level]->owns_lock());
      // Remove lock and corresponding mutex.
      RemoveMutex(instruction.hierarchy_level, &mutexes, &locks);
      assert(locks.count(instruction.hierarchy_level) == 0);
      assert(mutexes.count(instruction.hierarchy_level) == 0);
    }
  }
}

std::string GetThreadInfo(const std::chrono::milliseconds& check_point,
                          const std::vector<LockInstruction>& instructions) {
  std::ostringstream out;
  out << "Thread(" << std::this_thread::get_id() << "): [";
  for (std::size_t i = 0; i < instructions.size(); ++i) {
    if (!i) out << "\n";
    out << "\t";
    out << (instructions[i].lock_command == LockInstruction::LOCK ?
           "Lock" : "Unlock");
    out << " at " << instructions[i].hierarchy_level;
    out << "\n";
  }
  out << "]\n";
  out << "Current delta: " << std::chrono::milliseconds(check_point).count();

  return out.str();
}

void LockHierarchicalMutexesInOrder(
    const std::vector<LockInstruction>& instructions,
    const std::chrono::milliseconds& time_out,
    const std::chrono::milliseconds& time_delta) {
  std::chrono::milliseconds now(0);
  while (now < time_out) {
    LockMutexes(instructions);
    std::cerr << GetThreadInfo(now, instructions) << std::endl;
    now += time_delta;
  }
}

void LaunchLockThreadWithParamsAndWait(
    const std::vector<std::vector<LockInstruction>>& instructions,
    const std::chrono::milliseconds& time_out,
    const std::chrono::milliseconds& time_delta) {
  std::vector<std::thread> threads;
  for (std::size_t i = 0; i < instructions.size(); ++i) {
    threads.emplace_back(LockHierarchicalMutexesInOrder,
                         std::ref(instructions[i]), std::ref(time_out),
                         std::ref(time_delta));
  }
  std::for_each(threads.begin(), threads.end(),
                [](std::thread& thread) {
                    thread.join();
                });
}
}  // namespace helpers

using helpers::LockInstruction;
using helpers::LaunchLockThreadWithParamsAndWait;

// Test scenario which checks that violation of hierarchical order causes
// program to terminate.
TEST(HierarchicalMutexTest, ViolatesLockHierarchy) {
  // Hierarchy is violated as 3000 < 4000, but locked before 4000.
  std::vector<LockInstruction> hierarchy_violation = {
    {5000, LockInstruction::LOCK},
    {3000, LockInstruction::LOCK},
    {4000, LockInstruction::LOCK}};
  // 2 seconds time out.
  std::chrono::milliseconds time_out(2000);
  // Half a second increment.
  std::chrono::milliseconds delta(500);
  EXPECT_DEATH(
      LaunchLockThreadWithParamsAndWait({hierarchy_violation}, time_out,
                                        delta),
      "");
}

// Test scenario which checks that unlock function restores previous state,
// so by unlocking i-th mutex any lock up to the (i-1)th hierarchy level can
// be successfully locked.
TEST(HierarchicalMutexTest, RestoresLockAfterUnlock) {
  // After 4000 is released, 4500 can freely acquire the lock.
  std::vector<LockInstruction> instructions = {
    {5000, LockInstruction::LOCK},
    {4000, LockInstruction::LOCK},
    {4000, LockInstruction::UNLOCK},
    {4500, LockInstruction::LOCK},
    {4500, LockInstruction::UNLOCK}};
  // 2 seconds time out.
  std::chrono::milliseconds time_out(2000);
  // Half a second increment.
  std::chrono::milliseconds delta(500);
  // This call should not fail.
  LaunchLockThreadWithParamsAndWait({instructions}, time_out, delta);
}

TEST(HierarchicalMutexTest, MultipleThreadsConfflicts) {
  std::vector<LockInstruction> instructions_t1 = {
    {5000, LockInstruction::LOCK},
    {4000, LockInstruction::LOCK},
    {3000, LockInstruction::LOCK},
    {3000, LockInstruction::UNLOCK},
    {4000, LockInstruction::UNLOCK}};
  std::vector<LockInstruction> instructions_t2 = {
    {10000, LockInstruction::LOCK},
    {9000, LockInstruction::LOCK},
    {8000, LockInstruction::LOCK},
    {8000, LockInstruction::UNLOCK},
    {9000, LockInstruction::UNLOCK}};
  std::vector<LockInstruction> instructions_t3 = {
    {100, LockInstruction::LOCK},
    {90, LockInstruction::LOCK},
    {80, LockInstruction::LOCK},
    {80, LockInstruction::UNLOCK},
    {90, LockInstruction::UNLOCK}};
  // 3 seconds time out.
  std::chrono::milliseconds time_out(3000);
  // 0.1 second increment.
  std::chrono::milliseconds delta(100);
  // If all the locks were from the same thread hierarchy would have been
  // violated but since they are in different threads of execution they should
  // succeed.
  LaunchLockThreadWithParamsAndWait(
      {instructions_t1, instructions_t2, instructions_t3}, time_out, delta);
}

// Test which makes sure that stack unwinding works fine.
TEST(HierarchicalMutex, ReliesOnStackUnwinding) {
  Mutex m5(5);
  std::lock_guard<Mutex> l5(m5);

  {
    Mutex m3(3);
    std::lock_guard<Mutex> l3(m3);
  }

  Mutex m4(4);
  std::lock_guard<Mutex> l4(m4);
}

}  // namespace
}  // namespace thread
