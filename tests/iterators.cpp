#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <algorithm>

using namespace sl;

TEST(Iterators, Increment) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{111};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  for (int i = 0; i < 4; ++i) {
    ASSERT_NO_THROW(sl.emplace(i));
  }
  ASSERT_EQ(sl.size(), 4);
  auto it = sl.begin();
  ASSERT_EQ(*it, 0);
  ASSERT_NO_THROW(++it);
  ASSERT_EQ(*it, 1);
  ASSERT_NO_THROW(it++);
  ASSERT_EQ(*it, 2);
  ASSERT_NO_THROW(++it);
  ASSERT_EQ(*it, 3);
  ASSERT_NE(it, sl.end());
  ASSERT_NO_THROW(it++);
  ASSERT_EQ(it, sl.end());
  ASSERT_NO_THROW(sl.clear());
  ASSERT_NO_THROW(sl.emplace(1));
  it = sl.begin();
  ASSERT_EQ(*it, 1);
  ASSERT_NO_THROW(it++);
  ASSERT_EQ(it, sl.end());
  ASSERT_NO_THROW(sl.emplace(2));
  it = sl.begin();
  ASSERT_EQ(*it, 1);
  ASSERT_NO_THROW(it++);
  ASSERT_NE(it, sl.end());
  ASSERT_EQ(*it, 2);
  ASSERT_NO_THROW(it++);
  ASSERT_EQ(it, sl.end());
}

TEST(Iterators, BeginEnd) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{123};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_EQ(sl.begin(), sl.end());
  ASSERT_EQ(sl.begin(), nullptr);
  ASSERT_EQ(sl.end(), nullptr);
  for (auto el : sl) {
    (void)el;
    ASSERT_TRUE(false);
  }
  for (auto it = sl.begin(); it != sl.end(); ++it) {
    (void)it;
    ASSERT_TRUE(false);
  }

  ASSERT_NO_THROW(sl.push(1));
  ASSERT_NE(sl.begin(), nullptr);
  ASSERT_EQ(*sl.begin(), 1);
  std::size_t for_counter = 0u;
  for (auto it = sl.begin(); it != sl.end(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, 1);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 1u);
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, 1);
    el = 5;
    ASSERT_EQ(el, 5);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 1u);
  ASSERT_EQ(*sl.begin(), 1);
  ASSERT_NO_THROW(sl.push(2));
  ASSERT_EQ(*sl.begin(), 1);
  ASSERT_EQ(*(++sl.begin()), 2);
  for_counter = 0u;
  for (auto it = sl.begin(); it != sl.end(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter + 1u);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 2u);
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, for_counter + 1u);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 2u);
  ASSERT_NO_THROW(sl.push(0));
  ASSERT_EQ(*sl.begin(), 0);
  ASSERT_EQ(*(++sl.begin()), 1);
  ASSERT_EQ(*(++(++sl.begin())), 2);
  for_counter = 0u;
  for (auto it = sl.begin(); it != sl.end(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 3u);
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 3u);
  for (int i = 3; i <= 99; ++i) {
    ASSERT_NO_THROW(sl.push(i));
  }
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 100u);
  for_counter = 0u;
  for (auto it = sl.begin(); it != sl.end(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 100u);
  ASSERT_EQ(*sl.begin(), 0);
}

TEST(Iterators, BeginEndConst) {
  const skip_list<int> sl_const;
  ASSERT_EQ(sl_const.begin(), sl_const.end());
  ASSERT_EQ(sl_const.begin(), nullptr);
  ASSERT_EQ(sl_const.end(), nullptr);
  for (auto it = sl_const.begin(); it != sl_const.end(); ++it) {
    (void)it;
    ASSERT_TRUE(false);
  }
  for (auto el : sl_const) {
    (void)el;
    ASSERT_TRUE(false);
  }

  skip_list<int> sl;
  auto SEED = std::seed_seq{33333};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_NO_THROW(sl.push(1));
  ASSERT_NE(sl.cbegin(), nullptr);
  ASSERT_EQ(*sl.cbegin(), 1);
  std::size_t for_counter = 0u;
  for (auto it = sl.cbegin(); it != sl.cend(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, 1);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 1u);
  for_counter = 0u;
  for (const auto &el : sl) {
    ASSERT_EQ(el, 1);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 1u);
  ASSERT_EQ(*sl.cbegin(), 1);
  ASSERT_NO_THROW(sl.push(2));
  ASSERT_EQ(*sl.cbegin(), 1);
  ASSERT_EQ(*(++sl.cbegin()), 2);
  for_counter = 0u;
  for (auto it = sl.cbegin(); it != sl.cend(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter + 1u);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 2u);
  for_counter = 0u;
  for (const auto &el : sl) {
    ASSERT_EQ(el, for_counter + 1u);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 2u);
  ASSERT_NO_THROW(sl.push(0));
  ASSERT_EQ(*sl.cbegin(), 0);
  ASSERT_EQ(*(++sl.cbegin()), 1);
  ASSERT_EQ(*(++(++sl.cbegin())), 2);
  for_counter = 0u;
  for (auto it = sl.cbegin(); it != sl.cend(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 3u);
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 3u);
  for (int i = 3; i <= 99; ++i) {
    ASSERT_NO_THROW(sl.push(i));
  }
  for_counter = 0u;
  for (auto el : sl) {
    ASSERT_EQ(el, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 100u);
  for_counter = 0u;
  for (auto it = sl.cbegin(); it != sl.cend(); ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, for_counter);
    ++for_counter;
  }
  ASSERT_EQ(for_counter, 100u);
  ASSERT_EQ(*sl.cbegin(), 0);
}

