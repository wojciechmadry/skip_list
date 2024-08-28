# Skip list

The project was created to learn [bazel](https://bazel.build/).

The project implements the skip list in C++20, with tests and other tools used with bazel.

Skip list interface based on the [`std::list`](https://en.cppreference.com/w/cpp/container/list) and [`std::set`](https://en.cppreference.com/w/cpp/container/set) functions.

# Requirements

* Your project must support `C++20`
* `Dazel`(`Bazel`) if you want to run tests
* [`genhtml`](https://linux.die.net/man/1/genhtml) to generate code coverage html

# Install

It is a header-only library.

Copy [`include/skip_list/skip_list.hpp`](include/skip_list/skip_list.hpp) into your project, include it and voilà

# Examples

#### Init list
```c++
  sl::skip_list<int> my_list{1,2,3}; // {1, 2, 3}
  sl::skip_list<int> cpy_list(my_list); // {1, 2, 3}
  sl::skip_list<int> cpy_list_iterator(my_list.begin(), my_list.end()); // {1, 2, 3}
  sl::skip_list<int> mv_list(std::move(my_list) /*-> {} */); // {1, 2, 3}
```

#### Finding with skipping
```c++
sl::skip_list<int> my_list{1, 3, 7, 8};
auto not_found = my_list.find(4);
ASSERT_EQ(not_found, my_list.end());
auto found = my_list.find(7);
ASSERT_NE(found, my_list.end());
ASSERT_EQ(*found, 7);
```

#### Add/Remove elements
```c++
sl::skip_list<int> my_list{5}; // {5}
// insert == push == emplace
my_list.insert(2); // {2, 5}
my_list.push(6); // {2, 5, 6}
my_list.emplace(3); // {2, 3, 5, 6}
auto next = my_list.erase(5); // {2, 3, 6}
ASSERT_EQ(*next, 6);
auto extractNoExist = my_list.extract(4);
ASSERT_FALSE(extractNoExist.has_value());
auto extractExist = my_list.extract(3);
ASSERT_TRUE(extractExist.has_value());
ASSERT_EQ(*extractExist, 3);
// my_list == {2, 6}
```

#### Iterating over list
```c++
sl::skip_list<int> my_list{1, 3, 7, 8};
int sum = 0;
for(auto it = my_list.begin(); it != my_list.end() ; ++it) {
  sum += *it;
}

for(const auto& el : my_list) {
  sum += el;
}
ASSERT_EQ(sum, 2 * std::accumulate(my_list.begin(), my_list.end(), 0));
```

#### Reverse sorting
```c++
sl::skip_list<int, std::greater<>> my_list{5, 3, 7, 12, -5};
// my_list == {12, 7, 5, 3, -5}
```

# Performance

TBD: vs std::list/std::set

# Tests

All tests described below are also run in CI.

Some tests require a `compile_commands.json` file to generate it run:
```sh
dazel run //:generate_cc
```

Unit tests:
```sh
dazel test //tests:skip_list_gtest_tests
```

[clang-tidy](https://clang.llvm.org/extra/clang-tidy/):
```sh
dazel test //tests:clang_tidy_skip_list_test
```

Generate code coverage:
```sh
dazel coverage //tests:skip_list_gtest_tests --combined_report=lcov --instrumentation_filter="/include"
genhtml bazel-out/_coverage/_coverage_report.dat -o test_coverage
```

Check code style:
```sh
dazel run //:check_format
```

Fix code style:
```
dazel run //:format
```

[cppcheck](https://cppcheck.sourceforge.io/):
```sh
dazel test //tests:cppcheck_skip_list_test
```

