#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <cassert>
#include <cstddef>
#include <functional>
#include <random>
#include <string_view>

namespace sl {

static constexpr std::string_view SKIP_LIST_VERSION{"1.0"};

template <typename T, std::size_t MaxNodeSize = 5u,
          typename Allocator = std::allocator<T>>
struct node {
  using size_type = std::size_t;
  template <typename U = T, typename Generator>
  node(U &&a_value, Generator &gen) {
    Allocator alloc;
    using traits = std::allocator_traits<Allocator>;
    traits::construct(alloc, &m_value, a_value);
    std::uniform_int_distribution<size_type> uniform_dist(1, MaxNodeSize);
    m_size = uniform_dist(gen);
  }

  void fill_nexts() {
    if (m_nexts[0] == nullptr) {
      return;
    }
    size_type i = 1u;
    auto next = m_nexts[0];
    while (i < m_size && next) {
      size_type j = i;
      for (; j < next->m_size && j < m_size; ++j) {
        m_nexts[j] = next;
        ++i;
      }
      next = next->m_nexts[0];
    }
  };

  void clear_nexts() {
    for (auto &next : m_nexts) {
      next = nullptr;
    }
  };

  size_type size() const noexcept { return m_size; }
  node *&get_next(size_type index) {
    assert(index < MaxNodeSize);
    return m_nexts[index];
  }
  const node *const &get_next(size_type index) const {
    assert(index < MaxNodeSize);
    return m_nexts[index];
  }
  T &get() { return m_value; }
  const T &get() const { return m_value; }

  auto rbegin() { return m_nexts.rbegin() + (MaxNodeSize - m_size); }

  auto rend() { return m_nexts.rend(); }

private:
  std::array<node *, MaxNodeSize> m_nexts{nullptr};
  T m_value;
  size_type m_size = 0u;
};

// TODO:
// Probability was changed to Int in template, because Bazel use C (clang)
// compiler to build program And when building with clang, cannot define float
// in template.
template <typename T, typename Compare = std::less<T>, int ProbabilityInt = 50,
          std::size_t MaxNodeSize = 5u, typename Allocator = std::allocator<T>>
class skip_list {
public:
  using node_type = node<T, MaxNodeSize, Allocator>;

private:
  template <typename IteratorValueType = node_type> class iterator_impl;

public:
  static constexpr float Probability =
      static_cast<float>(ProbabilityInt) / 100.f;
  static_assert(Probability >= 0.f && Probability < 1.0f);
  static_assert(MaxNodeSize >= 1);
  using value_type = T;
  using allocator_type = Allocator;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = value_type &;
  using const_reference = const value_type &;
  using pointer = std::allocator_traits<Allocator>::pointer;
  using const_pointer = std::allocator_traits<Allocator>::const_pointer;
  using iterator = iterator_impl<>;
  using const_iterator = iterator_impl<const node_type>;

  ~skip_list() noexcept { clear(); }

  const_reference front() const {
    assert(m_head != nullptr);
    return m_head->get();
  };

  const_reference back() const {
    assert(m_tail != nullptr);
    return m_tail->get();
  }

  iterator begin() { return iterator(m_head); }
  const_iterator begin() const { return const_iterator{m_head}; }
  const_iterator cbegin() const { return const_iterator{m_head}; }

  iterator end() { return iterator{nullptr}; }
  const_iterator end() const { return const_iterator{nullptr}; }
  const_iterator cend() const { return const_iterator{nullptr}; }

  size_type size() const noexcept { return m_size; };
  bool empty() const noexcept { return size() == 0; };

  void clear() noexcept {
    m_size = 0u;
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
      auto next = m_head->get_next(0);
      delete m_head;
      m_head = next;
    }
    delete m_tail;
    m_head = nullptr;
    m_tail = nullptr;
  }

  template <typename U = T>
  iterator emplace(U &&value, size_type *visited_nodes_counter = nullptr) {
    if (visited_nodes_counter) {
      *visited_nodes_counter = 0u;
    }
    ++m_size;
    auto new_node = create_node(std::forward<U>(value));
    if (m_head == nullptr) {
      m_head = new_node;
      m_tail = m_head;
      return iterator(m_head);
    }
    if (m_head == m_tail) {
      if (m_comparator(value, m_head->get())) {
        m_head = new_node;
        m_tail->clear_nexts();
      } else {
        m_tail = new_node;
        m_head->clear_nexts();
      }
      for (size_type i = 0u; i < m_head->size() && i < m_tail->size(); ++i) {
        m_head->get_next(i) = m_tail;
      }
      return iterator(new_node);
    };
    if (m_comparator(value, m_head->get())) {
      new_node->get_next(0) = m_head;
      std::swap(m_head, new_node);
      m_head->fill_nexts();
      return iterator(m_head);
    }
    bool found_place = false;
    node_type *nd = m_head;
    size_type visited = 0u;
    while (found_place == false) {
      bool looped = false;
      difference_type level = static_cast<difference_type>(nd->size());
      for (auto it = nd->rbegin(); it != nd->rend(); ++it, --level) {
        if (*it == nullptr) {
          if (new_node->size() >= level) {
            *it = new_node;
          }
          continue;
        }
        if (m_comparator((*it)->get(), value)) {
          nd = *it;
          looped = true;
          break;
        } else if (level > 1 && new_node->size() >= level) {
          nd->get_next(level - 1) = new_node;
        }
      }
      if (looped == false) {
        found_place = true;
      }
      ++visited;
    }
    if (visited_nodes_counter) {
      *visited_nodes_counter = visited;
    }
    if (nd == m_tail) {
      m_tail->get_next(0) = new_node;
      m_tail->fill_nexts();
      std::swap(m_tail, new_node);
      return iterator(m_tail);
    }

    new_node->get_next(0) = nd->get_next(0);
    new_node->fill_nexts();
    nd->get_next(0) = new_node;
    nd->fill_nexts();
    return iterator(new_node);
  }

  template <typename U = T>
  iterator push(U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <typename U = T>
  iterator insert(U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <typename Iter, typename U = T>
  iterator insert(Iter, U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <class SeedSeq> void set_seed(SeedSeq &seed) {
    m_generator.seed(seed);
  }

private:
  template <typename U = T> node_type *create_node(U &&value) {
    return new node_type(std::forward<U>(value), m_generator);
  }

  node_type *m_head = nullptr;
  node_type *m_tail = nullptr;
  size_type m_size = 0u;
  Allocator m_allocator;
  Compare m_comparator;
  std::mt19937 m_generator{std::random_device{}()};

  template <typename IteratorValueType> class iterator_impl {
  public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    iterator_impl() = default;
    iterator_impl(IteratorValueType *val_ptr) : m_it(val_ptr) {}

    const value_type &operator*() const {
      assert(m_it != nullptr);
      return m_it->get();
    }
    iterator_impl &operator++() {
      assert(m_it != nullptr);
      m_it = m_it->get_next(0);
    }
    iterator_impl operator++(int) {
      auto copy = *this;
      assert(m_it != nullptr);
      m_it = m_it->get_next(0);
      return copy;
    }
    bool operator==(const iterator_impl &rhs) const {
      return m_it == rhs.m_it;
    };

  private:
    IteratorValueType *m_it;
  };
};

} // namespace sl
#endif // SKIP_LIST_SKIP_LIST_HPP_
