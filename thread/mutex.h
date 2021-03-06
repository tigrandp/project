// Author: tigrandp@gmail.com (Tigran Hakobyan)

#include <cassert>
#include <limits>

#ifndef THREAD_MUTEX_H_
#define THREAD_MUTEX_H_

namespace thread {

// HierarchicalMutex
//
// This class is wrapper over std::mutex which allows to acquire a lock for the
// underneath mutex if all the acquisitions follow stricit hierachical order
// for current thread of execution. If mutex from higher level is acquired
// after locking mutex from lower level, program execution will be halted. The
// first lock of the current thread will always succeed.
// NOTE: user always should make sure that mutexes in hierarchy are unlocked in
// exact reverse order of locking. For example it is fine to rely on stack
// unwinding but if mutexes are placed in some containers the order should be
// preserved otherwise program will be halted.
// HierarchicalMutex class meets all the requirements of BasicLocable and can
// be used with std threading facilities.
//
// Example:
//   HierarchicalMutex h_mutex(1000);
//   HierarchicalMutex l_mutex(100);
//   std::lock_guard<HierarchicalMutex> lock1(h_mutex);
//   // Lock low level mutex after high level will succeed.
//   std::lock_guard<HierarchicalMutex> lock2(l_mutex);
//
// Example:
//   HierarchicalMutex h_mutex(1000);
//   HierarchicalMutex l_mutex(100);
//   // Note the order of locking.
//   std::lock_guard<HierarchicalMutex> lock1(l_mutex);
//   // Locking high level mutex after low level will fail !
//   std::lock_guard<HierarchicalMutex> lock2(h_mutex);
//
template<class Mutex>
class HierarchicalMutex {
 public:
  explicit HierarchicalMutex(int level)
    : current_level_(level), previous_level_(0) {}

  // Locks the mutex, if hierarchy is violated program will be halted.
  void lock();

  // Unlocks the mutex. After this call any hierarchical mutex up to the
  // previous locking level will be safe to lock.
  void unlock();

 private:
  void CheckHierarchyViolations();
  void UpdateHierarchyValue();

  Mutex m_;
  int current_level_;
  int previous_level_;
  static thread_local int this_thread_hierarchy_value_;
};

template<class Mutex>
thread_local int HierarchicalMutex<Mutex>::this_thread_hierarchy_value_(
    std::numeric_limits<int>::max());

template<class Mutex>
void HierarchicalMutex<Mutex>::CheckHierarchyViolations() {
  assert(current_level_ < HierarchicalMutex<Mutex>::this_thread_hierarchy_value_);
}

template<class Mutex>
void HierarchicalMutex<Mutex>::UpdateHierarchyValue() {
  previous_level_ = this_thread_hierarchy_value_;
  HierarchicalMutex<Mutex>::this_thread_hierarchy_value_ = current_level_;
}

template<class Mutex>
void HierarchicalMutex<Mutex>::lock() {
  CheckHierarchyViolations();
  m_.lock();
  UpdateHierarchyValue();
}

template<class Mutex>
void HierarchicalMutex<Mutex>::unlock() {
  HierarchicalMutex<Mutex>::this_thread_hierarchy_value_ = previous_level_;
  m_.unlock();
}

}  // namespace thread

#endif  // THREAD_MUTEX_H_
