
cc_library(
	name = "main",
	srcs = glob(
		   ["googletest-release-1.10.0/googletest/src/*.cc"],
		exclude = ["googletest-release-1.10.0/googletest/src/gtest-all.cc"]
	),
	hdrs = glob([
		"googletest-release-1.10.0/googletest/include/**/*.h",
		"googletest-release-1.10.0/googletest/src/*.h"
	]),
	copts = [
		"-Iexternal/gtestX/googletest-release-1.10.0/googletest/include",
		"-Iexternal/gtestX/googletest-release-1.10.0/googletest"
	],
#	linkopts = ["-pthread"],
	visibility = ["//visibility:public"],
)
