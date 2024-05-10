#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <memory>
#include <string>

using namespace sl;

TEST(Operations, Merge) {
  skip_list<int> sl1{1, 2, 3, 4};
  skip_list<int> sl2{0, 3, 5, 6};
  ASSERT_NO_THROW(sl2.merge(sl1));
  ASSERT_EQ(sl2.size(), 8u);
  ASSERT_EQ(sl1.size(), 0u);
  auto it = sl2.begin();
  for (int i = 0; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  for (int i = 3; i <= 6; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl2.end());
}

TEST(Operations, MergeStringsUniquePtr) {
  auto make_str = [](std::string str) {
    return std::make_unique<std::string>(std::move(str));
  };
  struct comp {
    bool operator()(const std::unique_ptr<std::string> &lhs,
                    const std::unique_ptr<std::string> &rhs) {
      return *lhs < *rhs;
    }
  };
  skip_list<std::unique_ptr<std::string>, comp> sl1;
  sl1.push(make_str("aa"));
  sl1.push(make_str("ak"));
  skip_list<std::unique_ptr<std::string>, comp> sl2;
  sl2.push(make_str("ab"));
  sl2.push(make_str("ac"));
  sl2.push(make_str("ae"));

  ASSERT_NO_THROW(sl2.merge(std::move(sl1)));
  ASSERT_EQ(sl2.size(), 5u);
  ASSERT_EQ(sl1.size(), 0u);
  auto it = sl2.begin();
  ASSERT_EQ(**(it++), "aa");
  ASSERT_EQ(**(it++), "ab");
  ASSERT_EQ(**(it++), "ac");
  ASSERT_EQ(**(it++), "ae");
  ASSERT_EQ(**(it++), "ak");
  ASSERT_EQ(it, sl2.end());
}

TEST(Operations, StdSwap) {
  skip_list<int> sl{1, 3, 2};
  skip_list<int> sl_neg{-3, -1, -2, 0};
  ASSERT_EQ(sl.size(), 3);
  ASSERT_EQ(sl_neg.size(), 4);
  ASSERT_NO_THROW(std::swap(sl, sl_neg));
  ASSERT_EQ(sl.size(), 4);
  ASSERT_EQ(sl_neg.size(), 3);
  auto it = sl.begin();
  for (int i = -3; i <= 0; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl.end());
  it = sl_neg.begin();
  for (int i = 1; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl_neg.end());
}

TEST(Operations, Swap) {
  skip_list<int> sl{1, 3, 2};
  skip_list<int> sl_neg{-3, -1, -2, 0};
  ASSERT_EQ(sl.size(), 3);
  ASSERT_EQ(sl_neg.size(), 4);
  ASSERT_NO_THROW(sl.swap(sl_neg));
  ASSERT_EQ(sl.size(), 4);
  ASSERT_EQ(sl_neg.size(), 3);
  auto it = sl.begin();
  for (int i = -3; i <= 0; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl.end());
  it = sl_neg.begin();
  for (int i = 1; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl_neg.end());
  ASSERT_NO_THROW(sl.swap(sl));
}
