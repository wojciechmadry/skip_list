#include <cctype>
#include <gtest/gtest.h>
#include <type_traits>

#include "skip_list.hpp"
using namespace sl;

TEST(Versioning, SKIP_LIST_VERSION) {
  static_assert(std::is_same_v<std::decay_t<decltype(SKIP_LIST_VERSION)>,
                               std::string_view>);
  const auto &sv = SKIP_LIST_VERSION;
  ASSERT_FALSE(sv.empty());
  ASSERT_NE(sv.find("."), std::string::npos);
  ASSERT_EQ(sv.size(), 3u);
  ASSERT_NE(std::isdigit(sv[0]), 0);
  ASSERT_EQ(std::isdigit(sv[1]), 0);
  ASSERT_EQ(sv[1], '.');
  ASSERT_NE(std::isdigit(sv[2]), 0);
  const int MAJOR = sv[0] - '0';
  // const int MINOR = sv[2] - '0';
  ASSERT_GE(MAJOR, 1);
}
