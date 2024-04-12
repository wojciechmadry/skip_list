load("@aspect_rules_lint//format:defs.bzl", "format_multirun", "format_test")

format_multirun(
  name = "format",
  visibility = ["//visibility:public"],
  cc = "@llvm_toolchain_llvm//:bin/clang-format",
)

format_test(
  name = "format_test",
  visibility = ["//visibility:public"],
  cc = "@llvm_toolchain_llvm//:bin/clang-format",
  srcs = ["*.hpp", ".cpp"],
)
