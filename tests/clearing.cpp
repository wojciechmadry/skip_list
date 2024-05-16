#include <cstdlib>
#include <exception>
#include <new>
#include <random>

#include "skip_list.hpp"
#include <gtest/gtest.h>

// NOLINTBEGIN
static std::size_t new_calls = 0;
static std::size_t delete_calls = 0;

// no inline, required by [replacement.functions]/3
void *operator new(std::size_t sz) {
  if (sz == 0) {
    ++sz; // avoid std::malloc(0) which may return nullptr on success
  }
  new_calls++;
  if (void *ptr = std::malloc(sz)) {
    return ptr;
  }

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void *operator new[](std::size_t sz) {
  if (sz == 0) {
    ++sz; // avoid std::malloc(0) which may return nullptr on success
  }
  new_calls++;
  if (void *ptr = std::malloc(sz)) {
    return ptr;
  }

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void operator delete(void *ptr) noexcept {
  delete_calls++;
  std::free(ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept {
  (void)size;
  delete_calls++;
  std::free(ptr);
}

void operator delete[](void *ptr) noexcept {
  delete_calls++;
  std::free(ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept {
  (void)size;
  delete_calls++;
  std::free(ptr);
}

using namespace sl;

TEST(Clearing, Destructor) {
  static constexpr std::size_t NEW_SIZE = 500u;
  std::seed_seq seed{555};
  delete_calls = 0;
  new_calls = 0;
  {
    skip_list<int> sl;
    ASSERT_NO_THROW(sl.set_seed(seed));
    for (std::size_t i = 1u; i <= NEW_SIZE; ++i) {
      ASSERT_NO_THROW(sl.push(i));
      ASSERT_EQ(new_calls, i);
    }
    ASSERT_EQ(delete_calls, 0);
  }
  ASSERT_EQ(delete_calls, NEW_SIZE);
}

TEST(Clearing, ClearFunction) {
  skip_list<int> sl;
  std::seed_seq seed{333};
  ASSERT_NO_THROW(sl.set_seed(seed));
  new_calls = 0;
  delete_calls = 0;
  ASSERT_NO_THROW(sl.push(5));
  ASSERT_EQ(new_calls, 1);
  ASSERT_EQ(delete_calls, 0);
  ASSERT_EQ(sl.size(), 1);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_EQ(delete_calls, new_calls);
  ASSERT_NO_THROW(sl.push(3));
  ASSERT_EQ(new_calls, 2);
  ASSERT_EQ(delete_calls, 1);
  ASSERT_EQ(sl.size(), 1);
  ASSERT_NO_THROW(sl.push(5));
  ASSERT_EQ(sl.size(), 2);
  ASSERT_EQ(new_calls, 3);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_EQ(delete_calls, new_calls);
  static constexpr std::size_t NEW_SIZE = 10u;
  for (std::size_t i = 0u; i < NEW_SIZE; ++i) {
    ASSERT_NO_THROW(sl.push(i));
    ASSERT_EQ(new_calls, 4u + i);
  }
  ASSERT_EQ(new_calls, 13u);
  ASSERT_EQ(delete_calls, 3u);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(delete_calls, new_calls);
}

TEST(Clearing, ClearFunction1000Elements) {
  skip_list<int> sl;
  std::seed_seq seed{500};
  ASSERT_NO_THROW(sl.set_seed(seed));
  new_calls = 0;
  delete_calls = 0;
  static constexpr std::size_t NEW_SIZE = 1000u;
  for (std::size_t i = 1u; i <= NEW_SIZE; ++i) {
    ASSERT_NO_THROW(sl.push(i));
    ASSERT_EQ(new_calls, i);
  }
  ASSERT_EQ(new_calls, NEW_SIZE);
  ASSERT_EQ(delete_calls, 0u);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(delete_calls, new_calls);
}

TEST(Clearing, ClearFunctionRandomSeed) {
  for (int i = 0; i <= 300; ++i) {
    skip_list<int> sl;
    std::seed_seq seed{i};
    ASSERT_NO_THROW(sl.set_seed(seed));
    new_calls = 0;
    delete_calls = 0;
    static constexpr std::size_t NEW_SIZE = 10u;
    for (std::size_t j = 1u; j <= NEW_SIZE; ++j) {
      ASSERT_NO_THROW(sl.push(j));
      ASSERT_EQ(new_calls, j);
    }
    ASSERT_EQ(new_calls, NEW_SIZE);
    ASSERT_EQ(delete_calls, 0u);
    ASSERT_NO_THROW(sl.clear());
    ASSERT_EQ(delete_calls, new_calls);
  }
}

TEST(Clearing, ClearErase) {
  skip_list<int> sl;
  std::seed_seq seed{500};
  ASSERT_NO_THROW(sl.set_seed(seed));
  new_calls = 0;
  delete_calls = 0;
  static constexpr std::size_t NEW_SIZE = 1000u;
  for (std::size_t i = 1u; i <= NEW_SIZE; ++i) {
    ASSERT_NO_THROW(sl.push(i));
    ASSERT_EQ(new_calls, i);
  }
  ASSERT_EQ(new_calls, NEW_SIZE);
  ASSERT_EQ(delete_calls, 0u);
  std::size_t deleted_calls = 1u;
  for (std::size_t i = NEW_SIZE / 2; i <= NEW_SIZE; ++i) {
    ASSERT_NO_THROW(sl.erase(static_cast<int>(i)));
    ASSERT_EQ(delete_calls, deleted_calls++);
  }
  for (std::size_t i = 1; i < NEW_SIZE / 2; ++i) {
    ASSERT_NO_THROW(sl.erase(static_cast<int>(i)));
    ASSERT_EQ(delete_calls, deleted_calls++);
  }
  ASSERT_EQ(delete_calls, new_calls);
}

TEST(Clearing, ClearEraseInsert) {
  skip_list<int> sl;
  std::seed_seq seed{500};
  ASSERT_NO_THROW(sl.set_seed(seed));
  ASSERT_NO_THROW(sl.push({1, 4, 7, 8, 9, 15, 25, 30}));
  const std::vector<int> elems{1, 4, 7, 8, 9, 15, 25, 30};
  new_calls = 0;
  delete_calls = 0;
  std::size_t new_counter{0u};
  std::size_t delete_counter{0u};
  for (std::size_t i = 0; i < 100; ++i) {
    for (auto el : elems) {
      auto found = sl.find(el);
      ASSERT_NE(found, nullptr);
      ASSERT_EQ(*found, el);
      ASSERT_NO_THROW(sl.erase(el));
      ++delete_counter;
      ASSERT_EQ(delete_counter, delete_calls);
      ASSERT_EQ(sl.find(el), nullptr);
      ASSERT_NO_THROW(sl.push(el));
      ++new_counter;
      ASSERT_EQ(new_counter, new_calls);
    }
  }
  auto it = sl.begin();
  for (auto el : elems) {
    ASSERT_NE(it, sl.end());
    ASSERT_EQ(*it, el);
    ++it;
  }
}

TEST(Clearing, ExceptionInClear) {
  struct custom_comp {
    bool operator()(const int &lhs, const int &rhs) { return lhs > rhs; }
    custom_comp operator=(custom_comp &&) {
      throw std::runtime_error("runtime error");
    }
  };
  auto testThrow = []() {
    skip_list<int, custom_comp> sl;
    ASSERT_NO_THROW(sl.push({1, 2, 3}));
    ASSERT_ANY_THROW(sl.clear());
  };
  ASSERT_NO_THROW(testThrow());
}
// NOLINTEND
