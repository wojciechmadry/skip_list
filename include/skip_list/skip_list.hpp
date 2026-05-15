#ifndef SKIP_LIST_SKIP_LIST_HPP_
#define SKIP_LIST_SKIP_LIST_HPP_

#include <algorithm>
#include <array>
#include <cassert>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <string_view>

namespace sl {

static constexpr std::string_view SKIP_LIST_VERSION{"1.0"};

static constexpr std::size_t DEFAULT_MAX_NODE_SIZE{5U};

static constexpr int DEFAULT_PROBABILITY{50};

template <typename T, std::size_t MaxNodeSize = DEFAULT_MAX_NODE_SIZE>
struct node {
  using size_type = std::size_t;

  template <typename... Args>
  explicit node(const size_type cap, Args &&...args)
      : m_value(std::forward<Args>(args)...),
        m_capacity(std::max<std::size_t>(std::min(cap, MaxNodeSize), 1)) {}

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

  [[nodiscard]] size_type capacity() const noexcept { return m_capacity; }

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
  size_type m_capacity{1U};
};

template <typename T, typename Compare = std::less<T>,
          int ProbabilityInt = DEFAULT_PROBABILITY,
          std::size_t MaxNodeSize = DEFAULT_MAX_NODE_SIZE,
          typename Allocator = std::allocator<T>>
class skip_list {
public:
  using node_type = node<T, MaxNodeSize>;

private:
  template <typename IteratorValueType = const node_type> class iterator_impl;
  using node_allocator = typename std::allocator_traits<
      Allocator>::template rebind_alloc<node_type>;
  using allocator_traits = std::allocator_traits<node_allocator>;

public:
  static constexpr float Probability =
      static_cast<float>(ProbabilityInt) / 100.F;
  static_assert(Probability >= 0.F && Probability < 1.0F);
  static_assert(MaxNodeSize >= 1U);
  using allocator_type = Allocator;
  using const_iterator = iterator_impl<const node_type>;
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

  skip_list(const skip_list &other)
      : skip_list(other,
                  allocator_traits::select_on_container_copy_construction(
                      other.m_allocator)) {}

  skip_list(skip_list &&other, const Allocator &alloc) noexcept
      : m_allocator(alloc) {
    *this = std::move(other);
  }

  skip_list(skip_list &&other) noexcept
      : skip_list(
            std::move(other),
            allocator_traits::propagate_on_container_move_assignment::value
                ? std::move(other.m_allocator)
                : allocator_traits::select_on_container_copy_construction(
                      other.m_allocator)) {}

  skip_list(std::initializer_list<T> init, const Allocator &alloc = Allocator())
      : skip_list(init.begin(), init.end(), alloc) {}

  ~skip_list() noexcept { clear(); }

  void clear() noexcept {
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
      auto next{deref(m_head).get_next(0U)};
      delete_node(m_head);
      m_head = next;
    }
    delete_node(m_tail);
    m_head = nullptr;
  }

  skip_list &operator=(const skip_list &other) {
    if (this == &other) {
      return *this;
    }
    clear();
    m_allocator = allocator_traits::select_on_container_copy_construction(
        other.m_allocator);
    std::copy(other.begin(), other.end(), std::inserter(*this, begin()));
    return *this;
  }

  skip_list &operator=(skip_list &&other) noexcept {
    if (this == &other) {
      return *this;
    }
    clear();
    m_allocator =
        allocator_traits::propagate_on_container_move_assignment::value
            ? std::move(other.m_allocator)
            : allocator_traits::select_on_container_copy_construction(
                  other.m_allocator);
    m_head = other.m_head;
    other.m_head = nullptr;
    m_tail = other.m_tail;
    other.m_tail = nullptr;
    m_size = other.m_size;
    other.m_size = 0U;
    return *this;
  }

  void swap(skip_list &other) noexcept {
    if (this == &other) {
      return;
    }
    std::swap(m_head, other.m_head);
    std::swap(m_tail, other.m_tail);
    std::swap(m_size, other.m_size);
    std::swap(m_comparator, other.m_comparator);
    std::swap(m_generator, other.m_generator);
    if constexpr (allocator_traits::propagate_on_container_swap::value) {
      std::swap(m_allocator, other.m_allocator);
    }
  }

