#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <cstddef>
#include <functional>
#include <iterator>

template <typename T, typename Compare = std::less<T>, float Probability = 0.5f,
          std::size_t MaxSize = 5, typename Allocator = std::allocator<T>>
class skip_list {
  class node;
  template <typename IteratorValueType = node>
  class iterator_impl;
  template <typename IteratorValueType = node>
  class reverse_iterator_impl;

 public:
  static_assert(Probability >= 0.f && Probability < 1.0f);
  static_assert(MaxSize >= 1);
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;
  using iterator = iterator_impl<>;
  using const_iterator = iterator_impl<const node>;
  using reverse_iterator = reverse_iterator_impl<>;
  using const_reverse_iterator = reverse_iterator_impl<const node>;

 private:
  struct node {
    struct bi_dir {
      node* next = nullptr;
      node* prev = nullptr;
    };
    std::array<bi_dir, MaxSize> nexts;
    size_type size = 0u;
    T value;
  };

  node* m_head = nullptr;
  node* m_tail = nullptr;
  size_type m_size = 0u;

  template <typename IteratorValueType>
  class iterator_impl {
   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    value_type& operator*() const { return m_value->value; }
    value_type& operator*() { return m_value->value; }
    iterator_impl& operator++() { m_value = m_value->nexts[0].next; }
    iterator_impl operator++(int) {
      auto copy = *this;
      m_value = m_value->nexts[0].next;
      return copy;
    }
    bool operator==(const iterator_impl& rhs) const {
      if (m_value == rhs.m_value) {
        return true;
      }
      if (m_value == nullptr || rhs.m_value == nullptr) {
        return false;
      }
      return m_value->value == rhs.m_value->value;
    };
    iterator_impl& operator--() { m_value = m_value->nexts[0].prev; }
    iterator_impl operator--(int) {
      auto copy = *this;
      m_value = m_value->nexts[0].prev;
      return copy;
    }

   private:
    IteratorValueType* m_value;
  };

  template <typename IteratorValueType>
  class reverse_iterator_impl {
   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    value_type& operator*() const { return m_value->value; }
    value_type& operator*() { return m_value->value; }
    reverse_iterator_impl& operator++() { m_value = m_value->nexts[0].prev; }
    reverse_iterator_impl operator++(int) {
      auto copy = *this;
      m_value = m_value->nexts[0].prev;
      return copy;
    }
    bool operator==(const reverse_iterator_impl& rhs) const {
      if (m_value == rhs.m_value) {
        return true;
      }
      if (m_value == nullptr || rhs.m_value == nullptr) {
        return false;
      }
      return m_value->value == rhs.m_value->value;
    };
    reverse_iterator_impl& operator--() { m_value = m_value->nexts[0].next; }
    reverse_iterator_impl operator--(int) {
      auto copy = *this;
      m_value = m_value->nexts[0].next;
      return copy;
    }

   private:
    IteratorValueType* m_value;
  };

  static_assert(std::bidirectional_iterator<iterator>);
  static_assert(std::bidirectional_iterator<reverse_iterator>);
};

#endif  // SKIP_LIST_SKIP_LIST_HPP_

