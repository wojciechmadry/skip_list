#include <cstdlib>
#include <exception>
#include <new>
#include <random>
static std::size_t new_calls = 0;
static std::size_t delete_calls = 0;
// no inline, required by [replacement.functions]/3
void *operator new(std::size_t sz) {
  if (sz == 0)
    ++sz; // avoid std::malloc(0) which may return nullptr on success
  new_calls++;
  if (void *ptr = std::malloc(sz))
    return ptr;

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void *operator new[](std::size_t sz) {
  if (sz == 0)
    ++sz; // avoid std::malloc(0) which may return nullptr on success
  new_calls++;
  if (void *ptr = std::malloc(sz))
    return ptr;

  throw std::bad_alloc{}; // required by [new.delete.single]/3
}

void operator delete(void *ptr) noexcept {
  delete_calls++;
  std::free(ptr);
}

void operator delete(void *ptr, std::size_t size) noexcept {
  delete_calls++;
  std::free(ptr);
}

void operator delete[](void *ptr) noexcept {
  delete_calls++;
  std::free(ptr);
}

void operator delete[](void *ptr, std::size_t size) noexcept {
  delete_calls++;
  std::free(ptr);
}

#include <gtest/gtest.h>

#include "skip_list.hpp"
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
    for (std::size_t i = 1u; i <= NEW_SIZE; ++i) {
      ASSERT_NO_THROW(sl.push(i));
      ASSERT_EQ(new_calls, i);
    }
    ASSERT_EQ(new_calls, NEW_SIZE);
    ASSERT_EQ(delete_calls, 0u);
    ASSERT_NO_THROW(sl.clear());
    ASSERT_EQ(delete_calls, new_calls);
  }
}
