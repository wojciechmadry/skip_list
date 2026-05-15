#pragma once

#include "skip_list.hpp"
#include <cstdint>

struct alloc_counters {
  std::size_t allocations = 0;
  std::size_t deallocations = 0;
};

template <typename T> class counting_allocator {
public:
  using value_type = T;

  counting_allocator() noexcept = default; // tracks nothing
  explicit counting_allocator(alloc_counters *counters) noexcept
      : m_counters(counters) {}

  template <typename U>
  explicit counting_allocator(const counting_allocator<U> &other) noexcept
      : m_counters(other.m_counters) {}

  T *allocate(std::size_t n) {
    if (m_counters != nullptr) {
      m_counters->allocations += n;
    }
    return static_cast<T *>(::operator new(n * sizeof(T)));
  }

  void deallocate(T *p, std::size_t n) noexcept {
    if (m_counters != nullptr) {
      m_counters->deallocations += n;
    }
    ::operator delete(p);
  }

  template <typename U>
  bool operator==(const counting_allocator<U> &rhs) const noexcept {
    return m_counters == rhs.m_counters;
  }
  template <typename U>
  bool operator!=(const counting_allocator<U> &rhs) const noexcept {
    return !(*this == rhs);
  }

private:
  alloc_counters *m_counters = nullptr;

  template <typename U> friend class counting_allocator;
};

template <typename T, typename U>
using tracked_skip_list =
    sl::skip_list<T, std::less<T>, sl::DEFAULT_PROBABILITY,
                  sl::DEFAULT_MAX_NODE_SIZE, counting_allocator<U>>;
