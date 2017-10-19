// Author: tigrandp@gmail.com (Tigran Hakobyan)
//
// Heap is a tree like data structure on array which allows to insert node into
// the tree in O(log(n)) time, and querying minimum element in O(1), where n is
// the number of elements which are currently in the heap.
//
// Implementation details:
//   - Internally vector has been used to store the elements of the heap instead
//   of pointers for efficiency.
//   - In constrast of STL heap, it provides iterator mechanism for more
//   efficient element removal.
#ifndef DS_HEAP_H_
#define DS_HEAP_H_

#include <iterator>
#include <vector>

namespace ds {

template<typename T,
         typename Comparator = std::less<T>,
         typename Vector = std::vector<T>>
class Heap {
 public:
  typedef T ValueType;
  typedef T& Reference;
  typedef const T& ConstReference;
  typedef typename Vector::iterator Iterator;
  typedef typename Vector::const_iterator ConstIterator;

  // Creates empty heap.
  Heap();
  // Creates heap from elements in range [begin, end). The given order of
  // elements is not required to form a heap. Complexity is linear for the
  // number of elements in the given range.
  template<typename InputIterator>
  Heap(InputIterator begin, InputIterator end);
  // Creates heap from the given elements. The given order of elements is not
  // required to form a heap. Complexity is linear for the number of elements
  // provided.
  template<typename... Ts> Heap(Ts&&... args);

  // Main mechanism used for storing and retrieving data in the heap.
  bool Empty() const;
  std::size_t Size() const;
  ConstReference Top() const;
  void Pop();
  void Clear();

  // Sets a maximum size for the heap; see notes on push. Note that the heap's
  // size may be larger than this if it was already larger when set was called
  // or if assign is called with a range larger than this.
  void SetMax(int size);

  // Pops the top element of the heap and returns it.
  ValueType ConsumeTop();

  // If maximum number of elements in the heap is not configured or number of
  // elements in the heap is less than the maximum number of elements, then this
  // operation will always return true.
  //
  // Otherwise, if the item compares less than the top element, pops the top
  // item, adds this item, and returns true.
  //
  // Otherwise, does nothing and returns false.
  bool Push(const ValueType& item);
  // Just as above but instead of copying element into the heap it moves.
  bool Push(ValueType&& item);
  // Just as above but instead of copying/moving it creates element in place.
  template<typename... Ts>
  ValueType& Emplace(Ts&&... args);

  // Replace elements in the heap with the ones in new elements. After the
  // operation, new_elements will contain the old heap elements, and the order
  // of elements in the heap will be restored.
  void ReplaceElements(Vector* new_elements);

  // Replace the contents of the heap by the elements in [begin, end) and
  // rebuilds the heap.
  template<typename InputIterator>
  void Assign(InputIterator begin, InputIterator end);

  // Remove the element from the heap.
  void Remove(Iterator pos);

  // Rebuild a heap base on the given comparator.
  void Rebuild();

  // Iterators:
  //  If value of the element has been changed the Rebuild should be called to
  //  restore the invariant of the heap.
  Iterator Begin();
  Iterator End();

  ConstIterator Begin() const;
  ConstIterator End() const;

 private:
  // Recursively restores the heap order, as if the three was rooted at the
  // given index.
  void Heapify(int index);
  // Returns the left child of the given node.
  int Left(int index) { return (index << 1) | 1; }
  // Returns the rigth child of the given node.
  inline int Rigth(int index) { return (index + 1) << 1; }

  int max_size_;
  Vector heap_;
  Comparator comparator_;
};

template<typename T, typename Comparator, typename Vector>
Heap<T, Comparator, Vector>::Heap() : max_size_(-1) {}

template<typename T, typename Comparator, typename Vector>
template<typename InputIterator>
Heap<T, Comparator, Vector>::Heap(InputIterator begin, InputIterator end) {
  heap_.clear();
  heap_.reserve(std::distance(begin, end));
  for (InputIterator it = begin; it != end; ++it) {
    heap_.push_back(*it);
  }
  Rebuild();
}

template<typename T, typename Comparator, typename Vector>
template<typename... Ts> Heap<T, Comparator, Vector>::Heap(Ts&&... args) {
  heap_.clear();
  heap_ = {std::forward<Ts>(args)...};
  Rebuild();
}

template<typename T, typename Comparator, typename Vector>
void Heap<T, Comparator, Vector>::Rebuild() {
  int num_elements = heap_.size();
  for (int i = num_elements / 2; i >= 0; --i) {
    Heapify(i);
  }
}

template<typename T, typename Comparator, typename Vector>
void Heap<T, Comparator, Vector>::Heapify(int index) {
  int largest_index = index;
  int left_child_index = Left(index);
  if (left_child_index < heap_.size() &&
      !comparator_(heap_.at(largest_index), heap_.at(left_child_index))) {
    largest_index = left_child_index;
  }
  int rigth_child_index = Rigth(index);
  if (rigth_child_index < heap_.size() &&
      !comparator_(heap_.at(largest_index), heap_.at(rigth_child_index))) {
    largest_index = rigth_child_index;
  }
  using std::swap;
  if (largest_index != index) {
    swap(heap_.at(largest_index), heap_.at(index));
    Heapify(largest_index);
  }
}

template<typename T, typename Comparator, typename Vector>
typename Heap<T, Comparator, Vector>::Iterator
Heap<T, Comparator, Vector>::Begin() {
  return heap_.begin();
}

template<typename T, typename Comparator, typename Vector>
typename Heap<T, Comparator, Vector>::Iterator
Heap<T, Comparator, Vector>::End() {
  return heap_.end();
}

template<typename T, typename Comparator, typename Vector>
typename Heap<T, Comparator, Vector>::ConstIterator
Heap<T, Comparator, Vector>::Begin() const {
  return heap_.begin();
}

template<typename T, typename Comparator, typename Vector>
typename Heap<T, Comparator, Vector>::ConstIterator
Heap<T, Comparator, Vector>::End() const {
  return heap_.end();
}

}  // namespace ds

#endif  // DS_HEAP_H_
