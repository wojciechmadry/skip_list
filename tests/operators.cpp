#include <algorithm>
#include <gtest/gtest.h>

#include "skip_list.hpp"

using namespace sl;

TEST(Operators, OperatorEqCopy) {
  skip_list<int> sl{3, 1, 1, 1, 2, 2, 2, 2, 0};
  skip_list<int> copy_sl;
  ASSERT_NO_THROW(copy_sl = sl);
  ASSERT_EQ(copy_sl.size(), sl.size());
  ASSERT_EQ(copy_sl.front(), 0);
  ASSERT_EQ(copy_sl.back(), 3);
  auto it = copy_sl.begin();
  for (auto v : sl) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, v);
    ++it;
  }
  ASSERT_NO_THROW(copy_sl = copy_sl);
  ASSERT_EQ(copy_sl.size(), sl.size());
  ASSERT_EQ(copy_sl.front(), 0);
  ASSERT_EQ(copy_sl.back(), 3);
  it = copy_sl.begin();
  for (auto v : sl) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, v);
    ++it;
  }
}

TEST(Operators, OperatorEqMove) {
  skip_list<int> sl{1, 2, 3, 4, 5, -5, -4, -3, -2, -1, 0};
  const auto size = sl.size();
  skip_list<int> copy_sl;
  ASSERT_NO_THROW(copy_sl = std::move(sl));
  ASSERT_EQ(copy_sl.size(), size);
  ASSERT_EQ(copy_sl.front(), -5);
  ASSERT_EQ(copy_sl.back(), 5);
  auto it = copy_sl.begin();
  for (int i = -5; i <= 5; ++i) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
    ++it;
  }
  // NOLINT
  ASSERT_NO_THROW(copy_sl = std::move(copy_sl));
  ASSERT_EQ(copy_sl.size(), size);
  ASSERT_EQ(copy_sl.front(), -5);
  ASSERT_EQ(copy_sl.back(), 5);
  it = copy_sl.begin();
  for (int i = -5; i <= 5; ++i) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
    ++it;
  }
}

TEST(Operators, OperatorEqInit) {
  skip_list<int> sl;
  sl = {1};
  ASSERT_EQ(sl.size(), 1);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 1);
  sl = {2, 1};
  ASSERT_EQ(sl.size(), 2);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 2);
  sl = {2, 1, 3};
  ASSERT_EQ(sl.size(), 3);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 3);
  sl = {5, 4, 3, 2, 1, 0, -5, -4, -3, -2, -1};
  auto it = sl.begin();
  for (int i = -5; i <= 5; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
}

TEST(Operators, Assign) {
  skip_list<int> sl{1, 2, 3};
  std::vector<int> v{-3, -2, -1, 3, 2, 1, 0};
  ASSERT_NO_THROW(sl.assign(4, 1));
  ASSERT_EQ(sl.size(), 4);
  for (auto value : sl) {
    ASSERT_EQ(value, 1);
  }
  ASSERT_NO_THROW(sl.assign(v.begin(), v.end()));
  std::sort(v.begin(), v.end());
  ASSERT_EQ(v.size(), sl.size());
  auto it = sl.begin();
  for (auto value : v) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(value, *it);
    ++it;
  }
  ASSERT_NO_THROW(sl.assign({-3, -1, -2}));
  ASSERT_EQ(sl.size(), 3u);
  it = sl.begin();
  for (int i = -3; i <= -1; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(i, *it);
  }
}

TEST(Operators, OperatorEqComp) {
  skip_list<int> sl1{1, 2, 3};
  skip_list<int> sl2{1, 2, 3};
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(4));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(4));
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(5));
  ASSERT_NO_THROW(sl2.emplace(6));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.clear());
  ASSERT_NO_THROW(sl2.clear());
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 == sl2);
}

TEST(Operators, OperatorEqCompDiffTypenames) {
  skip_list<int, std::less<>, 30, 6> sl1{1, 2, 3};
  skip_list<int, std::less<>, 70, 3> sl2{1, 2, 3};
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(4));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(4));
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(5));
  ASSERT_NO_THROW(sl2.emplace(6));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.clear());
  ASSERT_NO_THROW(sl2.clear());
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 == sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 == sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 == sl2);
}

TEST(Operators, OperatorShipNotEqDiffTypenames) {
  skip_list<int, std::less<>, 30, 6> sl1{1, 2, 3};
  skip_list<int, std::less<>, 70, 3> sl2{1, 2, 3};
  ASSERT_FALSE(sl1 != sl2);
  ASSERT_NO_THROW(sl2.emplace(4));
  ASSERT_TRUE(sl1 != sl2);
  ASSERT_NO_THROW(sl1.emplace(4));
  ASSERT_FALSE(sl1 != sl2);
  ASSERT_NO_THROW(sl1.emplace(5));
  ASSERT_NO_THROW(sl2.emplace(6));
  ASSERT_TRUE(sl1 != sl2);
  ASSERT_NO_THROW(sl1.clear());
  ASSERT_NO_THROW(sl2.clear());
  ASSERT_FALSE(sl1 != sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 != sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_FALSE(sl1 != sl2);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 != sl2);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_FALSE(sl1 != sl2);
}

TEST(Operators, OperatorLt) {
  skip_list<int> sl1{};
  skip_list<int> sl2{};
  ASSERT_FALSE(sl1 < sl2);
  ASSERT_FALSE(sl2 < sl1);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 < sl2);
  ASSERT_FALSE(sl2 < sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 < sl2);
  ASSERT_FALSE(sl2 < sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 < sl2);
  ASSERT_TRUE(sl2 < sl1);
}

TEST(Operators, OperatorLe) {
  skip_list<int> sl1{};
  skip_list<int> sl2{};
  ASSERT_TRUE(sl1 <= sl2);
  ASSERT_TRUE(sl2 <= sl1);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_TRUE(sl1 <= sl2);
  ASSERT_FALSE(sl2 <= sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 <= sl2);
  ASSERT_TRUE(sl2 <= sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 <= sl2);
  ASSERT_TRUE(sl2 <= sl1);
}

TEST(Operators, OperatorGt) {
  skip_list<int> sl1{};
  skip_list<int> sl2{};
  ASSERT_FALSE(sl1 > sl2);
  ASSERT_FALSE(sl2 > sl1);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_FALSE(sl1 > sl2);
  ASSERT_TRUE(sl2 > sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_FALSE(sl1 > sl2);
  ASSERT_FALSE(sl2 > sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 > sl2);
  ASSERT_FALSE(sl2 > sl1);
}

TEST(Operators, OperatorGe) {
  skip_list<int> sl1{};
  skip_list<int> sl2{};
  ASSERT_TRUE(sl1 >= sl2);
  ASSERT_TRUE(sl2 >= sl1);
  ASSERT_NO_THROW(sl2.emplace(1));
  ASSERT_FALSE(sl1 >= sl2);
  ASSERT_TRUE(sl2 >= sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 >= sl2);
  ASSERT_TRUE(sl2 >= sl1);
  ASSERT_NO_THROW(sl1.emplace(1));
  ASSERT_TRUE(sl1 >= sl2);
  ASSERT_FALSE(sl2 >= sl1);
}
