load("@bazel_clang_tidy//clang_tidy:clang_tidy_test.bzl", "clang_tidy_test")
load("@bazel_cppcheck//cppcheck:cppcheck.bzl", "cppcheck_test")

filegroup(
    name = "test_sources",
    srcs = glob([
        "*.cpp",
    ]),
)

clang_tidy_test(
  name = "clang_tidy_skip_list_test",
  clang_tidy_config = "//:clang_tidy_config",
  srcs = [
    "//include/skip_list:skip_list_hdr",
  ],
  use_flags = False,
    visibility = ["//visibility:public"],
  deps = ["@com_google_googletest//:gtest_main", "//include/skip_list:skip_list"],
)

cppcheck_test(
  name = "cppcheck_skip_list_test",
  srcs = [
    "//include/skip_list:skip_list_hdr",
  ],
  visibility = ["//visibility:public"],
deps = ["@com_google_googletest//:gtest_main", "//include/skip_list:skip_list"],
  cppcheck_flags = "//:cppcheck_flags",
)

cc_test(
  name = "skip_list_gtest_tests",
  size = "small",
  srcs = [
      ":test_sources"
      ],
  deps = ["@com_google_googletest//:gtest_main", "//include/skip_list:skip_list"],
  visibility = ["//visibility:public"],
)
