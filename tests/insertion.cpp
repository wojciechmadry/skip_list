#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>
#include <vector>

#include "skip_list.hpp"

using namespace sl;

class InsertionPushEmplace
    : public ::testing::TestWithParam<std::function<skip_list<int> &(int)>> {};

TEST_P(InsertionPushEmplace, InsertData) {
  auto func = GetParam();
  auto &sl = func(2);
  auto SEED = std::seed_seq{0};
  ASSERT_NO_THROW(sl.set_seed(SEED));
  ASSERT_NO_THROW(sl.clear());
  ASSERT_NO_THROW(func(2));
  ASSERT_EQ(sl.front(), 2);
  ASSERT_EQ(&sl.front(), &sl.back());
  ASSERT_EQ(sl.size(), 1u);
  ASSERT_NO_THROW(func(3));
  ASSERT_EQ(sl.front(), 2);
  ASSERT_NE(&sl.front(), &sl.back());
  ASSERT_EQ(sl.size(), 2u);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_NO_THROW(func(3));
  ASSERT_EQ(sl.front(), 3);
  ASSERT_EQ(&sl.front(), &sl.back());
  ASSERT_EQ(sl.size(), 1u);
  ASSERT_NO_THROW(func(2));
  ASSERT_EQ(sl.front(), 2);
  ASSERT_NE(&sl.front(), &sl.back());
  ASSERT_EQ(sl.size(), 2u);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_NO_THROW(func(3));
  ASSERT_NO_THROW(func(2));
  ASSERT_EQ(sl.size(), 2u);
  ASSERT_NO_THROW(func(1));
  ASSERT_EQ(sl.size(), 3u);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 3);
  ASSERT_NO_THROW(func(4));
  ASSERT_EQ(sl.back(), 4);
  ASSERT_NO_THROW(func(6));
  ASSERT_EQ(sl.size(), 5u);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.back(), 6);
  ASSERT_NO_THROW(func(5));
  ASSERT_EQ(sl.back(), 6);
  ASSERT_EQ(sl.front(), 1);
  ASSERT_EQ(sl.size(), 6u);
  ASSERT_NO_THROW(func(0));
  ASSERT_EQ(sl.front(), 0);
  ASSERT_EQ(sl.size(), 7);
  auto it = sl.begin();
  // List el: [0, 1, 2, 3, 4, 5, 6]
  for (int i = 0; i <= 6; ++i) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(i, *it);
    ASSERT_NO_THROW(++it);
  }
  ASSERT_NO_THROW(sl.clear());
  ASSERT_TRUE(sl.empty());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_EQ(sl.begin(), nullptr);
}

TEST_P(InsertionPushEmplace, InsertRandomized_100) {
  std::mt19937 gen{10};
  std::uniform_int_distribution<int> uniform_dist(-100, 100);
  std::vector<int> vec;
  auto func = GetParam();
  auto &sl = func(0);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_TRUE(sl.empty());
  for (std::size_t i = 0u; i < 100; ++i) {
    int value = uniform_dist(gen);
    ASSERT_NO_THROW(vec.emplace_back(value));
    ASSERT_NO_THROW(sl.push(value));
  }
  ASSERT_EQ(vec.size(), sl.size());
  std::sort(vec.begin(), vec.end());
  auto it = sl.begin();
  for (auto val : vec) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(val, *it);
    ++it;
  }
}

TEST_P(InsertionPushEmplace, InsertRandomized_1000) {
  std::mt19937 gen{100};
  std::uniform_int_distribution<int> uniform_dist(-100, 100);
  std::vector<int> vec;
  auto func = GetParam();
  auto &sl = func(0);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_TRUE(sl.empty());
  for (std::size_t i = 0u; i < 1000; ++i) {
    int value = uniform_dist(gen);
    ASSERT_NO_THROW(vec.emplace_back(value));
    ASSERT_NO_THROW(sl.push(value));
  }
  ASSERT_EQ(vec.size(), sl.size());
  std::sort(vec.begin(), vec.end());
  auto it = sl.begin();
  for (auto val : vec) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(val, *it);
    ++it;
  }
}

