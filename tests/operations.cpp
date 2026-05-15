#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <memory>
#include <random>
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
  const auto sl_before = sl1;
  const auto ptr_before = &sl1;
  ASSERT_NO_THROW(sl1.merge(sl1));
  ASSERT_EQ(sl1, sl_before);
  ASSERT_EQ(&sl1, ptr_before);
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

TEST(Operators, lowerBound) {
  const skip_list<int> empty;
  EXPECT_EQ(empty.lower_bound(0), empty.end());

  for (size_t seed = 0; seed <= 100; ++seed) {
    std::seed_seq SEED{seed};
    skip_list<int> sl;
    sl.set_seed(SEED);
    sl.push({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12});
    EXPECT_EQ(sl.lower_bound(std::numeric_limits<int>::max()), sl.end());

    auto check_bound = [&](const int key, const int expected) {
      auto returned = sl.lower_bound(key);
      ASSERT_NE(returned, sl.end());
      EXPECT_EQ(*returned, expected);
    };

    check_bound(0, 1);
    for (int i = 1; i <= 10; ++i) {
      check_bound(i, i);
    }
    check_bound(11, 12);
    check_bound(12, 12);
  }
}

TEST(Operators, lowerBoundCustomOp) {
  sl::skip_list<int, std::greater<int>> list{3, 1, 5, 2, 4};
  auto it = list.lower_bound(3);
  ASSERT_NE(it, list.end());
  EXPECT_EQ(*it, 3);
}

TEST(Operators, upperBound) {
  const skip_list<int> empty;
  EXPECT_EQ(empty.upper_bound(0), empty.end());

  for (size_t seed = 0; seed <= 100; ++seed) {
    std::seed_seq SEED{seed};
    skip_list<int> sl;
    sl.set_seed(SEED);
    sl.push({-1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12});
    EXPECT_EQ(sl.upper_bound(std::numeric_limits<int>::max()), sl.end());
    EXPECT_EQ(sl.upper_bound(12), sl.end());

    auto check_bound = [&](const int key, const int expected) {
      auto lb = sl.upper_bound(key);
      ASSERT_NE(lb, sl.end());
      EXPECT_EQ(*lb, expected);
    };

    check_bound(-2, -1);
    check_bound(-1, 1);
    check_bound(0, 1);
    for (int i = 1; i <= 9; ++i) {
      check_bound(i, i + 1);
    }
    check_bound(10, 12);
    check_bound(11, 12);
  }
}

TEST(Operators, upperBoundCustomOp) {
  sl::skip_list<int, std::greater<int>> list{3, 1, 5, 2, 4};
  auto it = list.upper_bound(3);
  ASSERT_NE(it, list.end());
  EXPECT_EQ(*it, 2);
}
