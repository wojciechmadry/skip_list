#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <type_traits>

#include "skip_list.hpp"

using namespace sl;

TEST(Initialization, ClassCreation) {
  ASSERT_NO_THROW(skip_list<int>{});
  ASSERT_NO_THROW(skip_list<std::string>{});
  ASSERT_NO_THROW(skip_list<std::unique_ptr<int>>{});
  ASSERT_NO_THROW((skip_list<std::unique_ptr<int>, std::greater<>, 25, 10,
                             std::allocator<std::unique_ptr<int>>>{}));
}

TEST(Initialization, Typedefs) {
  skip_list<int> sl;
  ASSERT_TRUE((std::is_same_v<int, decltype(sl)::value_type>));
  ASSERT_TRUE(
      (std::is_same_v<std::allocator<int>, decltype(sl)::allocator_type>));
  ASSERT_TRUE((std::is_unsigned_v<decltype(sl)::size_type>));
  ASSERT_TRUE((std::is_signed_v<decltype(sl)::difference_type>));
  ASSERT_TRUE((std::is_same_v<int &, decltype(sl)::reference>));
  ASSERT_TRUE((std::is_same_v<const int &, decltype(sl)::const_reference>));
  ASSERT_TRUE((std::is_same_v<int *, decltype(sl)::pointer>));
  ASSERT_TRUE((std::is_same_v<const int *, decltype(sl)::const_pointer>));
  ASSERT_TRUE(
      (std::is_same_v<decltype(sl)::iterator, decltype(sl)::const_iterator>));
}

TEST(Initialization, EmptyList) {
  skip_list<int> sl;
  const skip_list<int> const_sl;
  ASSERT_NO_THROW(sl.begin());
  ASSERT_NO_THROW(const_sl.begin());
  ASSERT_NO_THROW(const_sl.cbegin());
  ASSERT_NO_THROW(sl.end());
  ASSERT_NO_THROW(const_sl.end());
  ASSERT_NO_THROW(const_sl.cend());
  ASSERT_TRUE(sl.empty());
  ASSERT_TRUE(const_sl.empty());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_EQ(const_sl.size(), 0);
  ASSERT_EQ(const_sl.max_size(),
            std::numeric_limits<skip_list<int>::difference_type>::max());
}

TEST(Initialization, ConstructorsSimple) {
  skip_list<int> sl_def;
  ASSERT_EQ(sl_def.begin(), nullptr);
  ASSERT_EQ(sl_def.end(), nullptr);
  ASSERT_EQ(sl_def.size(), 0u);
  std::allocator<int> alloc;
  std::less<int> comp;
  skip_list<int> sl_alloc(comp, alloc);
  const auto is_same_allocator =
      std::is_same_v<decltype(sl_alloc.get_allocator()),
                     skip_list<int>::allocator_type>;
  ASSERT_TRUE(is_same_allocator);
  skip_list<int> sl_count_n(10, 5);
  ASSERT_EQ(sl_count_n.size(), 10u);
  ASSERT_EQ(sl_count_n.front(), 5);
  ASSERT_EQ(sl_count_n.back(), 5);
  {
    std::size_t count = 0u;
    for (auto v : sl_count_n) {
      ASSERT_EQ(v, 5);
      ++count;
    }
    ASSERT_EQ(count, 10);
  }
  skip_list<int> sl_count(7);
  ASSERT_EQ(sl_count.size(), 7u);
  ASSERT_EQ(sl_count.front(), int{});
  ASSERT_EQ(sl_count.back(), int{});
  {
    std::size_t count = 0u;
    for (auto v : sl_count) {
      ASSERT_EQ(v, int{});
      ++count;
    }
    ASSERT_EQ(count, 7);
  }
}

TEST(Initialization, ConstructorIterators) {
  std::vector<int> v{5, 2, 1, 3, 0, 4};
  skip_list<int> sl_copy_from;
  for (int i = -5; i <= 5; ++i) {
    ASSERT_NO_THROW(sl_copy_from.push(i));
  }
  skip_list<int> first_sl(v.begin(), v.end());
  ASSERT_EQ(v.size(), first_sl.size());
  auto first_it = first_sl.begin();
  for (int i = 0; i <= 5; ++i, first_it++) {
    ASSERT_NE(first_it, nullptr);
    ASSERT_EQ(*first_it, i);
  }
  skip_list<int> second_sl(sl_copy_from.begin(), sl_copy_from.end());
  ASSERT_EQ(sl_copy_from.size(), second_sl.size());
  auto second_it = second_sl.begin();
  for (auto value : sl_copy_from) {
    ASSERT_NE(second_it, nullptr);
    ASSERT_EQ(value, *second_it);
    ++second_it;
  }
}

TEST(Initialization, ConstructorsCopy) {
  skip_list<int> sl;
  for (int i = -5; i <= 5; ++i) {
    sl.push(i);
  }
  skip_list<int> copy_sl(sl);
  ASSERT_EQ(copy_sl.size(), sl.size());
  auto it = copy_sl.begin();
  for (const auto &v : sl) {
    ASSERT_NE(it, nullptr);
    ASSERT_NE(&v, &(*it));
    ASSERT_EQ(v, *it);
    ++it;
  }
  const auto size = sl.size();
  const auto &front = sl.front();
  const auto &back = sl.back();
  const auto begin = sl.begin();
  skip_list<int> moved_sl(std::move(sl));
  ASSERT_EQ(moved_sl.size(), size);
  ASSERT_EQ(&moved_sl.front(), &front);
  ASSERT_EQ(&moved_sl.back(), &back);
  ASSERT_EQ(moved_sl.begin(), begin);
  it = moved_sl.begin();
  for (int i = -5; i <= 5; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  // sl is not a valid until clear
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_EQ(sl.begin(), nullptr);
  // Double check, if data still here, after sl clear
  ASSERT_EQ(moved_sl.size(), size);
  ASSERT_EQ(&moved_sl.front(), &front);
  ASSERT_EQ(&moved_sl.back(), &back);
  ASSERT_EQ(moved_sl.begin(), begin);
  it = moved_sl.begin();
  for (int i = -5; i <= 5; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
}

TEST(Initialization, ConstructorsInitList) {
  skip_list<int> sl{1, -1, 2};
  ASSERT_EQ(sl.front(), -1);
  ASSERT_EQ(sl.back(), 2);
  ASSERT_EQ(*(++sl.begin()), 1);
  skip_list<int> sl1{0, 1, 2, -1, -2};
  auto it = sl1.cbegin();
  for (int i = -2; i <= 2; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
}
