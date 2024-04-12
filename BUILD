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
  actual = "//tests:skip_list_tests",
)

alias(
  name = "generate_cc",
  actual = ":refresh_compile_commands"
)


refresh_compile_commands(
    name = "refresh_compile_commands",

    targets = {
      "//tests/...": "",
    },
)
