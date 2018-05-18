#ifndef THREAD_THREADSAFEQUEUE_H_
#define THREAD_THREADSAFEQUEUE_H_

#include <condition_variable>
#include <deque>
#include <mutex>

namespace thread {

// Queue which synchronizes all the operations using only locking.
// Queue semantics are provided by PushBack/PopFront, stack semantics by
// PushBack/PopBack. All functions are atomic.
template <typename T>
class SynchronizedQueue {
 public:
  using ValueType = T;
  using ContainerType = std::deque<T>;

  bool Empty() const {
    std::lock_guard<std::mutex> lk(m_);
    return elements_.empty();
  }

  std::size_t Size() const {
    std::lock_guard<std::mutex> lk(m_);
    return elements_.size();
  }

  void PushBack(const ValueType& elem) {
    std::lock_guard<std::mutex> lk(m_);
    elements_.push_back(elem);
  }

  void PushFront(const ValueType& elem) {
    std::lock_guard<std::mutex> lk(m_);
    elements_.push_front(elem);
  }

  // If queue is empty returns false, otherwise returns true and stores the top
  // element in the provided output.
  bool PopBack(ValueType* top);

  // If queue is empty returns false, otherwise returns true and store the front
  // element in the provided output.
  bool PopFront(ValueType* front);

  // Swaps the content of the queue with the empty container, effective way to
  // retrieve the content of the queue.
  void SwapWithEmpty(ContainerType* other);

 private:
  std::mutex m_;
  ContainerType elements_;
};

// Thread safe queue which allows waiting for elements to appear or to be
// consumed. All the operations are atomic.
template <typename T>
class WaitSyncQueue {
 public:
  using ValueType = T;
  using ContainerType = std::deque<T>;
  using SizeType = typename ContainerType::size_type;

  WaitSyncQueue() {}

  bool Empty() const {
    std::lock_guard<std::mutex> lk(beasy_);
    return elements_.empty();
  }

  SizeType Size() const {
    std::lock_guard<std::mutex> lk(beasy_);
    return elements_.size();
  }

  // Sets the maximal number of elements that queue can hold, by default value
  // is inifinity. If the queue is full and some insertion was requeusted
  // thread will be blocked until the queue becomes unfull again.
  void SetMaxQueueSize(SizeType new_max_queue_size) {
    std::lock_guard<std::mutex> lk(beasy_);
    max_queue_size_ = new_max_queue_size;
  }

  SizeType MaxQueueSize() const {
    std::lock_guard<std::mutex> lk(beasy_);
    return max_queue_size_;
  }

  // Appends the given element to the end of the container, if container
  // already reached its maximum capacity thread will wait until some of the
  // elements is consumed, if during the wait period waiters stop was requested
  // it will return false otherwise true.
  bool PushBack(const ValueType& value);

  // Inserts the given element to the beginning of the container, if container
  // is already reached it maximum capacity thread will wait until some of the
  // elements is consumed, if during the wait period waiters stop was requested
  // it will return false otherwise true.
  bool PushFront(const ValueType& value);

  // Removes the element from the back of the container and copies that value
  // to the given output, if queue is empty then thread will be blocked until
  // some elements appear, if during the wait period waiters stop was requested
  // it will return false otherwise true.
  bool PopBack(ValueType* result);

  // Removes the element from the front of the container and copies that value
  // to the given output, if queue is empty then thread will be blocked until
  // some elements appear, if during the wait period waiters stop was requested
  // it will return false otherwise true.
  bool PopFront(ValueType* result);

  // Effective way of copying the content of the queue.
  void SwapWithEmpty(ContainerType* other);

  // Stops all the waiters.
  void StopAllWaiters();

 private:
  void PushUnsafe(const ValueType& value, bool push_to_back) {
    if (push_to_back) {
      elements_.push_back(value);
    } else {
      elements_.push_front(value);
    }
  }

  void PopUnsafe(ValueType* result, bool pop_from_back) {
    if (pop_from_back) {
      *result = elements_.back();
      elements_.pop_back();
    } else {
      *result = elements_.front();
      elements_.pop_front();
    }
  }

  bool PopInternal(ValueType* result, bool pop_from_back);
  bool PushInternal(const ValueType& value, bool push_to_back);

  mutable std::mutex beasy_;
  ContainerType elements_;

  std::condition_variable queue_not_full_;
  std::condition_variable queue_not_empty_;

  bool stop_requested_ = false;
  // Since size type is unsigned this will end up being infinity.
  SizeType max_queue_size_ = -1;
};

template<typename T>
void WaitSyncQueue<T>::StopAllWaiters() {
  std::lock_guard<std::mutex> lk(beasy_);
  stop_requested_ = true;
  queue_not_full_.notify_all();
  queue_not_empty_.notify_all();
}

template<typename T>
void WaitSyncQueue<T>::SwapWithEmpty(ContainerType* other) {
  assert(other != nullptr);
  assert(other->empty());
  std::lock_guard<std::mutex> lk(beasy_);
  elements_.swap(*other);
  queue_not_full_.notify_all();
}

template <typename T>
bool WaitSyncQueue<T>::PopInternal(ValueType* result, bool pop_from_back) {
  std::unique_lock<std::mutex> lk(beasy_);
  while (elements_.empty()) {
    queue_not_empty_.wait(lk);
    if (stop_requested_) return false;
  }

  PopUnsafe(result, pop_from_back);
  if (elements_.size() < max_queue_size_) {
    queue_not_full_.notify_one();
  }
  return true;
}

template <typename T>
bool WaitSyncQueue<T>::PopBack(ValueType* result) {
  return PopInternal(result, true /* pop_from_back */);
}

template <typename T>
bool WaitSyncQueue<T>::PopFront(ValueType* result) {
  return PopInternal(result, false /* pop_from_back */);
}

template <typename T>
bool WaitSyncQueue<T>::PushInternal(const ValueType& value, bool push_to_back) {
  std::unique_lock<std::mutex> lk(beasy_);
  while (elements_.size() >= max_queue_size_) {
    queue_not_full_.wait(lk);
    if (stop_requested_) return false;
  }

  if (elements_.empty()) queue_not_empty_.notify_one();
  PushUnsafe(value, push_to_back);
  return true;
}

template <typename T>
bool WaitSyncQueue<T>::PushBack(const ValueType& value) {
  return PushInternal(value, true /* push_to_back */);
}

template <typename T>
bool WaitSyncQueue<T>::PushFront(const ValueType& value) {
  return PushInternal(value, false /* push_to_back */);
}

template <typename T>
bool SynchronizedQueue<T>::PopBack(ValueType* top) {
  std::lock_guard<std::mutex> lk(m_);
  if (elements_.empty()) return false;
  *top = elements_.back();
  elements_.pop_back();
  return true;
}

template <typename T>
bool SynchronizedQueue<T>::PopFront(ValueType* front) {
  std::lock_guard<std::mutex> lk(m_);
  if (elements_.empty()) return false;
  *front = elements_.front();
  elements_.pop_front();
  return true;
}

template <typename T>
void SynchronizedQueue<T>::SwapWithEmpty(ContainerType* other) {
  assert(other->empty());
  std::lock_guard<std::mutex> lk(m_);
  elements_.swap(*other);
}

}  // namespace thread

#endif  // THREAD_THREADSAFEQUEUE_H_