TEST(Insertion, VisitedNodesEmplace) {
  skip_list<int> sl;
  auto SEED = std::seed_seq({0});
  sl.set_seed(SEED);
  std::mt19937 gen;
  gen.seed(SEED);
  /*                     0
   * 0                   0
   * 0       0   X  0   0 0 0
   * 0 0 0   0   0 0   0 0 0 0
   * 0 0 0 0 0 0 0 0 0 0 0 0 0
   * ==========================
   * 1 0 1 2 3 4 5 6 7 8 9 0 1
   */
  for (int i = 0; i < 20; ++i) {
    // Code only to check what size of node is
    node<int> ng(i, gen);
    ASSERT_GE(ng.size(), 1);
    // std::cout << i << " = " << ng.size() << '\n';
  }
  auto emplace = [&](int value) {
    std::size_t visited = 0;
    sl.emplace(value, &visited);
    return visited;
  };
  ASSERT_EQ(emplace(0), 0); // i == 0
  ASSERT_EQ(emplace(1), 0);
  ASSERT_EQ(emplace(2), 2);
  ASSERT_EQ(emplace(3), 3);
  ASSERT_EQ(emplace(4), 3);
  ASSERT_EQ(emplace(5), 4);
  ASSERT_EQ(emplace(6), 4);
  ASSERT_EQ(emplace(7), 4);
  ASSERT_EQ(emplace(8), 5);
  ASSERT_EQ(emplace(9), 5);  // i == 9
  ASSERT_EQ(emplace(-1), 0); // i == 10
  ASSERT_EQ(emplace(10), 2); // i == 11
  ASSERT_EQ(emplace(11), 3); // i == 12
  ASSERT_EQ(emplace(5), 3);  // i == 13
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

TEST(Insertion, ReversedPush) {
  skip_list<int, std::greater<>> sl;
  std::seed_seq seed{11};
  sl.set_seed(seed);
  ASSERT_NO_THROW(sl.push(5));
  ASSERT_NO_THROW(sl.push(9));
  ASSERT_GT(sl.front(), sl.back());
  ASSERT_NO_THROW(sl.push(3));
  ASSERT_EQ(sl.back(), 3);
  ASSERT_NO_THROW(sl.push(1));
  ASSERT_EQ(sl.back(), 1);
  ASSERT_EQ(sl.front(), 9);
  ASSERT_NO_THROW(sl.push(13));
  ASSERT_EQ(sl.front(), 13);
  ASSERT_NO_THROW(sl.push(100));
  ASSERT_EQ(sl.front(), 100);
}

TEST(Insertion, ReversedPush_100) {
  std::mt19937 gen{100};
  std::uniform_int_distribution<int> uniform_dist(-100, 100);
  std::vector<int> vec;
  skip_list<int, std::greater<>> sl;
  std::seed_seq seed{7};
  sl.set_seed(seed);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_TRUE(sl.empty());
  for (std::size_t i = 0u; i < 100; ++i) {
    int value = uniform_dist(gen);
    ASSERT_NO_THROW(vec.emplace_back(value));
    ASSERT_NO_THROW(sl.push(value));
  }
  ASSERT_EQ(vec.size(), sl.size());
  std::sort(vec.begin(), vec.end(), std::greater<>());
  auto it = sl.begin();
  for (auto val : vec) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(val, *it);
    ++it;
  }
}

TEST(Insertion, ReversedPush_1000) {
  std::mt19937 gen{10};
  std::uniform_int_distribution<int> uniform_dist(-100, 100);
  std::vector<int> vec;
  skip_list<int, std::greater<>> sl;
  std::seed_seq seed{3489005176};
  sl.set_seed(seed);
  ASSERT_NO_THROW(sl.clear());
  ASSERT_EQ(sl.size(), 0);
  ASSERT_TRUE(sl.empty());
  for (std::size_t i = 0u; i < 1000; ++i) {
    int value = uniform_dist(gen);
    ASSERT_NO_THROW(vec.emplace_back(value));
    ASSERT_NO_THROW(sl.push(value));
  }
  ASSERT_EQ(vec.size(), sl.size());
  std::sort(vec.begin(), vec.end(), std::greater<>());
  auto it = sl.begin();
  for (auto val : vec) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(val, *it);
    ++it;
  }
}

