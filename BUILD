load("@hedron_compile_commands//:refresh_compile_commands.bzl", "refresh_compile_commands")

alias(
  name = "format",
  actual = "//tools/format",
)

alias(
  name = "check_format",
  actual = "//tools/format:format.check",
)

alias(
  name = "test",
  actual = "//tests:skip_list_gtest_tests",
)

alias(
  name = "generate_cc",
  actual = ":refresh_compile_commands"
)

filegroup(
       name = "clang_tidy_config",
       srcs = [".clang-tidy"],
       visibility = ["//visibility:public"],
)

filegroup(
       name = "cppcheck_flags",
       srcs = [".cppcheck"],
       visibility = ["//visibility:public"],
)

refresh_compile_commands(
    name = "refresh_compile_commands",

    targets = {
      "//tests/...": "",
    },
)
