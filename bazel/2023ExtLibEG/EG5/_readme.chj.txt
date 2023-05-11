[2023-05-11] Using Bazel 6.0 on Win10+VC2019.

EG5 is actually a *success* case, EXE calling googletest success.

In order to build and run EG55, before run `bazel build` you have to manually delete(or rename):

	C:\Users\chj\_bazel_chj\j4upianb\external\gtestX\googletest-release-1.10.0\BUILD.bazel
	
	(Note: `chj` and `j4upianb` are placeholders that differ from machine to machine.)

Only after deleting that BUILD.bazel, `gtest.BUILD` here can actually take effect.

Otherwise, you will see error messages like below:

ERROR: D:/gitw/trivials/bazel/2023extlibeg/eg5/BUILD.bazel:2:10: Compiling hello-world.cc failed: undeclared inclusion(s) in rule '//:hello-world':
this rule is missing dependency declarations for the following files included by 'hello-world.cc':
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-internal.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-port.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-port-arch.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/custom/gtest-port.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-message.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-filepath.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-string.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-type-util.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-death-test.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-death-test-internal.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-matchers.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-printers.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/custom/gtest-printers.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-param-test.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/internal/gtest-param-util.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest_prod.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-test-part.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest-typed-test.h'
  'external/gtestX/googletest-release-1.10.0/googletest/include/gtest/gtest_pred_impl.h'
Target //:hello-world failed to build
Use --verbose_failures to see the command lines of failed build steps.