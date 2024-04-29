#include <gtest/gtest.h>

#include "skip_list.hpp"

using namespace sl;

TEST(Erase, Erase) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{999};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_EQ(sl.erase(100), sl.end());
  ASSERT_NO_THROW(sl.push(3));
  ASSERT_EQ(sl.erase(100), sl.end());
  ASSERT_FALSE(sl.empty());
  ASSERT_EQ(sl.erase(3), sl.end());
  ASSERT_TRUE(sl.empty());
  ASSERT_NO_THROW(sl.push({1, 2, 3}));
  auto last_it = sl.begin();
  ASSERT_NO_THROW(++last_it);
  ASSERT_NO_THROW(++last_it);
  ASSERT_EQ(*last_it, 3);
  // Remove middle element
  auto following_it = sl.erase(2);
  ASSERT_NE(following_it, nullptr);
  ASSERT_EQ(sl.size(), 2);
  ASSERT_EQ(last_it, following_it);
  ASSERT_EQ(*following_it, 3);
  auto it = sl.begin();
  ASSERT_NE(it, nullptr);
  ASSERT_EQ(*it, 1);
  ASSERT_NO_THROW(++it);
  ASSERT_NE(it, nullptr);
  ASSERT_EQ(*it, 3);
  ASSERT_NO_THROW(sl.push(2));
  // Remove last element
  following_it = sl.erase(3);
  ASSERT_EQ(following_it, nullptr);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 2);
  ASSERT_NO_THROW(sl.push(3));
  // Remove first element
  last_it = sl.begin();
  ASSERT_NO_THROW(++last_it);
  following_it = sl.erase(1);
  ASSERT_EQ(following_it, last_it);
  ASSERT_EQ(sl.front(), 2);
  ASSERT_EQ(sl.back(), 3);
  ASSERT_EQ(sl.size(), 2);
  following_it = sl.erase(2);
  ASSERT_NE(following_it, nullptr);
  ASSERT_EQ(*following_it, 3);
  following_it = sl.erase(3);
  ASSERT_EQ(following_it, nullptr);
  ASSERT_TRUE(sl.empty());
  ASSERT_EQ(sl.begin(), nullptr);
  ASSERT_EQ(sl.end(), nullptr);
}

TEST(Erase, Erase_1000) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{9999};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  for (int i = -500; i < 500; ++i) {
    ASSERT_NO_THROW(sl.emplace(i));
  }
  ASSERT_EQ(sl.size(), 1000);
  std::uniform_int_distribution<int> uniform_dist(-600, 600);
  std::mt19937 gen(SEED);
  std::size_t erasedElements = 0;
  for (int i = 0; i < 888; ++i) {
    const auto rnd_val = uniform_dist(gen);
    auto found = sl.find(rnd_val);
    if (rnd_val >= 500 || rnd_val < -500) {
      ASSERT_EQ(found, nullptr);
      ASSERT_EQ(found, sl.end());
      ASSERT_EQ(sl.erase(rnd_val), sl.cend());
      continue;
    }
    auto isFound = found != sl.end();
    auto erased_it = sl.erase(rnd_val);
    if (!isFound) {
      ASSERT_EQ(erased_it, sl.end());
    } else {
      ++erasedElements;
      found = sl.find(rnd_val);
      ASSERT_EQ(found, sl.end());
      ASSERT_EQ(sl.size(), 1000u - erasedElements);
    }
  }
  ASSERT_EQ(sl.size(), 1000u - erasedElements);
  auto it = sl.begin();
  int prevNum = std::numeric_limits<int>::min();
  for (std::size_t i = 0u; i < sl.size(); ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_GT(*it, prevNum);
    prevNum = *it;
    const auto found = sl.find(*it);
    ASSERT_NE(found, nullptr);
    ASSERT_EQ(*found, *it);
  }
  ASSERT_EQ(it, sl.end());
}

