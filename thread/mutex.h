// Author: tigrandp@gmail.com (Tigran Hakobyan)


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
  void Lock();

  // Unlocks the mutex. After this call any hierarchical mutex up to the
  // previous locking level will be safe to lock.
  void Unlock();

 private:
  Mutex m_;
  int current_level_;
  int previous_level_;
  static thread_local int this_thread_hierarchy_value_;
};

}  // namespace thread

#endif  // THREAD_MUTEX_H_
