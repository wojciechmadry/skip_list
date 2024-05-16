#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <string_view>

namespace sl {

static constexpr std::string_view SKIP_LIST_VERSION{"1.0"};

static constexpr std::size_t DEFAULT_MAX_NODE_SIZE{5u};

static constexpr int DEFAULT_PROBABILITY{50};

template <typename T, std::size_t MaxNodeSize = DEFAULT_MAX_NODE_SIZE>
struct node {
  using size_type = std::size_t;

  template <typename U, typename Generator>
  node(U &&a_value, Generator &gen) : m_value(std::forward<U>(a_value)) {
    std::uniform_int_distribution<size_type> uniform_dist(1U, MaxNodeSize);
    m_capacity = uniform_dist(gen);
  }

  void fill_nexts() {
    auto next = m_nexts[0U];
    size_type i = 1U;
    while (next && i < capacity()) {
      while (i < next->capacity() && i < capacity()) {
        m_nexts[i++] = next;
      }
      next = (i < next->capacity()) ? next->m_nexts[i] : next->m_nexts[0U];
    }
  }

  void clear_nexts() {
    for (auto &next : m_nexts) {
      next = nullptr;
    }
  }

  size_type capacity() const noexcept { return m_capacity; }

  node *&get_next(size_type index) noexcept {
    assert(index < MaxNodeSize);
    return m_nexts[index];
  }
  const node *const &get_next(size_type index) const noexcept {
    assert(index < MaxNodeSize);
    return m_nexts[index];
  }
  T &get() noexcept { return m_value; }
  const T &get() const noexcept { return m_value; }

  auto rbegin() noexcept {
    return m_nexts.rbegin() + (MaxNodeSize - capacity());
  }

  auto rend() noexcept { return m_nexts.rend(); }

private:
  std::array<node *, MaxNodeSize> m_nexts{nullptr};
  T m_value;
  size_type m_capacity{0U};
};

// TODO(skiplist):
// Probability was changed to Int in template, because Bazel use C (clang)
// compiler to build program And when building with clang, cannot define float
// in template.
template <typename T, typename Compare = std::less<T>,
          int ProbabilityInt = DEFAULT_PROBABILITY,
          std::size_t MaxNodeSize = DEFAULT_MAX_NODE_SIZE,
          typename Allocator = std::allocator<T>>
class skip_list {
public:
  using node_type = node<T, MaxNodeSize>;

private:
  template <typename IteratorValueType = const node_type> class iterator_impl;
  using allocator_traits = typename std::allocator_traits<
      Allocator>::template rebind_traits<node_type>;
  using allocator_rebind = typename std::allocator_traits<
      Allocator>::template rebind_alloc<node_type>;

public:
  static constexpr float Probability =
      static_cast<float>(ProbabilityInt) / 100.f;
  static_assert(Probability >= 0.f && Probability < 1.0f);
  static_assert(MaxNodeSize >= 1U);
  using allocator_type = Allocator;
  using const_iterator = iterator_impl<const node_type>;
  using const_pointer =
      typename std::allocator_traits<Allocator>::const_pointer;
  using value_type = T;
  using const_reference = const value_type &;
  using difference_type = std::ptrdiff_t;
  using iterator = const_iterator;
  using pointer = typename std::allocator_traits<Allocator>::pointer;
  using reference = value_type &;
  using size_type = std::size_t;

  skip_list() = default;

  explicit skip_list(const Compare &comp, const Allocator &alloc)
      : m_comparator(comp), m_allocator(alloc) {}

  skip_list(size_type count, const T &value, const Compare &comp = Compare(),
            const Allocator &alloc = Allocator())
      : m_comparator(comp), m_allocator(alloc) {
    for (size_type i{0U}; i < count; ++i) {
      push(value);
    }
  }

