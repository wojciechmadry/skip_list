#include "skip_list.hpp"
#include <gtest/gtest.h>
#include <memory>

TEST(Examples, InitList) {
  sl::skip_list<int> my_list{1, 2, 3};  // {1, 2, 3}
  sl::skip_list<int> cpy_list(my_list); // {1, 2, 3}
  sl::skip_list<int> cpy_list_iterator(my_list.begin(),
                                       my_list.end()); // {1, 2, 3}
  sl::skip_list<int> mv_list(std::move(my_list));      // {1, 2, 3}

  ASSERT_EQ(cpy_list, cpy_list_iterator);
  ASSERT_EQ(cpy_list_iterator, mv_list);
}

TEST(Examples, AddRemoveElements) {
  sl::skip_list<int> my_list{5}; // {5}
  // insert == push == emplace
  my_list.insert(2);            // {2, 5}
  my_list.push(6);              // {2, 5, 6}
  my_list.emplace(3);           // {2, 3, 5, 6}
  auto next = my_list.erase(5); // {2, 3, 6}
  ASSERT_EQ(*next, 6);
  auto extractNoExist = my_list.extract(4);
  ASSERT_FALSE(extractNoExist.has_value());
  auto extractExist = my_list.extract(3);
  ASSERT_TRUE(extractExist.has_value());
  ASSERT_EQ(*extractExist, 3);
  // my_list == {2, 6}
  ASSERT_EQ(my_list.front(), 2);
  ASSERT_EQ(my_list.back(), 6);
  ASSERT_EQ(my_list.size(), 2);
}

TEST(Examples, Find) {
  sl::skip_list<int> my_list{1, 3, 7, 8};
  auto not_found = my_list.find(4);
  ASSERT_EQ(not_found, my_list.end());
  auto found = my_list.find(7);
  ASSERT_NE(found, my_list.end());
  ASSERT_EQ(*found, 7);
}

TEST(Examples, IteratingOverList) {
  sl::skip_list<int> my_list{1, 3, 7, 8};
  int sum = 0;
  for (auto it = my_list.begin(); it != my_list.end(); ++it) {
    sum += *it;
  }

  for (const auto &el : my_list) {
    sum += el;
  }
  ASSERT_EQ(sum, 2 * std::accumulate(my_list.begin(), my_list.end(), 0));
}

TEST(Examples, ReverseSorting) {
  sl::skip_list<int, std::greater<>> my_list{5, 3, 7, 12, -5};
  // my_list == {12, 7, 5, 3, -5}
  auto it = my_list.begin();
  ASSERT_EQ(*it, 12);
  ++it;
  ASSERT_EQ(*it, 7);
  ++it;
  ASSERT_EQ(*it, 5);
  ++it;
  ASSERT_EQ(*it, 3);
  ++it;
  ASSERT_EQ(*it, -5);
}
