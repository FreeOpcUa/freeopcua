#include "class_data.h"

#include <gtest/gtest.h>
#include <iostream>

int Argc;
char** Argv;

int main(int argc, char **argv)
{
  try
  {
    std::cout << "Running main() from gtest_main.cc\n";
    OPCTest::InitializeClassData(argc, argv);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
  }
  catch (const std::exception& exc)
  {
    std::cout << exc.what();
  }
}
