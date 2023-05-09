
cc_library(
    name = "main",
    srcs = glob(
        ["googletest-release-1.10.0/src/*.cc"],
        exclude = ["googletest-release-1.10.0/src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest-release-1.10.0/include/**/*.h",
        "googletest-release-1.10.0/src/*.h"
    ]),
    copts = [
        "-Iexternal/gtest/googletest-release-1.10.0/include",
        "-Iexternal/gtest/googletest-release-1.10.0"
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)

