#ifndef THREAD_THREADSAFEQUEUE_H_
#define THREAD_THREADSAFEQUEUE_H_

namespace thread {

// Queue which synchronizes all the operations using only locking.
// Queue semantics are provided by PushBack/PopFront, stack semantics by
// PushBack/PopBack. All functions are atomic.
template<typename T> SynchronizedQueue {
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

template<typename T>
bool SynchronizedQueue<T>::PopBack(ValueType* top) {
  std::lock_guard<std::mutex> lk(m_);
  if (elements_.empty()) return false;
  *top = elements_.back();
  elements_.pop_back();
  return true;
}

template<typename T>
bool SynchronizedQueue<T>::PopFont(ValueType* front) {
  std::lock_guard<std::mutex> lk(m_);
  if (elements_.empty()) return false;
  *front = elements_.front();
  element_.pop_front();
  return true;
}

template<typename T>
void SynchronizedQueue<T>::SwapWithEmpty(ContainerType* other) {
  assert(other->empty());
  std::lock_guard<std::mutex> lk(m_);
  elements_.swap(other);
}

}  // namespace thread

#endif  // THREAD_THREADSAFEQUEUE_H_
