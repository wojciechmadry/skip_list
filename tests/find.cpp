#include <gtest/gtest.h>
#include <random>

#include "skip_list.hpp"

using namespace sl;

TEST(Find, Find) {
  skip_list<int> sl;
  std::seed_seq SEED{113};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_EQ(sl.find(5), sl.end());
  ASSERT_EQ(sl.find(11), sl.end());
  ASSERT_NO_THROW(sl.push(5));
  ASSERT_EQ(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_NO_THROW(sl.push(11));
  ASSERT_NE(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_EQ(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_NO_THROW(sl.push(30));
  ASSERT_NE(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_NE(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_EQ(*sl.find(30), 30);
  ASSERT_NO_THROW(sl.push(13));
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_EQ(*sl.find(13), 13);
  ASSERT_EQ(sl.back(), 30);
  ASSERT_NE(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(30), 30);
  for (int i = 25; i <= 50; ++i) {
    ASSERT_NO_THROW(sl.push(i));
    ASSERT_NE(sl.find(i), sl.end());
    ASSERT_EQ(*sl.find(i), i);
  }
}

TEST(Find, Find_1000) {
  skip_list<int> sl;
  std::seed_seq SEED{115};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  for (int i = -500; i < 500; ++i) {
    ASSERT_NO_THROW(sl.push(i));
  }
  for (int i = -1000; i <= 1000; ++i) {
    auto found = sl.find(i);
    if (i >= -500 && i < 500) {
      ASSERT_NE(found, sl.end());
      ASSERT_EQ(*found, i);
    } else {
      ASSERT_EQ(found, sl.end());
    }
  }
}

TEST(Find, FindReversed) {
  skip_list<int, std::greater<>> sl;
  std::seed_seq SEED{113};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_EQ(sl.find(5), sl.end());
  ASSERT_EQ(sl.find(11), sl.end());
  ASSERT_NO_THROW(sl.push(5));
  ASSERT_EQ(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_NO_THROW(sl.push(11));
  ASSERT_NE(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_EQ(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_NO_THROW(sl.push(30));
  ASSERT_NE(sl.find(11), sl.end());
  ASSERT_NE(sl.find(5), sl.end());
  ASSERT_NE(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_EQ(*sl.find(30), 30);
  ASSERT_NO_THROW(sl.push(13));
  ASSERT_EQ(*sl.find(5), 5);
  ASSERT_EQ(*sl.find(11), 11);
  ASSERT_EQ(*sl.find(13), 13);
  ASSERT_EQ(sl.back(), 5);
  ASSERT_NE(sl.find(30), sl.end());
  ASSERT_EQ(*sl.find(30), 30);
  for (int i = 25; i <= 50; ++i) {
    ASSERT_NO_THROW(sl.push(i));
    ASSERT_NE(sl.find(i), sl.end());
    ASSERT_EQ(*sl.find(i), i);
  }
}

TEST(Find, FindReversed_1000) {
  skip_list<int, std::greater<>> sl;
  std::seed_seq SEED{115};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  for (int i = -500; i < 500; ++i) {
    ASSERT_NO_THROW(sl.push(i));
  }
  for (int i = -1000; i <= 1000; ++i) {
    auto found = sl.find(i);
    if (i >= -500 && i < 500) {
      ASSERT_NE(found, sl.end());
      ASSERT_EQ(*found, i);
    } else {
      ASSERT_EQ(found, sl.end());
    }
  }
}

TEST(Find, FindString) {
  skip_list<std::string> sl{"a",  "ab", "abc", "abcde",    "kf",
                            "11", "kc", "00",  "skip_list"};
  EXPECT_EQ(sl.find("aa"), sl.end());
  EXPECT_EQ(sl.find("0"), sl.end());
  EXPECT_EQ(sl.find(""), sl.end());
  EXPECT_EQ(sl.find("avv"), sl.end());
  EXPECT_EQ(sl.find("abb"), sl.end());
  EXPECT_EQ(sl.find("skip list"), sl.end());
  EXPECT_EQ(sl.find("kf1"), sl.end());
  EXPECT_EQ(sl.find("000"), sl.end());
  EXPECT_EQ(sl.find("01"), sl.end());
  for (const auto &str : sl) {
    auto found = sl.find(str);
    ASSERT_NE(found, sl.end());
    ASSERT_EQ(*found, str);
  }
}

// Is not optimal to use std search function
// Use builtin find instead
TEST(Find, StdFind) {
  skip_list<int> sl{-5, -4, -3, 0, 5, 10};
  auto find = [&](const int &key) {
    return std::find(sl.begin(), sl.end(), key);
  };
  ASSERT_EQ(find(1), sl.end());
  ASSERT_EQ(find(11), sl.end());
  ASSERT_NE(find(-5), sl.end());
  ASSERT_EQ(*find(-5), -5);
  ASSERT_NE(find(0), sl.end());
  ASSERT_EQ(*find(0), 0);
  ASSERT_NE(find(10), sl.end());
  ASSERT_EQ(*find(10), 10);
}