TEST(Insertion, PushMultipleSeeds) {
  for (std::size_t seed = 0u; seed <= 100; ++seed) {
    std::mt19937 gen{seed};
    std::uniform_int_distribution<int> uniform_dist(-100, 100);
    std::vector<int> vec, vecRev;
    skip_list<int> sl;
    skip_list<int, std::greater<>> slRev;
    std::seed_seq seed_seq{seed};
    sl.set_seed(seed_seq);
    ASSERT_NO_THROW(sl.clear());
    ASSERT_EQ(sl.size(), 0);
    ASSERT_EQ(slRev.size(), 0);
    ASSERT_TRUE(sl.empty());
    ASSERT_TRUE(slRev.empty());
    for (std::size_t i = 0u; i < 10; ++i) {
      int value = uniform_dist(gen);
      ASSERT_NO_THROW(vec.emplace_back(value));
      ASSERT_NO_THROW(vecRev.emplace_back(value));
      ASSERT_NO_THROW(sl.push(value));
      ASSERT_NO_THROW(slRev.push(value));
    }
    ASSERT_EQ(vec.size(), sl.size());
    ASSERT_EQ(vecRev.size(), slRev.size());
    ASSERT_EQ(vecRev.size(), vec.size());
    std::sort(vec.begin(), vec.end());
    std::sort(vecRev.begin(), vecRev.end(), std::greater<>());
    auto it = sl.begin();
    auto itRev = slRev.begin();
    for (std::size_t i = 0u; i < vec.size(); ++i, ++it, ++itRev) {
      ASSERT_NE(it, nullptr);
      ASSERT_NE(itRev, nullptr);
      ASSERT_EQ(vec[i], *it);
      ASSERT_EQ(vecRev[i], *itRev);
    }
  }
}

TEST(Insertion, VisitedNodesReversedEmplace) {

  skip_list<int, std::greater<>> sl;
  auto SEED = std::seed_seq({123456});
  sl.set_seed(SEED);
  std::mt19937 gen;
  gen.seed(SEED);
  /*
   *       0
   *     0 0 0
   * 0   0 0 0
   * 0 0 0 0 0
   * =======================
   * 5 4 3 2 1
   */
  for (int i = 0; i < 20; ++i) {
    // Code only to check what size of node is
    node<int> ng(i, gen);
    ASSERT_GE(ng.size(), 1);
    // std::cout << i << " = " << ng.size() << '\n';
  }
  auto emplace = [&](int value) {
    std::size_t visited = 0;
    sl.emplace(value, &visited);
    return visited;
  };
  ASSERT_EQ(emplace(5), 0); // i == 0
  ASSERT_EQ(emplace(4), 0);
  ASSERT_EQ(emplace(3), 2);
  ASSERT_EQ(emplace(2), 2);
  ASSERT_EQ(emplace(1), 3);
  ASSERT_EQ(emplace(10), 0); // i = 5, h = 2
  ASSERT_EQ(emplace(6), 1);  // i = 6, h = 3
  ASSERT_EQ(emplace(0), 5);  // i = 7, h = 4
  auto it = sl.begin();
  ASSERT_EQ(*(it++), 10);
  for (int i = 6; i >= 0; --i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
}

auto lambda_push = [](int value) -> skip_list<int> & {
  static skip_list<int> sl;
  sl.push(value);
  return sl;
};

auto lambda_emplace = [](int value) -> skip_list<int> & {
  static skip_list<int> sl;
  sl.emplace(value);
  return sl;
};

INSTANTIATE_TEST_CASE_P(Push, InsertionPushEmplace,
                        ::testing::Values(lambda_push));

INSTANTIATE_TEST_CASE_P(Emplace, InsertionPushEmplace,
                        ::testing::Values(lambda_emplace));
