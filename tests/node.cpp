#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <vector>

using namespace sl;

TEST(Node, Constructor) {
  ASSERT_NO_THROW(node<int>(5, 5));
  constexpr std::size_t MAX_SIZE = 10u;
  node<int, MAX_SIZE> nd(8, 12);
  ASSERT_EQ(nd.capacity(), 8);
  ASSERT_LE(nd.capacity(), MAX_SIZE);
  ASSERT_EQ(nd.get(), 12);
  for (std::size_t i = 0u; i < MAX_SIZE; ++i) {
    ASSERT_EQ(nd.get_next(i), nullptr);
  }
  nd.get() = 5;
  ASSERT_EQ(nd.get(), 5);
}

TEST(Node, clear_nexts) {
  node<int> nd0(1, 2);
  node<int> nd1(2, 2);
  for (std::size_t i = 0u; i < nd0.capacity(); ++i) {
    ASSERT_EQ(nd0.get_next(i), nullptr);
    nd0.get_next(i) = &nd1;
    ASSERT_EQ(nd0.get_next(i), &nd1);
  }
  ASSERT_NO_THROW(nd0.clear_nexts());
  for (std::size_t i = 0u; i < nd0.capacity(); ++i) {
    ASSERT_EQ(nd0.get_next(i), nullptr);
  }
}

TEST(Node, rbegin_rend) {
  constexpr std::size_t MAX_SIZE = 5u;
  node<int, MAX_SIZE> nd0(2, 1);
  node<int, MAX_SIZE> nd1(2, 2);
  ASSERT_EQ(nd0.capacity(), 2);
  nd0.get_next(0) = &nd1;
  nd0.get_next(1) = &nd1;
  nd0.get_next(2) = &nd1;
  std::size_t count_iterations = 0u;
  for (auto it = nd0.rbegin(); it != nd0.rend(); ++it, ++count_iterations) {
    ASSERT_NE(*it, nullptr);
    ASSERT_EQ(*it, &nd1);
  }
  ASSERT_EQ(count_iterations, nd0.capacity());
}

TEST(Node, fill_nexts_insert_third) {
  node<int> nd0(2, 1);
  node<int> nd1(2, 2);
  ASSERT_EQ(nd0.get(), 1);
  ASSERT_EQ(nd1.get(), 2);
  ASSERT_GE(nd0.capacity(), 2);
  ASSERT_GE(nd1.capacity(), 2);
  for (std::size_t i = 0u; i < nd0.capacity(); ++i) {
    nd0.get_next(i) = &nd1;
  }
  node<int> nd2(2, 3);
  ASSERT_GE(nd2.capacity(), 2);
  nd2.get_next(0) = &nd0;
  ASSERT_NO_THROW(nd2.fill_nexts());
}

TEST(Node, fill_nexts_1) {
  node<int> nd(2, 1);
  ASSERT_EQ(nd.get(), 1);
  ASSERT_GE(nd.capacity(), 2);
  nd.get_next(0) = &nd;
  ASSERT_EQ(&nd, nd.get_next(0));
  ASSERT_NO_THROW(nd.fill_nexts());
  for (std::size_t i = 0u; i < nd.capacity(); ++i) {
    ASSERT_EQ(&nd, nd.get_next(i));
  }
}

TEST(Node, fill_nexts_5) {
  constexpr std::size_t SIZE = 5;
  ASSERT_GE(SIZE, 2u);
  std::vector<node<int>> nodes;
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes.emplace_back(i + 1, i));
    ASSERT_EQ(nodes.back().get(), i);
    ASSERT_EQ(nodes.back().capacity(), i + 1);
  }
  for (std::size_t i = 0u; i < SIZE - 1u; ++i) {
    nodes[i].get_next(0) = &nodes[i + 1u];
  }
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes[i].fill_nexts());
  }

  // Check for first node
  ASSERT_EQ(nodes[0].get_next(0), &nodes[1]);

  // Check for second node
  ASSERT_EQ(nodes[1].get_next(0), &nodes[2]);
  ASSERT_EQ(nodes[1].get_next(1), &nodes[2]);

  // Check for third node
  ASSERT_EQ(nodes[2].get_next(0), &nodes[3]);
  ASSERT_EQ(nodes[2].get_next(1), &nodes[3]);
  ASSERT_EQ(nodes[2].get_next(2), &nodes[3]);

  // Check for fourth node
  ASSERT_EQ(nodes[3].get_next(0), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(1), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(2), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(3), &nodes[4]);
}

TEST(Node, fill_nexts_10) {
  constexpr std::size_t SIZE = 10;
  ASSERT_GE(SIZE, 2u);
  std::vector<node<int>> nodes;
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes.emplace_back(SIZE, i));
    ASSERT_EQ(nodes.back().get(), i);
    ASSERT_GE(nodes.back().capacity(), 1);
  }

  for (std::size_t i = 0u; i < SIZE - 1u; ++i) {
    nodes[i].get_next(0) = &nodes[i + 1u];
  }
  for (std::size_t i = 0u; i < SIZE; ++i) {
    ASSERT_NO_THROW(nodes[i].fill_nexts());
  }

  // Check for 1 node
  ASSERT_EQ(nodes[0].get_next(2), &nodes[1]);
  ASSERT_EQ(nodes[0].get_next(1), &nodes[1]);
  ASSERT_EQ(nodes[0].get_next(0), &nodes[1]);

  // Check for 2 node
  ASSERT_EQ(nodes[1].get_next(0), &nodes[2]);

  // Check for 3 node
  ASSERT_EQ(nodes[2].get_next(0), &nodes[3]);

  // Check for 4 node
  ASSERT_EQ(nodes[3].get_next(0), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(1), &nodes[4]);
  ASSERT_EQ(nodes[3].get_next(2), &nodes[4]);

  // Check for 5 node
  ASSERT_EQ(nodes[4].get_next(0), &nodes[5]);
  ASSERT_EQ(nodes[4].get_next(1), &nodes[5]);
  ASSERT_EQ(nodes[4].get_next(2), &nodes[5]);

  // Check for 6 node
  ASSERT_EQ(nodes[5].get_next(0), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(1), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(2), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(3), &nodes[6]);
  ASSERT_EQ(nodes[5].get_next(4), &nodes[6]);

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
  ASSERT_EQ(nodes[8].get_next(2), &nodes[9]);

  // Check for 10 node
  ASSERT_EQ(nodes[9].get_next(0), nullptr);
  ASSERT_EQ(nodes[9].get_next(1), nullptr);
}
