#include <conio.h>
#include "gmock/gmock.h"
#include "gtest/gtest.h"
// # include <tchar.h>

// GTEST_API_ int _tmain(int argc, TCHAR** argv) {
GTEST_API_ int main(int argc, char** argv) {

  testing::InitGoogleMock(&argc, argv);

  const int retVal = RUN_ALL_TESTS();
  printf("\nPress any key to complete.");
  getch();
  return retVal;
}