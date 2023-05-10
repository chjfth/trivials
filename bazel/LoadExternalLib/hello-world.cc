#include <ctime>
#include <string>
#include <iostream>
#include "gtest/gtest.h"

std::string get_greet(const std::string& who) {
  return "Hello " + who;
}

void print_localtime() {
  std::time_t result = std::time(nullptr);
  std::cout << std::asctime(std::localtime(&result));
}

TEST(HelloTest, GetGreet) {
  EXPECT_EQ(123, 100+23);
}


int main(int argc, char** argv) {
  std::string who = "world";
  if (argc > 1) {
    who = argv[1];
  }
  std::cout << get_greet(who) << std::endl;
  print_localtime();


  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