  explicit skip_list(size_type count, const Compare &comp = Compare(),
                     const Allocator &alloc = Allocator())
      : m_comparator(comp), m_allocator(alloc) {
    for (size_type i{0U}; i < count; ++i) {
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

  skip_list(skip_list &&other, const Allocator &alloc) noexcept
      : m_allocator(alloc) {
    *this = std::move(other);
  }

  skip_list(skip_list &&other) noexcept
      : skip_list(std::move(other), Allocator{}) {}

  skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator())
      : skip_list(init.begin(), init.end(), alloc) {}

  ~skip_list() noexcept {
    try {
      clear();
    } catch (...) {
    }
  }

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
    other.m_size = 0U;
    m_comparator = std::move(other.m_comparator);
    m_generator = std::move(other.m_generator);
    m_allocator = std::move(other.m_allocator);
    return *this;
  }

  void swap(skip_list &other) noexcept {
    if (this == &other) {
      return;
    }
    std::swap(m_head, other.m_head);
    std::swap(m_tail, other.m_tail);
    std::swap(m_size, other.m_size);
    std::swap(m_allocator, other.m_allocator);
    std::swap(m_comparator, other.m_comparator);
    std::swap(m_generator, other.m_generator);
  }

  skip_list &operator=(std::initializer_list<T> ilist) {
    clear_elements();
    std::copy(ilist.begin(), ilist.end(), std::inserter(*this, begin()));
    return *this;
  }