TEST(Iterators, Emplace) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{123456};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  auto it = sl.push(2);
  EXPECT_EQ(*it, sl.front());
  EXPECT_EQ(it, sl.begin());
  it = sl.push(4);
  EXPECT_NE(it, sl.begin());
  EXPECT_EQ(*it, sl.back());
  it = sl.push(0);
  EXPECT_EQ(*it, sl.front());
  EXPECT_EQ(it, sl.begin());
  for (int i = -5; i <= 10; ++i) {
    it = sl.push(i);
    EXPECT_EQ(*it, i);
  }
}

TEST(Iterators, Inserter) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{133};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  std::fill_n(std::inserter(sl, sl.end()), 3, 5);
  ASSERT_EQ(sl.front(), 5);
  ASSERT_EQ(sl.back(), 5);
  ASSERT_EQ(*sl.begin(), 5);
  ASSERT_EQ(*(++sl.begin()), 5);
  ASSERT_EQ(*(++(++sl.begin())), 5);
  skip_list<int> sl_1;
  auto SEED_1 = std::seed_seq{123455};
  ASSERT_NO_THROW(sl_1.set_seed(SEED_1));
  int i = 1;
  std::transform(sl.begin(), sl.end(), std::inserter(sl_1, sl_1.end()),
                 [&i](int v) mutable { return v + (i++); });
  auto it = sl_1.begin();
  for (int i = 6; i <= 8; ++i, ++it) {
    ASSERT_EQ(*it, i);
  }
  skip_list<int> sl_2;
  auto SEED_2 = std::seed_seq{13455};
  ASSERT_NO_THROW(sl_2.set_seed(SEED_2));
  std::copy(sl_1.begin(), sl_1.end(), std::inserter(sl_2, sl_2.end()));
  it = sl_1.begin();
  for (auto v : sl_2) {
    ASSERT_EQ(v, *(it++));
  }
}

TEST(Iterators, Algorithms) {
  skip_list<int> sl;
  skip_list<int> sl1;
  auto SEED = std::seed_seq{444};
  auto SEED1 = std::seed_seq{555};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_NO_THROW(sl1.set_seed(SEED1));

  for (int i = -10; i <= 10; ++i) {
    ASSERT_NO_THROW(sl.push(i));
  }
  auto it = sl.begin();
  for (int i = -10; i <= 10; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl.end());
  // for_each
  {
    int i = -10;
    std::for_each(sl.begin(), sl.end(), [&](int v) { ASSERT_EQ(v, i++); });
    ASSERT_EQ(i, 11);
    i = -10;
    std::for_each_n(sl.begin(), 3, [&](int v) { ASSERT_EQ(v, i++); });
    ASSERT_EQ(i, -7);
  }
  // find - This is not an effective search -> use builtin search instead
  {
    auto it_begin = sl.begin();
    auto it_end = sl.end();
    const auto found_5 = std::find(it_begin, it_end, 5);
    ASSERT_NE(found_5, it_end);
    ASSERT_EQ(*found_5, 5);
    const auto found_12 = std::find(it_begin, it_end, 12);
    ASSERT_EQ(found_12, it_end);
    const auto found_if = std::find_if(
        it_begin, it_end, [](int v) { return v > 0 && v % 2 == 0; });
    ASSERT_NE(found_if, it_end);
    ASSERT_EQ(*found_if, 2);
    for (const auto v : sl) {
      const auto found = std::find(it_begin, it_end, v);
      ASSERT_NE(found, it_end);
      ASSERT_EQ(*found, v);
    }
  }

  // copy
  {
    std::copy(sl.begin(), sl.end(), std::inserter(sl1, sl1.begin()));
    auto it = sl.begin();
    auto it1 = sl1.begin();
    while (it != sl.end()) {
      ASSERT_NE(it, it1);
      ASSERT_NE(it, sl.end());
      ASSERT_NE(it1, sl1.end());
      ASSERT_EQ(*it, *it1);
      ++it;
      ++it1;
    }
    ASSERT_NO_THROW(sl1.clear());
    ASSERT_TRUE(sl1.empty());
  }

  // transform
  {
    std::transform(sl.begin(), sl.end(), std::inserter(sl1, sl1.begin()),
                   [](int v) { return std::abs(v) + 1; });
    auto it = sl.begin();
    auto it1 = sl1.begin();
    std::vector<int> sorted;
    while (it != sl.end()) {
      ASSERT_NE(it, it1);
      ASSERT_NE(it, sl.end());
      ASSERT_NE(it1, sl1.end());
      ASSERT_NO_THROW(sorted.emplace_back(std::abs(*it) + 1));
      ++it;
      ++it1;
    }
    std::sort(sorted.begin(), sorted.end());
    ASSERT_EQ(sl1.size(), sorted.size());
    ASSERT_TRUE(std::equal(sl1.begin(), sl1.end(), sorted.begin()));
    ASSERT_TRUE(std::is_sorted(sorted.begin(), sorted.end()));
    ASSERT_TRUE(std::is_sorted(sl.begin(), sl.end()));
    ASSERT_TRUE(std::is_sorted(std::begin(sl1), std::end(sl1)));
    ASSERT_NO_THROW(sl1.clear());
    ASSERT_TRUE(sl1.empty());
  }
}
