#include <gtest/gtest.h>

#include "allocator.hpp"

TEST(Allocator, ErasedNodeIsReleasedThroughAllocator) {
  alloc_counters counters;
  {
    tracked_skip_list list(
        std::less<int>{},
        counting_allocator<sl::node<int, sl::DEFAULT_MAX_NODE_SIZE>>{
            &counters});
    list.push(10);
    list.push(20);
    list.push(30);

    ASSERT_GT(counters.allocations, 0u);

    list.erase(20);
    EXPECT_EQ(counters.deallocations, 1u)
        << "erase_node()'s bypassed the allocator ";
  }
  EXPECT_EQ(counters.allocations, counters.deallocations)
      << "every allocation must be matched by a deallocation via the "
         "allocator ";
}

TEST(Allocator, CreateNodeUsesListAllocator) {
  alloc_counters counters;
  tracked_skip_list list(
      std::less<int>{},
      counting_allocator<sl::node<int, sl::DEFAULT_MAX_NODE_SIZE>>{&counters});

  list.push(10);
  list.push(20);
  list.push(30);

  EXPECT_EQ(counters.allocations, 3u)
      << "create_node() ignored m_allocator and used a temporary allocator";
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

auto operator<=>(const my_struct &lhs, const my_struct &rhs) {
  EXPECT_NE(lhs.ptr, nullptr);
  EXPECT_NE(rhs.ptr, nullptr);
  return *lhs.ptr <=> *rhs.ptr;
}

} // namespace

TEST(Operators, CustomAllocator) {
  using T = my_struct;
  using node_type = sl::node<T, sl::DEFAULT_MAX_NODE_SIZE>;
  custom_allocator<node_type> custom;
  using allocator_traits_custom = typename std::allocator_traits<
      custom_allocator<node_type>>::template rebind_traits<node_type>;

  std::allocator<node_type> stl;
  using allocator_traits_stl = typename std::allocator_traits<
      std::allocator<node_type>>::template rebind_traits<node_type>;

  auto custom_p = custom.allocate(8);
  auto stl_p = stl.allocate(8);
  allocator_traits_stl::construct(stl, stl_p, sl::DEFAULT_MAX_NODE_SIZE, 5);
  allocator_traits_stl::construct(stl, stl_p + 1, sl::DEFAULT_MAX_NODE_SIZE,
                                  10);
  stl_p->get_next(0) = stl_p + 1;
  allocator_traits_custom::construct(custom, custom_p,
                                     sl::DEFAULT_MAX_NODE_SIZE, 5);
  allocator_traits_custom::construct(custom, custom_p + 1,
                                     sl::DEFAULT_MAX_NODE_SIZE, 10);
  custom_p->get_next(0) = custom_p + 1;
  EXPECT_EQ(*custom_p->get().ptr, 5);
  EXPECT_EQ(*custom_p->get_next(0)->get().ptr, 10);
  EXPECT_EQ(*stl_p->get().ptr, 5);
  EXPECT_EQ(*stl_p->get_next(0)->get().ptr, 10);
  allocator_traits_stl::destroy(stl, stl_p);
  allocator_traits_stl::destroy(stl, stl_p + 1);
  allocator_traits_custom::destroy(custom, custom_p);
  allocator_traits_custom::destroy(custom, custom_p + 1);
  stl.deallocate(stl_p, 8);
  custom.deallocate(custom_p, 8);
  sl::skip_list<T, std::less<T>, 50, 5,
                custom_allocator<sl::node<T, sl::DEFAULT_MAX_NODE_SIZE>>>
      sl;
  std::set<T, std::less<T>, custom_allocator<T>> set;
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

TEST(Operators, TypeConstructThrow) {
  struct ThrowType {
    explicit ThrowType(int) { throw std::runtime_error(""); }
    bool operator<(const ThrowType &) const { return true; }
  };
  sl::skip_list<ThrowType> sl;
  EXPECT_THROW(sl.insert(1), std::runtime_error);
}
