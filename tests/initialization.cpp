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
      (!std::is_same_v<decltype(sl)::iterator, decltype(sl)::const_iterator>));
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
}
