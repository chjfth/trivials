
cc_library(
    name = "gtestmain",
#    srcs = ["googletest-release-1.10.0/googletest/src/gtest.cc"],
    srcs = glob(
        ["googletest-release-1.10.0/googletest/src/*.cc"],
        exclude = ["googletest-release-1.10.0/googletest/src/gtest-all.cc"]
    ),
    hdrs = glob([
        "googletest-release-1.10.0/googletest/include/**/*.h",
        "googletest-release-1.10.0/googletest/src/*.h"
    ]),
    copts = [
        "-I external/gtest/googletest-release-1.10.0/googletest/include",
        "-I external/gtest/googletest-release-1.10.0/googletest",
    ],
    linkopts = ["-pthread"],
    visibility = ["//visibility:public"],
)


##