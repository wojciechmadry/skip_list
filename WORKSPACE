load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load(
    "@bazel_tools//tools/build_defs/repo:git.bzl",
    "git_repository",
)

http_archive(
  name = "com_google_googletest",
  urls = ["https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip"],
  strip_prefix = "googletest-f8d7d77c06936315286eb55f8de22cd23c188571",
  integrity = "sha256-uXbPT9V7MYr9sb2yf8cIkEs+S+1IKFnrlLorS90Hf+I=",
)

git_repository(
  name = "bazel_clang_tidy",
  commit = "bd242cd869fad9f5644e0917971ab592b91cc3ae",
  remote = "https://github.com/wojciechmadry/bazel_clang_tidy.git",
)

git_repository(
  name = "bazel_cppcheck",
  commit = "754843a7373b5fb55ddd32b501a786aba531daba",
  remote = "https://github.com/wojciechmadry/bazel_cppcheck.git",
)
