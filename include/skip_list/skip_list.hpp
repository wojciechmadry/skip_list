#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <cassert>
#include <cstddef>
#include <functional>
#include <iterator>

static constexpr const char* SKIP_LIST_VERSION = "1.0";

template <typename T, typename Compare = std::less<T>, float Probability = 0.5f,
          std::size_t MaxNodeSize = 5, typename Allocator = std::allocator<T>>
class skip_list {
  class node;
  template <typename IteratorValueType = node>
  class iterator_impl;

 public:
  static_assert(Probability >= 0.f && Probability < 1.0f);
  static_assert(MaxNodeSize >= 1);
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

  ~skip_list() noexcept { clear(); }
  reference front() {
    assert(m_head != nullptr);
    return m_head->value;
  };
  const_reference front() const {
    assert(m_head != nullptr);
    return m_head->value;
  };

  reference back() {
    assert(m_tail != nullptr);
    return m_tail->value;
  }
  const_reference back() const {
    assert(m_tail != nullptr);
    return m_tail->value;
  }

  iterator begin() { return iterator(m_head); }
  const_iterator begin() const { return const_iterator{m_head}; }
  const_iterator cbegin() const { return const_iterator{m_head}; }

  iterator end() { return iterator{m_tail + 1}; }
  const_iterator end() const { return const_iterator{m_tail + 1}; }
  const_iterator cend() const { return const_iterator{m_tail + 1}; }

  size_type size() const noexcept { return m_size; };
  bool empty() const noexcept { return size() == 0; };

  void clear() noexcept {
    if (m_head == nullptr) {
      return;
    }
    if (m_head == m_tail) {
      delete m_head;
      m_head = nullptr;
      m_tail = nullptr;
      return;
    }
    while (m_head != m_tail) {
      auto next = m_head->nexts[0];
      delete m_head;
      m_head = next;
    }
    delete m_tail;
    m_head = nullptr;
    m_tail = nullptr;
  }

  template <typename U = T>
  void emplace(U&& value) {
    if (m_head == nullptr) {
      m_head = new node(std::forward<U>(value), m_allocator);
      m_head->nexts[0] = m_tail;
      m_tail = m_head;
      return;
    }
    if (m_head == m_tail) {
      if (m_comparator(value, m_head->value)) {
        m_head = new node(std::forward<U>(value), m_allocator);
      } else {
        m_tail = new node(std::forward<U>(value), m_allocator);
      }
      m_head->nexts[0] = m_tail;
      m_head->size = 1;
      return;
    };
    /*node* place = m_head;
    for (difference_type i = static_cast<difference_type>(place->size); i >= 0;
         --i) {
      if (place->nexts[i] && m_comparator(place->nexts[i]->value, value)) {
      }
    }*/
  }

  template <typename U = T>
  void push(U&& value) {
    emplace(std::forward<U>(value));
  }

 private:
  struct node {
    template <typename U = T>
    node(U&& a_value, Allocator alloc) {
      using traits = std::allocator_traits<Allocator>;
      traits::construct(alloc, &value, a_value);
    }
    std::array<node*, MaxNodeSize> nexts;
    T value;
    size_type size = 0u;
  };

  node* m_head = nullptr;
  node* m_tail = nullptr;
  size_type m_size = 0u;
  Allocator m_allocator;
  Compare m_comparator;

  template <typename IteratorValueType>
  class iterator_impl {
   public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    iterator_impl() = default;
    iterator_impl(IteratorValueType* val_ptr) : m_value(val_ptr) {}

    value_type& operator*() const {
      assert(m_value != nullptr);
      return m_value->value;
    }
    decltype(auto) operator*() {
      assert(m_value != nullptr);
      return m_value->value;
    }
    iterator_impl& operator++() {
      assert(m_value != nullptr);
      m_value = m_value->nexts[0];
    }
    iterator_impl operator++(int) {
      auto copy = *this;
      assert(m_value != nullptr);
      m_value = m_value->nexts[0];
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

   private:
    IteratorValueType* m_value;
  };
};

#endif  // SKIP_LIST_SKIP_LIST_HPP_

