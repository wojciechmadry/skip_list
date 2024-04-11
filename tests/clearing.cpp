#include <cstdlib>
#include <exception>

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

TEST(Clearing, Destructor) {}
TEST(Clearing, ClearFunction) {
  skip_list<int> sl;
  new_calls = 0;
  delete_calls = 0;
  sl.push(5);
  ASSERT_EQ(new_calls, 1);
  ASSERT_EQ(delete_calls, 0);
  sl.clear();
  ASSERT_EQ(delete_calls, new_calls);
  sl.push(3);
  ASSERT_EQ(new_calls, 2);
  ASSERT_EQ(delete_calls, 1);
  sl.push(5);
  ASSERT_EQ(new_calls, 3);
  sl.clear();
  ASSERT_EQ(delete_calls, new_calls);
}
