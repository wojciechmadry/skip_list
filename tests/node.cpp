#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <vector>

using namespace sl;

TEST(Node, Constructor) {
  std::mt19937 gen(0);
  ASSERT_NO_THROW(node<int>(5, gen));
  constexpr std::size_t MAX_SIZE = 10u;
  node<int, MAX_SIZE> nd(12, gen);
  ASSERT_EQ(nd.size(), 6);
  ASSERT_LE(nd.size(), MAX_SIZE);
  ASSERT_EQ(nd.get(), 12);
  for (std::size_t i = 0u; i < MAX_SIZE; ++i) {
    ASSERT_EQ(nd.get_next(i), nullptr);
  }
  nd.get() = 5;
  ASSERT_EQ(nd.get(), 5);
}

TEST(Node, clear_nexts) {
  constexpr int SEED = 1;
  std::mt19937 gen(SEED);
  node<int> nd0(1, gen);
  node<int> nd1(2, gen);
  for (std::size_t i = 0u; i < nd0.size(); ++i) {
    ASSERT_EQ(nd0.get_next(i), nullptr);
    nd0.get_next(i) = &nd1;
    ASSERT_EQ(nd0.get_next(i), &nd1);
  }
  ASSERT_NO_THROW(nd0.clear_nexts());
  for (std::size_t i = 0u; i < nd0.size(); ++i) {
    ASSERT_EQ(nd0.get_next(i), nullptr);
  }
}

TEST(Node, rbegin_rend) {
  constexpr int SEED = 1;
  constexpr std::size_t MAX_SIZE = 5u;
  std::mt19937 gen(SEED);
  node<int, MAX_SIZE> nd0(1, gen);
  node<int, MAX_SIZE> nd1(2, gen);
  ASSERT_EQ(nd0.size(), 3);
  nd0.get_next(0) = &nd1;
  nd0.get_next(1) = &nd1;
  nd0.get_next(2) = &nd1;
  std::size_t count_iterations = 0u;
  for (auto it = nd0.rbegin(); it != nd0.rend(); ++it, ++count_iterations) {
    ASSERT_NE(*it, nullptr);
    ASSERT_EQ(*it, &nd1);
  }
  ASSERT_EQ(count_iterations, nd0.size());
}

TEST(Node, fill_nexts_insert_third) {
  constexpr int SEED = 1;
  std::mt19937 gen(SEED);
  node<int> nd0(1, gen);
  node<int> nd1(2, gen);
  ASSERT_EQ(nd0.get(), 1);
  ASSERT_EQ(nd1.get(), 2);
  ASSERT_GT(nd0.size(), 2);
  ASSERT_GT(nd1.size(), 2);
  for (std::size_t i = 0u; i < nd0.size(); ++i) {
    nd0.get_next(i) = &nd1;
  }
  node<int> nd2(3, gen);
  ASSERT_GT(nd2.size(), 2);
  nd2.get_next(0) = &nd0;
  ASSERT_NO_THROW(nd2.fill_nexts());
}

TEST(Node, fill_nexts_1) {
  constexpr int SEED = 1;
  std::mt19937 gen(SEED);
  node<int> nd(1, gen);
  ASSERT_EQ(nd.get(), 1);
  ASSERT_GT(nd.size(), 2);
  nd.get_next(0) = &nd;
  ASSERT_EQ(&nd, nd.get_next(0));
  ASSERT_NO_THROW(nd.fill_nexts());
  for (std::size_t i = 0u; i < nd.size(); ++i) {
    ASSERT_EQ(&nd, nd.get_next(i));
  }
}