  skip_list &operator=(std::initializer_list<T> ilist) {
    clear();
    std::copy(ilist.begin(), ilist.end(), std::inserter(*this, begin()));
    return *this;
  }

  void assign(size_type count, const T &value) {
    clear();
    while (count-- > 0U) {
      push(value);
    }
  }

  template <class InputIt>
    requires(!std::is_same_v<typename std::iterator_traits<InputIt>::value_type,
                             void>)
  void assign(InputIt first, InputIt last) {
    clear();
    std::copy(first, last, std::inserter(*this, begin()));
  }

  void assign(std::initializer_list<T> ilist) {
    assign(ilist.begin(), ilist.end());
  }

  allocator_type get_allocator() const noexcept {
    return allocator_type{m_allocator};
  }

  const_reference front() const noexcept { return deref(m_head).get(); };

  const_reference back() const noexcept { return deref(m_tail).get(); }

  iterator begin() const noexcept { return iterator(m_head); }
  const_iterator cbegin() const noexcept { return const_iterator{m_head}; }

  iterator end() const noexcept { return iterator{nullptr}; }
  const_iterator cend() const noexcept { return const_iterator{nullptr}; }

  [[nodiscard]] size_type size() const noexcept { return m_size; };
  [[nodiscard]] bool empty() const noexcept { return size() == 0U; };
  [[nodiscard]] size_type max_size() const noexcept {
    return std::numeric_limits<size_type>::max();
  };

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
  iterator insert(InputIt /*unused*/, U &&value,
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

  template <class SeedSeq> void set_seed(SeedSeq &&seed) {
    m_generator.seed(std::forward<SeedSeq>(seed));
  }

  void merge(skip_list &other) {
    if (this == &other) {
      return;
    }
    auto it{other.m_head};
    while (it != nullptr) {
      auto &nd = deref(it);
      push(std::move(nd.get()));
      it = nd.get_next(0U);
    }
    other.clear();
  }

  void merge(skip_list &&other) { merge(other); }

  template <typename... Args> iterator emplace(Args &&...args) {
    return emplace_impl(nullptr, std::forward<Args>(args)...);
  }

  template <typename U>
    requires(std::is_convertible_v<U, T> || std::is_constructible_v<T, U>)
  iterator emplace(U &&value, size_type *visited_nodes_counter = nullptr) {
    return emplace_impl(visited_nodes_counter, std::forward<U>(value));
  }

  const_iterator find(const T &key) const {
    if (value_out_of_range(key)) {
      return cend();
    }
    if (m_head != nullptr && equal(deref(m_head).get(), key)) {
      return cbegin();
    }
    for (auto it{m_head}; it != nullptr;) {
      auto rbegin{it->rbegin()};
      const auto rend{it->rend()};
      for (; rbegin != rend; ++rbegin) {
        if (*rbegin == nullptr) {
          continue;
        }
        const auto &value{deref(*rbegin).get()};
        if (equal(value, key)) {
          return const_iterator{*rbegin};
        }
        if (m_comparator(value, key)) {
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

  void pop_back() {
    assert(m_tail != nullptr);
    extract(const_iterator{m_tail});
  }

  void pop_front() { erase(deref(m_head).get()); }

  std::optional<T> extract(const T &key) {
    auto node_ptr{erase_node(key)};
    if (node_ptr) {
      return std::move(deref(node_ptr).get());
    }
    return {};
  }

  std::optional<T> extract(const_iterator position) {
    if (position == cend() || empty()) {
      return {};
    }
    const auto &addr = std::addressof(deref(position));
    if (addr == std::addressof(deref(m_head).get())) {
      return extract(deref(m_head).get());
    }
    node_type *head{m_head};
    node_type *second_to_last{nullptr};
    node_type *found{nullptr};
    for (; head != nullptr; head = deref(head).get_next(0)) {
      const auto &cap = deref(head).capacity();
      if (deref(head).get_next(0) == m_tail) {
        second_to_last = head;
      }
      for (std::size_t i{0U}; i < cap; ++i) {
        auto &next = deref(head).get_next(i);
        if (next && std::addressof(deref(next).get()) == addr) {
          found = next;
          next = deref(next).get_next(i);
        }
      }
    }
    std::optional<T> result;
    if (found) {
      if (found == m_tail) {
        m_tail = second_to_last;
      }
      result = std::move(deref(found).get());
      delete_node(found);
      --m_size;
    }
    return result;
  }

  const_iterator erase(const T &key) {
    auto node_ptr{erase_node(key)};
    return node_ptr ? const_iterator{deref(node_ptr).get_next(0)}
                    : const_iterator{cend()};
  }

  const_iterator lower_bound(const T &key) const {
    return bound_impl(key, [this](const auto &lhs, const auto &rhs) {
      return !m_comparator(lhs, rhs);
    });
  }

  const_iterator upper_bound(const T &key) const {
    return bound_impl(key, [this](const auto &lhs, const auto &rhs) {
      return m_comparator(rhs, lhs);
    });
  }

private:
  bool equal(const T &lhs, const T &rhs) const {
    return !m_comparator(lhs, rhs) && !m_comparator(rhs, lhs);
  }
  std::size_t generate_capacity() {
    std::binomial_distribution<std::size_t> dist(MaxNodeSize - 1, Probability);
    return 1U + dist(m_generator);
  }

  template <typename... Args> node_type *create_node(Args &&...args) {
    node_type *result = allocator_traits::allocate(m_allocator, 1);
    try {
      allocator_traits::construct(m_allocator, result, generate_capacity(),
                                  std::forward<Args>(args)...);
    } catch (...) {
      allocator_traits::deallocate(m_allocator, result, 1);
      throw;
    }
    return result;
  }

  void delete_node(node_type *&node_ptr) noexcept {
    allocator_traits::destroy(m_allocator, node_ptr);
    allocator_traits::deallocate(m_allocator, node_ptr, 1);
    node_ptr = nullptr;
  }

  auto erase_node(const T &key) {
    auto deleter = [this](auto *ptr) { this->delete_node(ptr); };
    using handler = std::unique_ptr<node_type, decltype(deleter)>;

    if (empty() || value_out_of_range(key)) {
      return handler{nullptr, deleter};
    }
    if (m_head != nullptr && equal(deref(m_head).get(), key)) {
      if (m_head == m_tail) {
        m_tail = nullptr;
      }
      --m_size;
      auto next{deref(m_head).get_next(0U)};
      auto old_head{m_head};
      m_head = next;
      return handler{old_head, deleter};
    }

    std::array<node_type *, MaxNodeSize> observers{nullptr};
    auto obs{observers.begin()};
    node_type *found{nullptr};
    for (auto it{m_head}; it != nullptr;) {
      auto rbegin{deref(it).rbegin()};
      const auto rend{deref(it).rend()};
      for (; rbegin != rend; ++rbegin) {
        if (*rbegin == nullptr) {
          continue;
        }
        const auto &value{deref(*rbegin).get()};
        if (equal(value, key)) {
          deref(obs) = it;
          obs++;
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
      return handler{nullptr, deleter};
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
      if (deref(o).get_next(0) == nullptr) {
        deref(o).get_next(0) = deref(found).get_next(0);
      }
    }
    if (found == m_tail) {
      m_tail = deref(obs - 1);
    }
    for (auto o : observers) {
      if (o != nullptr) {
        o->fill_nexts();
      }
    }
    return handler{found, deleter};
  }

  template <typename... Args>
  iterator emplace_impl(size_type *visited_nodes_counter, Args &&...args) {
    if (visited_nodes_counter) {
      *visited_nodes_counter = 0U;
    }
    auto new_node = create_node(std::forward<Args>(args)...);
    ++m_size;
    if (m_head == nullptr) {
      m_head = new_node;
      m_tail = m_head;
      return iterator(m_head);
    }
    if (m_head == m_tail) {
      if (m_comparator(deref(new_node).get(), deref(m_head).get())) {
        m_head = new_node;
        deref(m_tail).clear_nexts();
      } else {
        m_tail = new_node;
        deref(m_head).clear_nexts();
      }
      const auto head_cap = deref(m_head).capacity();
      const auto tail_cap = deref(m_tail).capacity();
      for (size_type i{0U}; i < head_cap && i < tail_cap; ++i) {
        deref(m_head).get_next(i) = m_tail;
      }
      return iterator(new_node);
    };
    if (m_comparator(deref(new_node).get(), deref(m_head).get())) {
      deref(new_node).get_next(0U) = m_head;
      std::swap(m_head, new_node);
      deref(m_head).fill_nexts();
      return iterator(m_head);
    }
    bool found_place{false};
    node_type *nd{m_head};
    size_type visited{0U};
    while (!found_place) {
      bool looped{false};
      auto level{static_cast<difference_type>(deref(nd).capacity())};
      for (auto it{nd->rbegin()}; it != nd->rend(); ++it, --level) {
        if (*it == nullptr) {
          if (deref(new_node).capacity() >= level) {
            *it = new_node;
          }
          continue;
        }
        if (m_comparator(deref(*it).get(), deref(new_node).get())) {
          nd = *it;
          looped = true;
          break;
        }
        if (level > 1 && deref(new_node).capacity() >= level) {
          nd->get_next(static_cast<size_type>(level - 1)) = new_node;
        }
      }
      if (!looped) {
        found_place = true;
      }
      ++visited;
    }
    if (visited_nodes_counter) {
      *visited_nodes_counter = visited;
    }
    if (nd == m_tail) {
      deref(m_tail).get_next(0U) = new_node;
      deref(m_tail).fill_nexts();
      std::swap(m_tail, new_node);
      return iterator(m_tail);
    }

    deref(new_node).get_next(0U) = deref(nd).get_next(0U);
    deref(new_node).fill_nexts();
    deref(nd).get_next(0U) = new_node;
    deref(nd).fill_nexts();
    return iterator(new_node);
  }

  bool value_out_of_range(const T &value) const {
    return m_tail != nullptr && (m_comparator(deref(m_tail).get(), value) ||
                                 m_comparator(value, deref(m_head).get()));
  }

  const_iterator bound_impl(const T &key, const auto &cmp) const {
    if (empty()) {
      return {};
    }
    auto it{m_head};
    while (it != nullptr) {
      if (cmp(deref(it).get(), key)) {
        return const_iterator{it};
      }
      size_type last_gt{1U};
      const auto cap = deref(it).capacity();
      for (; last_gt < cap; ++last_gt) {
        const auto next = deref(it).get_next(last_gt);
        if (!next || cmp(deref(next).get(), key)) {
          break;
        }
      }
      it = it->get_next(last_gt - 1U);
    }
    return {};
  }

  auto &deref(auto &ptr) noexcept {
    assert(ptr != nullptr);
    return *ptr;
  }

  const auto &deref(const auto &ptr) const noexcept {
    assert(ptr != nullptr);
    return *ptr;
  }

  node_type *m_head{nullptr};
  node_type *m_tail{nullptr};
  size_type m_size{0U};
  node_allocator m_allocator;
  Compare m_comparator;
  std::mt19937 m_generator{std::random_device{}()};

  template <typename IteratorValueType> class iterator_impl {
  public:
    using difference_type = ptrdiff_t;
    using value_type = T;
    using reference = T &;
    using iterator_category = std::forward_iterator_tag;
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
          typename AllocatorLhs = std::allocator<T>,
          typename AllocatorRhs = std::allocator<T>>
inline auto operator==(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, AllocatorLhs> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, AllocatorRhs>
        &rhs) {
  return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template <typename T, typename CompareLhs, typename CompareRhs, int ProbLhs,
          int ProbRhs, std::size_t MaxNodeSizeLhs, std::size_t MaxNodeSizeRhs,
          typename AllocatorLhs = std::allocator<T>,
          typename AllocatorRhs = std::allocator<T>>
inline auto operator<=>(
    const skip_list<T, CompareLhs, ProbLhs, MaxNodeSizeLhs, AllocatorLhs> &lhs,
    const skip_list<T, CompareRhs, ProbRhs, MaxNodeSizeRhs, AllocatorRhs>
        &rhs) {
  return std::lexicographical_compare_three_way(lhs.cbegin(), lhs.cend(),
                                                rhs.cbegin(), rhs.cend());
}

} // namespace sl
#endif // SKIP_LIST_SKIP_LIST_HPP_
