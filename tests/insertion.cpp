#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <vector>

#include "skip_list.hpp"

class InsertionPushEmplace
    : public ::testing::TestWithParam<std::function<skip_list<int> &(int)>> {};

TEST_P(InsertionPushEmplace, InsertData) {
  auto func = GetParam();
  auto &sl = func(2);
  ASSERT_EQ(sl.front(), 2);
  func(3);
  ASSERT_EQ(sl.front(), 2);
  // func(1);
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