TEST(Node, fill_nexts_5) {
  constexpr int SEED = 1;
  constexpr std::size_t SIZE = 5;
  ASSERT_GE(SIZE, 2u);
  std::mt19937 gen(SEED);
  std::vector<node<int>> nodes;
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes.emplace_back(i, gen));
    ASSERT_EQ(nodes.back().get(), i);
    ASSERT_GT(nodes.back().size(), 0);
  }
  /*
   *   0---0
   *   0-0-0
   * 0-0-0-0
   * 0-0-0-0
   * 0-0-0-0-0
   */
  for (std::size_t i = 0u; i < SIZE - 1u; ++i) {
    nodes[i].get_next(0) = &nodes[i + 1u];
  }
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes[i].fill_nexts());
  }

  // Check for first node
  ASSERT_EQ(nodes[0].get_next(2), &nodes[1]);
  ASSERT_EQ(nodes[0].get_next(1), &nodes[1]);
  ASSERT_EQ(nodes[0].get_next(0), &nodes[1]);

  // Check for second node
  ASSERT_EQ(nodes[1].get_next(0), &nodes[2]);
  ASSERT_EQ(nodes[1].get_next(1), &nodes[2]);
  ASSERT_EQ(nodes[1].get_next(2), &nodes[2]);
  ASSERT_EQ(nodes[1].get_next(3), &nodes[2]);
  ASSERT_EQ(nodes[1].get_next(4), &nodes[3]);

  // Check for third node
  ASSERT_EQ(nodes[2].get_next(0), &nodes[3]);
  ASSERT_EQ(nodes[2].get_next(1), &nodes[3]);
  ASSERT_EQ(nodes[2].get_next(2), &nodes[3]);
  ASSERT_EQ(nodes[2].get_next(3), &nodes[3]);

  // Check for fourth node
  ASSERT_EQ(nodes[3].get_next(0), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(1), nullptr);
  ASSERT_EQ(nodes[3].get_next(2), nullptr);
  ASSERT_EQ(nodes[3].get_next(3), nullptr);
  ASSERT_EQ(nodes[3].get_next(4), nullptr);
}

TEST(Node, fill_nexts_10) {
  constexpr int SEED = 2;
  constexpr std::size_t SIZE = 10;
  ASSERT_GE(SIZE, 2u);
  std::mt19937 gen(SEED);
  std::vector<node<int>> nodes;
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes.emplace_back(i, gen));
    ASSERT_EQ(nodes.back().get(), i);
    ASSERT_GT(nodes.back().size(), 0);
  }
  /*
   *       0   0
   *       0   0
   * 0     0 0 0 0 0 0
   * 0     0 0 0 0 0 0 0
   * 0 0 0 0 0 0 0 0 0 0
   */
  for (std::size_t i = 0u; i < SIZE - 1u; ++i) {
    nodes[i].get_next(0) = &nodes[i + 1u];
  }
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes[i].fill_nexts());
  }

  // Check for 1 node
  ASSERT_EQ(nodes[0].get_next(2), &nodes[3]);
  ASSERT_EQ(nodes[0].get_next(1), &nodes[3]);
  ASSERT_EQ(nodes[0].get_next(0), &nodes[1]);

  // Check for 2 node
  ASSERT_EQ(nodes[1].get_next(0), &nodes[2]);

  // Check for 3 node
  ASSERT_EQ(nodes[2].get_next(0), &nodes[3]);

  // Check for 4 node
  ASSERT_EQ(nodes[3].get_next(0), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(1), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(2), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(3), &nodes[5]);
  ASSERT_EQ(nodes[3].get_next(4), &nodes[5]);

  // Check for 5 node
  ASSERT_EQ(nodes[4].get_next(0), &nodes[5]);
  ASSERT_EQ(nodes[4].get_next(1), &nodes[5]);
  ASSERT_EQ(nodes[4].get_next(2), &nodes[5]);

  // Check for 6 node
  ASSERT_EQ(nodes[5].get_next(0), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(1), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(2), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(3), nullptr);
  ASSERT_EQ(nodes[5].get_next(4), nullptr);

  // Check for 7 node
  ASSERT_EQ(nodes[6].get_next(0), &nodes[7]);
  ASSERT_EQ(nodes[6].get_next(1), &nodes[7]);
  ASSERT_EQ(nodes[6].get_next(2), &nodes[7]);

  // Check for 8 node
  ASSERT_EQ(nodes[7].get_next(0), &nodes[8]);
  ASSERT_EQ(nodes[7].get_next(1), &nodes[8]);
  ASSERT_EQ(nodes[7].get_next(2), &nodes[8]);

  // Check for 9 node
  ASSERT_EQ(nodes[8].get_next(0), &nodes[9]);
  ASSERT_EQ(nodes[8].get_next(1), &nodes[9]);
  ASSERT_EQ(nodes[8].get_next(2), nullptr);

  // Check for 10 node
  ASSERT_EQ(nodes[9].get_next(0), nullptr);
  ASSERT_EQ(nodes[9].get_next(1), nullptr);
}
