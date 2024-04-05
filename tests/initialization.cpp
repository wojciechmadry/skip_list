#include <gtest/gtest.h>

#include <memory>
#include <string>
#include <type_traits>

#include "skip_list.hpp"

TEST(Initialization, ClassCreation) {
  ASSERT_NO_THROW(skip_list<int>{});
  ASSERT_NO_THROW(skip_list<std::string>{});
  ASSERT_NO_THROW(skip_list<std::unique_ptr<int>>{});
  ASSERT_NO_THROW((skip_list<std::unique_ptr<int>, std::greater<>, 0.25f, 10,
                             std::allocator<std::unique_ptr<int>>>{}));
}

TEST(Initialization, Typedefs) {
  skip_list<int> sl;
  ASSERT_TRUE((std::is_same_v<int, decltype(sl)::value_type>));
  ASSERT_TRUE(
      (std::is_same_v<std::allocator<int>, decltype(sl)::allocator_type>));
  ASSERT_TRUE((std::is_unsigned_v<decltype(sl)::size_type>));
  ASSERT_TRUE((std::is_signed_v<decltype(sl)::difference_type>));
  ASSERT_TRUE((std::is_same_v<int&, decltype(sl)::reference>));
  ASSERT_TRUE((std::is_same_v<const int&, decltype(sl)::const_reference>));
  ASSERT_TRUE((std::is_same_v<int*, decltype(sl)::pointer>));
  ASSERT_TRUE((std::is_same_v<const int*, decltype(sl)::const_pointer>));
  ASSERT_TRUE(
      (!std::is_same_v<decltype(sl)::iterator, decltype(sl)::const_iterator>));
  ASSERT_TRUE((!std::is_same_v<decltype(sl)::reverse_iterator,
                               decltype(sl)::const_reverse_iterator>));
}

