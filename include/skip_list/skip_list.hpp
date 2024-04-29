#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
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
    traits::construct(alloc, &m_value, std::forward<U>(a_value));
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
  template <typename IteratorValueType = const node_type> class iterator_impl;

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
  using const_iterator = iterator_impl<const node_type>;
  using iterator = const_iterator;

  skip_list() = default;

  explicit skip_list(const Compare &comp, const Allocator &alloc)
      : m_comparator(comp), m_allocator(alloc) {}

  skip_list(size_type count, const T &value, const Compare &comp = Compare(),
            const Allocator &alloc = Allocator())
      : m_comparator(comp), m_allocator(alloc) {
    for (size_type i = 0u; i < count; ++i) {
      push(value);
    }
  }

  explicit skip_list(size_type count, const Compare &comp = Compare(),
                     const Allocator &alloc = Allocator())
      : m_comparator(comp), m_allocator(alloc) {
    for (size_type i = 0u; i < count; ++i) {
      push(T{});
    }
  }

  template <class InputIt>
    requires(!std::is_same_v<typename std::iterator_traits<InputIt>::value_type,
                             void>)
  skip_list(InputIt first, InputIt last, const Allocator &alloc = Allocator())
      : m_allocator(alloc) {
    std::copy(first, last, std::inserter(*this, begin()));
  }

  skip_list(const skip_list &other, const Allocator &alloc)
      : m_allocator(alloc) {
    *this = other;
  }

  skip_list(const skip_list &other) : skip_list(other, Allocator{}) {}

  skip_list(skip_list &&other, const Allocator &alloc) : m_allocator(alloc) {
    *this = std::move(other);
  }

  skip_list(skip_list &&other) : skip_list(std::move(other), Allocator{}) {}

  skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator())
      : skip_list(init.begin(), init.end(), alloc) {}

  ~skip_list() noexcept { clear(); }

  skip_list &operator=(const skip_list &other) {
    if (this == &other) {
      return *this;
    }
    clear();
    m_comparator = other.m_comparator;
    m_generator = other.m_generator;
    m_allocator = other.m_allocator;
    std::copy(other.begin(), other.end(), std::inserter(*this, begin()));
    return *this;
  }

  skip_list &operator=(skip_list &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    m_head = other.m_head;
    other.m_head = nullptr;
    m_tail = other.m_tail;
    other.m_tail = nullptr;
    m_size = other.m_size;
    other.m_size = 0u;
    m_comparator = std::move(other.m_comparator);
    m_generator = std::move(other.m_generator);
    m_allocator = std::move(other.m_allocator);
    return *this;
  }

  skip_list &operator=(std::initializer_list<T> ilist) {
    clear_elements();
    std::copy(ilist.begin(), ilist.end(), std::inserter(*this, begin()));
  }

  void assign(size_type count, const T &value) {
    clear_elements();
    for (size_type c = 0u; c < count; ++c) {
      push(value);
    }
  }

  template <class InputIt>
    requires(!std::is_same_v<typename std::iterator_traits<InputIt>::value_type,
                             void>)
  void assign(InputIt first, InputIt last) {
    clear_elements();
    std::copy(first, last, std::inserter(*this, begin()));
  }

  void assign(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
  }

  allocator_type get_allocator() const noexcept { return m_allocator; }

  const_reference front() const {
    assert(m_head != nullptr);
    return m_head->get();
  };

  const_reference back() const {
    assert(m_tail != nullptr);
    return m_tail->get();
  }

  iterator begin() const { return iterator(m_head); }
  const_iterator cbegin() const { return const_iterator{m_head}; }

  iterator end() const { return iterator{nullptr}; }
  const_iterator cend() const { return const_iterator{nullptr}; }

  size_type size() const noexcept { return m_size; };
  bool empty() const noexcept { return size() == 0; };
  difference_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max();
  };

  void clear() noexcept {
    m_comparator = Compare();
    m_allocator = Allocator();
    m_generator = std::mt19937{std::random_device{}()};
    clear_elements();
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
      if (m_comparator(new_node->get(), m_head->get())) {
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
    if (m_comparator(new_node->get(), m_head->get())) {
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
        if (m_comparator((*it)->get(), new_node->get())) {
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

  void push(std::initializer_list<T> ilist) {
    for (const auto &el : ilist) {
      emplace(el);
    }
  }

  void emplace(std::initializer_list<T> ilist) {
    for (const auto &el : ilist) {
      emplace(el);
    }
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

  void merge(skip_list &other) {
    if (this == &other) {
      return;
    }
    auto it = other.m_head;
    while (it != nullptr) {
      push(std::move(it->get()));
      it = it->get_next(0);
    }
    other.clear_elements();
  }

  void merge(skip_list &&other) { merge(other); }

  const_iterator find(const T &key) const {
    if (m_tail != nullptr && (m_comparator(m_tail->get(), key) ||
                              m_comparator(key, m_head->get()))) {
      return cend();
    }
    if (m_head != nullptr && m_head->get() == key) {
      return cbegin();
    }
    auto it = m_head;
    while (it != nullptr) {
      auto rbegin = it->rbegin();
      auto rend = it->rend();
      while (rbegin != rend) {
        if (*rbegin == nullptr) {
          ++rbegin;
          continue;
        }
        const auto &value = (*rbegin)->get();
        if (value == key) {
          return const_iterator{*rbegin};
        } else if (m_comparator(value, key)) {
          it = *rbegin;
          break;
        }
        ++rbegin;
      }
      if (rbegin == rend) {
        break;
      }
    }
    return cend();
  }

  const_iterator erase(const T &key) {
    if (empty()) {
      return cend();
    }
    if (m_tail != nullptr && (m_comparator(m_tail->get(), key) ||
                              m_comparator(key, m_head->get()))) {
      return cend();
    }
    if (m_head != nullptr && m_head->get() == key) {
      if (m_head == m_tail) {
        m_tail = nullptr;
      }
      --m_size;
      auto next = m_head->get_next(0);
      delete m_head;
      m_head = next;
      return cbegin();
    }

    std::array<node_type *, MaxNodeSize> observers{nullptr};
    auto obs = observers.begin();
    node_type *found = nullptr;
    auto it = m_head;
    while (it != nullptr) {
      auto rbegin = it->rbegin();
      auto rend = it->rend();
      while (rbegin != rend) {
        if (*rbegin == nullptr) {
          ++rbegin;
          continue;
        }
        const auto &value = (*rbegin)->get();
        if (value == key) {
          *(obs++) = it;
          found = *rbegin;
        } else if (m_comparator(value, key)) {
          it = *rbegin;
          break;
        }
        ++rbegin;
      }
      if (rbegin == rend) {
        break;
      }
    }
    if (found != nullptr) {
      --m_size;
      auto result = found->get_next(0);
      for (auto o : observers) {
        if (o == nullptr) {
          continue;
        }
        for (auto rit = o->rbegin(); rit != o->rend(); ++rit) {
          if (*rit == found) {
            *rit = nullptr;
          }
        }
        if (o->get_next(0) == nullptr) {
          o->get_next(0) = result;
        }
      }
      if (found == m_tail) {
        m_tail = *(obs - 1);
      }
      delete found;
      for (auto o : observers) {
        if (o != nullptr) {
          o->fill_nexts();
        }
      }
      return const_iterator{result};
    }

    return cend();
  }

private:
  template <typename U = T> node_type *create_node(U &&value) {
    return new node_type(std::forward<U>(value), m_generator);
  }

  void clear_elements() noexcept {
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

template <typename T, typename CompareLhs, typename CompareRhs, int ProbLhs,
          int ProbRhs, std::size_t MaxNodeSizeLhs, std::size_t MaxNodeSizeRhs,
          typename Allocator = std::allocator<T>>
bool operator==(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, Allocator> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, Allocator> &rhs) {
  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename CompareLhs, typename CompareRhs, int ProbLhs,
          int ProbRhs, std::size_t MaxNodeSizeLhs, std::size_t MaxNodeSizeRhs,
          typename Allocator = std::allocator<T>>
auto operator<=>(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, Allocator> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, Allocator> &rhs) {
  return std::lexicographical_compare_three_way(
      lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
      [](const T &lhs_l, const T &rhs_l) { return lhs_l <=> rhs_l; });
}

} // namespace sl
#endif // SKIP_LIST_SKIP_LIST_HPP_