  void assign(size_type count, const T &value) {
    clear_elements();
    while (count-- > 0U) {
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

  const_reference front() const noexcept {
    assert(m_head != nullptr);
    return m_head->get();
  };

  const_reference back() const noexcept {
    assert(m_tail != nullptr);
    return m_tail->get();
  }

  iterator begin() const noexcept { return iterator(m_head); }
  const_iterator cbegin() const noexcept { return const_iterator{m_head}; }

  iterator end() const noexcept { return iterator{nullptr}; }
  const_iterator cend() const noexcept { return const_iterator{nullptr}; }

  size_type size() const noexcept { return m_size; };
  bool empty() const noexcept { return size() == 0U; };
  difference_type max_size() const noexcept {
    return std::numeric_limits<difference_type>::max();
  };

  void clear() {
    m_comparator = Compare();
    m_allocator = Allocator();
    m_generator = std::mt19937{std::random_device{}()};
    clear_elements();
  }

  template <typename U>
  iterator emplace(U &&value, size_type *visited_nodes_counter = nullptr) {
    if (visited_nodes_counter) {
      *visited_nodes_counter = 0U;
    }
    auto new_node = create_node(std::forward<U>(value));
    ++m_size;
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
      for (size_type i{0U}; i < m_head->capacity() && i < m_tail->capacity();
           ++i) {
        m_head->get_next(i) = m_tail;
      }
      return iterator(new_node);
    };
    if (m_comparator(new_node->get(), m_head->get())) {
      new_node->get_next(0U) = m_head;
      std::swap(m_head, new_node);
      m_head->fill_nexts();
      return iterator(m_head);
    }
    bool found_place{false};
    node_type *nd{m_head};
    size_type visited{0U};
    while (found_place == false) {
      bool looped{false};
      auto level{static_cast<difference_type>(nd->capacity())};
      for (auto it{nd->rbegin()}; it != nd->rend(); ++it, --level) {
        if (*it == nullptr) {
          if (new_node->capacity() >= level) {
            *it = new_node;
          }
          continue;
        }
        if (m_comparator((*it)->get(), new_node->get())) {
          nd = *it;
          looped = true;
          break;
        } else if (level > 1 && new_node->capacity() >= level) {
          nd->get_next(static_cast<size_type>(level - 1)) = new_node;
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
      m_tail->get_next(0U) = new_node;
      m_tail->fill_nexts();
      std::swap(m_tail, new_node);
      return iterator(m_tail);
    }

    new_node->get_next(0U) = nd->get_next(0U);
    new_node->fill_nexts();
    nd->get_next(0U) = new_node;
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

  template <typename U>
  iterator push(U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <typename U>
  iterator insert(U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <typename InputIt, typename U>
    requires(!std::is_same_v<typename std::iterator_traits<InputIt>::value_type,
                             void>)
  iterator insert(InputIt, U &&value,
                  size_type *visited_nodes_counter = nullptr) {
    return emplace(std::forward<U>(value), visited_nodes_counter);
  }

  template <typename InputIt>
    requires(!std::is_same_v<typename std::iterator_traits<InputIt>::value_type,
                             void>)
  void insert(InputIt first, InputIt last) {
    while (first != last) {
      emplace(*first);
      ++first;
    }
  }

  template <class SeedSeq> void set_seed(SeedSeq &seed) {
    m_generator.seed(seed);
  }

  void merge(skip_list &other) {
    if (this == &other) {
      return;
    }
    auto it{other.m_head};
    while (it != nullptr) {
      push(std::move(it->get()));
      it = it->get_next(0U);
    }
    other.clear_elements();
  }

  void merge(skip_list &&other) {
    auto movedList = std::move(other);
    merge(movedList);
  }

  const_iterator find(const T &key) const noexcept {
    if (value_out_of_range(key)) {
      return cend();
    }
    if (m_head != nullptr && m_head->get() == key) {
      return cbegin();
    }
    for (auto it{m_head}; it != nullptr;) {
      auto rbegin{it->rbegin()};
      const auto rend{it->rend()};
      for (; rbegin != rend; ++rbegin) {
        if (*rbegin == nullptr) {
          continue;
        }
        const auto &value{(*rbegin)->get()};
        if (value == key) {
          return const_iterator{*rbegin};
        } else if (m_comparator(value, key)) {
          it = *rbegin;
          break;
        }
      }
      if (it != nullptr && rbegin == it->rend()) {
        break;
      }
    }
    return cend();
  }

  void pop_back() { erase(m_tail->get()); }

  void pop_front() { erase(m_head->get()); }

  std::optional<T> extract(const T &key) {
    auto node_ptr{erase_node(key)};
    if (node_ptr) {
      return std::move(node_ptr->get());
    }
    return {};
  }

  std::optional<T> extract(const_iterator position) {
    if (position != end()) {
      return extract(*position);
    }
    return {};
  }

  const_iterator erase(const T &key) {
    auto node_ptr{erase_node(key)};
    return node_ptr ? const_iterator{node_ptr->get_next(0)}
                    : const_iterator{cend()};
  }

  const_iterator lower_bound(const T &key) const {
    return bound_impl<std::greater_equal<T>>(key);
  }

  const_iterator upper_bound(const T &key) const {
    return bound_impl<std::greater<T>>(key);
  }

private:
  template <typename U> node_type *create_node(U &&value) {
    allocator_rebind allocator;
    node_type *result = allocator.allocate(1);
    allocator_traits::construct(allocator, result,
                                node_type(std::forward<U>(value), m_generator));
    return result;
  }

  void delete_node(node_type *&node_ptr) noexcept {
    allocator_rebind allocator;
    allocator_traits::destroy(allocator, node_ptr);
    allocator_traits::deallocate(allocator, node_ptr, 1);
    node_ptr = nullptr;
  }

  std::unique_ptr<node_type> erase_node(const T &key) {
    if (empty() || value_out_of_range(key)) {
      return nullptr;
    }
    if (m_head != nullptr && m_head->get() == key) {
      if (m_head == m_tail) {
        m_tail = nullptr;
      }
      --m_size;
      auto next{m_head->get_next(0U)};
      auto old_head{m_head};
      m_head = next;
      return std::unique_ptr<node_type>(old_head);
    }

    std::array<node_type *, MaxNodeSize> observers{nullptr};
    auto obs{observers.begin()};
    node_type *found{nullptr};
    for (auto it{m_head}; it != nullptr;) {
      auto rbegin{it->rbegin()};
      const auto rend{it->rend()};
      for (; rbegin != rend; ++rbegin) {
        if (*rbegin == nullptr) {
          continue;
        }
        const auto &value{(*rbegin)->get()};
        if (value == key) {
          *(obs++) = it;
          found = *rbegin;
        } else if (m_comparator(value, key)) {
          it = *rbegin;
          break;
        }
      }
      if (it != nullptr && rbegin == it->rend()) {
        break;
      }
    }
    if (found == nullptr) {
      return nullptr;
    }
    --m_size;
    for (auto o : observers) {
      if (o == nullptr) {
        continue;
      }
      for (auto rit{o->rbegin()}; rit != o->rend(); ++rit) {
        if (*rit == found) {
          *rit = nullptr;
        }
      }
      if (o->get_next(0) == nullptr) {
        o->get_next(0) = found->get_next(0);
      }
    }
    if (found == m_tail) {
      m_tail = *(obs - 1);
    }
    for (auto o : observers) {
      if (o != nullptr) {
        o->fill_nexts();
      }
    }
    return std::unique_ptr<node_type>(found);
  }

  bool value_out_of_range(const T &value) const noexcept {
    return m_tail != nullptr && (m_comparator(m_tail->get(), value) ||
                                 m_comparator(value, m_head->get()));
  }

  void clear_elements() noexcept {
    m_size = 0U;
    if (m_head == nullptr) {
      return;
    }
    if (m_head == m_tail) {
      delete_node(m_head);
      m_tail = nullptr;
      return;
    }
    while (m_head != m_tail) {
      auto next{m_head->get_next(0U)};
      delete_node(m_head);
      m_head = next;
    }
    delete_node(m_tail);
    m_head = nullptr;
  }

  template <typename Comparator> const_iterator bound_impl(const T &key) const {
    if (empty()) {
      return {};
    }
    Comparator cmp{};
    auto it{m_head};
    while (it != nullptr) {
      if (cmp(it->get(), key)) {
        return const_iterator{it};
      }
      size_type last_gt{1U};
      for (; last_gt < it->capacity(); ++last_gt) {
        const auto next = it->get_next(last_gt);
        if (!next || cmp(next->get(), key)) {
          break;
        }
      }
      it = it->get_next(last_gt - 1U);
    }
    return {};
  }

  node_type *m_head{nullptr};
  node_type *m_tail{nullptr};
  size_type m_size{0U};
  Allocator m_allocator;
  Compare m_comparator;
  std::mt19937 m_generator{std::random_device{}()};

  template <typename IteratorValueType> class iterator_impl {
  public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    iterator_impl() noexcept = default;
    explicit iterator_impl(IteratorValueType *val_ptr) noexcept
        : m_it(val_ptr) {}

    const value_type &operator*() const noexcept {
      assert(m_it != nullptr);
      return m_it->get();
    }

    const value_type *operator->() const noexcept {
      assert(m_it != nullptr);
      return &m_it->get();
    }

    iterator_impl &operator++() noexcept {
      assert(m_it != nullptr);
      m_it = m_it->get_next(0U);
      return *this;
    }
    iterator_impl operator++(int) noexcept {
      auto copy{*this};
      assert(m_it != nullptr);
      m_it = m_it->get_next(0U);
      return copy;
    }
    bool operator==(const iterator_impl &rhs) const {
      return m_it == rhs.m_it;
    };

    bool operator==(std::nullptr_t) const { return m_it == nullptr; }

  private:
    IteratorValueType *m_it{nullptr};
  };
};

template <typename T, typename CompareLhs, typename CompareRhs, int ProbLhs,
          int ProbRhs, std::size_t MaxNodeSizeLhs, std::size_t MaxNodeSizeRhs,
          typename Allocator = std::allocator<T>>
bool operator==(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, Allocator> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, Allocator>
        &rhs) noexcept {
  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename CompareLhs, typename CompareRhs, int ProbLhs,
          int ProbRhs, std::size_t MaxNodeSizeLhs, std::size_t MaxNodeSizeRhs,
          typename Allocator = std::allocator<T>>
auto operator<=>(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, Allocator> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, Allocator>
        &rhs) noexcept {
  return std::lexicographical_compare_three_way(
      lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend(),
      [](const T &lhs_l, const T &rhs_l) { return lhs_l <=> rhs_l; });
}

} // namespace sl
#endif // SKIP_LIST_SKIP_LIST_HPP_
