load("@rules_cc//cc:defs.bzl", "cc_library")

filegroup(
  name = "skip_list_hdr",
  srcs = ["skip_list.hpp"],
  visibility = ["//visibility:public"],
)

cc_library(
    name = "skip_list",
    hdrs = [":skip_list_hdr"],
    includes = ["."],
    visibility = ["//visibility:public"],
)