TEST(Erase, EraseAndFind) {
  skip_list<int> sl;
  auto SEED = std::seed_seq{99998};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_NO_THROW(sl.push({1, 3, 7, 9, 12}));
  ASSERT_NE(sl.find(1), sl.end());
  ASSERT_NE(sl.find(3), sl.end());
  ASSERT_NE(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_NE(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 5);
  for (auto i = 13; i < 100; ++i) {
    ASSERT_EQ(sl.erase(i), nullptr);
  }
  ASSERT_NE(sl.find(1), sl.end());
  ASSERT_NE(sl.find(3), sl.end());
  ASSERT_NE(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_NE(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 5);
  auto eraseIt = sl.erase(7);
  ASSERT_NE(eraseIt, nullptr);
  ASSERT_EQ(*eraseIt, 9);
  ASSERT_NE(sl.find(1), sl.end());
  ASSERT_NE(sl.find(3), sl.end());
  ASSERT_EQ(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_NE(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 4);
  eraseIt = sl.erase(1);
  ASSERT_NE(eraseIt, nullptr);
  ASSERT_EQ(*eraseIt, 3);
  ASSERT_EQ(sl.find(1), sl.end());
  ASSERT_NE(sl.find(3), sl.end());
  ASSERT_EQ(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_NE(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 3);
  eraseIt = sl.erase(12);
  ASSERT_EQ(eraseIt, nullptr);
  ASSERT_EQ(sl.find(1), sl.end());
  ASSERT_NE(sl.find(3), sl.end());
  ASSERT_EQ(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_EQ(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 2);
  ASSERT_EQ(sl.front(), 3);
  ASSERT_EQ(sl.back(), 9);
  // {3, 9}
  eraseIt = sl.erase(3);
  ASSERT_NE(eraseIt, nullptr);
  ASSERT_EQ(*eraseIt, 9);
  ASSERT_EQ(sl.find(1), sl.end());
  ASSERT_EQ(sl.find(3), sl.end());
  ASSERT_EQ(sl.find(7), sl.end());
  ASSERT_NE(sl.find(9), sl.end());
  ASSERT_EQ(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 1);
  ASSERT_EQ(sl.front(), 9);
  ASSERT_EQ(sl.back(), 9);
  ASSERT_EQ(*sl.begin(), 9);
  // {9}
  eraseIt = sl.erase(9);
  ASSERT_EQ(eraseIt, nullptr);
  ASSERT_EQ(sl.find(1), sl.end());
  ASSERT_EQ(sl.find(3), sl.end());
  ASSERT_EQ(sl.find(7), sl.end());
  ASSERT_EQ(sl.find(9), sl.end());
  ASSERT_EQ(sl.find(12), sl.end());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_EQ(sl.begin(), nullptr);
  ASSERT_TRUE(sl.empty());
  // {3, 9}
  ASSERT_NO_THROW(sl.push({3, 9}));
  ASSERT_EQ(sl.size(), 2);
  ASSERT_EQ(*sl.begin(), 3);
  ASSERT_EQ(sl.front(), 3);
  ASSERT_EQ(sl.back(), 9);
  int expected = 3;
  for (const auto val : sl) {
    ASSERT_EQ(val, expected);
    expected *= 3;
  }
  ASSERT_EQ(expected, 27);
  // {3}
  ASSERT_EQ(sl.erase(9), nullptr);
  ASSERT_EQ(sl.size(), 1);
  ASSERT_EQ(*sl.begin(), 3);
  ASSERT_EQ(sl.front(), 3);
  ASSERT_EQ(sl.back(), 3);
  ASSERT_EQ(&sl.back(), &sl.front());
  std::size_t calls = 0u;
  for (const auto &val : sl) {
    ASSERT_EQ(val, 3);
    ++calls;
  }
  ASSERT_EQ(calls, 1u);
}

TEST(Erase, EraseAndInsert) {
  skip_list<int> sl;
  auto emplace = [&](int value, int seed) {
    auto SEED = std::seed_seq({seed});
    sl.set_seed(SEED);
    std::size_t visited = 0;
    sl.emplace(value, &visited);
    return visited;
  };
  ASSERT_EQ(emplace(0, 0), 0);
  ASSERT_NO_THROW(sl.erase(0));
  ASSERT_EQ(emplace(0, 0), 0);

  ASSERT_EQ(emplace(1, 1), 0);
  ASSERT_NO_THROW(sl.erase(1));
  ASSERT_EQ(emplace(1, 1), 0);

  ASSERT_EQ(emplace(2, 2), 2);
  ASSERT_NO_THROW(sl.erase(2));
  ASSERT_EQ(emplace(2, 2), 2);

  ASSERT_EQ(emplace(3, 3), 2);
  ASSERT_NO_THROW(sl.erase(3));
  ASSERT_EQ(emplace(3, 3), 2);

  ASSERT_EQ(emplace(4, 4), 3);
  ASSERT_NO_THROW(sl.erase(4));
  ASSERT_EQ(emplace(4, 4), 3);

  ASSERT_EQ(emplace(5, 5), 3);
  ASSERT_NO_THROW(sl.erase(5));
  ASSERT_EQ(emplace(5, 5), 3);

  ASSERT_EQ(emplace(6, 6), 3);
  ASSERT_NO_THROW(sl.erase(6));
  ASSERT_EQ(emplace(6, 6), 3);

  ASSERT_EQ(emplace(7, 7), 4);
  ASSERT_NO_THROW(sl.erase(7));
  ASSERT_EQ(emplace(7, 7), 4);

  ASSERT_EQ(emplace(8, 8), 4);
  ASSERT_NO_THROW(sl.erase(8));
  ASSERT_EQ(emplace(8, 8), 4);

  ASSERT_EQ(emplace(9, 9), 5);
  ASSERT_NO_THROW(sl.erase(9));
  ASSERT_EQ(emplace(9, 9), 5);

  ASSERT_EQ(emplace(-1, 10), 0);
  ASSERT_NO_THROW(sl.erase(-1));
  ASSERT_EQ(emplace(-1, 10), 0);

  ASSERT_EQ(emplace(10, 11), 6);
  ASSERT_NO_THROW(sl.erase(10));
  ASSERT_EQ(emplace(10, 11), 6);

  ASSERT_EQ(emplace(11, 12), 7);
  ASSERT_NO_THROW(sl.erase(11));
  ASSERT_EQ(emplace(11, 12), 7);

  ASSERT_EQ(emplace(5, 13), 4);
  ASSERT_NO_THROW(sl.erase(5));
  ASSERT_EQ(emplace(5, 13), 4);
  auto it = sl.begin();
  for (int i = -1; i <= 5; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  for (int i = 5; i <= 11; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
}

TEST(Erase, EraseAndInsertRandomized) {
  skip_list<int> sl;
  std::mt19937 gen;
  auto SEED = std::seed_seq({1616});
  gen.seed(SEED);
  std::uniform_int_distribution<> distrib(-100, 100);
  for (int i = 0; i < 1000; ++i) {
    const auto val = distrib(gen);
    if (sl.find(val) != nullptr) {
      continue;
    }
    skip_list<int>::size_type visited{0u};
    skip_list<int>::size_type visited_after_erase{0u};
    auto local_seed = std::seed_seq({i});
    sl.set_seed(local_seed);
    ASSERT_NO_THROW(sl.push(val, &visited));
    ASSERT_NO_THROW(sl.erase(val));
    sl.set_seed(local_seed);
    ASSERT_NO_THROW(sl.push(val, &visited_after_erase));
    ASSERT_EQ(visited, visited_after_erase);
  }
}
