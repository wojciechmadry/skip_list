#include <gtest/gtest.h>

#include "skip_list.hpp"
#include <memory>
#include <random>
#include <string>

using namespace sl;

TEST(Operations, Merge) {
  skip_list<int> sl1{1, 2, 3, 4};
  skip_list<int> sl2{0, 3, 5, 6};
  ASSERT_NO_THROW(sl2.merge(sl1));
  ASSERT_EQ(sl2.size(), 8u);
  ASSERT_EQ(sl1.size(), 0u);
  auto it = sl2.begin();
  for (int i = 0; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  for (int i = 3; i <= 6; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl2.end());
  const auto sl_before = sl1;
  const auto ptr_before = &sl1;
  ASSERT_NO_THROW(sl1.merge(sl1));
  ASSERT_EQ(sl1, sl_before);
  ASSERT_EQ(&sl1, ptr_before);
}

TEST(Operations, MergeStringsUniquePtr) {
  auto make_str = [](std::string str) {
    return std::make_unique<std::string>(std::move(str));
  };
  struct comp {
    bool operator()(const std::unique_ptr<std::string> &lhs,
                    const std::unique_ptr<std::string> &rhs) {
      return *lhs < *rhs;
    }
  };
  skip_list<std::unique_ptr<std::string>, comp> sl1;
  sl1.push(make_str("aa"));
  sl1.push(make_str("ak"));
  skip_list<std::unique_ptr<std::string>, comp> sl2;
  sl2.push(make_str("ab"));
  sl2.push(make_str("ac"));
  sl2.push(make_str("ae"));

  ASSERT_NO_THROW(sl2.merge(std::move(sl1)));
  ASSERT_EQ(sl2.size(), 5u);
  ASSERT_EQ(sl1.size(), 0u);
  auto it = sl2.begin();
  ASSERT_EQ(**(it++), "aa");
  ASSERT_EQ(**(it++), "ab");
  ASSERT_EQ(**(it++), "ac");
  ASSERT_EQ(**(it++), "ae");
  ASSERT_EQ(**(it++), "ak");
  ASSERT_EQ(it, sl2.end());
}

TEST(Operations, StdSwap) {
  skip_list<int> sl{1, 3, 2};
  skip_list<int> sl_neg{-3, -1, -2, 0};
  ASSERT_EQ(sl.size(), 3);
  ASSERT_EQ(sl_neg.size(), 4);
  ASSERT_NO_THROW(std::swap(sl, sl_neg));
  ASSERT_EQ(sl.size(), 4);
  ASSERT_EQ(sl_neg.size(), 3);
  auto it = sl.begin();
  for (int i = -3; i <= 0; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl.end());
  it = sl_neg.begin();
  for (int i = 1; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl_neg.end());
}

TEST(Operations, Swap) {
  skip_list<int> sl{1, 3, 2};
  skip_list<int> sl_neg{-3, -1, -2, 0};
  ASSERT_EQ(sl.size(), 3);
  ASSERT_EQ(sl_neg.size(), 4);
  ASSERT_NO_THROW(sl.swap(sl_neg));
  ASSERT_EQ(sl.size(), 4);
  ASSERT_EQ(sl_neg.size(), 3);
  auto it = sl.begin();
  for (int i = -3; i <= 0; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl.end());
  it = sl_neg.begin();
  for (int i = 1; i <= 3; ++i, ++it) {
    ASSERT_NE(it, nullptr);
    ASSERT_EQ(*it, i);
  }
  ASSERT_EQ(it, sl_neg.end());
  ASSERT_NO_THROW(sl.swap(sl));
}

namespace {
template <typename T> struct custom_allocator {
  using value_type = T;
  [[nodiscard]] constexpr T *allocate(std::size_t n) {
    return static_cast<T *>(::operator new(n * sizeof(T)));
  }

  constexpr void deallocate(T *p, std::size_t n) {
    (void)n;
    ::operator delete(p);
  }
};
struct my_struct {
  std::unique_ptr<int> ptr = nullptr;
  my_struct() = delete;
  explicit my_struct(int val) : ptr(new int{val}) {}
};

auto operator==(const my_struct &lhs, const my_struct &rhs) {
  EXPECT_NE(lhs.ptr, nullptr);
  EXPECT_NE(rhs.ptr, nullptr);
  return *lhs.ptr == *rhs.ptr;
}

auto operator<=>(const my_struct &lhs, const my_struct &rhs) {
  EXPECT_NE(lhs.ptr, nullptr);
  EXPECT_NE(rhs.ptr, nullptr);
  return *lhs.ptr <=> *rhs.ptr;
}

} // namespace

TEST(Operators, CustomAllocator) {
  using T = my_struct;
  custom_allocator<T> custom;
  std::allocator<T> stl;
  auto custom_p = custom.allocate(8);
  auto stl_p = stl.allocate(8);
  using traits_stl = std::allocator_traits<decltype(stl)>;
  using traits_custom = std::allocator_traits<decltype(custom)>;
  traits_stl::construct(stl, stl_p, T(5));
  traits_stl::construct(stl, stl_p + 1, T(10));
  traits_custom::construct(custom, custom_p, T(5));
  traits_custom::construct(custom, custom_p + 1, T(10));
  EXPECT_EQ(*custom_p->ptr, 5);
  EXPECT_EQ(*custom_p[1].ptr, 10);
  EXPECT_EQ(*stl_p->ptr, 5);
  EXPECT_EQ(*stl_p[1].ptr, 10);
  traits_stl::destroy(stl, stl_p);
  traits_stl::destroy(stl, stl_p + 1);
  traits_custom::destroy(custom, custom_p);
  traits_custom::destroy(custom, custom_p + 1);
  stl.deallocate(stl_p, 8);
  custom.deallocate(custom_p, 8);
  skip_list<T, std::less<T>, 50, 5, custom_allocator<T>> sl;
  std::set<T, std::less<T>, custom_allocator<T>> set;
  const auto sameType =
      std::is_same_v<std::decay_t<std::set<int>::allocator_type>,
                     std::allocator<int>>;
  std::set<int> defSet;
  const auto sameTypeDef =
      std::is_same_v<std::decay_t<decltype(defSet.get_allocator())>,
                     std::allocator<int>>;
  EXPECT_TRUE(sameType);
  EXPECT_TRUE(sameTypeDef);
  for (int i = 1; i <= 5; ++i) {
    ASSERT_NO_THROW(set.insert(T(i)));
    ASSERT_NO_THROW(sl.push(T(i)));
  }
  ASSERT_EQ(sl.size(), 5);
  ASSERT_EQ(sl.size(), set.size());
  for (int i = 1; i <= 5; ++i) {
    ASSERT_FALSE(sl.empty());
    auto found_set = set.find(T(i));
    auto found_sl = sl.find(T(i));
    ASSERT_NE(found_set, set.end());
    ASSERT_NE(found_sl, sl.end());
    ASSERT_EQ(*found_set->ptr, *found_sl->ptr);
  }
}

TEST(Operators, lowerBound) {
  const skip_list<int> empty;
  EXPECT_EQ(empty.lower_bound(0), empty.end());

  for (size_t seed = 0; seed <= 100; ++seed) {
    std::seed_seq SEED{seed};
    skip_list<int> sl;
    sl.set_seed(SEED);
    sl.push({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12});
    EXPECT_EQ(sl.lower_bound(std::numeric_limits<int>::max()), sl.end());

    auto check_bound = [&](const int key, const int expected) {
      auto returned = sl.lower_bound(key);
      ASSERT_NE(returned, sl.end());
      EXPECT_EQ(*returned, expected);
    };

    check_bound(0, 1);
    for (int i = 1; i <= 10; ++i) {
      check_bound(i, i);
    }
    check_bound(11, 12);
    check_bound(12, 12);
  }
}

TEST(Operators, upperBound) {
  const skip_list<int> empty;
  EXPECT_EQ(empty.upper_bound(0), empty.end());

  for (size_t seed = 0; seed <= 100; ++seed) {
    std::seed_seq SEED{seed};
    skip_list<int> sl;
    sl.set_seed(SEED);
    sl.push({-1, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12});
    EXPECT_EQ(sl.upper_bound(std::numeric_limits<int>::max()), sl.end());
    EXPECT_EQ(sl.upper_bound(12), sl.end());

    auto check_bound = [&](const int key, const int expected) {
      auto lb = sl.upper_bound(key);
      ASSERT_NE(lb, sl.end());
      EXPECT_EQ(*lb, expected);
    };

    check_bound(-2, -1);
    check_bound(-1, 1);
    check_bound(0, 1);
    for (int i = 1; i <= 9; ++i) {
      check_bound(i, i + 1);
    }
    check_bound(10, 12);
    check_bound(11, 12);
  }
}
