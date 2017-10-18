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
         typename Container = std::vector<T>>
class Heap {
 public:
  typedef T ValueType;
  typedef T& Reference;
  typedef const T& ConstReference;
  typedef typename Container::iterator Iterator;
  typedef typename Container::const_iterator ConstIterator;

  // Creates empty heap.
  Heap();
  // Creates heap from elements in range [begin, end). The given order of
  // elements is not required to form a heap. Complexity is linear for the
  // number of elements in the given range.
  template<typename InputIterator>
  Heap(InputIterator begin, InputIterator end);

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
  void ReplaceElements(Container* new_elements);

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
  Container heap_;
};

}  // namespace ds

#endif  // DS_HEAP_H_
